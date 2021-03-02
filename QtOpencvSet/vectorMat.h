#pragma once
#include <opencv2/imgcodecs.hpp>
#include <vector>
using namespace std;
using namespace cv;
class vectorMat {
	/*��������Ϊ��һ��ͼƬ������ϲ���*/
public:
	vectorMat() = default;
	vectorMat(const vector<string>&);
	vectorMat(const vector<string>&, const double&);
	vectorMat(const vector<string>&, const int&, const int&);

	~vectorMat();
	void InitalizeFromVectorString(const vector<string>&,const double&);
	void InitalizeQtshow(vector<int>,vector<int>);
	void TransOutputToQtshow(int, int);
	void stitchingDetail();
	void stitchingAll();
	void printFilePaths();
	void showAllpic();
	void showAllpicQt();
	void showOutput();
	void showQtOutput();
	vector<Mat> picArray_Qtshow;//����Qtlabel��ʾ
	Mat output_Qtshow;//����Qtlabel��ʾ
private:
	vector<Mat> picArray;//����ʵ���ں�
	vector<string> filePaths;
	Mat output;//����ʵ���ں�

};