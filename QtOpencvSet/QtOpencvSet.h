#pragma once

#include <QtWidgets/QMainWindow>
#include "Gaussian_filter_UI.h"
#include "ui_QtOpencvSet.h"

class QtOpencvSet : public QMainWindow
{
	Q_OBJECT

public:
	QtOpencvSet(QWidget *parent = Q_NULLPTR);

public slots:       //�ۺ���������־
	void Gaussian_filter_UI_show();    //�ۺ���	
	void QtStitcherDemo_UI_show();    //�ۺ���
	void QtVideoDemo_UI_show();    //�ۺ���
	void QtVideoStitcherDemo_UI_show();    //�ۺ���
	void QtCameraStitcherDemo_UI_show();    //�ۺ���
private:
	Ui::QtOpencvSetClass ui;

};
