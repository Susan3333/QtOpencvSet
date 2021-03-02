#include "QtVideoDemo.h"
#include <opencv2/imgproc.hpp>
#include "opencv2/imgproc/types_c.h"
QtVideoDemo::QtVideoDemo(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui.setupUi(this);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(ReadFrame()));
}

QtVideoDemo::~QtVideoDemo()
{
}

void QtVideoDemo::ReadFrame()
{
	if (capture.isOpened())
	{
		capture >> frame;
		QImage image;
		if (!frame.empty())
		{
			if (frame.channels() == 3)    // RGB image
			{
				cvtColor(frame, result_frame, CV_BGR2RGB);
				cv::resize(result_frame, result_frame, Size(ui.label->width(), ui.label->height()));
				image = QImage((const uchar*)result_frame.data,
					result_frame.cols, 
					result_frame.rows, 
					result_frame.cols * result_frame.channels(),
					QImage::Format_RGB888);
			}
			else                     // gray image
			{
				cv::resize(frame, result_frame, Size(ui.label->width(), ui.label->height()));
				image = QImage((const uchar*)result_frame.data,
					result_frame.cols,
					result_frame.rows,
					result_frame.cols * result_frame.channels(),
					QImage::Format_Indexed8);
			}


			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			
			ui.label->setPixmap(QPixmap::fromImage(image));    //  将图片显示到label上
			ui.label->resize(ui.label->pixmap()->size());    //  将label控件resize到fame的尺寸
		}
	}

}



void QtVideoDemo::videoOpenprocess() {
	//这里需要清除vectormat类的内存 还没有写！！！！！！
	QStringList filenames;
	vector<string> pathnames;
	filenames = QFileDialog::getOpenFileNames(this,
		QString::fromUtf16(u"选择视频文件"),
		"",
		tr("Images (*.mp4)"));
	//  
	for (int i = 0; i < filenames.size(); i++) {
		QString str_path = filenames[i];
		pathnames.push_back(str_path.toStdString());
		cout << str_path.toStdString() << endl;
		//单个文件路径
	}
	capture.open(pathnames[0]);
	if (!capture.isOpened())
	{
		cout << "Movie open Error" << endl;
		return;
	}
	double rate = capture.get(CAP_PROP_FPS);
	cout << "帧率为:" << " " << rate << endl;
	cout << "总帧数为:" << " " << capture.get(CAP_PROP_FRAME_COUNT) << endl;//输出帧总数
	double position = 0.0;
	//设置播放到哪一帧，这里设置为第0帧
	capture.set(CAP_PROP_POS_FRAMES, position);
	timer->start(25);
}