#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

static const char* main_window = "OpenCV Window";

static const char* Lenna_Path = "..\\img_originals\\Lenna.png";
static const char* grassAndSky_Path = "..\\img_originals\\grassAndSky.jpg";
static const char* hulk_Path = "..\\img_originals\\hulk.jpg";

using namespace cv;

int main(char argc, char* argv[])
{
	Mat Img_orig = imread(hulk_Path);
	Mat Img_grey;
	Mat Img_Splitted[3];
	Mat Img_B, Img_G, Img_R;
	Mat Img_thresholded_Method1, Img_thresholded_Method2;
	Mat Img_Thresholded_Merged;
	Mat Img_SaturationTimes2;
	Mat Img_GreenMask;
	Mat Img_GreenMasked;

	//Greyscale
	cvtColor(Img_orig, Img_grey, CV_BGR2GRAY);

	//Splitted
	split(Img_orig, Img_Splitted);
	Img_Splitted[0].copyTo(Img_B);
	Img_Splitted[1].copyTo(Img_G);
	Img_Splitted[2].copyTo(Img_R);

	//Threshold
	//From greyscale image
	//Method 1 (overloaded '<, >' operators)
	Img_thresholded_Method1 = Img_grey > 100;
	//Method 2
	threshold(Img_grey, Img_thresholded_Method2, 100, 255, THRESH_BINARY);

	//From splitted images
	Mat Img_Splitted_Thresholded[3] = { Img_B > 100, Img_G > 100, Img_R > 100 };
	merge(Img_Splitted_Thresholded, 3, Img_Thresholded_Merged);

	//Saturation*2
	Mat Temp;
	Mat Temp_Splitted[3];
	cvtColor(Img_orig, Temp, CV_BGR2HSV);
	split(Temp, Temp_Splitted);
	Temp_Splitted[1] *= 2;
	merge(Temp_Splitted, 3, Img_SaturationTimes2);
	cvtColor(Img_SaturationTimes2, Img_SaturationTimes2, CV_HSV2BGR);

	//IsolateGreen
	/* 1 : Get green mask */
	Mat GreenMask_Splitted[3];
	cvtColor(Img_orig, Img_GreenMask, CV_BGR2HSV);
	split(Img_GreenMask, GreenMask_Splitted);
	Img_GreenMask = (GreenMask_Splitted[0] > 40) & (GreenMask_Splitted[0] < 80);
	/* 2 : Apply mask on image's saturation to grey part of the image */
	GreenMask_Splitted[1] = Img_GreenMask & GreenMask_Splitted[1];
	/* 3 : Merge and back to BGR */
	merge(GreenMask_Splitted, 3, Img_GreenMasked);
	cvtColor(Img_GreenMasked, Img_GreenMasked, CV_HSV2BGR);

	//Display
	imshow("Original", Img_orig);
	imshow("Grey", Img_grey);
	imshow("B", Img_B);
	imshow("G", Img_G);
	imshow("R", Img_R);
	imshow("Thresh_Over100_Method1", Img_thresholded_Method1);
	imshow("Thresh_Over100_Method2", Img_thresholded_Method2);
	imshow("Splitted_Thresh_Merged", Img_Thresholded_Merged);
	imshow("Saturation_Times2", Img_SaturationTimes2);
	imshow("GreenMask", Img_GreenMask);
	imshow("GreenMasked", Img_GreenMasked);

	waitKey(0);
}