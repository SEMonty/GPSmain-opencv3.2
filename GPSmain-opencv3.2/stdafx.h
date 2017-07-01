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


// TODO: プログラムに必要な追加ヘッダーをここで参照してください
cv::Mat ar_getPerspectiveTransform(cv::Mat frame);
cv::Mat man_getPerspectiveTransform(cv::Mat frame);
//マウス入力用のパラメータ
struct mouseParam {
	int x;
	int y;
	int event;
	int flags;
};
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