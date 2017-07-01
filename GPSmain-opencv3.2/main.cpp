// aruco_opencv3.2.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

using namespace std;
using namespace cv;
//�K�����ۂ��ϐ�
int camera_num = 0; //�r�f�I���͔ԍ�
string video_name = "video\\4_720_30fps_��.wmv"; //�r�f�I�̖��O
//string video_name = "video\\test1.wmv"; //�r�f�I�̖��O

bool online = false; //�J�����Ftrue�@�r�f�I:false

int CWIDTH = 1280; //�J�����A�r�f�I�̓��͉摜�T�C�Y
int CHEIGHT = 720;

float WIDTH = 520;	//�����ϊ���̉摜�T�C�Y
float HEIGHT = 275;

float REAL_WIDTH = 5.2;  //���ۂ̃t�B�[���h�T�C�Y(m)
float REAL_HEIGHT = 2.75;
//�r�f�I�����o���ݒ�
const int fourcc = VideoWriter::fourcc('W', 'M', 'V', '3');
string filename = "output.wmv";

//#define ARHOMO			//ARUCO�œ����ϊ�
#define MANHOMO			//�蓮�œ����ϊ�
//#define SAVEVIDEO			//����
#define TRACK			//�g���b�L���O����
int main()
{
	///////////////�Q�[���J�n�҂��A�J�n���߂̑��M///////////////
	//////
	//////
	//////
	//////////////////////�J�����ڑ��ƍŏ��̃t���[���̓ǂݏo��//////////////////
	//����̓ǂݍ���
	VideoCapture cap;
	if (online)
		cap.open(camera_num);
	else
		cap.open(video_name);

	if (!cap.isOpened()) {
		cout << "Camera or video can not be opened" <<endl;
		cout << "Press enter to Exit" << endl;
		getchar(); //
		return -1;
	}

	//cap.set(CAP_PROP_FRAME_WIDTH, CWIDTH);	//�T�C�Y�w��
	//cap.set(CAP_PROP_FRAME_HEIGHT, CHEIGHT);	//�T�C�Y�w��
	//cap.set(CAP_PROP_FPS, 30);				//FPS�ݒ�
	//�R���\�[���ɃJ�����ݒ�̏o��
	cout << "Web Camera settings:" << endl;
	cout << "Width:" + to_string(cap.get(CAP_PROP_FRAME_WIDTH)) << endl;
	cout << "Height:" + to_string(cap.get(CAP_PROP_FRAME_HEIGHT)) << endl;
	cout << "FPS:" + to_string(cap.get(CAP_PROP_FPS)) << endl;
	cout << "Exposure:" + to_string(cap.get(CAP_PROP_EXPOSURE)) << endl;
	cout << "Gain:" + to_string(cap.get(CAP_PROP_GAIN)) << endl;
	cout << "Iso_speed:" + to_string(cap.get(CAP_PROP_ISO_SPEED)) << endl;
	cout << "speed:" + to_string(cap.get(CAP_PROP_SPEED)) << endl;
			
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
		imshow("camera", fistframe);
	}
	destroyWindow("camera");
	//////////////�����ϊ��s��̎Z�o///////////////////////////////
	Mat homography_matrix;
#ifdef ARHOMO
	
	homography_matrix = ar_getPerspectiveTransform(fistframe);

	warpPerspective(fistframe, fistframe, homography_matrix, Size(WIDTH, HEIGHT));
#endif
	homography_matrix = man_getPerspectiveTransform(fistframe);

	warpPerspective(fistframe, fistframe, homography_matrix, Size(WIDTH, HEIGHT));
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
	// Tracker�̐���
	MultiTracker trackerKCF("KCF");
	// �ŏ��̃t���[������ǐՑΏۂ�I��

	vector<Rect2d> rois;
	selectROI("tracker", fistframe, rois); // �}�E�X�Œ��S��I�����ăh���b�O

	Point2f car_a_fr_pos = Point2f(rois[0].x + rois[0].width / 2, rois[0].y + rois[0].height / 2);
	Point2f car_a_rr_pos = Point2f(rois[1].x + rois[1].width / 2, rois[1].y + rois[1].height / 2);

	Point2f car_a_ce_pos = Point2f((car_a_fr_pos.x + car_a_rr_pos.x) / 2, (car_a_fr_pos.y + car_a_rr_pos.y) / 2);

	// Tracker�̏�����
	trackerKCF.add(fistframe, rois);
#endif // TRACK

	//���C�����[�v
	while (cap.grab()) {
		Mat frame;
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
		///////////////�����ϊ�///////////////
#ifdef MANHOMO || ARHOMO
		warpPerspective(frame, frame, homography_matrix, Size(WIDTH, HEIGHT));
		imshow("homography", frame);
#endif
		//�\��
		///////////////�F���o(�\��)///////////////
		//////http://d.hatena.ne.jp/Kazzz/20130118/p1
		//////colorExtraction(&frame, &frame, CV_BGR2HSV, 90, 210, 100, 255, 70, 255);
		//////
		///////////////�g���b�L���O///////////////
#ifdef TRACK
		trackerKCF.update(frame, rois);
		//�\��
		Mat cpframe;
		frame.copyTo(cpframe);
		for (unsigned int i = 0; i < rois.size(); i++) {
			rectangle(cpframe, rois[i], Scalar(0, (80 * (i % 4)), 0), 2, 1);
		}
		imshow("tracker", cpframe);

		///////////////���W��������E�̋����֕ϊ�///////////////
		Point2f car_a_fr_pos = Point2f(rois[0].x + rois[0].width / 2, rois[0].y + rois[0].height / 2);
		Point2f car_a_rr_pos = Point2f(rois[1].x + rois[1].width / 2, rois[1].y + rois[1].height / 2);

		Point2f car_a_ce_pos = Point2f((car_a_fr_pos.x + car_a_rr_pos.x) / 2, (car_a_fr_pos.y + car_a_rr_pos.y) / 2);

		double car_a_x = (car_a_ce_pos.x / WIDTH) * REAL_WIDTH;
		double car_a_y = (car_a_ce_pos.y / HEIGHT) * REAL_HEIGHT;
		double radian = atan2(car_a_rr_pos.y - car_a_fr_pos.y, car_a_fr_pos.x - car_a_rr_pos.x);//�����v���𐳁A-pi�`pi
		double car_a_degree = radian * 180 / 3.14159265358979323846;
		//�\��
		cout << "x:" << car_a_x << " y:" << car_a_y << " deg:" << car_a_degree << endl;
		cout << "x:" << car_a_x << " y:" << car_a_y << " deg:" << car_a_degree << endl;

#endif // TRACK
		///////////////���s�A�A�C�e������///////////////
		//////
		//////
		//////
		///////////////�f�[�^���M///////////////
		//////
		//////
		//////
		///////////////���̑�///////////////

		int key = waitKey(1);
		if (key == 113)//q�{�^���������ꂽ�Ƃ��I��
		{
		
			break;
		}

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
			switch (c%4)
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
			cv::waitKey(100);
			c++;
		}
		//4��ڂŔ�����
		if (c >= 4)
			break;
	}
	destroyWindow("manhomo");

	const Point2f src_pt[] = { lefttop, leftbottom,righttop,rightbottom };
	// �ϊ���̉摜�ł̍��W
	const Point2f dst_pt[] = { Point2f(0,0),Point2f(0,HEIGHT),Point2f(WIDTH, 0),Point2f(WIDTH,HEIGHT) };

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
	const Point2f dst_pt[] = { Point2f(0,0),Point2f(0,HEIGHT),Point2f(WIDTH, 0),Point2f(WIDTH,HEIGHT) };

	return  getPerspectiveTransform(src_pt, dst_pt);
}

//�l�b�g����E���Ă����F���o�@http://qiita.com/crakaC/items/65fab9d0b0ac29e68ab6
void colorExtraction(Mat* src, Mat* dst,
	int code,
	int ch1Lower, int ch1Upper,
	int ch2Lower, int ch2Upper,
	int ch3Lower, int ch3Upper
)
{
	Mat colorImage;
	int lower[3];
	int upper[3];

	Mat lut = Mat(256, 1, CV_8UC3);

	cvtColor(*src, colorImage, code);

	lower[0] = ch1Lower;
	lower[1] = ch2Lower;
	lower[2] = ch3Lower;

	upper[0] = ch1Upper;
	upper[1] = ch2Upper;
	upper[2] = ch3Upper;

	for (int i = 0; i < 256; i++) {
		for (int k = 0; k < 3; k++) {
			if (lower[k] <= upper[k]) {
				if ((lower[k] <= i) && (i <= upper[k])) {
					lut.data[i*lut.step + k] = 255;
				}
				else {
					lut.data[i*lut.step + k] = 0;
				}
			}
			else {
				if ((i <= upper[k]) || (lower[k] <= i)) {
					lut.data[i*lut.step + k] = 255;
				}
				else {
					lut.data[i*lut.step + k] = 0;
				}
			}
		}
	}

	//LUT���g�p���ē�l��
	LUT(colorImage, lut, colorImage);

	//Channel���ɕ���
	vector<Mat> planes;
	split(colorImage, planes);

	//�}�X�N���쐬
	Mat maskImage;
	bitwise_and(planes[0], planes[1], maskImage);
	bitwise_and(maskImage, planes[2], maskImage);

	//�o��
	Mat maskedImage;
	src->copyTo(maskedImage, maskImage);
	*dst = maskedImage;
}


