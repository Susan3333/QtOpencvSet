#include "QtOpencvSet.h"
#include "Gaussian_filter_UI.h"
#include "QtStitcherDemo.h"
#include "QtVideoStitcherDemo.h"
#include "QtVideoDemo.h"
#include "QtCameraStitcherDemo.h"
#include <QMessageBox>

QtOpencvSet::QtOpencvSet(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void QtOpencvSet::Gaussian_filter_UI_show() 
{
	Gaussian_filter_UI *Gu_ui = new Gaussian_filter_UI();
	Gu_ui->show();
}

void QtOpencvSet::QtStitcherDemo_UI_show()
{
	QtStitcherDemo* Gu_ui = new QtStitcherDemo();
	Gu_ui->show();
}

void QtOpencvSet::QtVideoDemo_UI_show()
{
	QtVideoDemo* Gu_ui = new QtVideoDemo();
	Gu_ui->show();
}

void QtOpencvSet::QtVideoStitcherDemo_UI_show()
{
	QtVideoStitcherDemo* Gu_ui = new QtVideoStitcherDemo();
	Gu_ui->show();
}

void QtOpencvSet::QtCameraStitcherDemo_UI_show()
{
	QtCameraStitcherDemo* Gu_ui = new QtCameraStitcherDemo();
	Gu_ui->show();
}
