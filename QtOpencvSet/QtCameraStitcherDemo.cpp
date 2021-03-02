#include "QtCameraStitcherDemo.h"
#include <opencv2/imgproc.hpp>
#include "opencv2/imgproc/types_c.h"
#include "vectorMatWithStitcher.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "vectorMatWithStitcher.h"
#include <iostream>
#include <opencv2/stitching.hpp>
#include "opencv2/stitching/detail/camera.hpp"
#include <opencv2/stitching/detail/timelapsers.hpp>
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"


#include "opencv2/opencv_modules.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/stitching/detail/autocalib.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/timelapsers.hpp"
#include "opencv2/stitching/detail/camera.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/util.hpp"
#include "opencv2/stitching/detail/warpers.hpp"
#include "opencv2/stitching/warpers.hpp"

#include "HK.h"
#include "vlc_reader.h"
#include <opencv2/highgui/highgui_c.h>
#include "vlc_reader.h"
#include "vectorMat.h"


#define ENABLE_LOG 1
#define LOG(msg) std::cout << msg
#define LOGLN(msg) std::cout << msg << std::endl

using namespace std;
using namespace cv;
using namespace cv::detail;

#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")

vlc_reader vr1((char*)"rtsp://admin:HIKVISION610@192.168.0.100");
vlc_reader vr2((char*)"rtsp://admin:HIKVISION610@192.168.0.102");
vlc_reader vr3((char*)"rtsp://admin:HIKVISION610@192.168.0.103");
Mat MoveDetectCamera(Mat before, Mat frame) {

	Mat result = frame.clone();
	//2.帧差
	Mat temp1, frame1;
	temp1 = before;
	frame1 = frame;
	//将background和frame转为灰度图  
	Mat gray1, gray2;
	cvtColor(temp1, gray1, CV_BGR2GRAY);
	cvtColor(frame1, gray2, CV_BGR2GRAY);
	//将background和frame做差  
	Mat diff;
	absdiff(gray1, gray2, diff);
	//imshow("帧差图", diff);


	//对差值图diff_thresh进行阈值化处理  二值化
	Mat diff_thresh;
	Mat kernel_erode = getStructuringElement(MORPH_RECT, Size(5, 5));//函数会返回指定形状和尺寸的结构元素。																 //调用之后，调用膨胀与腐蚀函数的时候，第三个参数值保存了getStructuringElement返回值的Mat类型变量。也就是element变量。
	Mat kernel_dilate = getStructuringElement(MORPH_RECT, Size(40, 40));
	//imshow("erode", kernel_erode);
	//imshow("dilate", kernel_dilate);
	//进行二值化处理，选择50，255为阈值
	threshold(diff, diff_thresh, 20, 255, CV_THRESH_BINARY);
	//imshow("二值化处理后", diff_thresh);

	//腐蚀  
	erode(diff_thresh, diff_thresh, kernel_erode);
	//imshow("腐蚀处理后", diff_thresh);
	//膨胀  
	cv::dilate(diff_thresh, diff_thresh, kernel_dilate);
	//imshow("膨胀处理后", diff_thresh);

	//查找轮廓并绘制轮廓  
	vector<vector<Point> > contours;
	findContours(diff_thresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//找轮廓函数
	//drawContours(result, contours, -1, Scalar(0, 0, 255), 2);//在result上绘制轮廓  
	 //查找正外接矩形  
	vector<Rect> boundRect(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(contours[i]);
		rectangle(result, boundRect[i], Scalar(0, 255, 0), 2);//在result上绘制正外接矩形  
	}



	temp1.release();
	frame1.release();
	gray1.release();
	gray2.release();
	diff.release();
	diff_thresh.release();
	kernel_dilate.release();
	kernel_dilate.release();
	contours.clear();
	boundRect.clear();

	return result;//返回result  
}

QtCameraStitcherDemo::QtCameraStitcherDemo(QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui.setupUi(this);


	//timer = new QTimer(this);
	//connect(timer, SIGNAL(timeout()), this, SLOT(ReadFrame()));
}

QtCameraStitcherDemo::~QtCameraStitcherDemo()
{

}

void QtCameraStitcherDemo::ReadFrame()
{

	//if (captureFirst.isOpened() && captureSecond.isOpened()){
		//captureFirst >> frame1;
	//	captureSecond >> frame2;
		//captureThird >> frame3;
		//frame1 = vr1.frame();
	//	waitKey(10);
	//frame2 = vr2.frame();
	//	waitKey(10);
	//	frame3 = vr3.frame();
	//	waitKey(10);
	QImage image1;
	if (!frame1.empty())
	{
		if (frame1.channels() == 3)    // RGB image
		{
			cv::cvtColor(frame1, result_frame1, CV_BGR2RGB);
			cv::resize(result_frame1, result_frame1, Size(ui.label->width(), ui.label->height()));
			image1 = QImage((const uchar*)result_frame1.data,
				result_frame1.cols,
				result_frame1.rows,
				result_frame1.cols * result_frame1.channels(),
				QImage::Format_RGB888);
		}
		else                     // gray image
		{
			cv::resize(frame1, result_frame1, Size(ui.label->width(), ui.label->height()));
			image1 = QImage((const uchar*)result_frame1.data,
				result_frame1.cols,
				result_frame1.rows,
				result_frame1.cols * result_frame1.channels(),
				QImage::Format_Indexed8);
		}

		// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
		ui.label->setPixmap(QPixmap::fromImage(image1));    //  将图片显示到label上
		ui.label->resize(ui.label->pixmap()->size());    //  将label控件resize到fame的尺寸
	}

	QImage image2;
	if (!frame2.empty())
	{
		if (frame2.channels() == 3)    // RGB image
		{
			cv::cvtColor(frame2, result_frame2, CV_BGR2RGB);
			cv::resize(result_frame2, result_frame2, Size(ui.label_2->width(), ui.label_2->height()));
			image2 = QImage((const uchar*)result_frame2.data,
				result_frame2.cols,
				result_frame2.rows,
				result_frame2.cols * result_frame2.channels(),
				QImage::Format_RGB888);
		}
		else                     // gray image
		{
			cv::resize(frame2, result_frame2, Size(ui.label_2->width(), ui.label_2->height()));
			image2 = QImage((const uchar*)result_frame2.data,
				result_frame2.cols,
				result_frame2.rows,
				result_frame2.cols * result_frame2.channels(),
				QImage::Format_Indexed8);
		}
		// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
		ui.label_2->setPixmap(QPixmap::fromImage(image2));    //  将图片显示到label上
		ui.label_2->resize(ui.label_2->pixmap()->size());    //  将label控件resize到fame的尺寸
	}

	QImage image3;
	if (!frame3.empty())
	{
		if (frame3.channels() == 3)    // RGB image
		{
			cv::cvtColor(frame3, result_frame3, CV_BGR2RGB);
			cv::resize(result_frame3, result_frame3, Size(ui.label_17->width(), ui.label_17->height()));
			image3 = QImage((const uchar*)result_frame3.data,
				result_frame3.cols,
				result_frame3.rows,
				result_frame3.cols * result_frame3.channels(),
				QImage::Format_RGB888);
		}
		else                     // gray image
		{
			cv::resize(frame3, result_frame3, Size(ui.label_17->width(), ui.label_17->height()));
			image3 = QImage((const uchar*)result_frame3.data,
				result_frame3.cols,
				result_frame3.rows,
				result_frame3.cols * result_frame3.channels(),
				QImage::Format_Indexed8);
		}
		// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
		ui.label_17->setPixmap(QPixmap::fromImage(image3));    //  将图片显示到label_17上
		ui.label_17->resize(ui.label_17->pixmap()->size());    //  将label_17控件resize到fame的尺寸
	}
	//}

	QImage image;
	switch (MainCameraShowMode)
	{
	case 0://第一个摄像头
		if (!frame1.empty())
		{
			if (frame1.channels() == 3)    // RGB image
			{
				cv::cvtColor(frame1, frame1, CV_BGR2RGB);
				cv::resize(frame1, frame1, Size(ui.label_3->width(), ui.label_3->height()));
				image = QImage((const uchar*)frame1.data,
					frame1.cols,
					frame1.rows,
					frame1.cols * frame1.channels(),
					QImage::Format_RGB888);
			}
			else                     // gray image
			{
				cv::resize(frame1, frame1, Size(ui.label_3->width(), ui.label_3->height()));
				image = QImage((const uchar*)frame1.data,
					frame1.cols,
					frame1.rows,
					frame1.cols * frame1.channels(),
					QImage::Format_Indexed8);
			}
			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			ui.label_3->setPixmap(QPixmap::fromImage(image));    //  将图片显示到label上
			ui.label_3->resize(ui.label_3->pixmap()->size());    //  将label控件resize到fame的尺寸
		}
		break; // 可选的

	case 1://第二个摄像头
		if (!frame2.empty())
		{
			if (frame2.channels() == 3)    // RGB image
			{
				cv::cvtColor(frame2, frame2, CV_BGR2RGB);
				cv::resize(frame2, frame2, Size(ui.label_3->width(), ui.label_3->height()));
				image = QImage((const uchar*)frame2.data,
					frame2.cols,
					frame2.rows,
					frame2.cols * frame2.channels(),
					QImage::Format_RGB888);
			}
			else                     // gray image
			{
				cv::resize(frame2, frame2, Size(ui.label_3->width(), ui.label_3->height()));
				image = QImage((const uchar*)frame2.data,
					frame2.cols,
					frame2.rows,
					frame2.cols * frame2.channels(),
					QImage::Format_Indexed8);
			}
			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			ui.label_3->setPixmap(QPixmap::fromImage(image));    //  将图片显示到label上
			ui.label_3->resize(ui.label_3->pixmap()->size());    //  将label控件resize到fame的尺寸
		}
		break; // 可选的

	case 2://第三个摄像头
		if (!frame3.empty())
		{
			if (frame3.channels() == 3)    // RGB image
			{
				cv::cvtColor(frame3, frame3, CV_BGR2RGB);
				cv::resize(frame3, frame3, Size(ui.label_3->width(), ui.label_3->height()));
				image = QImage((const uchar*)frame3.data,
					frame3.cols,
					frame3.rows,
					frame3.cols * frame3.channels(),
					QImage::Format_RGB888);
			}
			else                     // gray image
			{
				cv::resize(frame3, frame3, Size(ui.label_3->width(), ui.label_3->height()));
				image = QImage((const uchar*)frame3.data,
					frame3.cols,
					frame3.rows,
					frame3.cols * frame3.channels(),
					QImage::Format_Indexed8);
			}
			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			ui.label_3->setPixmap(QPixmap::fromImage(image));    //  将图片显示到label上
			ui.label_3->resize(ui.label_3->pixmap()->size());    //  将label控件resize到fame的尺寸
		}
		break; // 可选的

	case 3://拼接

		cv::resize(frame1, frame1, Size(), definition, definition);
		cv::resize(frame2, frame2, Size(), definition, definition);
		cv::resize(frame3, frame3, Size(), definition, definition);
		if (frame1.rows != 0 && frame2.rows != 0 && frame3.rows != 0)
		{
			picArray.push_back(frame1);
			picArray.push_back(frame2);
			picArray.push_back(frame3);
			waitKey(10);

			Mat img_warped, img_warped_s;
			Mat dilated_mask, seam_mask, mask, mask_warped;
			Ptr<Blender> blender;
			Ptr<Timelapser> timelapser;
			bool timelapse = false;
			int blend_type = Blender::MULTI_BAND;
			float blend_strength = 5;
			//int timelapse_type = Timelapser::AS_IS;
			for (int img_idx = 0; img_idx < 3; ++img_idx)
			{
				cout << "Compositing image #" << indices[img_idx] + 1 << endl;
				// Read image and resize it if necessary
				Mat K;
				cameras_inclass[img_idx].K().convertTo(K, CV_32F);
				warper->warp(picArray[img_idx], K, cameras_inclass[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);
				Size img_size = picArray[img_idx].size();
				mask.create(img_size, CV_8U);
				mask.setTo(Scalar::all(255));
				warper->warp(mask, K, cameras_inclass[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);
				compensator->apply(img_idx, corners_inclass[img_idx], img_warped, mask_warped);
				img_warped.convertTo(img_warped_s, CV_16S);

				img_warped.release();
				mask.release();
				cv::dilate(masks_warped_inclass[img_idx], dilated_mask, Mat());
				cv::resize(dilated_mask, seam_mask, mask_warped.size(), 0, 0, INTER_LINEAR_EXACT);
				mask_warped = seam_mask & mask_warped;
				if (!blender && !timelapse)
				{
					blender = Blender::createDefault(blend_type, 0);
					Size dst_sz = resultRoi(corners_inclass, sizes_inclass).size();
					float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
					if (blend_width < 1.f)
						blender = Blender::createDefault(Blender::NO, 0);
					else if (blend_type == Blender::MULTI_BAND)
					{
						MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(blender.get());
						mb->setNumBands(static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.));
						cout << "Multi-band blender, number of bands: " << mb->numBands() << endl;
					}
					else if (blend_type == Blender::FEATHER)
					{
						FeatherBlender* fb = dynamic_cast<FeatherBlender*>(blender.get());
						fb->setSharpness(1.f / blend_width);
						cout << "Feather blender, sharpness: " << fb->sharpness() << endl;
					}
					blender->prepare(corners_inclass, sizes_inclass);
				}

				img_warped_s.convertTo(img_warped_s, CV_8U);
				blender->feed(img_warped_s, mask_warped, corners_inclass[img_idx]);


			}

			Mat result, result_mask;
			blender->blend(result, result_mask);
			result.convertTo(result, CV_8U);
			Mat oldresult = result.clone();
			for (int y = 0; y < oldresult.rows; y++) {
				for (int x = 0; x < oldresult.cols; x++) {
					for (int c = 0; c < oldresult.channels(); c++) {
						result.at<Vec3b>(y, x)[c] =
							saturate_cast<uchar>(alpha * oldresult.at<Vec3b>(y, x)[c] + beta);
					}
				}
			}

			//截图
			if (ScreenShootFlag == 1) {
				imwrite(".//ScreenShoot.jpg", result);
				ui.label_12->setText("Screenshot Success!!");
				ScreenShootFlag = 0;
			}

			//目标检测
			if (movingDetectionFlag) {
				//待建设
				result = MoveDetectCamera(before, result);
			}
			//imwrite("result.jpg", result);
			//imwrite("result_mask.jpg", result_mask);

			before = result.clone();

			waitKey(1);
			picArray.clear();

			if (!result.empty())
			{

				if (result.channels() == 3)    // RGB image
				{
					cv::cvtColor(result, result, CV_BGR2RGB);
					cv::resize(result, result, Size(ui.label_3->width(), ui.label_3->height()));
					image = QImage((const uchar*)result.data,
						result.cols,
						result.rows,
						result.cols * result.channels(),
						QImage::Format_RGB888);
				}
				else                     // gray image
				{
					cv::resize(result, result, Size(ui.label_3->width(), ui.label_3->height()));
					image = QImage((const uchar*)result.data,
						result.cols,
						result.rows,
						result.cols * result.channels(),
						QImage::Format_Indexed8);
				}

				// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
				ui.label_3->setPixmap(QPixmap::fromImage(image));    //  将图片显示到label上
				ui.label_3->resize(ui.label_3->pixmap()->size());    //  将label控件resize到fame的尺寸

			}

		}
		break; // 可选的

	default:
		break;
	}



}

void QtCameraStitcherDemo::cameraLink() {
	//这里需要清除vectormat类的内存 还没有写！！！！！！

	//captureFirst.set(CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'));
	//Login("192.168.0.101", 8000, "admin", "HIKVISION610");
	//captureFirst.open("rtsp://admin:HIKVISION610@192.168.0.101/554/h264/ch1/sub/av_stream");
	//captureSecond.open("rtsp://admin:HIKVISION610@192.168.0.103/554/h264/ch1/sub/av_stream");
	//captureThird.open("rtsp://admin:HIKVISION610@192.168.0.102/554/h264/ch1/sub/av_stream");
//	if (!captureFirst.isOpened() || !captureSecond.isOpened()|| !captureThird.isOpened())
	//{
	//	cout << "Movie open Error" << endl;
	//	return;
//	}

	//double rateFirst = captureFirst.get(CAP_PROP_FPS);
	//double rateSecond = captureSecond.get(CAP_PROP_FPS);
	//cout << "帧率为:" << " " << rateFirst << endl;
	//cout << "总帧数为:" << " " << captureFirst.get(CAP_PROP_FRAME_COUNT) << endl;//输出帧总数
	//cout << "帧率为:" << " " << rateSecond << endl;
	//double position = 0.0;
	//设置播放到哪一帧，这里设置为第0帧
	//captureFirst.set(CAP_PROP_POS_FRAMES, position);
//	captureSecond.set(CAP_PROP_POS_FRAMES, position);
	//captureThird.set(CAP_PROP_POS_FRAMES, position);
	//阶段信息显示
	//ui.progressBar->setValue(10);
//	ui.label_16->setText("Progress info: Reading Camera files.");

	//captureFirst >> frame1;
	//captureSecond >> frame2;
	//captureThird >> frame3;

	//frame 是mat,把海康从这里接进来,by gsy



	//	vector<cv::Mat> srcImage = get_srcImage();
	//	frame1 = srcImage[0];
	//	frame2 = srcImage[1];
		//frame3 = srcImage[2];
		//end add by gsy
		//cout << frame1.size() << "okokok" << endl;
	//	cout << frame2.size() << endl;
		//cout << frame3.size() << endl;

	//frame 是mat,把vlc从这里接进来,by gsy
	/*

	login();

	cout << "ok1" << endl;
	int count = 3;
	vector<string>first;
	cout << "ok2" << endl;
	string firstA = "G:\\c++Projects\\Test20190907 camera\\Test20190907\\firstA.bmp";  //若要求获取用户指定的地址可以将其一对一赋值给这里的string
	string firstB = "G:\\c++Projects\\Test20190907 camera\\Test20190907\\firstB.bmp";
	string firstC = "G:\\c++Projects\\Test20190907 camera\\Test20190907\\firstC.bmp";
	string firstD = "G:\\c++Projects\\Test20190907 camera\\Test20190907\\firstD.bmp";
	string firstE = "G:\\c++Projects\\Test20190907 camera\\Test20190907\\firstE.bmp";
	cout << "ok3" << endl;
	first.push_back(firstA);
	first.push_back(firstB);
	first.push_back(firstC);
	cout << "ok4" << endl;
	first.push_back(firstD);
	first.push_back(firstE);
	for (int i = 0; i < 5 - count; i++) {

		first.pop_back();

	}
	cout << first.size() << endl;
	cout << "ok5" << endl;

	vlc_reader vlc1("rtsp://admin:HIKVISION610@192.168.0.100");
	vlc_reader vlc2("rtsp://admin:HIKVISION610@192.168.0.102");
	vlc_reader vlc3("rtsp://admin:HIKVISION610@192.168.0.103");
	vlc_reader vlc4("rtsp://admin:HIKVISION610@192.168.0.100");
	vlc_reader vlc5("rtsp://admin:HIKVISION610@192.168.0.100");

	cout << "ok6" << endl;

	cout << vlc1.frame().size() << endl;



	vector<vlc_reader*> allframe;
	allframe.push_back(&vlc1);
	allframe.push_back(&vlc2);
	allframe.push_back(&vlc3);
	allframe.push_back(&vlc4);
	allframe.push_back(&vlc5);
	cout << "ok7" << endl;
	for (int i = 0; i < 5 - count; i++) {

		allframe.pop_back();

	}
	int width = 400, height =400;
	vlc1.start(width, height);
	vlc2.start(width, height);
	vlc3.start(width, height);




	frame1 = vlc1.frame();
	frame2 = vlc2.frame();
	frame3 = vlc3.frame();
	cout << frame1.size() << endl;
	cout << frame2.size() << endl;
	cout << frame3.size() << endl;

	*/
	vlc_reader vr1("rtsp://admin:HIKVISION610@192.168.0.100");
	vlc_reader vr2("rtsp://admin:HIKVISION610@192.168.0.102");
	vlc_reader vr3("rtsp://admin:HIKVISION610@192.168.0.103");
	int width = 640, height = 384;
	vr1.start(width, height);
	vr2.start(width, height);
	vr3.start(width, height);
	ui.progressBar->setValue(10);
	ui.label_16->setText("Progress info: Reading Camera files.");
	while (1) {
		frame1 = vr1.frame();
		waitKey(10);
		frame2 = vr2.frame();
		waitKey(10);
		frame3 = vr3.frame();
		waitKey(10);

		//cout << frame1.size() << endl;
		//cout << frame2.size() << endl;
	//	cout << frame3.size() << endl;


		/*


		if (!frame1.empty())
		{
			imshow("test", frame1);
			waitKey(1);

		}
		if (!frame2.empty())
		{
			imshow("test2", frame2);
			waitKey(1);
		}
		if (!frame3.empty())
		{
			imshow("test3", frame3);
			waitKey(1);
		}
		*/
		//}//while的括号

			//
		//if (!frame1.empty() && !frame2.empty() && !frame3.empty()) 
		if (frame1.rows != 0 && frame2.rows != 0 && frame3.rows != 0)
		{
			//cv::resize(frame1, frame1, Size(), definition, definition);
		//	cv::resize(frame2, frame2, Size(), definition, definition);
			//cv::resize(frame3, frame3, Size(), definition, definition);

			picArray.push_back(frame1);
			picArray.push_back(frame2);
			picArray.push_back(frame3);
			cv::waitKey(0);
			//Ptr<Feature2D> finder;
			//~~~~~~~~~~~~~~改为可选-----改完了
			//finder = xfeatures2d::SURF::create();//ORB or SURF or AKAZE or SIFT
			int num_images = picArray.size();
			vector<ImageFeatures> features(num_images);
			for (int i = 0; i < num_images; ++i) {
				cout << finder << endl;
				cout << picArray[i] << endl;
				computeImageFeatures(finder, picArray[i], features[i]);
			}
			//阶段信息显示
			ui.progressBar->setValue(30);
			ui.label_16->setText("Progress info: Computing features.");
			vector<MatchesInfo> pairwise_matches;
			Ptr<FeaturesMatcher> matcher;
			//~~~~~~~~~~~~~~改为可改---改完了
			//float match_conf = 0.3f;//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			matcher = makePtr<AffineBestOf2NearestMatcher>(false, 0, match_conf);
			// AffineBestOf2NearestMatcher BestOf2NearestMatcher BestOf2NearestRangeMatcher

			(*matcher)(features, pairwise_matches);
			matcher->collectGarbage();
			//~~~~~~~~~~~~~~改为可改---改完了
			//float conf_thresh = 0.5f;//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			indices = leaveBiggestComponent(features, pairwise_matches, conf_thresh);
			for (int i = 0; i < indices.size(); ++i) {
				cout << indices[i] << endl;
			}

			Ptr<Estimator> estimator;
			estimator = makePtr<AffineBasedEstimator>();

			vector<CameraParams> cameras;
			if (!(*estimator)(features, pairwise_matches, cameras))
			{
				cout << "Homography estimation failed.\n";
			}

			//阶段信息显示
			ui.progressBar->setValue(50);
			ui.label_16->setText("Progress info: Computing Camera parameters.");

			for (size_t i = 0; i < cameras.size(); ++i)
			{
				Mat R;
				cameras[i].R.convertTo(R, CV_32F);
				cameras[i].R = R;
				cout << "Initial camera intrinsics #" << indices[i] + 1 << ":\nK:\n" << cameras[i].K() << "\nR:\n" << cameras[i].R << endl;
			}
			//Ptr<detail::BundleAdjusterBase> adjuster;
			//~~~~~~~~~~~需要修改---改完了
			//adjuster = makePtr<detail::NoBundleAdjuster>();
			//detail::BundleAdjusterReproj  detail::BundleAdjusterRay  detail::BundleAdjusterAffinePartial NoBundleAdjuster
			adjuster->setConfThresh(conf_thresh);

			string ba_refine_mask = "xxxxx";
			Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
			if (ba_refine_mask[0] == 'x') refine_mask(0, 0) = 1;
			if (ba_refine_mask[1] == 'x') refine_mask(0, 1) = 1;
			if (ba_refine_mask[2] == 'x') refine_mask(0, 2) = 1;
			if (ba_refine_mask[3] == 'x') refine_mask(1, 1) = 1;
			if (ba_refine_mask[4] == 'x') refine_mask(1, 2) = 1;
			adjuster->setRefinementMask(refine_mask);
			if (!(*adjuster)(features, pairwise_matches, cameras))
			{
				cout << "Camera parameters adjusting failed.\n";
			}
			vector<double> focals;
			for (size_t i = 0; i < cameras.size(); ++i)
			{
				cout << "Camera #" << indices[i] + 1 << ":\nK:\n" << cameras[i].K() << "\nR:\n" << cameras[i].R << endl;
				focals.push_back(cameras[i].focal);
			}
			sort(focals.begin(), focals.end());
			float warped_image_scale;
			if (focals.size() % 2 == 1)
				warped_image_scale = static_cast<float>(focals[focals.size() / 2]);
			else
				warped_image_scale = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;




			vector<Point> corners(num_images);
			vector<UMat> masks_warped(num_images);
			vector<UMat> images_warped(num_images);
			vector<Size> sizes(num_images);
			vector<UMat> masks(num_images);
			double seam_work_aspect = 1;
			for (int i = 0; i < num_images; ++i)
			{
				masks[i].create(picArray[i].size(), CV_8U);
				masks[i].setTo(Scalar::all(255));
			}


			//阶段信息显示
			ui.progressBar->setValue(80);
			ui.label_16->setText("Progress info: Computing Warper parameters.");

			warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));
			for (int i = 0; i < num_images; ++i)
			{
				Mat_<float> K;
				cameras[i].K().convertTo(K, CV_32F);
				float swa = (float)seam_work_aspect;
				K(0, 0) *= swa; K(0, 2) *= swa;
				K(1, 1) *= swa; K(1, 2) *= swa;
				corners[i] = warper->warp(picArray[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
				sizes[i] = images_warped[i].size();
				warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]);
			}
			vector<UMat> images_warped_f(num_images);
			for (int i = 0; i < num_images; ++i) {
				images_warped[i].convertTo(images_warped_f[i], CV_32F);
				//imshow(to_string(i), images_warped[i]);
			}

			int expos_comp_nr_feeds = 1;
			int expos_comp_nr_filtering = 2;
			int expos_comp_block_size = 32;
			int expos_comp_type = ExposureCompensator::GAIN_BLOCKS;

			compensator = ExposureCompensator::createDefault(expos_comp_type);
			if (dynamic_cast<GainCompensator*>(compensator.get()))
			{
				GainCompensator* gcompensator = dynamic_cast<GainCompensator*>(compensator.get());
				gcompensator->setNrFeeds(expos_comp_nr_feeds);
			}
			if (dynamic_cast<ChannelsCompensator*>(compensator.get()))
			{
				ChannelsCompensator* ccompensator = dynamic_cast<ChannelsCompensator*>(compensator.get());
				ccompensator->setNrFeeds(expos_comp_nr_feeds);
			}
			if (dynamic_cast<BlocksCompensator*>(compensator.get()))
			{
				BlocksCompensator* bcompensator = dynamic_cast<BlocksCompensator*>(compensator.get());
				bcompensator->setNrFeeds(expos_comp_nr_feeds);
				bcompensator->setNrGainsFilteringIterations(expos_comp_nr_filtering);
				bcompensator->setBlockSize(expos_comp_block_size, expos_comp_block_size);
			}
			compensator->feed(corners, images_warped, masks_warped);
			for (int i = 0; i < num_images; ++i) {
				images_warped[i].convertTo(images_warped_f[i], CV_32F);
				//imshow(to_string(i), images_warped[i]);
			}

			Ptr<SeamFinder> seam_finder;
			//seam_finder = makePtr<detail::NoSeamFinder>();
			seam_finder = new detail::GraphCutSeamFinder(GraphCutSeamFinderBase::COST_COLOR);
			if (!seam_finder)
			{
				cout << "Can't create the following seam finder '" << "'\n";
			}
			seam_finder->find(images_warped_f, corners, masks_warped);
			corners_inclass = corners;
			masks_warped_inclass = masks_warped;
			sizes_inclass = sizes;
			cameras_inclass = cameras;
			images_warped.clear();
			images_warped_f.clear();
			masks.clear();
			picArray.clear();
			timer->start(25);
			//阶段信息显示
			ui.progressBar->setValue(100);
			ui.label_16->setText("Progress info: Completed");

		}
	}



}

void QtCameraStitcherDemo::changeMainCameraShowMode(int index)
{
	MainCameraShowMode = index;
	cout << "MainCameraShowMode:" << index << endl;
}

void QtCameraStitcherDemo::setScreenShootFlag()
{
	ScreenShootFlag = 1;
}

void QtCameraStitcherDemo::setBrightnessAdjustment(int A)
{
	beta = 0;
	beta = A + beta;
}

void QtCameraStitcherDemo::setContrastAdjustment(int A)
{
	alpha = 1;
	alpha = float(A - 50) / 100 + alpha;
}

void QtCameraStitcherDemo::setDefinition(int definitionMode)
{
	switch (definitionMode)
	{
	case 0:
		definition = 0.1;
		break;
	case 1:
		definition = 0.3;
		break;
	case 2:
		definition = 0.5;
		break;
	default:
		break;
	}
}

void QtCameraStitcherDemo::setmovingDetectionFlag()
{
	movingDetectionFlag = !movingDetectionFlag;
}

void QtCameraStitcherDemo::setAdjuster(int A) {
	switch (A) {
	case 0:
		adjuster = makePtr<detail::NoBundleAdjuster>();
		cout << "adjuster:NoBundleAdjuster" << endl;
		break;
	case 1:
		adjuster = makePtr<detail::BundleAdjusterReproj>();
		cout << "adjuster:BundleAdjusterReproj" << endl;
		break;
	case 2:
		adjuster = makePtr<detail::BundleAdjusterRay>();
		cout << "adjuster:BundleAdjusterRay" << endl;
		break;
	case 3:
		adjuster = makePtr<detail::BundleAdjusterAffinePartial>();
		cout << "adjuster:BundleAdjusterAffinePartial" << endl;
		break;
	default:
		break;

	}

}

void QtCameraStitcherDemo::setWarper_creator(QString warp_type) {
	if (warp_type == "PlaneWarper") {
		warper_creator = makePtr<cv::PlaneWarper>();
	}

	else if (warp_type == "AffineWarper")
		warper_creator = makePtr<cv::AffineWarper>();
	else if (warp_type == "CylindricalWarper")
		warper_creator = makePtr<cv::CylindricalWarper>();
	else if (warp_type == "SphericalWarper")
		warper_creator = makePtr<cv::SphericalWarper>();
	else if (warp_type == "FisheyeWarper")
		warper_creator = makePtr<cv::FisheyeWarper>();
	else if (warp_type == "StereographicWarper")
		warper_creator = makePtr<cv::StereographicWarper>();
	else if (warp_type == "CompressedRectilinearWarper_higher")
		warper_creator = makePtr<cv::CompressedRectilinearWarper>(2.0f, 1.0f);
	else if (warp_type == "CompressedRectilinearWarper_lower")
		warper_creator = makePtr<cv::CompressedRectilinearWarper>(1.5f, 1.0f);
	else if (warp_type == "CompressedRectilinearPortraitWarper_higher")
		warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(2.0f, 1.0f);
	else if (warp_type == "CompressedRectilinearPortraitWarper_lower")
		warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(1.5f, 1.0f);
	else if (warp_type == "PaniniWarper_higher")
		warper_creator = makePtr<cv::PaniniWarper>(2.0f, 1.0f);
	else if (warp_type == "PaniniWarper_lower")
		warper_creator = makePtr<cv::PaniniWarper>(1.5f, 1.0f);
	else if (warp_type == "PaniniPortraitWarper_higher")
		warper_creator = makePtr<cv::PaniniPortraitWarper>(2.0f, 1.0f);
	else if (warp_type == "PaniniPortraitWarper_lower")
		warper_creator = makePtr<cv::PaniniPortraitWarper>(1.5f, 1.0f);
	else if (warp_type == "MercatorWarper")
		warper_creator = makePtr<cv::MercatorWarper>();
	else if (warp_type == "TransverseMercatorWarper")
		warper_creator = makePtr<cv::TransverseMercatorWarper>();

	if (!warper_creator) {
		cout << "Can't create the following warper '" << "'\n";
	}

}

void QtCameraStitcherDemo::setFinder(int A) {
	switch (A) {
	case 0:
		finder = xfeatures2d::SURF::create();
		cout << "Finder:SURF" << endl;
		break;
	case 1:
		finder = xfeatures2d::SIFT::create();
		cout << "Finder:SIFT" << endl;
		break;
	case 2:
		finder = AKAZE::create();
		cout << "Finder:AKAZE" << endl;
		break;
	case 3:
		finder = ORB::create();
		cout << "Finder:ORB" << endl;
		break;
	default:
		break;
	}
}

void QtCameraStitcherDemo::setMatch_conf(QString F) {
	float f = F.toFloat();
	match_conf = f;
	cout << "match_conf:" << match_conf << endl;
}

void QtCameraStitcherDemo::setConf_thresh(QString F) {
	float f = F.toFloat();
	conf_thresh = f;
	cout << "conf_thresh:" << conf_thresh << endl;
}

void QtCameraStitcherDemo::displayAll() {


	// Default command line args
	vector<String> img_names;
	bool preview = false;
	bool try_cuda = false;
	bool try_gpu = true;
	double work_megapix = 0.6;
	double seam_megapix = 0.1;
	double compose_megapix = 0.15;
	float conf_thresh = 0.8f;
	string features_type = "orb";
	string matcher_type = "homography";
	string estimator_type = "homography";
	string ba_cost_func = "ray";
	string ba_refine_mask = "xxxxx";
	bool do_wave_correct = true;
	WaveCorrectKind wave_correct = detail::WAVE_CORRECT_HORIZ;
	bool save_graph = true;
	std::string save_graph_to;
	string warp_type = "spherical";
	int expos_comp_type = ExposureCompensator::GAIN_BLOCKS;
	float match_conf = 0.3f;
	string seam_find_type = "gc_color";
	int blend_type = Blender::MULTI_BAND;
	int timelapse_type = Timelapser::AS_IS;
	float blend_strength = 5;
	string result_name = "result.jpg";
	bool timelapse = false;
	int range_width = 9;
	bool capture1;
	bool capture2;
	bool capture3;
	Mat before;
	Mat videobuf;


	Mat rimg1;
	Mat rimg2;
	Mat rimg3;
	Mat rimg4;
	login();
	/*
	
	char firstA[] = "G:\\c++Projects\\stitch420\\firstA.bmp";
	char firstB[] = "G:\\c++Projects\\stitch420\\firstB.bmp";
	char firstC[] = "G:\\c++Projects\\stitch420\\firstC.bmp";*/
	/*

	NET_DVR_CapturePicture(lRealPlayHandle1, firstA);
	NET_DVR_CapturePicture(lRealPlayHandle2, firstB);
	NET_DVR_CapturePicture(lRealPlayHandle3, firstC);
	Sleep(300);
*/
	bool stop(false);
	Mat frame1; //定义一个Mat变量，用于存储每一帧的图像
	Mat frame2; //定义一个Mat变量，用于存储每一帧的图像
	Mat frame3; //定义一个Mat变量，用于存储每一帧的图像
	Mat frame4;

	Mat frame;//合并

	Mat rframe1;
	Mat rframe2;
	Mat rframe3; //定义一个Mat变量，用于存储每一帧的图像
	Mat rframe4;

	Mat cap1;
	Mat cap2;
	Mat cap3;
	Mat cap4;
	int k = 100;

	namedWindow("stitch", CV_WINDOW_AUTOSIZE);

#if ENABLE_LOG
	int64 app_start_time = getTickCount();
#endif

#if 0
	cv::setBreakOnError(true);
#endif

	String path1 = "G:\\c++Projects\\QtOpencvSet\\QtOpencvSet\\firstA.bmp";
	String path2 = "G:\\c++Projects\\QtOpencvSet\\QtOpencvSet\\firstB.bmp";
	String path3 = "G:\\c++Projects\\QtOpencvSet\\QtOpencvSet\\firstC.bmp";
	//String path4 = "G:\\c++Projects\\QtOpencvSet\\QtOpencvSet\\suitD.bmp";
	String path4 = "C:\\Users\\DELL\\Desktop\\rimg2.bmp";

	cap1 = imread(path1);
	cap2 = imread(path2);
	cap3 = imread(path3);
	cap4 = imread(path4);



	cv::resize(cap1, rimg1, Size(640, 384), 0, 0, CV_INTER_AREA);
	cv::resize(cap2, rimg2, Size(640, 384), 0, 0, CV_INTER_AREA);
	cv::resize(cap3, rimg3, Size(640, 384), 0, 0, CV_INTER_AREA);
	cv::resize(cap4, rimg4, Size(640, 384), 0, 0, CV_INTER_AREA);

	imwrite("rimg1.bmp", rimg1);
	imwrite("rimg2.bmp", rimg2);
	imwrite("rimg3.bmp", rimg3);
	imwrite("rimg4.bmp", rimg4);

	img_names.push_back("rimg1.bmp");
	img_names.push_back("rimg2.bmp");
	img_names.push_back("rimg3.bmp");
	img_names.push_back("rimg4.bmp");

	int num_images = static_cast<int>(img_names.size());
	//cout << "num_images111" << num_images << endl;
	if (num_images < 2)
	{
		LOGLN("Need more images");

	}

	double work_scale = 1, seam_scale = 1, compose_scale = 1;
	bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;

	LOGLN("Finding features...");
#if ENABLE_LOG
	int64 t = getTickCount();
#endif

	Ptr<Feature2D> finder;
	if (features_type == "orb")
	{
		finder = ORB::create();
	}
	else if (features_type == "sift") {
		finder = xfeatures2d::SIFT::create();
	}
	else
	{
		cout << "Unknown 2D features type: '" << features_type << "'.\n";
		//return -1;
	}

	Mat full_img, img;
	vector<ImageFeatures> features(num_images);
	vector<Mat> images(num_images);
	vector<Size> full_img_sizes(num_images);
	double seam_work_aspect = 1;

	for (int i = 0; i < num_images; ++i)
	{
		full_img = imread(samples::findFile(img_names[i]));
		full_img_sizes[i] = full_img.size();

		if (full_img.empty())
		{
			LOGLN("Can't open image " << img_names[i]);
			//return -1;
		}
		if (work_megapix < 0)
		{
			img = full_img;
			work_scale = 1;
			is_work_scale_set = true;
		}
		else
		{
			if (!is_work_scale_set)
			{
				work_scale = min(1.0, sqrt(work_megapix * 1e6 / full_img.size().area()));
				is_work_scale_set = true;
			}
			cv::resize(full_img, img, Size(), work_scale, work_scale, INTER_LINEAR_EXACT);
		}
		if (!is_seam_scale_set)
		{
			seam_scale = min(1.0, sqrt(seam_megapix * 1e6 / full_img.size().area()));
			seam_work_aspect = seam_scale / work_scale;
			is_seam_scale_set = true;
		}

		for (int i = 0; i < num_images; ++i) {
			
			computeImageFeatures(finder, images[i], features[i]);
		}


		features[i].img_idx = i;
		LOGLN("Features in image #" << i + 1 << ": " << features[i].keypoints.size());
		cv::resize(full_img, img, Size(), seam_scale, seam_scale, INTER_LINEAR_EXACT);
		images[i] = img.clone();
	}

	//finder->collectGarbage();
	full_img.release();
	img.release();
	cout << "num_images_"<<num_images << endl;
	cout << "Finding features, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec" << endl;

	cout << ("Pairwise matching") << endl;
#if ENABLE_LOG
	t = getTickCount();
#endif
	vector<MatchesInfo> pairwise_matches;
	Ptr<FeaturesMatcher> matcher;
	if (matcher_type == "affine")
		matcher = makePtr<AffineBestOf2NearestMatcher>(false, try_cuda, match_conf);
	else if (range_width == -1)
		matcher = makePtr<BestOf2NearestMatcher>(try_cuda, match_conf);
	else
		matcher = makePtr<BestOf2NearestRangeMatcher>(range_width, try_cuda, match_conf);

	(*matcher)(features, pairwise_matches);
	
	matcher->collectGarbage();

	cout << ("Pairwise matching, time: ") << ((getTickCount() - t) / getTickFrequency()) << " sec" << endl;

	// Check if we should save matches graph
	if (save_graph)
	{
		LOGLN("Saving matches graph...");
		ofstream f(save_graph_to.c_str());
		f << matchesGraphAsString(img_names, pairwise_matches, conf_thresh);
	}

	cout << "pairwise_matches.size() " << pairwise_matches.size() << endl;
	cout << "features.size() " << features.size() << endl;
	// Leave only images we are sure are from the same panorama
	vector<int> indices = leaveBiggestComponent(features, pairwise_matches, conf_thresh);
	cout << "pairwise_matches.size() " << pairwise_matches.size() << endl;
	cout << "features.size() " << features.size() << endl;

	vector<Mat> img_subset;
	vector<String> img_names_subset;
	vector<Size> full_img_sizes_subset;
	cout << "indices.size()" << indices.size() << endl;

	for (size_t i = 0; i < indices.size(); ++i)
	{
		img_names_subset.push_back(img_names[indices[i]]);
		img_subset.push_back(images[indices[i]]);
		full_img_sizes_subset.push_back(full_img_sizes[indices[i]]);
	}

	images = img_subset;
	img_names = img_names_subset;
	full_img_sizes = full_img_sizes_subset;

	// Check if we still have enough images
	num_images = static_cast<int>(img_names.size());
	cout << " num_images"<<num_images << endl;
	if (num_images < 2)
	{
		LOGLN("Need more images");
		//return -1;
	}

	Ptr<Estimator> estimator;
	if (estimator_type == "affine")
		estimator = makePtr<AffineBasedEstimator>();
	else
		estimator = makePtr<HomographyBasedEstimator>();

	vector<CameraParams> cameras;
	if (!(*estimator)(features, pairwise_matches, cameras))
	{
		cout << "Homography estimation failed.\n" << endl;
		//return -1;
	}
	cout << " cameras.size" << cameras.size() << endl;
	for (size_t i = 0; i < cameras.size(); ++i)
	{
		Mat R;
		cameras[i].R.convertTo(R, CV_32F);
		cameras[i].R = R;
		cout << ("Initial camera intrinsics #") << indices[i] + 1 << ":\n" << cameras[i].K() << cameras[i].R << endl;
	}

	Ptr<detail::BundleAdjusterBase> adjuster;
	if (ba_cost_func == "reproj") adjuster = makePtr<detail::BundleAdjusterReproj>();
	else if (ba_cost_func == "ray") adjuster = makePtr<detail::BundleAdjusterRay>();
	else if (ba_cost_func == "affine") adjuster = makePtr<detail::BundleAdjusterAffinePartial>();
	else if (ba_cost_func == "no") adjuster = makePtr<NoBundleAdjuster>();
	else
	{
		cout << "Unknown bundle adjustment cost function: '" << ba_cost_func << "'.\n";
		//return -1;
	}
	adjuster->setConfThresh(conf_thresh);
	Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
	if (ba_refine_mask[0] == 'x') refine_mask(0, 0) = 1;
	if (ba_refine_mask[1] == 'x') refine_mask(0, 1) = 1;
	if (ba_refine_mask[2] == 'x') refine_mask(0, 2) = 1;
	if (ba_refine_mask[3] == 'x') refine_mask(1, 1) = 1;
	if (ba_refine_mask[4] == 'x') refine_mask(1, 2) = 1;
	adjuster->setRefinementMask(refine_mask);
	(*adjuster)(features, pairwise_matches, cameras);


	// Find median focal length
	vector<double> focals;
	for (size_t i = 0; i < cameras.size(); ++i)
	{
		cout << ("Camera #") << indices[i] + 1 << ":\n" << cameras[i].K() << endl;
		focals.push_back(cameras[i].focal);
	}

	sort(focals.begin(), focals.end());
	float warped_image_scale;
	if (focals.size() % 2 == 1)
		warped_image_scale = static_cast<float>(focals[focals.size() / 2]);
	else
		warped_image_scale = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;

	if (do_wave_correct)
	{
		vector<Mat> rmats;
		for (size_t i = 0; i < cameras.size(); ++i)
			rmats.push_back(cameras[i].R.clone());
		waveCorrect(rmats, wave_correct);
		for (size_t i = 0; i < cameras.size(); ++i)
			cameras[i].R = rmats[i];
	}

	cout << ("Warping images (auxiliary)... ") << endl;
#if ENABLE_LOG
	t = getTickCount();
#endif

	vector<Point> corners(num_images);
	vector<UMat> masks_warped(num_images);
	vector<UMat> images_warped(num_images);
	vector<Size> sizes(num_images);
	vector<UMat> masks(num_images);

	// Preapre images masks
	for (int i = 0; i < num_images; ++i)
	{
		masks[i].create(images[i].size(), CV_8U);
		masks[i].setTo(Scalar::all(255));
	}

	// Warp images and their masks

	Ptr<WarperCreator> warper_creator;
#ifdef HAVE_OPENCV_CUDAWARPING
	if (try_cuda && cuda::getCudaEnabledDeviceCount() > 0)
	{
		if (warp_type == "plane")
			warper_creator = makePtr<cv::PlaneWarperGpu>();
		else if (warp_type == "cylindrical")
			warper_creator = makePtr<cv::CylindricalWarperGpu>();
		else if (warp_type == "spherical")
			warper_creator = makePtr<cv::SphericalWarperGpu>();
}
	else
#endif
	{
		if (warp_type == "plane")
			warper_creator = makePtr<cv::PlaneWarper>();
		else if (warp_type == "affine")
			warper_creator = makePtr<cv::AffineWarper>();
		else if (warp_type == "cylindrical")
			warper_creator = makePtr<cv::CylindricalWarper>();
		else if (warp_type == "spherical")
			warper_creator = makePtr<cv::SphericalWarper>();
		else if (warp_type == "fisheye")
			warper_creator = makePtr<cv::FisheyeWarper>();
		else if (warp_type == "stereographic")
			warper_creator = makePtr<cv::StereographicWarper>();
		else if (warp_type == "compressedPlaneA2B1")
			warper_creator = makePtr<cv::CompressedRectilinearWarper>(2.0f, 1.0f);
		else if (warp_type == "compressedPlaneA1.5B1")
			warper_creator = makePtr<cv::CompressedRectilinearWarper>(1.5f, 1.0f);
		else if (warp_type == "compressedPlanePortraitA2B1")
			warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(2.0f, 1.0f);
		else if (warp_type == "compressedPlanePortraitA1.5B1")
			warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(1.5f, 1.0f);
		else if (warp_type == "paniniA2B1")
			warper_creator = makePtr<cv::PaniniWarper>(2.0f, 1.0f);
		else if (warp_type == "paniniA1.5B1")
			warper_creator = makePtr<cv::PaniniWarper>(1.5f, 1.0f);
		else if (warp_type == "paniniPortraitA2B1")
			warper_creator = makePtr<cv::PaniniPortraitWarper>(2.0f, 1.0f);
		else if (warp_type == "paniniPortraitA1.5B1")
			warper_creator = makePtr<cv::PaniniPortraitWarper>(1.5f, 1.0f);
		else if (warp_type == "mercator")
			warper_creator = makePtr<cv::MercatorWarper>();
		else if (warp_type == "transverseMercator")
			warper_creator = makePtr<cv::TransverseMercatorWarper>();
	}

	if (!warper_creator)
	{
		cout << "Can't create the following warper '" << warp_type << "'\n";
		//return 1;
	}

	Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));

	for (int i = 0; i < num_images; ++i)
	{
		Mat_<float> K;
		cameras[i].K().convertTo(K, CV_32F);
		float swa = (float)seam_work_aspect;
		K(0, 0) *= swa; K(0, 2) *= swa;
		K(1, 1) *= swa; K(1, 2) *= swa;

		corners[i] = warper->warp(images[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
		sizes[i] = images_warped[i].size();

		warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]);
	}

	vector<UMat> images_warped_f(num_images);
	for (int i = 0; i < num_images; ++i)
		images_warped[i].convertTo(images_warped_f[i], CV_32F);

	LOGLN("Warping images, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");

	Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(expos_comp_type);
	compensator->feed(corners, images_warped, masks_warped);

	Ptr<SeamFinder> seam_finder;
	if (seam_find_type == "no")
		seam_finder = makePtr<detail::NoSeamFinder>();
	else if (seam_find_type == "voronoi")
		seam_finder = makePtr<detail::VoronoiSeamFinder>();
	else if (seam_find_type == "gc_color")
	{
#ifdef HAVE_OPENCV_CUDALEGACY
		if (try_cuda && cuda::getCudaEnabledDeviceCount() > 0)
			seam_finder = makePtr<detail::GraphCutSeamFinderGpu>(GraphCutSeamFinderBase::COST_COLOR);
		else
#endif
			seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR);
	}
	else if (seam_find_type == "gc_colorgrad")
	{
#ifdef HAVE_OPENCV_CUDALEGACY
		if (try_cuda && cuda::getCudaEnabledDeviceCount() > 0)
			seam_finder = makePtr<detail::GraphCutSeamFinderGpu>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
		else
#endif
			seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
	}
	else if (seam_find_type == "dp_color")
		seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR);
	else if (seam_find_type == "dp_colorgrad")
		seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR_GRAD);
	if (!seam_finder)
	{
		cout << "Can't create the following seam finder '" << seam_find_type << "'\n";
		//return 1;
	}

	seam_finder->find(images_warped_f, corners, masks_warped);

	// Release unused memory
	images.clear();
	images_warped.clear();
	images_warped_f.clear();
	masks.clear();

	//实时拼接
	vlc_reader vr1((char*)"rtsp://admin:HIKVISION610@192.168.0.100");
	vlc_reader vr2((char*)"rtsp://admin:HIKVISION610@192.168.0.102");
	vlc_reader vr3((char*)"rtsp://admin:HIKVISION610@192.168.0.103");
	int width = 640, height = 384;
	vr1.start(width, height);
	vr2.start(width, height);
	vr3.start(width, height);
	/*std::promise<vlc_reader> pr1;
	std::future<vlc_reader> fu1 = pr1.get_future();
	std::promise<vlc_reader> pr2;
	std::future<vlc_reader> fu2 = pr2.get_future();
	std::thread th1(std::ref(pr1));
	th1.join();
	std::thread th2(std::ref(pr2));
	th2.join();*/
	//frame1 = fu1.get();
	//frame2 = fu2.get();

	QImage image1, image2, image3, image4, image;





	ui.progressBar->setValue(10);
	ui.label_16->setText("Progress info: Reading Camera files.");
	//阶段信息显示
	ui.progressBar->setValue(30);
	ui.label_16->setText("Progress info: Computing features.");
	//阶段信息显示
	ui.progressBar->setValue(50);
	ui.label_16->setText("Progress info: Computing Camera parameters.");
	//阶段信息显示
	ui.progressBar->setValue(80);
	ui.label_16->setText("Progress info: Computing Warper parameters.");

	//imshow("test", frame1);

	frame4 = cv::imread(path4, cv::IMREAD_UNCHANGED);
	cv::cvtColor(frame4, result_frame4, CV_BGR2RGB);
	cv::resize(result_frame4, result_frame4, Size(ui.label_19->width(), ui.label_19->height()));
	image4 = QImage((const uchar*)result_frame4.data,
		result_frame4.cols,
		result_frame4.rows,
		result_frame4.cols * result_frame4.channels(),
		QImage::Format_RGB888);
	// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
	ui.label_19->setPixmap(QPixmap::fromImage(image4));    //  将图片显示到label上
	ui.label_19->resize(ui.label_19->pixmap()->size());    //  将label控件resize到fame的尺寸


	while (!stop)
	{
#if ENABLE_LOG
		t = getTickCount();
#endif


		frame1 = vr1.frame();
		waitKey(10);
		frame2 = vr2.frame();
		waitKey(10);
		frame3 = vr3.frame();
		waitKey(10);

		if (!frame1.empty())
		{
			//imshow("test", frame1);
			cv::cvtColor(frame1, result_frame1, CV_BGR2RGB);
			cv::resize(result_frame1, result_frame1, Size(ui.label->width(), ui.label->height()));
			image1 = QImage((const uchar*)result_frame1.data,
				result_frame1.cols,
				result_frame1.rows,
				result_frame1.cols * result_frame1.channels(),
				QImage::Format_RGB888);
			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			ui.label->setPixmap(QPixmap::fromImage(image1));    //  将图片显示到label上
			ui.label->resize(ui.label->pixmap()->size());    //  将label控件resize到fame的尺寸


			waitKey(1);

		}
		if (!frame2.empty())
		{
			//imshow("test2", frame2);
			cv::cvtColor(frame2, result_frame2, CV_BGR2RGB);
			cv::resize(result_frame2, result_frame2, Size(ui.label_2->width(), ui.label_2->height()));
			image2 = QImage((const uchar*)result_frame2.data,
				result_frame2.cols,
				result_frame2.rows,
				result_frame2.cols * result_frame2.channels(),
				QImage::Format_RGB888);
			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			ui.label_2->setPixmap(QPixmap::fromImage(image2));    //  将图片显示到label_2上
			ui.label_2->resize(ui.label_2->pixmap()->size());    //  将label控件resize到fame的尺寸
			waitKey(1);

		}
		if (!frame3.empty())
		{
			//imshow("test3", frame3);
			cv::cvtColor(frame3, result_frame3, CV_BGR2RGB);
			cv::resize(result_frame3, result_frame3, Size(ui.label_17->width(), ui.label_17->height()));
			image3 = QImage((const uchar*)result_frame3.data,
				result_frame3.cols,
				result_frame3.rows,
				result_frame3.cols * result_frame3.channels(),
				QImage::Format_RGB888);
			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			ui.label_17->setPixmap(QPixmap::fromImage(image3));    //  将图片显示到label上
			ui.label_17->resize(ui.label_17->pixmap()->size());    //  将label控件resize到fame的尺寸

			waitKey(1);

		}

		if (frame1.rows != 0 && frame2.rows != 0 && frame3.rows != 0 && frame4.rows != 0)
		{
			cv::resize(frame1, rframe1, Size(640, 384), 0, 0, CV_INTER_AREA);
			cv::resize(frame2, rframe2, Size(640, 384), 0, 0, CV_INTER_AREA);
			cv::resize(frame3, rframe3, Size(640, 384), 0, 0, CV_INTER_AREA);
			cv::resize(frame4, rframe4, Size(640, 384), 0, 0, CV_INTER_AREA);

			waitKey(30);
			cout << "Compositing..." << endl;
#if ENABLE_LOG
			t = getTickCount();
#endif

			Mat img_warped, img_warped_s;
			Mat dilated_mask, seam_mask, mask, mask_warped;
			Ptr<Blender> blender;
			//Ptr<Timelapser> timelapser;
			//double compose_seam_aspect = 1;
			double compose_work_aspect = 1;


			vector<Mat> img_set;


			img_set.push_back(rframe1);
			img_set.push_back(rframe2);
			img_set.push_back(rframe3);
			img_set.push_back(rframe4);

			for (int img_idx = 0; img_idx < num_images; ++img_idx)
			{
				LOGLN("Compositing image #" << indices[img_idx] + 1);

				// Read image and resize it if necessary
				full_img = img_set[img_idx];
				if (!is_compose_scale_set)
				{
					if (compose_megapix > 0)
						compose_scale = min(1.0, sqrt(compose_megapix * 1e6 / full_img.size().area()));
					is_compose_scale_set = true;

					// Compute relative scales
					//compose_seam_aspect = compose_scale / seam_scale;
					compose_work_aspect = compose_scale / work_scale;

					// Update warped image scale
					warped_image_scale *= static_cast<float>(compose_work_aspect);
					warper = warper_creator->create(warped_image_scale);

					// Update corners and sizes
					for (int i = 0; i < num_images; ++i)
					{
						// Update intrinsics
						cameras[i].focal *= compose_work_aspect;
						cameras[i].ppx *= compose_work_aspect;
						cameras[i].ppy *= compose_work_aspect;

						// Update corner and size
						Size sz = full_img_sizes[i];
						if (std::abs(compose_scale - 1) > 1e-1)
						{
							sz.width = cvRound(full_img_sizes[i].width * compose_scale);
							sz.height = cvRound(full_img_sizes[i].height * compose_scale);
						}

						Mat K;
						cameras[i].K().convertTo(K, CV_32F);
						Rect roi = warper->warpRoi(sz, K, cameras[i].R);
						corners[i] = roi.tl();
						sizes[i] = roi.size();
					}
				}
				if (abs(compose_scale - 1) > 1e-1)
					cv::resize(full_img, img, Size(), compose_scale, compose_scale, INTER_LINEAR_EXACT);
				else
					img = full_img;
				full_img.release();
				Size img_size = img.size();



				Mat K;
				cameras[img_idx].K().convertTo(K, CV_32F);

				// Warp the current image
				warper->warp(img, K, cameras[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);

				// Warp the current image mask
				mask.create(img_size, CV_8U);
				mask.setTo(Scalar::all(255));
				warper->warp(mask, K, cameras[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);

				// Compensate exposure
				compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);

				img_warped.convertTo(img_warped_s, CV_16S);
				img_warped.release();
				img.release();
				mask.release();

				dilate(masks_warped[img_idx], dilated_mask, Mat());
				cv::resize(dilated_mask, seam_mask, mask_warped.size(), 0, 0, INTER_LINEAR_EXACT);
				mask_warped = seam_mask & mask_warped;

				if (blender.empty())
				{
					blender = Blender::createDefault(blend_type, try_gpu);
					Size dst_sz = resultRoi(corners, sizes).size();
					float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
					if (blend_width < 1.f)
						blender = Blender::createDefault(Blender::NO, try_gpu);
					else if (blend_type == Blender::MULTI_BAND)
					{
						MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(static_cast<Blender*>(blender));
						mb->setNumBands(static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.));
						cout << "Multi-band blender, number of bands: " << mb->numBands() << endl;
					}
					else if (blend_type == Blender::FEATHER)
					{
						FeatherBlender* fb = dynamic_cast<FeatherBlender*>(static_cast<Blender*>(blender));
						fb->setSharpness(1.f / blend_width);
						LOGLN("Feather blender, sharpness: " << fb->sharpness());
					}
					blender->prepare(corners, sizes);
				}

				// Blend the current image
				blender->feed(img_warped_s, mask_warped, corners[img_idx]);
			}
			Mat result, result_mask;
			blender->blend(result, result_mask);

			cout << "Compositing, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec" << endl;

			result.convertTo(frame, CV_8UC1);
			namedWindow("stitch", 0);
			imshow("stitch", frame);
			waitKey(1);
			cv::cvtColor(frame, frame, CV_BGR2RGB);
			cv::resize(frame, frame, Size(ui.label_3->width(), ui.label_3->height()));
			image = QImage((const uchar*)frame.data,
				frame.cols,
				frame.rows,
				frame.cols * frame.channels(),
				QImage::Format_RGB888);

			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			ui.label_3->setPixmap(QPixmap::fromImage(image));    //  将图片显示到label上
			ui.label_3->resize(ui.label_3->pixmap()->size());    //  将label控件resize到fame的尺寸
		}
		else {
			cout << "----------------------" << endl;
			cout << "waitting..." << endl;
		}
		ui.progressBar->setValue(100);
		ui.label_16->setText("Progress info: Completed");
	}
}