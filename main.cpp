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

string type2str(int type) {
  string r;
  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);
  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }
  r += "C";
  r += (chans+'0');
  return r;
}

bool readFileNameList(const char *filename, vector<string> &fileNameList){
	FILE *f = fopen(filename, "r");
	if (!f){
		printf("File<%s> not found\n", filename);
		return false;
	}
	
	char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
	while (fgets(buf, BUF_SIZE, f)!=NULL){
		buf[strlen(buf)-1] = 0;
		fileNameList.push_back(string(buf+7));
	}
	

	free(buf);
	return true;
}


bool writeToFile(string fromDir, string savedName, 
		vector<string> &fileNameList){

	FILE *f = fopen(savedName.c_str(), "wb");
	int n = fileNameList.size();
	fwrite(&n, sizeof(n), 1, f);

	for (int i=0; i<n; ++i){		
		Mat img = imread((fromDir+fileNameList[i]).c_str());
		if (!img.data){
			printf("img file<%s> is unable to open\n", fileNameList[i].c_str());
			return false;
		}
		fwrite(&img.rows, sizeof(img.rows), 1, f);
		fwrite(&img.cols, sizeof(img.cols), 1, f);
		fwrite(img.data, sizeof(unsigned char) * 3, img.rows * img.cols, f);
	}
	fclose(f);
	return true;
}

vector<vector<string> > splitList(vector<string> fileNameList, int n){
	vector<vector<string> >ret(n);
	int m = (fileNameList.size() + n - 1) / n;

	for (int i=0; i<fileNameList.size(); ++i)
		ret[i/m].push_back(fileNameList[i]);
	return ret;
}

int main(){
	vector<string> fileNameList;
	if (!readFileNameList("Flickr/filename.txt", fileNameList))
		return 1;

	int n = 10;
	vector<vector<string> > listVec = splitList(fileNameList, n);


	for (int i=0; i<n; ++i){
		cout<<listVec[i].size()<<endl;
		string savedName = "data";
		savedName.push_back('0'+i);
		writeToFile("Flickr/images/", savedName, listVec[i]);
	}


	return 0;
}

