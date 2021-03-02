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


using namespace std;
using namespace cv;
using namespace cv::detail;

Stitcher::Mode mode2 = Stitcher::PANORAMA;

vectorMatWithStitcher::vectorMatWithStitcher(const vector<string>& filePaths)
{
	if (filePaths.empty()) {
		return;
	}
	this->filePaths = filePaths;
	for (auto it = filePaths.begin(); it != filePaths.end(); it++) {
		Mat image = imread(*it, IMREAD_COLOR);  // Read the file
		if (image.empty())			// Check for invalid input
		{
			printf("filePath error");
		}
		else {
			picArray.push_back(image);
		}
	}
}

vectorMatWithStitcher::vectorMatWithStitcher(const vector<string>& filePaths, const double& resizeFactor)
{
	if (filePaths.empty()) {
		return;
	}
	this->filePaths = filePaths;
	for (auto it = filePaths.begin(); it != filePaths.end(); it++) {
		Mat image = imread(*it, IMREAD_COLOR);  // Read the file
		if (image.empty())			// Check for invalid input
		{
			printf("filePath error");
		}
		else {
			resize(image, image, Size(), resizeFactor, resizeFactor);
			picArray.push_back(image);
		}
	}
}

vectorMatWithStitcher::vectorMatWithStitcher( vector<VideoCapture*> allframe) {

	Mat first1;
	Mat first2;
	
	(*allframe[0]) >> first1;
	
	(*allframe[1]) >> first2;
	resize(first1, first1, Size(), 0.3, 0.3);
	resize(first2, first2, Size(), 0.3, 0.3);
	picArray.push_back(first1);
	picArray.push_back(first2);

}



void vectorMatWithStitcher::stitchingDetail( vector<VideoCapture*> allframe)
{
	bool stop(false);
	Ptr<Feature2D> finder;
	finder = xfeatures2d::SURF::create();//ORB or SURF or AKAZE or SIFT
	int num_images = picArray.size();
	vector<ImageFeatures> features(num_images);
	for (int i = 0; i < num_images; ++i) {
		computeImageFeatures(finder, picArray[i], features[i]);
	}

	vector<MatchesInfo> pairwise_matches;
	Ptr<FeaturesMatcher> matcher;
	float match_conf = 0.3f;//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	matcher = makePtr<AffineBestOf2NearestMatcher>(false, 0, match_conf);
	// AffineBestOf2NearestMatcher BestOf2NearestMatcher BestOf2NearestRangeMatcher

	(*matcher)(features, pairwise_matches);
	matcher->collectGarbage();
	float conf_thresh = 0.5f;//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	vector<int> indices = leaveBiggestComponent(features, pairwise_matches, conf_thresh);
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
	for (size_t i = 0; i < cameras.size(); ++i)
	{
		Mat R;
		cameras[i].R.convertTo(R, CV_32F);
		cameras[i].R = R;
		cout << "Initial camera intrinsics #" << indices[i] + 1 << ":\nK:\n" << cameras[i].K() << "\nR:\n" << cameras[i].R << endl;
	}
	Ptr<detail::BundleAdjusterBase> adjuster;
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
		cout << "Can't create the following warper '"  << "'\n";
	}

	Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));
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

	Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(expos_comp_type);
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
	images_warped.clear();
	images_warped_f.clear();
	masks.clear();
	picArray.clear();

	Mat frame1;
	Mat frame2;

	while (!stop)
	{
		(*allframe[0]) >> frame1;
		(*allframe[1]) >> frame2;
		resize(frame1, frame1, Size(), 0.3, 0.3);
		resize(frame2, frame2, Size(), 0.3, 0.3);
		imshow("a", frame1);
		imshow("b", frame2);
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
			for (int img_idx = 0; img_idx < num_images; ++img_idx)
			{
				cout << "Compositing image #" << indices[img_idx] + 1 << endl;
				// Read image and resize it if necessary
				Mat K;
				cameras[img_idx].K().convertTo(K, CV_32F);
				warper->warp(picArray[img_idx], K, cameras[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);
				Size img_size = picArray[img_idx].size();
				mask.create(img_size, CV_8U);
				mask.setTo(Scalar::all(255));
				warper->warp(mask, K, cameras[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);
				compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);
				img_warped.convertTo(img_warped_s, CV_16S);

				img_warped.release();
				mask.release();
				dilate(masks_warped[img_idx], dilated_mask, Mat());
				resize(dilated_mask, seam_mask, mask_warped.size(), 0, 0, INTER_LINEAR_EXACT);
				mask_warped = seam_mask & mask_warped;
				if (!blender && !timelapse)
				{
					blender = Blender::createDefault(blend_type, 0);
					Size dst_sz = resultRoi(corners, sizes).size();
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
					blender->prepare(corners, sizes);
				}
				
					img_warped_s.convertTo(img_warped_s, CV_8U);
					blender->feed(img_warped_s, mask_warped, corners[img_idx]);
				

			}
			
				Mat result, result_mask;
				blender->blend(result, result_mask);
				result.convertTo(result, CV_8U);
				imshow("result", result);
				
				//imwrite("result.jpg", result);
				//imwrite("result_mask.jpg", result_mask);
				waitKey(1);
				picArray.clear();
			
		}
	}
	cout << "Success.\n";
}

void vectorMatWithStitcher::stitchingAll()
{
	Mat pano;
	Ptr<Stitcher> stitcher = Stitcher::create(mode2);
	Stitcher::Status status = stitcher->stitch(picArray, pano);
	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code = " << int(status) << endl;
	}
	else {
		output = pano;
		cout << "stitching completed successfully\n";
	}
}

void vectorMatWithStitcher::printFilePaths()
{
	for (auto it = filePaths.begin(); it != filePaths.end(); it++) {
		cout << *it << endl;
	}
}

void vectorMatWithStitcher::showAllpic()
{
	int ArraySize = picArray.size();
	for (int i = 0; i < ArraySize; i++) {
		imshow(filePaths[i], picArray[i]);
	}
}

void vectorMatWithStitcher::showOutput()
{
	imshow("output", output);
}
