#pragma once
#include <opencv2/imgcodecs.hpp>
#include <vector>
using namespace std;
using namespace cv;
class vectorMatWithStitcher {
	/*这个类设计为对一组图片进行组合操作*/
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