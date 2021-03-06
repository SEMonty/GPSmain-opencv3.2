// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/tracking.hpp>
#include <iostream>
#include <math.h>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace cv;


class MySender
{
private:
	struct sockaddr_in addr;
	WSAData wsaData;
	SOCKET sock;
public:
	MySender() {
	}

	~MySender()//disconnect忘れ防止
	{
		closesocket(sock);
		WSACleanup();
	}

	void MySender::connect(char* address, int port)
	{
		WSAStartup(MAKEWORD(2, 0), &wsaData);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		inet_pton(AF_INET, address, &addr.sin_addr.S_un.S_addr);
		sock = socket(AF_INET, SOCK_DGRAM, 0);
	}

	void MySender::disconnect()
	{
		closesocket(sock);
		WSACleanup();
	}
	void MySender::send(char* message) {
		sendto(sock, message, strlen(message), 0, (struct sockaddr *)&addr, sizeof(addr));
	}
};


// TODO: プログラムに必要な追加ヘッダーをここで参照してください

//マウス入力用のパラメータ
struct mouseParam {
	int x;
	int y;
	int event;
	int flags;
};

///認識するオブジェクト情報
struct TrackingObj {
	const int H_MIN;
	const int H_MAX;
	const int S_MIN;
	const int S_MAX;
	const int V_MIN;
	const int V_MAX;
	int x;
	int y;
};
// 関数ヘッダー
cv::Mat ar_getPerspectiveTransform(cv::Mat frame);
cv::Mat man_getPerspectiveTransform(cv::Mat frame);
void CallBackFunc(int eventType, int x, int y, int flags, void* userdata);
Mat getBinFrame(Mat rgbframe, struct TrackingObj &obj);
void morphOps(Mat &thresh);
bool trackFilteredObject(struct TrackingObj &obj, Mat threshold, int max_num_obj, int min_obj_area, int max_obj_area);

#pragma comment(lib,"ws2_32.lib")
#ifdef _DEBUG
//Debugモードの場合
#pragma comment(lib,"opencv_aruco320d.lib")
#pragma comment(lib,"opencv_bgsegm320d.lib")
#pragma comment(lib,"opencv_calib3d320d.lib")
#pragma comment(lib,"opencv_ccalib320d.lib")
#pragma comment(lib,"opencv_core320d.lib")
#pragma comment(lib,"opencv_datasets320d.lib")
#pragma comment(lib,"opencv_dnn320d.lib")
#pragma comment(lib,"opencv_dpm320d.lib")
#pragma comment(lib,"opencv_face320d.lib")
#pragma comment(lib,"opencv_features2d320d.lib")
#pragma comment(lib,"opencv_flann320d.lib")
#pragma comment(lib,"opencv_fuzzy320d.lib")
#pragma comment(lib,"opencv_highgui320d.lib")
#pragma comment(lib,"opencv_imgcodecs320d.lib")
#pragma comment(lib,"opencv_imgproc320d.lib")
#pragma comment(lib,"opencv_line_descriptor320d.lib")
#pragma comment(lib,"opencv_ml320d.lib")
#pragma comment(lib,"opencv_objdetect320d.lib")
#pragma comment(lib,"opencv_optflow320d.lib")
#pragma comment(lib,"opencv_photo320d.lib")
#pragma comment(lib,"opencv_plot320d.lib")
#pragma comment(lib,"opencv_reg320d.lib")
#pragma comment(lib,"opencv_rgbd320d.lib")
#pragma comment(lib,"opencv_saliency320d.lib")
#pragma comment(lib,"opencv_shape320d.lib")
#pragma comment(lib,"opencv_stereo320d.lib")
#pragma comment(lib,"opencv_stitching320d.lib")
#pragma comment(lib,"opencv_structured_light320d.lib")
#pragma comment(lib,"opencv_superres320d.lib")
#pragma comment(lib,"opencv_surface_matching320d.lib")
#pragma comment(lib,"opencv_text320d.lib")
#pragma comment(lib,"opencv_tracking320d.lib")
#pragma comment(lib,"opencv_video320d.lib")
#pragma comment(lib,"opencv_videoio320d.lib")
#pragma comment(lib,"opencv_videostab320d.lib")
#pragma comment(lib,"opencv_xfeatures2d320d.lib")
#pragma comment(lib,"opencv_ximgproc320d.lib")
#pragma comment(lib,"opencv_xobjdetect320d.lib")
#pragma comment(lib,"opencv_xphoto320d.lib") // opencv_core
#else											  
//Releaseモードの場合								
#pragma comment(lib,"opencv_aruco320.lib")
#pragma comment(lib,"opencv_bgsegm320.lib")
#pragma comment(lib,"opencv_calib3d320.lib")
#pragma comment(lib,"opencv_ccalib320.lib")
#pragma comment(lib,"opencv_core320.lib")
#pragma comment(lib,"opencv_datasets320.lib")
#pragma comment(lib,"opencv_dnn320.lib")
#pragma comment(lib,"opencv_dpm320.lib")
#pragma comment(lib,"opencv_face320.lib")
#pragma comment(lib,"opencv_features2d320.lib")
#pragma comment(lib,"opencv_flann320.lib")
#pragma comment(lib,"opencv_fuzzy320.lib")
#pragma comment(lib,"opencv_highgui320.lib")
#pragma comment(lib,"opencv_imgcodecs320.lib")
#pragma comment(lib,"opencv_imgproc320.lib")
#pragma comment(lib,"opencv_line_descriptor320.lib")
#pragma comment(lib,"opencv_ml320.lib")
#pragma comment(lib,"opencv_objdetect320.lib")
#pragma comment(lib,"opencv_optflow320.lib")
#pragma comment(lib,"opencv_photo320.lib")
#pragma comment(lib,"opencv_plot320.lib")
#pragma comment(lib,"opencv_reg320.lib")
#pragma comment(lib,"opencv_rgbd320.lib")
#pragma comment(lib,"opencv_saliency320.lib")
#pragma comment(lib,"opencv_shape320.lib")
#pragma comment(lib,"opencv_stereo320.lib")
#pragma comment(lib,"opencv_stitching320.lib")
#pragma comment(lib,"opencv_structured_light320.lib")
#pragma comment(lib,"opencv_superres320.lib")
#pragma comment(lib,"opencv_surface_matching320.lib")
#pragma comment(lib,"opencv_text320.lib")
#pragma comment(lib,"opencv_tracking320.lib")
#pragma comment(lib,"opencv_video320.lib")
#pragma comment(lib,"opencv_videoio320.lib")
#pragma comment(lib,"opencv_videostab320.lib")
#pragma comment(lib,"opencv_xfeatures2d320.lib")
#pragma comment(lib,"opencv_ximgproc320.lib")
#pragma comment(lib,"opencv_xobjdetect320.lib")
#pragma comment(lib,"opencv_xphoto320.lib")
#endif