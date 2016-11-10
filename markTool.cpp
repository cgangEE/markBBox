#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define BUF_SIZE 1000

inline bool fileExist (const std::string& name) {
	if (FILE *file = fopen(name.c_str(), "rb")) {
		fclose(file);
		return true;
	} else {
		return false;
	}   
}

bool readFromFile(vector<Mat> &imgList, int &savedIdx){
	FILE *f = NULL;

	for (int i=0; i<10; ++i){
		string savedName = "data";
		savedName.push_back('0'+i);
		if (fileExist(savedName)){
			f = fopen(savedName.c_str(), "rb");
			savedIdx = i;
			break;
		}
	}
	if (f==NULL)
		return false;

	int n;
	fread(&n, sizeof(n), 1, f);
	for (int i=0; i<n; ++i){
		int rows, cols;
		fread(&rows, sizeof(rows), 1, f);
		fread(&cols, sizeof(cols), 1, f);

		unsigned char *buf = (unsigned char*) 
			malloc(sizeof(unsigned char) * rows * cols * 3);

		fread(buf, sizeof(unsigned char) * 3, rows * cols, f);
		Mat img(rows, cols, CV_8UC3, buf);

		imgList.push_back(img);
	}
	fclose(f);
	return true;
}


#define pow(x) ((x)*(x))
int dis(Point a, Point b){
	return pow(a.x - b.x) + pow(a.y - b.y);
}


bool draw = false;
int ok = 0;
Point start;
Point finish;
Mat img;
vector<vector<Rect> > rectList;
int idx = 0;
string windowName;

void saveRectList(int savedIdx){
	string savedName = "rect";
	savedName.push_back('0' + savedIdx);
	FILE *f = fopen(savedName.c_str(), "wb");

	int n = rectList.size();
	fwrite(&n, sizeof(n), 1, f);

	for (int i=0; i<n; ++i){
		int m = rectList[i].size();
		fwrite(&m, sizeof(m), 1, f);
		for (int j=0; j<m; ++j)
			fwrite(&rectList[i][j], sizeof(rectList[i][j]), 1, f);
	}
	fclose(f);
}


void readRectList(int savedIdx, int imgCnt){
	string savedName = "rect";
	savedName.push_back('0' + savedIdx);
	if (!fileExist(savedName)){
		rectList = vector<vector<Rect> >(imgCnt);
		return;
	}

	FILE *f = fopen(savedName.c_str(), "rb");
	int n;

	fread(&n, sizeof(n), 1, f);
	rectList = vector<vector<Rect> >(n);
	for (int i=0; i<n; ++i){
		int m;
		fread(&m, sizeof(m), 1, f);
		rectList[i] = vector<Rect>(m);
		for (int j=0; j<m; ++j)
			fread(&rectList[i][j], sizeof(rectList[i][j]), 1, f);
	}
	fclose(f);
}

void drawRect(){
	Mat tmp;
	img.copyTo(tmp);
	rectangle(tmp,
			Point(start.x, start.y),
			Point(finish.x, finish.y),
			Scalar( 0, 255, 0));
	imshow(windowName, tmp);
}

void onMouse(int event, int x, int y, int flags, void *param){  
	switch (event){ 
		case CV_EVENT_LBUTTONDOWN:          
			if (!ok)
				start = Point(x, y);  
			else {
				Point now(x, y);
				int d1 = abs(x - start.x);
				int d2 = abs(x - finish.x);
				int d3 = abs(y - start.y);
				int d4 = abs(y - finish.y);
				int mn = min(min(d1, d2), min(d3, d4));
				if (mn == d1){
					ok = 1;
					start.x = x;
				} else if (mn == d2){
					ok = 2;
					finish.x = x;
				} else if (mn == d3){
					ok = 3;
					start.y = y;
				} else {
					ok = 4;
					finish.y = y;
				}
				drawRect();
			}

			draw = true;  
			break;  
		case CV_EVENT_LBUTTONUP:           
			if (abs(start.x - finish.x)>0 && abs(start.y - finish.y)>0){
				ok = 1;
			}
			draw = false;  
			break;  
		case CV_EVENT_MOUSEMOVE:  
			if (draw){  
				if (ok == 0)
					finish = Point(x, y);
				drawRect();
			}  
			break;  
	}
}

bool keyIsValid(int key, vector<int> validKeyList = {83, 81, 27, 10, 8}){
	for (int i=0; i<validKeyList.size(); ++i)
		if (validKeyList[i] == key)
			return true;
	return false;
}

void showUI(vector<Mat> &imgList, int savedIdx){

	int n = imgList.size();
	char buf[100];

	for (;;){
		imgList[idx].copyTo(img);

		for (int i=0; i<rectList[idx].size(); ++i){
			Rect rect = rectList[idx][i];
			rectangle(img,
					Point(rect.x, rect.y),
					Point(rect.x + rect.width, rect.y + rect.height),
					Scalar(0, 0, 255));

		}

		double scale = 800.0 / img.rows;
		int rows =  round(img.rows * scale);
		int cols =  round(img.cols * scale);
		Size size(cols, rows);
		resize(img, img, size);


		destroyAllWindows();

		sprintf(buf, "Image %d, Rect %lu", idx+1, rectList[idx].size());
		windowName = buf;

		namedWindow(windowName);
		moveWindow(windowName, 0, 0);

		imshow(windowName, img);

		ok = 0;
		setMouseCallback(windowName, onMouse, NULL); 


		int key;
		for(;;){
			key = waitKey() & 0xff;
//			cout<<key<<endl;
			if (keyIsValid(key))
				break;
		}
		if (key == 83)
			idx = (idx + 1) % n;
		else if (key == 81)
			idx = (idx - 1 + n) % n;
		else if (key == 27)
			break;
		else if (key == 10 && ok){
			Rect rect( round(min(start.x, finish.x) / scale), 
					round(min(start.y, finish.y) / scale), 
					round(abs(start.x-finish.x) / scale), 
					round(abs(start.y-finish.y) / scale));
			rectList[idx].push_back(rect);
		} else if (key == 8) {
			if (ok)
				ok = 0;
			else if (rectList[idx].size()>0)
				rectList[idx].pop_back();
		}


		saveRectList(savedIdx);

	}
}

int main(){

	vector<Mat> imgList;
	int savedIdx;
	if (!readFromFile(imgList, savedIdx)){
		puts("Cannot find data file!");
		return -1;
	}


	readRectList(savedIdx, imgList.size());

	showUI(imgList, savedIdx);

	return 0;
}

