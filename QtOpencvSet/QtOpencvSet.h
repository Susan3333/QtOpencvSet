#pragma once

#include <QtWidgets/QMainWindow>
#include "Gaussian_filter_UI.h"
#include "ui_QtOpencvSet.h"

class QtOpencvSet : public QMainWindow
{
	Q_OBJECT

public:
	QtOpencvSet(QWidget *parent = Q_NULLPTR);

public slots:       //槽函数声明标志
	void Gaussian_filter_UI_show();    //槽函数	
	void QtStitcherDemo_UI_show();    //槽函数
	void QtVideoDemo_UI_show();    //槽函数
	void QtVideoStitcherDemo_UI_show();    //槽函数
	void QtCameraStitcherDemo_UI_show();    //槽函数
private:
	Ui::QtOpencvSetClass ui;

};
