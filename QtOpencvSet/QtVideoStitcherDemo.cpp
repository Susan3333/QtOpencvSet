#include "QtVideoStitcherDemo.h"
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


using namespace std;
using namespace cv;
using namespace cv::detail;

Mat MoveDetect(Mat before, Mat frame) {

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
	dilate(diff_thresh, diff_thresh, kernel_dilate);
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

QtVideoStitcherDemo::QtVideoStitcherDemo(QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui.setupUi(this);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(ReadFrame()));
}

QtVideoStitcherDemo::~QtVideoStitcherDemo()
{
}

void QtVideoStitcherDemo::ReadFrame()
{
	if (captureFirst.isOpened()&& captureSecond.isOpened())
	{
		captureFirst >> frame1;
		captureSecond >> frame2;
		QImage image;
		if (!frame1.empty())
		{
			if (frame1.channels() == 3)    // RGB image
			{
				cv::cvtColor(frame1, result_frame1, CV_BGR2RGB);
				cv::resize(result_frame1, result_frame1, Size(ui.label->width(), ui.label->height()));
				image = QImage((const uchar*)result_frame1.data,
					result_frame1.cols,
					result_frame1.rows,
					result_frame1.cols * result_frame1.channels(),
					QImage::Format_RGB888);
			}
			else                     // gray image
			{
				cv::resize(frame1, result_frame1, Size(ui.label->width(), ui.label->height()));
				image = QImage((const uchar*)result_frame1.data,
					result_frame1.cols,
					result_frame1.rows,
					result_frame1.cols * result_frame1.channels(),
					QImage::Format_Indexed8);
			}
			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			ui.label->setPixmap(QPixmap::fromImage(image));    //  将图片显示到label上
			ui.label->resize(ui.label->pixmap()->size());    //  将label控件resize到fame的尺寸
		}
	}

	if (captureFirst.isOpened()&& captureSecond.isOpened())
	{
		captureFirst >> frame1;
		captureSecond >> frame2;
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
	}
	QImage image3;
	switch (MainVideoShowMode)
	{
	case 0:

		if (!frame1.empty())
		{
			if (frame1.channels() == 3)    // RGB image
			{
				cv::cvtColor(frame1, frame1, CV_BGR2RGB);
				cv::resize(frame1, frame1, Size(ui.label_3->width(), ui.label_3->height()));
				image3 = QImage((const uchar*)frame1.data,
					frame1.cols,
					frame1.rows,
					frame1.cols * frame1.channels(),
					QImage::Format_RGB888);
			}
			else                     // gray image
			{
				cv::resize(frame1, frame1, Size(ui.label_3->width(), ui.label_3->height()));
				image3 = QImage((const uchar*)frame1.data,
					frame1.cols,
					frame1.rows,
					frame1.cols * frame1.channels(),
					QImage::Format_Indexed8);
			}
			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			ui.label_3->setPixmap(QPixmap::fromImage(image3));    //  将图片显示到label上
			ui.label_3->resize(ui.label_3->pixmap()->size());    //  将label控件resize到fame的尺寸
		}
		break; // 可选的

	case 1:

		if (!frame2.empty())
		{
			if (frame2.channels() == 3)    // RGB image
			{
				cv::cvtColor(frame2, frame2, CV_BGR2RGB);
				cv::resize(frame2, frame2, Size(ui.label_3->width(), ui.label_3->height()));
				image3 = QImage((const uchar*)frame2.data,
					frame2.cols,
					frame2.rows,
					frame2.cols * frame2.channels(),
					QImage::Format_RGB888);
			}
			else                     // gray image
			{
				cv::resize(frame2, frame2, Size(ui.label_3->width(), ui.label_3->height()));
				image3 = QImage((const uchar*)frame2.data,
					frame2.cols,
					frame2.rows,
					frame2.cols * frame2.channels(),
					QImage::Format_Indexed8);
			}
			// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
			ui.label_3->setPixmap(QPixmap::fromImage(image3));    //  将图片显示到label上
			ui.label_3->resize(ui.label_3->pixmap()->size());    //  将label控件resize到fame的尺寸
		}
		break; // 可选的

	case 2:
		cv::resize(frame1, frame1, Size(), definition, definition);
		cv::resize(frame2, frame2, Size(), definition, definition);
		if (frame1.rows != 0 && frame2.rows != 0)
		{
			picArray.push_back(frame1);
			picArray.push_back(frame2);
			waitKey(10);

			Mat img_warped, img_warped_s;
			Mat dilated_mask, seam_mask, mask, mask_warped;
			Ptr<Blender> blender;
			Ptr<Timelapser> timelapser;
			bool timelapse = false;
			int blend_type = Blender::MULTI_BAND;
			float blend_strength = 5;
			//int timelapse_type = Timelapser::AS_IS;
			for (int img_idx = 0; img_idx < 2; ++img_idx)
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
				result = MoveDetect(before, result);
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
					image3 = QImage((const uchar*)result.data,
						result.cols,
						result.rows,
						result.cols * result.channels(),
						QImage::Format_RGB888);
				}
				else                     // gray image
				{
					cv::resize(result, result, Size(ui.label_3->width(), ui.label_3->height()));
					image3 = QImage((const uchar*)result.data,
						result.cols,
						result.rows,
						result.cols * result.channels(),
						QImage::Format_Indexed8);
				}

				// 将抓取到的帧，转换为QImage格式。QImage::Format_RGB32不同的摄像头用不同的格式。
				ui.label_3->setPixmap(QPixmap::fromImage(image3));    //  将图片显示到label上
				ui.label_3->resize(ui.label_3->pixmap()->size());    //  将label控件resize到fame的尺寸

			}

		}

		break; // 可选的

	default:
		break;
	}
	
	
	
}

void QtVideoStitcherDemo::videoOpenprocess() {
	//这里需要清除vectormat类的内存 还没有写！！！！！！
	QStringList filenames;
	vector<string> pathnames;
	filenames = QFileDialog::getOpenFileNames(this,
		QString::fromUtf16(u"选择视频文件"),
		"",
		tr("Images (*.mp4)"));
	for (int i = 0; i < filenames.size(); i++) {
		QString str_path = filenames[i];
		pathnames.push_back(str_path.toStdString());
		cout << str_path.toStdString() << endl;
		//单个文件路径
	}
	captureFirst.open(pathnames[0]);
	captureSecond.open(pathnames[1]);


	if (!captureFirst.isOpened()|| !captureSecond.isOpened())
	{
		cout << "Movie open Error" << endl;
		return;
	}
	double rateFirst = captureFirst.get(CAP_PROP_FPS);
	double rateSecond = captureSecond.get(CAP_PROP_FPS);
	cout << "帧率为:" << " " << rateFirst << endl;
	cout << "总帧数为:" << " " << captureFirst.get(CAP_PROP_FRAME_COUNT) << endl;//输出帧总数
	cout << "帧率为:" << " " << rateSecond << endl;
	cout << "总帧数为:" << " " << captureSecond.get(CAP_PROP_FRAME_COUNT) << endl;//输出帧总数
	double position = 0.0;
	//设置播放到哪一帧，这里设置为第0帧
	captureFirst.set(CAP_PROP_POS_FRAMES, position);
	captureSecond.set(CAP_PROP_POS_FRAMES, position);
	//阶段信息显示
	ui.progressBar->setValue(10);
	ui.label_16->setText("Progress info: Reading Video files.");

	captureFirst >> frame1;
	captureSecond >> frame2;
	cv::resize(frame1, frame1, Size(), definition, definition);
	cv::resize(frame2, frame2, Size(), definition, definition);
	picArray.push_back(frame1);
	picArray.push_back(frame2);
	waitKey(0);
	Ptr<Feature2D> finder;
	//~~~~~~~~~~~~~~改为可选
	finder = xfeatures2d::SURF::create();//ORB or SURF or AKAZE or SIFT
	int num_images = picArray.size();
	vector<ImageFeatures> features(num_images);
	for (int i = 0; i < num_images; ++i) {
		computeImageFeatures(finder, picArray[i], features[i]);
	}
	//阶段信息显示
	ui.progressBar->setValue(30);
	ui.label_16->setText("Progress info: Computing features.");
	vector<MatchesInfo> pairwise_matches;
	Ptr<FeaturesMatcher> matcher;
	//~~~~~~~~~~~~~~改为可改
	float match_conf = 0.3f;//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	matcher = makePtr<AffineBestOf2NearestMatcher>(false, 0, match_conf);
	// AffineBestOf2NearestMatcher BestOf2NearestMatcher BestOf2NearestRangeMatcher

	(*matcher)(features, pairwise_matches);
	matcher->collectGarbage();
	//~~~~~~~~~~~~~~改为可改
	float conf_thresh = 0.5f;//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
	Ptr<detail::BundleAdjusterBase> adjuster;
	//~~~~~~~~~~~需要修改
	adjuster = makePtr<detail::NoBundleAdjuster>();
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

	Ptr<WarperCreator> warper_creator;
	warper_creator = makePtr<cv::AffineWarper>();
	/* {
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
	}*/
	if (!warper_creator)
	{
		cout << "Can't create the following warper '" << "'\n";
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


void QtVideoStitcherDemo::changeMainVideoShowMode(int index)
{
	MainVideoShowMode = index;
	cout << "MainVideoShowMode:" << index << endl;
}

void QtVideoStitcherDemo::setScreenShootFlag()
{
	ScreenShootFlag = 1;
}

void QtVideoStitcherDemo::setBrightnessAdjustment(int A)
{
	beta = 0;
	beta = A + beta;
}

void QtVideoStitcherDemo::setContrastAdjustment(int A)
{
	alpha = 1;
	alpha = float(A - 50) / 100 + alpha;
}

void QtVideoStitcherDemo::setDefinition(int definitionMode)
{
	switch (definitionMode)
	{
	case 0 :
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

void QtVideoStitcherDemo::setmovingDetectionFlag()
{
	movingDetectionFlag = !movingDetectionFlag;
}



