#pragma once

#include <QWidget>
#include "ui_QtVideoDemo.h"
#include <qfiledialog.h>                //getopenfilename ¿‡…Í√˜
#include <iostream>
#include <vector>
#include <opencv2/highgui.hpp>
#include <QTimer>
using namespace cv;
using namespace std;
class QtVideoDemo : public QWidget
{
	Q_OBJECT

public:
	QtVideoDemo(QWidget *parent = Q_NULLPTR);
	~QtVideoDemo();
public slots:
	void videoOpenprocess();
	void ReadFrame();
private:
	Ui::QtVideoDemo ui;

	QTimer* timer;
	VideoCapture capture;
	Mat frame;
	Mat result_frame;

};
