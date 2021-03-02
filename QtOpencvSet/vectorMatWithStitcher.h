#pragma once
#include <opencv2/imgcodecs.hpp>
#include <vector>
using namespace std;
using namespace cv;
class vectorMatWithStitcher {
	/*��������Ϊ��һ��ͼƬ������ϲ���*/
public:
	vectorMatWithStitcher() = default;
	vectorMatWithStitcher(const vector<string>&);
	vectorMatWithStitcher(const vector<string>&, const double&);
	vectorMatWithStitcher( vector<VideoCapture*>);

	void stitchingDetail(vector<VideoCapture*> allframe);
	void stitchingAll();
	void printFilePaths();
	void showAllpic();
	void showOutput();
private:
	vector<Mat> picArray;
	vector<string> filePaths;
	Mat output;
};