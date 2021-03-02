#pragma once

#include <QWidget>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "ui_QtStitcherDemo.h"
#include "vectorMat.h"
using namespace std;
using namespace cv;

class QtStitcherDemo : public QWidget
{
	Q_OBJECT

public:
	QtStitcherDemo(QWidget *parent = Q_NULLPTR);
	~QtStitcherDemo();
public slots:
	void picOpenprocess();
	void picStitcherProcess();
private:
	Ui::QtStitcherDemo ui;
	vectorMat picArray;
};
