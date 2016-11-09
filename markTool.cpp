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



bool draw = false;
Point cursor;//初始坐标   
Rect rect;//标记ROI的矩形框
Mat img;



void onMouse(int event, int x, int y, int flags, void *param){  
	switch (event){ 
		//按下鼠标左键
		case CV_EVENT_LBUTTONDOWN:          
			cursor = Point(x, y);  
			draw = true;  
			break;  

			//松开鼠标左键      
		case CV_EVENT_LBUTTONUP:           
			if (rect.height > 0 && rect.width > 0){  
				cout<<rect.x<<' '<<rect.y<<' '<<rect.height<<' '<<rect.width<<endl;
			}  
			draw = false;  
			break;  

			//移动光标
		case CV_EVENT_MOUSEMOVE:  
			if (draw){  
				Mat tmp;
				img.copyTo(tmp);
				rectangle(tmp,
						Point(cursor.x, cursor.y),
						Point(x, y),
						Scalar( 255, 0, 0));
				imshow("x", tmp);
			}  
			break;  
	}  
}  

bool keyIsValid(int key, vector<int> validKeyList = {83, 81, 27}){
	for (int i=0; i<validKeyList.size(); ++i)
		if (validKeyList[i] == key)
			return true;
	return false;
}

void showUI(vector<Mat> &imgList){
	namedWindow("x");

	int n = imgList.size();
	int idx = 0;

	for (;;){
		img = imgList[idx];

		double scale = 800.0 / img.rows;
		int rows = (int) (img.rows * scale);
		int cols = (int) (img.cols * scale);
		Size size(cols, rows);
		resize(img, img, size);

		imshow("x", img);
		setMouseCallback("x", onMouse, NULL); 


		int key;
		for(;;){
			key = waitKey() & 0xff;
			if (keyIsValid(key))
				break;
		}
		if (key == 83)
			idx = (idx + 1) % n;
		else if (key == 81)
			idx = (idx - 1 + n) % n;
		else if (key == 27)
			break;
	}
}

int main(){

	vector<Mat> imgList;
	readFromFile(imgList);
	cout<<imgList.size()<<endl;

	showUI(imgList);

	return 0;
}

