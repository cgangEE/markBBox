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


vector<vector<Rect> > rectList;
vector<vector<pair<int,int> > > typeList;
int idx = 0;
string windowName;
char buf[100];


void readRectList(int savedIdx, 
		vector<string> &imgName, int &imgIdx){

	string savedName = "rect";
	savedName.push_back('0' + savedIdx);

	FILE *f = fopen(savedName.c_str(), "rb");

	fread(&idx, sizeof(idx), 1, f);


	int n;
	fread(&n, sizeof(n), 1, f);
	rectList = vector<vector<Rect> >(n);
	typeList = vector<vector<pair<int, int> > > (n);


	for (int i=0; i<n; ++i){
		int m;
		fread(&m, sizeof(m), 1, f);

		rectList[i] = vector<Rect>(m);
		typeList[i] = vector<pair<int, int> >(m);

		cout<<imgName[imgIdx++];
		cout<<m<<endl;
		for (int j=0; j<m; ++j){
			fread(&rectList[i][j], sizeof(rectList[i][j]), 1, f);
			fread(&typeList[i][j], sizeof(typeList[i][j]), 1, f);

			Rect rect = rectList[i][j];
			cout<<rect.x<<' '<<rect.y<<' '<<rect.width<<' '<<rect.height<<endl;
		}
	}
	fclose(f);
}


#define BUF_SIZE 10000

bool readImageName(const char *filename, vector<string> &imgName){
	FILE *f = fopen(filename, "r");
	if (f==NULL){
		fprintf(stderr, "open file<%s> failed\n", filename);
		return false;
	}

	char *buf = (char*)malloc(sizeof(char) * BUF_SIZE);
	while (fgets(buf, BUF_SIZE, f) != NULL){
		imgName.push_back( strchr(buf, '|') + 1 );
	}
	
	return true;
}


int main(){

	vector<string> imgName;
	if (!readImageName("Flickr/filename.txt", imgName))
		return -1;

	int imgIdx = 0;
	for (int savedIdx=0; savedIdx<10; ++savedIdx){
		readRectList(savedIdx, imgName, imgIdx);
	}


	return 0;
}

