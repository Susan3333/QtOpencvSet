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


			// ��ץȡ����֡��ת��ΪQImage��ʽ��QImage::Format_RGB32��ͬ������ͷ�ò�ͬ�ĸ�ʽ��
			
			ui.label->setPixmap(QPixmap::fromImage(image));    //  ��ͼƬ��ʾ��label��
			ui.label->resize(ui.label->pixmap()->size());    //  ��label�ؼ�resize��fame�ĳߴ�
		}
	}

}



void QtVideoDemo::videoOpenprocess() {
	//������Ҫ���vectormat����ڴ� ��û��д������������
	QStringList filenames;
	vector<string> pathnames;
	filenames = QFileDialog::getOpenFileNames(this,
		QString::fromUtf16(u"ѡ����Ƶ�ļ�"),
		"",
		tr("Images (*.mp4)"));
	//  
	for (int i = 0; i < filenames.size(); i++) {
		QString str_path = filenames[i];
		pathnames.push_back(str_path.toStdString());
		cout << str_path.toStdString() << endl;
		//�����ļ�·��
	}
	capture.open(pathnames[0]);
	if (!capture.isOpened())
	{
		cout << "Movie open Error" << endl;
		return;
	}
	double rate = capture.get(CAP_PROP_FPS);
	cout << "֡��Ϊ:" << " " << rate << endl;
	cout << "��֡��Ϊ:" << " " << capture.get(CAP_PROP_FRAME_COUNT) << endl;//���֡����
	double position = 0.0;
	//���ò��ŵ���һ֡����������Ϊ��0֡
	capture.set(CAP_PROP_POS_FRAMES, position);
	timer->start(25);
}