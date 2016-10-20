#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>

void findMatchings(cv::Mat&, cv::Mat&, std::vector<cv::Point2f>, std::vector<cv::Point2f>);

static const char* main_window = "OpenCV Window";
static const char* tb_window = "Trackbars Window";

static const char* img_left_path = "..\\img_originals\\I_Left.jpg";
static const char* img_right_path = "..\\img_originals\\I_Right.jpg";

using namespace cv;

int main(char argc, char* argv[])
{
	auto Img_left_orig = imread(img_left_path);
	auto Img_right_orig = imread(img_right_path);
	std::vector<Point2f> P_A, P_B;



	//Display
	imshow("Left_orig", Img_left_orig);
	imshow("Right_orig", Img_right_orig);

	waitKey(0);
}

void findMatchings(Mat& left, Mat& right, std::vector<Point2f> P_A, std::vector<Point2f> P_B)
{
	Mat temp_left, temp_right;
	Mat status, err;

	goodFeaturesToTrack(left, temp_left, 10, 0, 0);
	goodFeaturesToTrack(right, temp_right, 10, 0, 0);

	calcOpticalFlowPyrLK(left, right, temp_left, temp_right, status, err);
}