#pragma once
#include <opencv2/imgcodecs.hpp>
#include <vector>
using namespace std;
using namespace cv;
class vectorMat {
	/*这个类设计为对一组图片进行组合操作*/
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
	vector<Mat> picArray_Qtshow;//用于Qtlabel显示
	Mat output_Qtshow;//用于Qtlabel显示
private:
	vector<Mat> picArray;//用于实际融合
	vector<string> filePaths;
	Mat output;//用于实际融合

};