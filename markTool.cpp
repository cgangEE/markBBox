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


void readRectList(int savedIdx, int imgCnt){
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
		rectList[i] = vector<Rect>(m);
		typeList[i] = vector<pair<int, int> >(m);
		for (int j=0; j<m; ++j){
			fread(&rectList[i][j], sizeof(rectList[i][j]), 1, f);
			fread(&typeList[i][j], sizeof(typeList[i][j]), 1, f);
		}
	}
	fclose(f);
}

#define RED (Scalar(0, 0, 255))
#define BLUE (Scalar(255, 0, 0))
#define GREEN (Scalar(0, 255, 0))
#define FONT FONT_HERSHEY_PLAIN
#define RECT_SIZE 1.5

void drawRect(Scalar color){
	Mat tmp;
	img.copyTo(tmp);
	if (ok == 2)
		rectangle(tmp,
			Point(start.x, start.y),
			Point(finish.x, finish.y),
			color, RECT_SIZE);
	if (ok == 1)
		circle(tmp, start, 3, color);
	imshow(windowName, tmp);
}

void onMouse(int event, int x, int y, int flags, void *param){  
	if (readType) return;
	if (event == CV_EVENT_LBUTTONDOWN){
		if (ok == 0){
			start = Point(x, y);
			++ok;
		} else if (ok == 1){
			finish = Point(x, y);
			++ok;
		} else {
			if (dis(start, Point(x, y)) < dis(finish, Point(x,y)))
				start = Point(x, y);
			else
				finish = Point(x, y);
		}
		drawRect(RED);
	}
}


vector< vector<string> > classList(9);

void initClassList(){
	classList[0] = {"Human", 
		"Animals", 
		"Plants", 
		"Flags", 
		"Objects",
		"Abstractions", 
		"Insignias & Symbols", 
		"Other Images"};
	classList[1] = {
		"Male Face", 
		"Female Face", 
		"Abstract Face",
		"Male Body", 
		"Female Body",
		"Abstract Body", 
		"Roles (Knight, Witch, man, etc.)", 
		"Sport Figures (Football Player, Skier, etc)", 
		"Male Body Parts", 
		"Female Body Parts", 
		"Abstract Body Parts", 
		"Skulls", 
		"Miscellaneous Human Forms"};
	classList[2] = {
		"Cats & Cat Heads",
		"Dogs & Dog Heads",
		"Other Domestic Animals",
		"Vicious Animals (Lions, Tigers, etc.)",
		"Horses (Donkeys, Mules, etc.)",
		"Other Wild Animals", 
		"Snakes", 
		"Dragons", 
		"Birds (Cardinal, Hawk, etc.)",
		"Spiders, Bugs, and Insects",
		"Abstract Animals",
		"Animal Parts",
		"Miscellaneous Animal Forms"};
	classList[3] = {
		"Narcotics",
		"Red Flowers",
		"Blue Flowers",
		"Yellow Flowers",
		"Drawings of Flowers",
		"Rose",
		"Tulip",
		"Lily",
		"Miscellaneous Plants, Flowers, Vegetables"};
	classList[4] = {
		"American Flag",
		"State Flag",
		"Nazi Flag",
		"Confederate Flag",
		"British Flag",
		"Miscellaneous Flags"};
	classList[5] = {
		"Fire",
		"Weapons (Guns, Arrows, etc.)",
		"Airplanes",
		"Boats, Ships, and Other",
		"Vessels",
		"Trains",
		"Cars, Trucks, and Vehicles",
		"Mythical (Unicorns, etc.)",
		"Sporting Objects (Football, Ski, Hurdles, etc.)",
		"Water & Nature Scenes (Rivers, Sky, Trees, etc.)",
		"Miscellaneous Objects"};
	classList[6] = {"Figure(s)",
		"Sleeve",
		"Bracelet",
		"Anklet",
		"Necklace",
		"Shirt",
		"Body Band",
		"Head Band",
		"MiscellaneousAbstract"};
	classList[7] = {"National Symbols",
		"Political Symbols",
		"Military Symbols",
		"Fraternal Symbols",
		"Professional Symbols",
		"Gang Symbols",
		"Miscellaneous Symbols"};
	classList[8] = {"Wording (Mom, Dad, Mary, etc.)", 
		"Freeform Drawings",
		"Miscellaneous Images"};

}
char num[] = "1234qwerasdfzx";

void drawRect(Scalar color, vector<string> list, const char *classStr){
	Mat tmp;
	img.copyTo(tmp);
	transpose(tmp, tmp);
	tmp.resize(tmp.rows + 450, 0);
	transpose(tmp, tmp);

	putText(tmp, classStr, Point(img.cols,(1)*40), FONT, 1, cvScalar(255,255,255));

	for (int i=0; i<list.size(); ++i){
		sprintf(buf, "%c. ", toupper(num[i]));
		putText(tmp, buf + list[i], Point(img.cols,(2+i)*40), FONT, 1, cvScalar(255,255,255));
	}

	rectangle(tmp,
			Point(start.x, start.y),
			Point(finish.x, finish.y),
			color, RECT_SIZE);
	imshow(windowName, tmp);
}


bool getType(pair<int,int> &tattooType){
	int idx = 0;

	for (;;){
		drawRect(GREEN, classList[idx], idx==0?"Class":"SubClass");
		char key = waitKey() & 0xff;
		char *pos = strchr(num, key);
		if (pos != NULL){
			if (idx == 0){
				idx = pos - num + 1;
				tattooType.first = idx;
			} else {
				idx = pos - num + 1;
				tattooType.second = idx;
				return true;
			}
		} else if (key == 27){
			if (idx == 0)
				return false;
			else
				idx = 0;
		}
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

	for (;;){
		imgList[idx].copyTo(img);

		for (int i=0; i<rectList[idx].size(); ++i){
			Rect rect = rectList[idx][i];
			pair<int,int> typeName = typeList[idx][i];
			rectangle(img,
					Point(rect.x, rect.y),
					Point(rect.x + rect.width, rect.y + rect.height),
					BLUE);

			putText(img, classList[0][typeName.first-1], Point(rect.x+10, rect.y+15), FONT, 1, GREEN);
			putText(img, classList[typeName.first][typeName.second-1], Point(rect.x+10, rect.y+35), FONT, 1, GREEN);
		}

		double scale = 700.0 / img.rows;
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
		readType = false;
		setMouseCallback(windowName, onMouse, NULL); 


		int key;
		for(;;){
			key = waitKey() & 0xff;
			if (keyIsValid(key, {'a', 'f', ' ', 27, 8}))
				break;
		}
		if (key == 'f')
			idx = (idx + 1) % n;
		else if (key == 'a')
			idx = (idx - 1 + n) % n;
		else if (key == 27)
			break;
		else if (key == ' ' && ok==2){
			Rect rect( round(min(start.x, finish.x) / scale), 
					round(min(start.y, finish.y) / scale), 
					round(abs(start.x-finish.x) / scale), 
					round(abs(start.y-finish.y) / scale));

			readType = true;
			pair<int,int> tattooType;
			if (getType(tattooType)){
				rectList[idx].push_back(rect);
				typeList[idx].push_back(tattooType);
			}

		} else if (key == 8) {
			if (ok)
				ok = 0;
			else if (rectList[idx].size()>0){
				rectList[idx].pop_back();
				typeList[idx].pop_back();
			}
		}


		saveRectList(savedIdx);

	}
}

int main(){
	initClassList();

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

