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

bool readFromFile(vector<Mat> &imgList, int &savedIdx, int &imgCnt){
	FILE *f = NULL;


	string savedName = "data";
	savedName.push_back('0'+savedIdx);
	if (fileExist(savedName)){
		f = fopen(savedName.c_str(), "rb");
	}

	if (f==NULL)
		return false;

	int n;
	fread(&n, sizeof(n), 1, f);
	imgCnt = n;
	return true;

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



bool draw = false;
int ok = 0;
bool readType = false;
Point start;
Point finish;
Mat img;
vector<vector<Rect> > rectList;
vector<vector<pair<int,int> > > typeList;
int idx = 0;
string windowName;
char buf[100];

void saveRectList(int savedIdx){
	string savedName = "rect";
	savedName.push_back('0' + savedIdx);
	FILE *f = fopen(savedName.c_str(), "wb");

	fwrite(&idx, sizeof(idx), 1, f);
	int n = rectList.size();
	fwrite(&n, sizeof(n), 1, f);


	for (int i=0; i<n; ++i){
		int m = rectList[i].size();
		fwrite(&m, sizeof(m), 1, f);
		for (int j=0; j<m; ++j){
			fwrite(&rectList[i][j], sizeof(rectList[i][j]), 1, f);
			fwrite(&typeList[i][j], sizeof(typeList[i][j]), 1, f);
		}
	}
	fclose(f);
}


void readRectList(int savedIdx, int imgCnt, int &rectCnt){
	string savedName = "rect";
	savedName.push_back('0' + savedIdx);
	if (!fileExist(savedName)){
		rectList = vector<vector<Rect> >(imgCnt);
		typeList = vector<vector<pair<int, int> > > (imgCnt);
		return;
	}

	FILE *f = fopen(savedName.c_str(), "rb");

	fread(&idx, sizeof(idx), 1, f);


	int n;
	fread(&n, sizeof(n), 1, f);
	rectList = vector<vector<Rect> >(n);
	typeList = vector<vector<pair<int, int> > > (n);


	for (int i=0; i<n; ++i){
		int m;
		fread(&m, sizeof(m), 1, f);
		rectCnt += m;

		rectList[i] = vector<Rect>(m);
		typeList[i] = vector<pair<int, int> >(m);
		for (int j=0; j<m; ++j){
			fread(&rectList[i][j], sizeof(rectList[i][j]), 1, f);
			fread(&typeList[i][j], sizeof(typeList[i][j]), 1, f);
		}
	}
	fclose(f);
}





int main(){

	int rectTotCnt = 0;
	for (int savedIdx=0; savedIdx<10; ++savedIdx){
		vector<Mat> imgList;
		int imgCnt = 0;

		if (!readFromFile(imgList, savedIdx, imgCnt)){
			puts("Cannot find data file!");
			return -1;
		}

		int rectCnt = 0;
		readRectList(savedIdx, imgCnt, rectCnt);
		rectTotCnt += rectCnt;
		cout<<savedIdx<<'\t'<<rectCnt<<endl;
	}
	cout<<rectTotCnt<<endl;


	return 0;
}

