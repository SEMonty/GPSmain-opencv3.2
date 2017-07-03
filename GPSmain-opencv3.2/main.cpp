// aruco_opencv3.2.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

using namespace std;
using namespace cv;
//�K�����ۂ��ϐ�
int camera_num = 0; //�r�f�I���͔ԍ�
string video_name = "video\\4_720_30fps_��.wmv"; //�r�f�I�̖��O
//string video_name = "video\\test1.wmv"; //�r�f�I�̖��O

bool online = true; //�J�����Ftrue�@�r�f�I:false

int CM_WIDTH = 1280; //�J�����A�r�f�I�̓��͉摜�T�C�Y
int CM_HEIGHT = 720;

float HM_WIDTH = 520;	//�����ϊ���̉摜�T�C�Y
float HM_HEIGHT = 275;

float REAL_WIDTH = 5.2;  //���ۂ̃t�B�[���h�T�C�Y(m)
float REAL_HEIGHT = 2.75;
//�r�f�I�����o���ݒ�
const int fourcc = VideoWriter::fourcc('W', 'M', 'V', '2');
string filename = "output.wmv";

//#define ARHOMO			//ARUCO�œ����ϊ�
#define MANHOMO			//�蓮�œ����ϊ�
//#define SAVEVIDEO			//����ۑ�
//#define TRACK			//�g���b�L���O����
#define TRACK2			//�g���b�L���O2


int main()
{

	///////////////�Q�[���J�n�҂��A�J�n���߂̑��M///////////////
	//���M����
	MySender sen;
	sen.connect("192.168.10.3", 6003);		//"���M�����IP",�|�[�g

	char value_car_a_x[256]; //�ꎞ�I�ȕϐ�
	char value_car_a_y[256]; //�ꎞ�I�ȕϐ�
	char value_car_a_radian[256]; //�ꎞ�I�ȕϐ�
	/*alt
	char value_car_b_x[256]; //�ꎞ�I�ȕϐ�
	char value_car_b_y[256]; //�ꎞ�I�ȕϐ�
	char value_car_b_radian[256]; //�ꎞ�I�ȕϐ�
	*/
	//�J�n���߂̑��M
	sen.send("s");	//s:�J�n
	//////////////////////�J�����ڑ��ƍŏ��̃t���[���̓ǂݏo��//////////////////
	//����̓ǂݍ���
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

	//cap.set(CAP_PROP_FRAME_WIDTH, CWIDTH);	//�T�C�Y�w��
	//cap.set(CAP_PROP_FRAME_HEIGHT, CHEIGHT);	//�T�C�Y�w��
	//cap.set(CAP_PROP_FPS, 30);				//FPS�ݒ�
	//�R���\�[���ɃJ�����ݒ�̏o��
	std::cout << "Web Camera settings:" << endl;
	std::cout << "Width:" + to_string(cap.get(CAP_PROP_FRAME_WIDTH)) << endl;
	std::cout << "Height:" + to_string(cap.get(CAP_PROP_FRAME_HEIGHT)) << endl;
	std::cout << "FPS:" + to_string(cap.get(CAP_PROP_FPS)) << endl;
	std::cout << "Exposure:" + to_string(cap.get(CAP_PROP_EXPOSURE)) << endl;
	std::cout << "Gain:" + to_string(cap.get(CAP_PROP_GAIN)) << endl;
	std::cout << "Iso_speed:" + to_string(cap.get(CAP_PROP_ISO_SPEED)) << endl;
	std::cout << "speed:" + to_string(cap.get(CAP_PROP_SPEED)) << endl;

	//�����t���[���̓ǂݏo��
	Mat fistframe;
	cap >> fistframe;

	//�J�����̏ꍇ�͉摜�m�F�@n�L�[�Ŏ���
	while (cap.grab() && online) {
		cap.retrieve(fistframe);
		///////////////���̑�///////////////
		int key = waitKey(1);
		if (key == 110)
		{
			break;
		}
		cv::imshow("camera", fistframe);
	}
	destroyWindow("camera");
	//////////////�����ϊ��s��̎Z�o///////////////////////////////
	Mat homography_matrix;
#ifdef ARHOMO

	homography_matrix = ar_getPerspectiveTransform(fistframe);

	warpPerspective(fistframe, fistframe, homography_matrix, Size(WIDTH, HEIGHT));
#endif
	homography_matrix = man_getPerspectiveTransform(fistframe);

	warpPerspective(fistframe, fistframe, homography_matrix, Size(HM_WIDTH, HM_HEIGHT));
#ifdef MANHOMO	//�蓮�z���O���t�B
#endif // MANHOMO


	//////////////����擾�̏���///////////////////////////////
#ifdef SAVEVIDEO
	double fps = cap.get(CV_CAP_PROP_FPS);
	Size size = Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	VideoWriter writer(filename, fourcc, fps, size);
#endif//SAVEVIDEO

	//////////////�g���b�L���O�̏���///////////////////////////////
#ifdef TRACK
	// �ŏ��̃t���[������ǐՑΏۂ�I��

	vector<Rect2d> rois;
	selectROI("tracker", fistframe, rois);

	// Tracker�̐���/������
	MultiTracker trackerKCF("KCF");
	trackerKCF.add(fistframe, rois);
#endif // TRACK
#ifdef TRACK2
	//�g���b�L���O����F�̎w��
	struct TrackingObj ego_fr_obj = { 0, 65, 93, 256, 37, 256,0,0 };	//���ԃt�����g�̃I�u�W�F�N�g�i�s���N�j
	struct TrackingObj ego_rr_obj = { 40, 93, 116, 256, 109, 179,0,0 };	//���ԃ��A�̃I�u�W�F�N�g�i���F�j
	/*alt
		 { 146, 231, 7, 256, 65, 231,0,0 };	//���ԃt�����g�̃I�u�W�F�N�g�i�s���N�j
		 { 21, 56, 49, 256, 144, 227,0,0 };	//���ԃ��A�̃I�u�W�F�N�g�i���F�j
	struct TrackingObj alt_fr_obj = { 0, 65, 93, 256, 37, 256,0,0 };	//����t�����g�̃I�u�W�F�N�g�i�I�����W�j	
	struct TrackingObj alt_rr_obj = { 40, 93, 116, 256, 109, 179,0,0 };	//���胊�A�̃I�u�W�F�N�g�i�΁j
	*/
#endif // TRACK2



	//���C�����[�v
	while (cap.grab()) {
		Mat frame;

		std::chrono::system_clock::time_point  start, end;
		start = std::chrono::system_clock::now(); // �v���J�n����
		///////////////�J�����摜�擾///////////////
		cap.retrieve(frame);
		imshow("frame", frame);

#ifdef SAVEVIDEO		//����̕ۑ�
		writer << frame;//�t���[���𓮉�ɕۑ�
#endif	

		///////////////�c�ݕ␳(�\��)///////////////
		//////
		//////
		//////
		//////
		//////
		///////////////�����ϊ�///////////////
#ifdef MANHOMO || ARHOMO
		warpPerspective(frame, frame, homography_matrix, Size(HM_WIDTH, HM_HEIGHT));
		imshow("homoed", frame);
#endif
		///////////////�g���b�L���O1///////////////
#ifdef TRACK
		trackerKCF.update(frame, rois);

		//�\��
		for (unsigned int i = 0; i < rois.size(); i++) {
			rectangle(frame, rois[i], Scalar(0, (80 * (i % 4)), 0), 2, 1);
		}
		imshow("tracker", frame);

		///////////////���W��������E�̋����֕ϊ�///////////////
		Point2f car_a_fr_pos = Point2f(rois[0].x + rois[0].width / 2, rois[0].y + rois[0].height / 2);
		Point2f car_a_rr_pos = Point2f(rois[1].x + rois[1].width / 2, rois[1].y + rois[1].height / 2);
		Point2f car_a_ce_pos = Point2f((car_a_fr_pos.x + car_a_rr_pos.x) / 2, (car_a_fr_pos.y + car_a_rr_pos.y) / 2);

		double car_a_x = (car_a_ce_pos.x / HM_WIDTH) * REAL_WIDTH;
		double car_a_y = (car_a_ce_pos.y / HM_HEIGHT) * REAL_HEIGHT;
		double radian = atan2(car_a_rr_pos.y - car_a_fr_pos.y, car_a_fr_pos.x - car_a_rr_pos.x);//�����v���𐳁A-pi�`pi
		double car_a_degree = radian * 180 / 3.14159265358979323846;

#endif // TRACK
		///////////////�g���b�L���O2///////////////
#ifdef TRACK2
		//���ԃt�����g
		Mat ego_fr_bin = getBinFrame(frame, ego_fr_obj);//2�l�摜
		trackFilteredObject(ego_fr_obj, ego_fr_bin, 10, 300, HM_WIDTH * HM_HEIGHT / 1.5);

		//���ԃ��A
		Mat ego_rr_bin = getBinFrame(frame, ego_rr_obj);//2�l�摜
		trackFilteredObject(ego_rr_obj, ego_rr_bin, 10, 300, HM_WIDTH * HM_HEIGHT / 1.5);

		//�\��
		circle(frame, Point2d(ego_fr_obj.x, ego_fr_obj.y), 10, cv::Scalar(200, 0, 0), 5);
		circle(frame, Point2d(ego_rr_obj.x, ego_rr_obj.y), 10, cv::Scalar(0, 200, 0), 5);
		/*alt
		//����t�����g
		Mat alt_fr_bin = getBinFrame(frame, alt_fr_obj);//2�l�摜
		trackFilteredObject(alt_fr_obj, alt_fr_bin, 10, 300, HM_WIDTH * HM_HEIGHT / 1.5);

		//���ԃ��A
		Mat alt_rr_bin = getBinFrame(frame, alt_rr_obj);//2�l�摜
		trackFilteredObject(alt_rr_obj, alt_rr_bin, 10, 300, HM_WIDTH * HM_HEIGHT / 1.5);

		//�\��
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
		///////////////���W��������E�̋����֕ϊ�///////////////
		Point2f car_a_ce_pos = Point2f((ego_fr_obj.x + ego_rr_obj.x) / 2, (ego_fr_obj.y + ego_rr_obj.y) / 2);//���Ԃ̏d�S
		double car_a_x = (car_a_ce_pos.x / HM_WIDTH) * REAL_WIDTH;
		double car_a_y = (car_a_ce_pos.y / HM_HEIGHT) * REAL_HEIGHT;
		double car_a_radian = atan2(ego_rr_obj.y - ego_fr_obj.y, ego_fr_obj.x - ego_rr_obj.x);//�����v���𐳁A-pi�`pi
		//double car_a_degree = car_a_radian * 180 / 3.14159265358979323846;

		/*alt
		Point2f car_b_ce_pos = Point2f((alt_fr_obj.x + alt_rr_obj.x) / 2, (alt_fr_obj.y + alt_rr_obj.y) / 2);//���Ԃ̏d�S
		double car_b_x = (car_b_ce_pos.x / HM_WIDTH) * REAL_WIDTH;
		double car_b_y = (car_b_ce_pos.y / HM_HEIGHT) * REAL_HEIGHT;
		double car_b_radian = atan2(alt_rr_obj.y - alt_fr_obj.y, alt_fr_obj.x - alt_rr_obj.x);//�����v���𐳁A-pi�`pi
		//double car_b_degree = car_a_radian * 180 / 3.14159265358979323846;
		*/

#endif // TRACK2

		///////////////���s�A�A�C�e������///////////////
		//////
		//////
		//////
		///////////////�f�[�^���M///////////////
		//
		sprintf_s(value_car_a_x,"%.3lf", car_a_x); //�ϐ��̒l���܂߂��\��������������
		sprintf_s(value_car_a_y,"%.3lf", car_a_y); //�ϐ��̒l���܂߂��\��������������
		sprintf_s(value_car_a_radian,"%.2lf", abs(car_a_radian)); //�ϐ��̒l���܂߂��\��������������
		/*alt
		sprintf_s(value_car_b_x, "%.3lf", car_b_x); //�ϐ��̒l���܂߂��\��������������
		sprintf_s(value_car_b_y, "%.3lf", car_b_y); //�ϐ��̒l���܂߂��\��������������
		sprintf_s(value_car_b_radian, "%.2lf", abs(car_b_radian)); //�ϐ��̒l���܂߂��\��������������
		*/

		string st_car_a_x = string(value_car_a_x);
		st_car_a_x.replace(1,1, "");

		string st_car_a_y = string(value_car_a_y);
		st_car_a_y.replace(1, 1, "");

		string st_car_a_radian = string(value_car_a_radian);
		
		//���W�A���̕��������@
		if (car_a_radian > 0)
		{
			st_car_a_radian.replace(0, 0, "0");	//���W�A�������̎��擪��"0"��ǉ�
			st_car_a_radian.replace(2, 1, "");
		}
		else{
			st_car_a_radian.replace(0, 0, "1");	//���W�A�������̎��擪��"1"��ǉ�
			st_car_a_radian.replace(2, 1, "");


		}
		


		//string str2send = st_car_a_x + st_car_a_y + st_car_a_radian;	//�܂Ƃ�
		string str2send = st_car_a_x + st_car_a_y + st_car_a_radian+ st_car_a_x + st_car_a_y + st_car_a_radian;	//�܂Ƃ� 24moji

		int len = str2send.length();
		char* cstr = new char[len + 1];
		memcpy(cstr, str2send.c_str(), len + 1);

		sen.send(cstr);	//	 xxxxyyyy0/1rrr(0:+�@1:-�@x:xx.xx,y:yy.yy,rad:�}rr.r

		//////
		//////
		///////////////���̑�///////////////
		int key = waitKey(1);
		if (key == 113)//q�{�^���������ꂽ�Ƃ��I��
		{
			sen.send("q"); //����I��
			break;
		}

		end = std::chrono::system_clock::now();  // �v���I�����ԁ@
		double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //�����ɗv�������Ԃ��~���b�ɕϊ�


		//�\��
		std::cout << "x:" << car_a_x << " y:" << car_a_y << " rad:" << car_a_radian ;
		std::cout << " \t elapsed_time:" << elapsed << endl;
		std::cout << "send:" << str2send;
		std::cout << " \t elapsed_time:" << elapsed << endl;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//�蓮��4�_��I�ԁA�����ϊ��s��̎Z�o
//���N���b�N�ŁA���と�E�と�E���������̏��Ɏw��
Mat man_getPerspectiveTransform(Mat frame) {

	mouseParam mouseEvent;
	// �ϊ��O�̉摜�ł̍��W
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
		//4��ڂŔ�����
		if (c >= 4)
			break;
	}
	destroyWindow("manhomo");

	const Point2f src_pt[] = { lefttop, leftbottom,righttop,rightbottom };
	// �ϊ���̉摜�ł̍��W
	const Point2f dst_pt[] = { Point2f(0,0),Point2f(0,HM_HEIGHT),Point2f(HM_WIDTH, 0),Point2f(HM_WIDTH,HM_HEIGHT) };

	return  getPerspectiveTransform(src_pt, dst_pt);
}
//�R�[���o�b�N�֐�
void CallBackFunc(int eventType, int x, int y, int flags, void* userdata)
{
	mouseParam *ptr = static_cast<mouseParam*> (userdata);

	ptr->x = x;
	ptr->y = y;
	ptr->event = eventType;
	ptr->flags = flags;
}

//aruco�^�O���g���������ϊ��s��̎Z�o
Mat ar_getPerspectiveTransform(Mat frame) {

	//�����ϊ��O��4�����W
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
	//�\��
	Mat cpframe;
	frame.copyTo(cpframe);
	aruco::drawDetectedMarkers(cpframe, markerCorners, markerIds);
	imshow("marker", cpframe);

	// �ϊ��O�̉摜�ł̍��W
	const Point2f src_pt[] = { lefttop, leftbottom,righttop,rightbottom };
	// �ϊ���̉摜�ł̍��W
	const Point2f dst_pt[] = { Point2f(0,0),Point2f(0,HM_HEIGHT),Point2f(HM_WIDTH, 0),Point2f(HM_WIDTH,HM_HEIGHT) };

	return  getPerspectiveTransform(src_pt, dst_pt);
}

//obj�Ŏw�肳�ꂽ�������l�����Ƃ�2�l���i�X���[�W���O���݁j
Mat getBinFrame(Mat rgbframe, struct TrackingObj &obj) {
	Mat result;
	//RGB��HSV
	cvtColor(rgbframe, result, COLOR_BGR2HSV);
	//HSV���o
	inRange(result, Scalar(obj.H_MIN, obj.S_MIN, obj.V_MIN), Scalar(obj.H_MAX, obj.S_MAX, obj.V_MAX), result);
	//�X���[�W���O
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

				//�d�S�����߂�
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
			//�R���\�[���ɃJ�����ݒ�̏o��
			std::cout << "TOO MUCH NOISE! ADJUST FILTER" << endl;

		}
	}
	return objectFound;
}



