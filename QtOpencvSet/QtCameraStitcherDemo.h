#pragma once

#include <QWidget>
#include "ui_QtCameraStitcherDemo.h"
#include <qfiledialog.h>                //getopenfilename 类申明
#include <iostream>
#include <vector>
#include <opencv2/highgui.hpp>
#include <QTimer>
#include <opencv2/stitching.hpp>
#include "opencv2/stitching/detail/camera.hpp"
#include <opencv2/stitching/detail/timelapsers.hpp>
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include <opencv2\imgproc\types_c.h>
using namespace cv;
using namespace cv::detail;
using namespace std;

class QtCameraStitcherDemo : public QWidget
{
	Q_OBJECT

public:
	QtCameraStitcherDemo(QWidget* parent = Q_NULLPTR);
	~QtCameraStitcherDemo();





public slots:
	void ReadFrame();
	void cameraLink();
	void changeMainCameraShowMode(int);

	void setScreenShootFlag();


	void setBrightnessAdjustment(int);
	void setContrastAdjustment(int);
	void setDefinition(int);
	void setmovingDetectionFlag();
	void setAdjuster(int);
	void setWarper_creator(QString);
	void setFinder(int);
	void setMatch_conf(QString);
	void setConf_thresh(QString);

	void displayAll();
private:
	Ui::QtCameraStitcherDemo ui;

	int MainCameraShowMode = 3;
	QTimer* timer;

	VideoCapture captureFirst;
	VideoCapture captureSecond;
	VideoCapture captureThird;
	VideoCapture captureFourth;
	VideoCapture captureFifth;


	Mat frame1;
	Mat frame2;
	Mat frame3;
	Mat frame4;
	Mat frame5;


	Mat rframe1;
	Mat rframe2;
	Mat rframe3;
	Mat rframe4;
	Mat rframe5;

	//第一帧的配对参数
	vector<Mat> picArray;
	vector<int> indices;
	vector<CameraParams> cameras_inclass;
	Ptr<RotationWarper> warper;
	Ptr<ExposureCompensator> compensator;
	vector<Point> corners_inclass;
	vector<UMat> masks_warped_inclass;


	vector<Size> sizes_inclass;

	//截图flag
	int ScreenShootFlag = 0;
	//亮度对比度参数
	float alpha = 1;
	float beta = 0;

	//清晰度参数
	double definition = 0.3;

	//运动检测flag
	bool movingDetectionFlag = 0;

	//目标检测帧
	Mat before;

	//Finder
	Ptr<Feature2D> finder = xfeatures2d::SURF::create();


	//Adjuster
	Ptr<detail::BundleAdjusterBase> adjuster = makePtr<detail::NoBundleAdjuster>();


	float match_conf = 0.3f;
	float conf_thresh = 0.5f;//调0.1，或0.9

	Ptr<WarperCreator> warper_creator = makePtr<cv::AffineWarper>();

	Mat result_frame1;
	Mat result_frame2;
	Mat result_frame3;
	Mat result_frame4;
	Mat result_frame5;

};


