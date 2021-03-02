#include "QtStitcherDemo.h"
#include <qfiledialog.h>                //getopenfilename 类申明
#include <opencv2/imgproc.hpp>
#include<iostream>
using namespace std;

QtStitcherDemo::QtStitcherDemo(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui.setupUi(this);
}

QtStitcherDemo::~QtStitcherDemo()
{
}

void QtStitcherDemo::picStitcherProcess()
{
	QImage img;
	picArray.stitchingDetail();
	picArray.TransOutputToQtshow(ui.pic_merge->width(), ui.pic_merge->height());
	if (picArray.output_Qtshow.channels() == 3)    // RGB image
	{
		img = QImage ((const uchar*)picArray.output_Qtshow.data,
			picArray.output_Qtshow.cols,
			picArray.output_Qtshow.rows,
			picArray.output_Qtshow.cols * picArray.output_Qtshow.channels(),
			QImage::Format_RGB888);
	}
	else                     // gray image
	{
		img = QImage ((const uchar*)picArray.output_Qtshow.data,
			picArray.output_Qtshow.cols,
			picArray.output_Qtshow.rows,
			picArray.output_Qtshow.cols * picArray.output_Qtshow.channels(),
			QImage::Format_Indexed8);
	}

	ui.pic_merge->setPixmap(QPixmap::fromImage(img));
	ui.pic_merge->resize(ui.pic_merge->pixmap()->size());
}

void QtStitcherDemo::picOpenprocess() {
	//这里需要清除vectormat类的内存 还没有写！！！！！！
	QStringList filenames;
	vector<string> pathnames;
	vector<QLabel*> QLabelSet;
	vector<int>W;
	vector<int>H;
	QLabelSet.push_back(ui.pic_show_1);
	QLabelSet.push_back(ui.pic_show_2);
	QLabelSet.push_back(ui.pic_show_3);
	filenames = QFileDialog::getOpenFileNames(this,
		QString::fromUtf16(u"选择图片文件"),
		"",
		tr("Images (*.png *.bmp *.jpg *.tif *.GIF *.jpeg)"));
	for (int i = 0; i < filenames.size(); i++) {
		QString str_path = filenames[i];
		pathnames.push_back(str_path.toStdString());
		//单个文件路径
	}
	for (int i = 0; i < QLabelSet.size(); i++) {
		H.push_back(QLabelSet[i]->height());
		W.push_back(QLabelSet[i]->width());
	}
	picArray.InitalizeFromVectorString(pathnames, 0.2);
	picArray.InitalizeQtshow(W, H);
	for (int i = 0; i < QLabelSet.size(); i++) {
		QImage img = QImage((const unsigned char*)(picArray.picArray_Qtshow[i].data), picArray.picArray_Qtshow[i].cols, picArray.picArray_Qtshow[i].rows, QImage::Format_RGB888);
		QLabelSet[i]->setPixmap(QPixmap::fromImage(img));
	}
	//picArray.showAllpic();
	//picArray.showAllpicQt();
}