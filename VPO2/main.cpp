#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"


void updateCanny_Callback(int val, void* ptr);

static const char* main_window = "OpenCV Window";
static const char* tb_window = "Trackbars Window";

static const char* lenna_Path = "..\\img_originals\\Lenna.png";
static const char* grassAndSky_Path = "..\\img_originals\\grassAndSky.jpg";
static const char* hulk_Path = "..\\img_originals\\hulk.jpg";
static const char* balls_Path = "..\\img_originals\\Colorful_Super_balls.jpg";

using namespace cv;

int cannyThreshold = 30;
int blurSize = 5;
Mat Img_grey;
Mat BlurCanny;

int main(char argc, char* argv[])
{
	Mat Img_orig = imread(balls_Path);
	Mat Sobel1, Sobel2, Sobel3;
	Mat Surimpression;
	Mat HoughCircle;

	//Greyscale
	cvtColor(Img_orig, Img_grey, CV_BGR2GRAY);

	//Sobel
	Sobel(Img_grey, Sobel1, Img_grey.depth(), 1, 0);
	Sobel(Img_grey, Sobel2, Img_grey.depth(), 0, 1);
	Sobel(Img_grey, Sobel3, Img_grey.depth(), 1, 1);

	//Canny/Blur + Changing values during execution (Trackbars)
	namedWindow(tb_window);
	updateCanny_Callback(0, nullptr);
	createTrackbar("CannyThreshold", tb_window, &cannyThreshold, 255, updateCanny_Callback, &cannyThreshold);
	createTrackbar("BlurSize", tb_window, &blurSize, 35, updateCanny_Callback, &blurSize);

	//Surimpression
	cvtColor(Img_grey, Surimpression, CV_GRAY2BGR);
	Surimpression.setTo(Scalar(200, 0, 0), BlurCanny);

	//Hough_Circle
	cvtColor(Img_orig, HoughCircle, CV_BGR2GRAY);
	std::vector<Vec3f> CirclesVec;
	HoughCircles(HoughCircle, CirclesVec, CV_HOUGH_GRADIENT, 1, 20, 55, 55, 0, 1000);
	cvtColor(HoughCircle, HoughCircle, CV_GRAY2BGR);

	//for(auto i=0; i < CirclesVec.size(); ++i)
	//{
	//	circle(HoughCircle, Point(CirclesVec[i].val[0], CirclesVec[i].val[1]), CirclesVec[i].val[2], Scalar(255, 0, 0));
	//}
	for(auto it = CirclesVec.begin(); it != CirclesVec.end(); ++it)
	{
		circle(HoughCircle, Point((*it)[0], (*it)[1]), (*it)[2], Scalar(0, 0, 200), 2);
	}

	//Display
	imshow("Original", Img_orig);
	imshow("Sobel1", Sobel1);
	imshow("Sobel2", Sobel2);
	imshow("Sobel3", Sobel3);
	imshow("BlurCanny", BlurCanny);
	imshow("Surimpression", Surimpression);
	imshow("HoughCircle", HoughCircle);

	waitKey(0);
}

void updateCanny_Callback(int val, void* ptr)
{
	if (!(blurSize > 1 && blurSize % 2 != 0))
		blurSize++;

	blur(Img_grey, BlurCanny, Size(blurSize, blurSize));
	Canny(BlurCanny, BlurCanny, cannyThreshold / 2, cannyThreshold);
	imshow("BlurCanny", BlurCanny);
}