// aruco_opencv3.2.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

using namespace std;
using namespace cv;
//適合っぽい変数
int camera_num = 0; //ビデオ入力番号
string video_name = "video\\4_720_30fps_横.wmv"; //ビデオの名前
//string video_name = "video\\test1.wmv"; //ビデオの名前

bool online = true; //カメラ：true　ビデオ:false

int CM_WIDTH = 1280; //カメラ、ビデオの入力画像サイズ
int CM_HEIGHT = 720;

float HM_WIDTH = 520;	//透視変換後の画像サイズ
float HM_HEIGHT = 275;

float REAL_WIDTH = 5.2;  //実際のフィールドサイズ(m)
float REAL_HEIGHT = 2.75;
//ビデオ書き出し設定
const int fourcc = VideoWriter::fourcc('W', 'M', 'V', '2');
string filename = "output.wmv";

//#define ARHOMO			//ARUCOで透視変換
#define MANHOMO			//手動で透視変換
//#define SAVEVIDEO			//動画保存
//#define TRACK			//トラッキングする
#define TRACK2			//トラッキング2


int main()
{

	///////////////ゲーム開始待ち、開始命令の送信///////////////
	//送信準備
	MySender sen;
	sen.connect("192.168.10.3", 6003);		//"送信相手のIP",ポート

	char value_car_a_x[256]; //一時的な変数
	char value_car_a_y[256]; //一時的な変数
	char value_car_a_radian[256]; //一時的な変数
	/*alt
	char value_car_b_x[256]; //一時的な変数
	char value_car_b_y[256]; //一時的な変数
	char value_car_b_radian[256]; //一時的な変数
	*/
	//開始命令の送信
	sen.send("s");	//s:開始
	//////////////////////カメラ接続と最初のフレームの読み出し//////////////////
	//動画の読み込み
	VideoCapture cap;
	if (online)
		cap.open(camera_num);
	else
		cap.open(video_name);

	if (!cap.isOpened()) {
		std::cout << "Camera or video can not be opened" << endl;
		std::cout << "Press enter to Exit" << endl;
		getchar(); //
		return -1;
	}

	//cap.set(CAP_PROP_FRAME_WIDTH, CWIDTH);	//サイズ指定
	//cap.set(CAP_PROP_FRAME_HEIGHT, CHEIGHT);	//サイズ指定
	//cap.set(CAP_PROP_FPS, 30);				//FPS設定
	//コンソールにカメラ設定の出力
	std::cout << "Web Camera settings:" << endl;
	std::cout << "Width:" + to_string(cap.get(CAP_PROP_FRAME_WIDTH)) << endl;
	std::cout << "Height:" + to_string(cap.get(CAP_PROP_FRAME_HEIGHT)) << endl;
	std::cout << "FPS:" + to_string(cap.get(CAP_PROP_FPS)) << endl;
	std::cout << "Exposure:" + to_string(cap.get(CAP_PROP_EXPOSURE)) << endl;
	std::cout << "Gain:" + to_string(cap.get(CAP_PROP_GAIN)) << endl;
	std::cout << "Iso_speed:" + to_string(cap.get(CAP_PROP_ISO_SPEED)) << endl;
	std::cout << "speed:" + to_string(cap.get(CAP_PROP_SPEED)) << endl;

	//初期フレームの読み出し
	Mat fistframe;
	cap >> fistframe;

	//カメラの場合は画像確認　nキーで次へ
	while (cap.grab() && online) {
		cap.retrieve(fistframe);
		///////////////その他///////////////
		int key = waitKey(1);
		if (key == 110)
		{
			break;
		}
		cv::imshow("camera", fistframe);
	}
	destroyWindow("camera");
	//////////////透視変換行列の算出///////////////////////////////
	Mat homography_matrix;
#ifdef ARHOMO

	homography_matrix = ar_getPerspectiveTransform(fistframe);

	warpPerspective(fistframe, fistframe, homography_matrix, Size(WIDTH, HEIGHT));
#endif
	homography_matrix = man_getPerspectiveTransform(fistframe);

	warpPerspective(fistframe, fistframe, homography_matrix, Size(HM_WIDTH, HM_HEIGHT));
#ifdef MANHOMO	//手動ホモグラフィ
#endif // MANHOMO


	//////////////動画取得の準備///////////////////////////////
#ifdef SAVEVIDEO
	double fps = cap.get(CV_CAP_PROP_FPS);
	Size size = Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	VideoWriter writer(filename, fourcc, fps, size);
#endif//SAVEVIDEO

	//////////////トラッキングの準備///////////////////////////////
#ifdef TRACK
	// 最初のフレームから追跡対象を選択

	vector<Rect2d> rois;
	selectROI("tracker", fistframe, rois);

	// Trackerの生成/初期化
	MultiTracker trackerKCF("KCF");
	trackerKCF.add(fistframe, rois);
#endif // TRACK
#ifdef TRACK2
	//トラッキングする色の指定
	struct TrackingObj ego_fr_obj = { 0, 65, 93, 256, 37, 256,0,0 };	//自車フロントのオブジェクト（ピンク）
	struct TrackingObj ego_rr_obj = { 40, 93, 116, 256, 109, 179,0,0 };	//自車リアのオブジェクト（黄色）
	/*alt
		 { 146, 231, 7, 256, 65, 231,0,0 };	//自車フロントのオブジェクト（ピンク）
		 { 21, 56, 49, 256, 144, 227,0,0 };	//自車リアのオブジェクト（黄色）
	struct TrackingObj alt_fr_obj = { 0, 65, 93, 256, 37, 256,0,0 };	//相手フロントのオブジェクト（オレンジ）	
	struct TrackingObj alt_rr_obj = { 40, 93, 116, 256, 109, 179,0,0 };	//相手リアのオブジェクト（緑）
	*/
#endif // TRACK2



	//メインループ
	while (cap.grab()) {
		Mat frame;

		std::chrono::system_clock::time_point  start, end;
		start = std::chrono::system_clock::now(); // 計測開始時間
		///////////////カメラ画像取得///////////////
		cap.retrieve(frame);
		imshow("frame", frame);

#ifdef SAVEVIDEO		//動画の保存
		writer << frame;//フレームを動画に保存
#endif	

		///////////////歪み補正(予定)///////////////
		//////
		//////
		//////
		//////
		//////
		///////////////透視変換///////////////
#ifdef MANHOMO || ARHOMO
		warpPerspective(frame, frame, homography_matrix, Size(HM_WIDTH, HM_HEIGHT));
		imshow("homoed", frame);
#endif
		///////////////トラッキング1///////////////
#ifdef TRACK
		trackerKCF.update(frame, rois);

		//表示
		for (unsigned int i = 0; i < rois.size(); i++) {
			rectangle(frame, rois[i], Scalar(0, (80 * (i % 4)), 0), 2, 1);
		}
		imshow("tracker", frame);

		///////////////座標から実世界の距離へ変換///////////////
		Point2f car_a_fr_pos = Point2f(rois[0].x + rois[0].width / 2, rois[0].y + rois[0].height / 2);
		Point2f car_a_rr_pos = Point2f(rois[1].x + rois[1].width / 2, rois[1].y + rois[1].height / 2);
		Point2f car_a_ce_pos = Point2f((car_a_fr_pos.x + car_a_rr_pos.x) / 2, (car_a_fr_pos.y + car_a_rr_pos.y) / 2);

		double car_a_x = (car_a_ce_pos.x / HM_WIDTH) * REAL_WIDTH;
		double car_a_y = (car_a_ce_pos.y / HM_HEIGHT) * REAL_HEIGHT;
		double radian = atan2(car_a_rr_pos.y - car_a_fr_pos.y, car_a_fr_pos.x - car_a_rr_pos.x);//反時計回りを正、-pi～pi
		double car_a_degree = radian * 180 / 3.14159265358979323846;

#endif // TRACK
		///////////////トラッキング2///////////////
#ifdef TRACK2
		//自車フロント
		Mat ego_fr_bin = getBinFrame(frame, ego_fr_obj);//2値画像
		trackFilteredObject(ego_fr_obj, ego_fr_bin, 10, 300, HM_WIDTH * HM_HEIGHT / 1.5);

		//自車リア
		Mat ego_rr_bin = getBinFrame(frame, ego_rr_obj);//2値画像
		trackFilteredObject(ego_rr_obj, ego_rr_bin, 10, 300, HM_WIDTH * HM_HEIGHT / 1.5);

		//表示
		circle(frame, Point2d(ego_fr_obj.x, ego_fr_obj.y), 10, cv::Scalar(200, 0, 0), 5);
		circle(frame, Point2d(ego_rr_obj.x, ego_rr_obj.y), 10, cv::Scalar(0, 200, 0), 5);
		/*alt
		//相手フロント
		Mat alt_fr_bin = getBinFrame(frame, alt_fr_obj);//2値画像
		trackFilteredObject(alt_fr_obj, alt_fr_bin, 10, 300, HM_WIDTH * HM_HEIGHT / 1.5);

		//自車リア
		Mat alt_rr_bin = getBinFrame(frame, alt_rr_obj);//2値画像
		trackFilteredObject(alt_rr_obj, alt_rr_bin, 10, 300, HM_WIDTH * HM_HEIGHT / 1.5);

		//表示
		circle(frame, Point2d(alt_fr_obj.x, alt_fr_obj.y), 10, cv::Scalar(200, 0, 0), 5);
		circle(frame, Point2d(alt_rr_obj.x, alt_rr_obj.y), 10, cv::Scalar(0, 200, 0), 5);
		*/
		imshow("fr_binframe", ego_fr_bin);
		imshow("rr_binframe", ego_rr_bin);
		/*alt
		imshow("fr_binframe", alt_fr_bin);
		imshow("rr_binframe", alt_rr_bin);
		*/
		imshow("tracker", frame);
		///////////////座標から実世界の距離へ変換///////////////
		Point2f car_a_ce_pos = Point2f((ego_fr_obj.x + ego_rr_obj.x) / 2, (ego_fr_obj.y + ego_rr_obj.y) / 2);//自車の重心
		double car_a_x = (car_a_ce_pos.x / HM_WIDTH) * REAL_WIDTH;
		double car_a_y = (car_a_ce_pos.y / HM_HEIGHT) * REAL_HEIGHT;
		double car_a_radian = atan2(ego_rr_obj.y - ego_fr_obj.y, ego_fr_obj.x - ego_rr_obj.x);//反時計回りを正、-pi～pi
		//double car_a_degree = car_a_radian * 180 / 3.14159265358979323846;

		/*alt
		Point2f car_b_ce_pos = Point2f((alt_fr_obj.x + alt_rr_obj.x) / 2, (alt_fr_obj.y + alt_rr_obj.y) / 2);//自車の重心
		double car_b_x = (car_b_ce_pos.x / HM_WIDTH) * REAL_WIDTH;
		double car_b_y = (car_b_ce_pos.y / HM_HEIGHT) * REAL_HEIGHT;
		double car_b_radian = atan2(alt_rr_obj.y - alt_fr_obj.y, alt_fr_obj.x - alt_rr_obj.x);//反時計回りを正、-pi～pi
		//double car_b_degree = car_a_radian * 180 / 3.14159265358979323846;
		*/

#endif // TRACK2

		///////////////勝敗、アイテム判定///////////////
		//////
		//////
		//////
		///////////////データ送信///////////////
		//
		sprintf_s(value_car_a_x,"%.3lf", car_a_x); //変数の値も含めた表示したい文字列
		sprintf_s(value_car_a_y,"%.3lf", car_a_y); //変数の値も含めた表示したい文字列
		sprintf_s(value_car_a_radian,"%.2lf", abs(car_a_radian)); //変数の値も含めた表示したい文字列
		/*alt
		sprintf_s(value_car_b_x, "%.3lf", car_b_x); //変数の値も含めた表示したい文字列
		sprintf_s(value_car_b_y, "%.3lf", car_b_y); //変数の値も含めた表示したい文字列
		sprintf_s(value_car_b_radian, "%.2lf", abs(car_b_radian)); //変数の値も含めた表示したい文字列
		*/

		string st_car_a_x = string(value_car_a_x);
		st_car_a_x.replace(1,1, "");

		string st_car_a_y = string(value_car_a_y);
		st_car_a_y.replace(1, 1, "");

		string st_car_a_radian = string(value_car_a_radian);
		
		//ラジアンの符号処理　
		if (car_a_radian > 0)
		{
			st_car_a_radian.replace(0, 0, "0");	//ラジアンが正の時先頭に"0"を追加
			st_car_a_radian.replace(2, 1, "");
		}
		else{
			st_car_a_radian.replace(0, 0, "1");	//ラジアンが負の時先頭に"1"を追加
			st_car_a_radian.replace(2, 1, "");


		}
		


		//string str2send = st_car_a_x + st_car_a_y + st_car_a_radian;	//まとめ
		string str2send = st_car_a_x + st_car_a_y + st_car_a_radian+ st_car_a_x + st_car_a_y + st_car_a_radian;	//まとめ 24moji

		int len = str2send.length();
		char* cstr = new char[len + 1];
		memcpy(cstr, str2send.c_str(), len + 1);

		sen.send(cstr);	//	 xxxxyyyy0/1rrr(0:+　1:-　x:xx.xx,y:yy.yy,rad:±rr.r

		//////
		//////
		///////////////その他///////////////
		int key = waitKey(1);
		if (key == 113)//qボタンが押されたとき終了
		{
			sen.send("q"); //制御終了
			break;
		}

		end = std::chrono::system_clock::now();  // 計測終了時間　
		double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換


		//表示
		std::cout << "x:" << car_a_x << " y:" << car_a_y << " rad:" << car_a_radian ;
		std::cout << " \t elapsed_time:" << elapsed << endl;
		std::cout << "send:" << str2send;
		std::cout << " \t elapsed_time:" << elapsed << endl;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//手動で4点を選ぶ、透視変換行列の算出
//左クリックで、左上→右上→右下→左下の順に指定
Mat man_getPerspectiveTransform(Mat frame) {

	mouseParam mouseEvent;
	// 変換前の画像での座標
	Point2f lefttop;
	Point2f righttop;
	Point2f leftbottom;
	Point2f rightbottom;

	imshow("manhomo", frame);
	setMouseCallback("manhomo", CallBackFunc, &mouseEvent);
	int c = 0;
	while (1) {
		imshow("manhomo", frame);
		cv::waitKey(1);
		if (mouseEvent.event == cv::EVENT_LBUTTONDOWN) {
			std::cout << mouseEvent.x << " , " << mouseEvent.y << std::endl;
			Point2f poi = Point2f(mouseEvent.x, mouseEvent.y);
			switch (c % 4)
			{
			case 0:
				lefttop = poi;
				circle(frame, poi, 5, cv::Scalar(200, 0, 0), -1);
				break;
			case 1:
				righttop = poi;
				circle(frame, poi, 5, cv::Scalar(0, 200, 0), -1);
				break;
			case 2:
				rightbottom = poi;
				circle(frame, poi, 5, cv::Scalar(0, 0, 200), -1);
				break;
			case 3:
				leftbottom = poi;
				circle(frame, poi, 5, cv::Scalar(200, 0, 200), -1);
				break;
			default:
				break;
			}
			imshow("manhomo", frame);
			cv::waitKey(300);
			c++;
		}
		//4回目で抜ける
		if (c >= 4)
			break;
	}
	destroyWindow("manhomo");

	const Point2f src_pt[] = { lefttop, leftbottom,righttop,rightbottom };
	// 変換後の画像での座標
	const Point2f dst_pt[] = { Point2f(0,0),Point2f(0,HM_HEIGHT),Point2f(HM_WIDTH, 0),Point2f(HM_WIDTH,HM_HEIGHT) };

	return  getPerspectiveTransform(src_pt, dst_pt);
}
//コールバック関数
void CallBackFunc(int eventType, int x, int y, int flags, void* userdata)
{
	mouseParam *ptr = static_cast<mouseParam*> (userdata);

	ptr->x = x;
	ptr->y = y;
	ptr->event = eventType;
	ptr->flags = flags;
}

//arucoタグを使った透視変換行列の算出
Mat ar_getPerspectiveTransform(Mat frame) {

	//透視変換前の4隅座標
	Point2f lefttop;
	Point2f righttop;
	Point2f leftbottom;
	Point2f rightbottom;

	vector<int> markerIds;
	vector<vector<Point2f>> markerCorners;
	Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_ARUCO_ORIGINAL);

	aruco::detectMarkers(frame, dictionary, markerCorners, markerIds);


	for (unsigned int i = 0; i < markerIds.size(); i++) {
		if (markerIds[i] == 0)
		{
			lefttop = markerCorners[i][2];
			circle(frame, lefttop, 2, Scalar(0, 255, 0), 3, 4);
		}
		if (markerIds[i] == 1)
		{
			righttop = markerCorners[i][3];//Point2f(Markers[i][0].x, Markers[i][2].y);
			circle(frame, righttop, 2, Scalar(255, 0, 0), 3, 4);
		}

		if (markerIds[i] == 2)
		{
			rightbottom = markerCorners[i][0];
			circle(frame, rightbottom, 2, Scalar(255, 0, 255), 3, 4);
		}

		if (markerIds[i] == 3)
		{
			leftbottom = markerCorners[i][1];
			circle(frame, leftbottom, 2, Scalar(0, 255, 255), 3, 4);
		}
	}
	//表示
	Mat cpframe;
	frame.copyTo(cpframe);
	aruco::drawDetectedMarkers(cpframe, markerCorners, markerIds);
	imshow("marker", cpframe);

	// 変換前の画像での座標
	const Point2f src_pt[] = { lefttop, leftbottom,righttop,rightbottom };
	// 変換後の画像での座標
	const Point2f dst_pt[] = { Point2f(0,0),Point2f(0,HM_HEIGHT),Point2f(HM_WIDTH, 0),Point2f(HM_WIDTH,HM_HEIGHT) };

	return  getPerspectiveTransform(src_pt, dst_pt);
}

//objで指定されたしきい値をもとに2値化（スムージング込み）
Mat getBinFrame(Mat rgbframe, struct TrackingObj &obj) {
	Mat result;
	//RGB→HSV
	cvtColor(rgbframe, result, COLOR_BGR2HSV);
	//HSV抽出
	inRange(result, Scalar(obj.H_MIN, obj.S_MIN, obj.V_MIN), Scalar(obj.H_MAX, obj.S_MAX, obj.V_MAX), result);
	//スムージング
	morphOps(result);
	return result;
}

void morphOps(Mat &thresh) {

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	cv::erode(thresh, thresh, erodeElement);
	cv::erode(thresh, thresh, erodeElement);


	cv::dilate(thresh, thresh, dilateElement);
	cv::dilate(thresh, thresh, dilateElement);

}


bool trackFilteredObject(struct TrackingObj &obj, Mat binframe, int max_num_obj, int min_obj_area, int max_obj_area) {

	//these two vectors needed for output of findContours
	vector< vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(binframe, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects < max_num_obj) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				//重心を求める
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area > min_obj_area && area<max_obj_area && area>refArea) {
					obj.x = moment.m10 / area;
					obj.y = moment.m01 / area;
					objectFound = true;
					refArea = area;
				}
				else objectFound = false;
			}

		}
		else {
			//コンソールにカメラ設定の出力
			std::cout << "TOO MUCH NOISE! ADJUST FILTER" << endl;

		}
	}
	return objectFound;
}



