// aruco_opencv3.2.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

using namespace std;
using namespace cv;
//�K�����ۂ��ϐ�
int video_num = 0; //�r�f�I���͔ԍ�

float WIDTH = 451;	//�����ϊ���̉摜�T�C�Y
float HEIGHT = 416;

float REAL_WIDTH = 451;  //���ۂ̃t�B�[���h�T�C�Y(cm)
float REAL_HEIGHT = 416;
//#define HOMO			//�����ϊ�����鎞
int main()
{

	///////////////�Q�[���J�n�҂��A�J�n���߂̑��M///////////////
	//////
	//////
	//////
	//////////////////////�J�����ڑ��ƍŏ��̃t���[���̓ǂݏo��//////////////////
	VideoCapture cap(video_num);
	Mat fistframe;
	Mat homography_matrix;
	//////////////����̏���///////////////////////////////
	//////////////�����ϊ��̏���///////////////////////////////
	cap >> fistframe;
#ifdef HOMO
	while (cap.grab()) {
		cap.retrieve(fistframe);

		homography_matrix = my_getPerspectiveTransform(fistframe);
		warpPerspective(fistframe, fistframe, homography_matrix, Size(WIDTH, HEIGHT));
		cv::imshow("homography", fistframe);

		///////////////���̑�///////////////
		int key = waitKey(1);
		if (key == 110)//n�{�^���������ꂽ�玟��
		{
			break;
		}
	}
#endif
	////�r�f�I�t�@�C���̃I�[�v���ƃr�f�I�L���v�`���N���X�̃C���X�^���X����
	//VideoCapture cap("video.avi");
	//"test1.avi"�Ƃ����t�@�C�����ō쐬����
	VideoWriter writer("test1.avi", CV_FOURCC_DEFAULT, 10, Size(640, 480), true);

	//////////////�g���b�L���O�̏���///////////////////////////////
	// Tracker�̐���
	MultiTracker trackerKCF("KCF");
	// �ŏ��̃t���[������ǐՑΏۂ�I��

	vector<Rect2d> rois;
	selectROI("tracker", fistframe, rois); // �}�E�X�Œ��S��I�����ăh���b�O

										   // Tracker�̏�����
	trackerKCF.add(fistframe, rois);

	//���C�����[�v
	while (cap.grab()) {
		Mat frame;
		///////////////�J�����摜�擾///////////////
		cap.retrieve(frame);
		///////////////�c�ݕ␳(�\��)///////////////
		//////
		//////
		//////
		///////////////�����ϊ�///////////////
#ifdef HOMO
		warpPerspective(frame, frame, homography_matrix, Size(WIDTH, HEIGHT));
		cv::imshow("homography", frame);
#endif
		//�\��
		///////////////�F���o(�\��)///////////////
		//////http://d.hatena.ne.jp/Kazzz/20130118/p1
		//////colorExtraction(&frame, &frame, CV_BGR2HSV, 90, 210, 100, 255, 70, 255);
		//////
		///////////////�g���b�L���O///////////////
		trackerKCF.update(frame, rois);
		//�\��
		Mat cpframe;
		frame.copyTo(cpframe);

		for (unsigned int i = 0; i < rois.size(); i++) {
			cv::rectangle(cpframe, rois[i], Scalar(0, (80 * (i % 4)), 0), 2, 1);
		}
		cv::imshow("tracker", cpframe);
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
		///////////////���s�A�A�C�e������///////////////
		//////
		//////
		//////
		///////////////�f�[�^���M///////////////
		//////
		//////
		//////
		///////////////���̑�///////////////

		//�t���[���̕ۑ�
		writer << frame;


		int key = waitKey(1);
		if (key == 113)//q�{�^���������ꂽ�Ƃ��I��
		{
			break;
		}
	}
}

//aruco�^�O���g���������ϊ��s��̎Z�o
Mat my_getPerspectiveTransform(Mat frame) {

	//�����ϊ��O��4�����W
	Point2f lefttop;
	Point2f righttop;
	Point2f leftbottom;
	Point2f rightbottom;

	std::vector<int> markerIds;
	std::vector<std::vector<Point2f>> markerCorners;
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
	cv::imshow("marker", cpframe);

	// �ϊ��O�̉摜�ł̍��W
	const Point2f src_pt[] = { lefttop, leftbottom,righttop,rightbottom };
	// �ϊ���̉摜�ł̍��W
	const Point2f dst_pt[] = { Point2f(0,0),Point2f(0,HEIGHT),Point2f(WIDTH, 0),Point2f(WIDTH,HEIGHT) };

	return  getPerspectiveTransform(src_pt, dst_pt);
}
//�l�b�g����E���Ă����F���o�@http://qiita.com/crakaC/items/65fab9d0b0ac29e68ab6
void colorExtraction(cv::Mat* src, cv::Mat* dst,
	int code,
	int ch1Lower, int ch1Upper,
	int ch2Lower, int ch2Upper,
	int ch3Lower, int ch3Upper
)
{
	cv::Mat colorImage;
	int lower[3];
	int upper[3];

	cv::Mat lut = cv::Mat(256, 1, CV_8UC3);

	cv::cvtColor(*src, colorImage, code);

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
	cv::LUT(colorImage, lut, colorImage);

	//Channel���ɕ���
	std::vector<cv::Mat> planes;
	cv::split(colorImage, planes);

	//�}�X�N���쐬
	cv::Mat maskImage;
	cv::bitwise_and(planes[0], planes[1], maskImage);
	cv::bitwise_and(maskImage, planes[2], maskImage);

	//�o��
	cv::Mat maskedImage;
	src->copyTo(maskedImage, maskImage);
	*dst = maskedImage;
}