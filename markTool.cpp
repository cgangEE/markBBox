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

bool readFromFile(vector<Mat> &imgList){
	FILE *f = NULL;

	for (int i=0; i<10; ++i){
		string savedName = "data";
		savedName.push_back('0'+i);
		if (fileExist(savedName))
			f = fopen(savedName.c_str(), "rb");
		break;
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



void onMouse(int event, int x, int y, int flags, void *param){  
	switch (event){ 
		case CV_EVENT_LBUTTONDOWN:          
			if (!ok)
				start = Point(x, y);  
			else {
				puts("FT");
				Point now(x, y);
				if (dis(now, finish) < dis(now, start)){
					ok = 1;
					finish = now;
				} else {
					ok = 2;
					start = now;
				}
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
				if (ok <= 1)
					finish = Point(x, y);
				else{
					start = Point(x, y);
				}
				Mat tmp;
				img.copyTo(tmp);
				rectangle(tmp,
						Point(start.x, start.y),
						Point(finish.x, finish.y),
						Scalar( 255, 0, 0));
				imshow("x", tmp);
			}  
			break;  
	}
}

bool keyIsValid(int key, vector<int> validKeyList = {83, 81, 27, 10}){
	for (int i=0; i<validKeyList.size(); ++i)
		if (validKeyList[i] == key)
			return true;
	return false;
}

void showUI(vector<Mat> &imgList){
	namedWindow("x");

	int n = imgList.size();

	for (;;){
		img = imgList[idx];

		double scale = 800.0 / img.rows;
		int rows = (int) (img.rows * scale);
		int cols = (int) (img.cols * scale);
		Size size(cols, rows);
		resize(img, img, size);

		imshow("x", img);

		ok = 0;
		setMouseCallback("x", onMouse, NULL); 


		int key;
		for(;;){
			key = waitKey() & 0xff;
			cout<<key<<endl;
			if (keyIsValid(key))
				break;
		}
		if (key == 83)
			idx = (idx + 1) % n;
		else if (key == 81)
			idx = (idx - 1 + n) % n;
		else if (key == 27)
			break;
		else if (key == 10){

		}

	}
}

int main(){

	vector<Mat> imgList;
	readFromFile(imgList);
	cout<<imgList.size()<<endl;

	rectList = vector<vector<Rect> >(imgList.size());
	showUI(imgList);

	return 0;
}

