#include <iostream>
#include <fstream>
#include <string>
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
#include "HCNetSDK.h"
#include "plaympeg4.h"
#include "vlc_reader.h"
#include <thread>
#include <future>
#include <opencv2\highgui\highgui_c.h>
#include "opencv2/xfeatures2d/nonfree.hpp"





#define ENABLE_LOG 1
#define LOG(msg) std::cout << msg
#define LOGLN(msg) std::cout << msg << std::endl

using namespace std;
using namespace cv;
using namespace cv::detail;


// Default command line args
vector<String> img_names;
bool preview = false;
bool try_cuda = false;
bool try_gpu = true;
double work_megapix = 0.6;
double seam_megapix = 0.1;
double compose_megapix = 0.15;
float conf_thresh = 0.8;
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

void capturepic1(LONG lRealPlayHandle1, char A[])
{

	NET_DVR_CapturePicture(lRealPlayHandle1, A);

};

void capturepic2(LONG lRealPlayHandle2, char B[])
{

	NET_DVR_CapturePicture(lRealPlayHandle2, B);

};
void capturepic3(LONG lRealPlayHandle3, char C[])
{

	NET_DVR_CapturePicture(lRealPlayHandle3, C);

};

int login() {
	NET_DVR_Init();
	//��������ʱ��������ʱ��
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);
	//---------------------------------------

	// ע���豸1
	LONG lUserID1;
	NET_DVR_DEVICEINFO_V30 struDeviceInfo1;
	lUserID1 = NET_DVR_Login_V30(const_cast<char*>("192.168.0.100"), 8000, const_cast<char*>("admin"), const_cast<char*>("HIKVISION610"), &struDeviceInfo1);
	cout << lUserID1 << endl;
	if (lUserID1 < 0)
	{
		printf("Login error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
	}
	//����Ԥ�������ûص�������
	cvNamedWindow("Mywindow1", WINDOW_NORMAL);
	LONG lRealPlayHandle1;
	HWND  h1 = (HWND)cvGetWindowHandle("Mywindow1");

	HWND hRawWnd1 = ::GetParent(h1); //��ø����ھ��
	ShowWindow(h1, 0); //0����ʱ���ʾ�����Ӵ���
	ShowWindow(hRawWnd1, 0); //0����ʱ���ʾ���ظ�����

	NET_DVR_PREVIEWINFO struPlayInfo1 = { 0 };
	struPlayInfo1.hPlayWnd = h1;         //��ҪSDK����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
	struPlayInfo1.lChannel = 1;           //Ԥ��ͨ����
	struPlayInfo1.dwStreamType = 1;       //0-��������1-��������2-����3��3-����4���Դ�����
	struPlayInfo1.dwLinkMode = 0;         //0- TCP��ʽ��1- UDP��ʽ��2- �ಥ��ʽ��3- RTP��ʽ��4-RTP/RTSP��5-RSTP/HTTP
	lRealPlayHandle1 = NET_DVR_RealPlay_V40(lUserID1, &struPlayInfo1, NULL, NULL);
	waitKey(1);
	// ע���豸2
	LONG lUserID2;
	NET_DVR_DEVICEINFO_V30 struDeviceInfo2;
	lUserID2 = NET_DVR_Login_V30(const_cast<char*>("192.168.0.102"), 8000, const_cast<char*>("admin"), const_cast<char*>("HIKVISION610"), &struDeviceInfo2);
	cout << lUserID2 << endl;
	if (lUserID2 < 0)
	{
		printf("Login error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
	}
	//����Ԥ�������ûص�������
	cvNamedWindow("Mywindow2", WINDOW_NORMAL);
	LONG lRealPlayHandle2;
	HWND  h2 = (HWND)cvGetWindowHandle("Mywindow2");
	
	HWND hRawWnd2 = ::GetParent(h2); //��ø����ھ��
	ShowWindow(h2, 0); //0����ʱ���ʾ�����Ӵ���
	ShowWindow(hRawWnd2, 0); //0����ʱ���ʾ���ظ�����

	NET_DVR_PREVIEWINFO struPlayInfo2 = { 0 };
	struPlayInfo2.hPlayWnd = h2;         //��ҪSDK����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
	struPlayInfo2.lChannel = 1;           //Ԥ��ͨ����
	struPlayInfo2.dwStreamType = 1;       //0-��������1-��������2-����3��3-����4���Դ�����
	struPlayInfo2.dwLinkMode = 0;         //0- TCP��ʽ��1- UDP��ʽ��2- �ಥ��ʽ��3- RTP��ʽ��4-RTP/RTSP��5-RSTP/HTTP
	lRealPlayHandle2 = NET_DVR_RealPlay_V40(lUserID2, &struPlayInfo2, NULL, NULL);
	waitKey(1);
	// ע���豸3
	LONG lUserID3;
	NET_DVR_DEVICEINFO_V30 struDeviceInfo3;
	lUserID3 = NET_DVR_Login_V30(const_cast<char*>("192.168.0.103"), 8000, const_cast<char*>("admin"), const_cast<char*>("HIKVISION610"), &struDeviceInfo3);
	cout << lUserID3 << endl;
	if (lUserID3 < 0)
	{
		printf("Login error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
	}
	//����Ԥ�������ûص�������
	cvNamedWindow("Mywindow3", WINDOW_NORMAL);
	LONG lRealPlayHandle3;
	HWND  h3 = (HWND)cvGetWindowHandle("Mywindow3");

	HWND hRawWnd3 = ::GetParent(h3); //��ø����ھ��
	ShowWindow(h3, 0); //0����ʱ���ʾ�����Ӵ���
	ShowWindow(hRawWnd3, 0); //0����ʱ���ʾ���ظ�����

	NET_DVR_PREVIEWINFO struPlayInfo3 = { 0 };
	struPlayInfo3.hPlayWnd = h3;         //��ҪSDK����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
	struPlayInfo3.lChannel = 1;           //Ԥ��ͨ����
	struPlayInfo3.dwStreamType = 1;       //0-��������1-��������2-����3��3-����4���Դ�����
	struPlayInfo3.dwLinkMode = 0;         //0- TCP��ʽ��1- UDP��ʽ��2- �ಥ��ʽ��3- RTP��ʽ��4-RTP/RTSP��5-RSTP/HTTP
	lRealPlayHandle3 = NET_DVR_RealPlay_V40(lUserID3, &struPlayInfo3, NULL, NULL);
	waitKey(1);



	Mat rimg1;
	Mat rimg2;
	Mat rimg3;

	char firstA[] = "G:\\c++Projects\\QtOpencvSet\\QtOpencvSet\\firstA.bmp";
	char firstB[] = "G:\\c++Projects\\QtOpencvSet\\QtOpencvSet\\firstB.bmp";
	char firstC[] = "G:\\c++Projects\\QtOpencvSet\\QtOpencvSet\\firstC.bmp";


	NET_DVR_CapturePicture(lRealPlayHandle1, firstA);
	NET_DVR_CapturePicture(lRealPlayHandle2, firstB);
	NET_DVR_CapturePicture(lRealPlayHandle3, firstC);
	Sleep(300);

	bool stop(false);
	Mat frame1; //����һ��Mat���������ڴ洢ÿһ֡��ͼ��
	Mat frame2; //����һ��Mat���������ڴ洢ÿһ֡��ͼ��
	Mat frame3; //����һ��Mat���������ڴ洢ÿһ֡��ͼ��
	Mat frame;
	Mat rframe1;
	Mat rframe2;
	Mat rframe3; //����һ��Mat���������ڴ洢ÿһ֡��ͼ��
	Mat cap1;
	Mat cap2;
	Mat cap3;
	int k = 100;

	namedWindow("stitch", CV_WINDOW_AUTOSIZE);

	if (!h1)
	{
		cout << "cannot open the camera1."; cin.get(); return -1;
	}

	if (!h2)
	{
		cout << "cannot open the camera2."; cin.get(); return -1;
	}

	if (!h3)
	{
		cout << "cannot open the camera3."; cin.get(); return -1;
	}


}

