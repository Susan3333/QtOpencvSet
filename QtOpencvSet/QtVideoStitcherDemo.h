#pragma once
#include <QWidget>
#include "ui_QtVideoStitcherDemo.h"
#include <qfiledialog.h>                //getopenfilename ������
#include <iostream>
#include <vector>
#include <opencv2/highgui.hpp>
#include <QTimer>
#include <opencv2/stitching.hpp>
#include "opencv2/stitching/detail/camera.hpp"
#include <opencv2/stitching/detail/timelapsers.hpp>
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
using namespace cv;
using namespace cv::detail;
using namespace std;
class QtVideoStitcherDemo : public QWidget
{
	Q_OBJECT

public:
	QtVideoStitcherDemo(QWidget* parent = Q_NULLPTR);
	~QtVideoStitcherDemo();
public slots:
	void ReadFrame();
	void videoOpenprocess();
	void changeMainVideoShowMode(int);
	void setScreenShootFlag();
	void setBrightnessAdjustment(int);
	void setContrastAdjustment(int);
	void setDefinition(int);
	void setmovingDetectionFlag();

private:
	Ui::QtVideoStitcherDemo ui;

	int MainVideoShowMode = 2;
	QTimer* timer;
	VideoCapture captureFirst;
	VideoCapture captureSecond;
	Mat frame1;
	Mat frame2;
	//��һ֡����Բ���
	vector<Mat> picArray;
	vector<int> indices;
	vector<CameraParams> cameras_inclass;
	Ptr<RotationWarper> warper;
	Ptr<ExposureCompensator> compensator;
	vector<Point> corners_inclass;
	vector<UMat> masks_warped_inclass;








	vector<Size> sizes_inclass;
	
	//��ͼflag
	int ScreenShootFlag = 0;
	//���ȶԱȶȲ���
	float alpha = 1;
	float beta = 0;

	//�����Ȳ���
	double definition = 0.3;

	//�˶����flag
	bool movingDetectionFlag = 0;

	//Ŀ����֡
	Mat before;

	Mat result_frame1;
	Mat result_frame2;
};