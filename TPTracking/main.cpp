/*
 * Author : Falisse Francois 5A 3DJV
 */

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/video/tracking.hpp"

void draw();
void detectPoints(cv::Mat&);
void video(std::string = "");
std::vector<cv::Point2f> purgePoints(std::vector<cv::Point2f>&,
									 std::vector<uchar>&);
void CallBackFunc(int, int, int, int, void*);
void updateROI();

static const char* main_window = "OpenCV Window";
static const char* tb_window = "Trackbars Window";

cv::Mat prevInput;
cv::Mat nextInput;
std::vector<cv::Point2f> prevPoints;
std::vector<cv::Point2f> nextPoints;
cv::Rect roi;
cv::Point start(-1, -1);

int main(char argc, char* argv[])
{
	video();
}

void draw()
{
	auto img = nextInput.clone();

	//Circle points of interest
	//for (auto it = nextPoints.begin(); it != nextPoints.end(); ++it)
	//{
	//	cv::circle(img, *it, 10, cv::Scalar(0, 0, 200), 2);
	//}
	for (auto i = 0; i < nextPoints.size() ; ++i)
	{
		cv::circle(img, nextPoints[i], 10, cv::Scalar(0, 0, 200), 2);
		cv::line(img, nextPoints[i], prevPoints[i], cv::Scalar(200, 0, 0), 2);
	}

	//Draw roi
	cv::rectangle(img, roi, cv::Scalar(0, 200, 0), 1);

	//Draw stuff
	imshow(main_window, img);

	//Callback souris éventuel
	cv::setMouseCallback(main_window, CallBackFunc);
}

void detectPoints(cv::Mat &img) //Why pass it as argument it's a global already
{
	cv::Mat grey_img;
	cv::cvtColor(img, grey_img, CV_BGR2GRAY);

	//cv::Mat mask = cv::Mat::zeros(nextInput.size(), CV_8UC1);
	//cv::Mat roi_Mat(mask, roi);
	//roi_Mat = cv::Scalar(255, 255, 255);
	//auto roi_Mat = grey_img(roi);

	cv::Mat mask(nextInput.size(), CV_8UC1);
	mask(roi).setTo(cv::Scalar::all(255));

	/*cv::goodFeaturesToTrack(grey_img, prevPoints, 50, 0.01, 10, roi_Mat);*/
	cv::goodFeaturesToTrack(grey_img, prevPoints, 50, 0.01, 10, mask);

	//Fonctionne mais goodFeaturesToTrack s'en bat les couilles du mask 1 fois sur 3
}

void trackPoints()
{
	if(!prevInput.empty())
	{
		//Copy nextPoints in prevPoints
		std::copy(nextPoints.begin(), nextPoints.end(), prevPoints.begin());

		if(prevPoints.size() < 25)
		{
			if(roi.area() > 10 && start.x < 0)
			{
				detectPoints(prevInput);
			}
			if (prevPoints.size() < 10)
				return;
		}

		cv::Mat error;
		std::vector<uchar> status;
		cv::calcOpticalFlowPyrLK(prevInput, nextInput, prevPoints, nextPoints, status, error);

		//Delete not tracked points from both point lists
		prevPoints = purgePoints(prevPoints, status);
		nextPoints = purgePoints(nextPoints, status);
	}

	prevInput = nextInput.clone();
	updateROI();
}

//Le ROI est mort, vive le ROI
void updateROI()
{
	if (roi.area() <= 10 || start.x >= 0)
		return;

	//Compute new roi //update : lol no there's a function that does it for me already
	roi = cv::boundingRect(nextPoints);
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if(event == cv::EVENT_LBUTTONDOWN)
	{
		start = cv::Point(x, y);
		roi = cv::Rect();
		prevPoints.clear();
		nextPoints.clear();
	}
	else
	{
		if (event == cv::EVENT_MOUSEMOVE)
		{
			if (start.x >= 0)
			{
				cv::Point end(x, y);
				roi = cv::Rect(start, end);
			}
		}
		else
		{
			if (event == cv::EVENT_LBUTTONUP)
			{
				cv::Point end(x, y);
				roi = cv::Rect(start, end);
				start = cv::Point(-1, -1);
			}
		}
	}
}

std::vector<cv::Point2f> purgePoints(std::vector<cv::Point2f>& points,
									 std::vector<uchar>& status)
{
	std::vector<cv::Point2f> result;
	for(auto i=0 ; i < points.size(); ++i)
	{
		if (status[i] > 0) result.push_back(points[i]);
	}

	return result;
}

void video(std::string videoname)
{
	auto continue_bool = true;
	cv::VideoCapture cap;

	//If not argument given, open webcam
	if (videoname.empty())
		cap.open(0);
	else
		cap.open(videoname);

	if (!cap.isOpened()) throw std::exception("Failed to open video capture stream");

	while(continue_bool)
	{
		cap.read(nextInput);

		//Test the read value
		if (nextInput.empty()) throw std::exception("Failed to read from video capture stream");

		//Traitement
		//detectPoints(nextInput);
		trackPoints();

		//Dessin
		draw();

		//next frame (why ???)
		cap.read(nextInput);
		if (nextInput.empty()) throw std::exception("Failed to read from video capture stream");

		//Wait 10ms for user input, exit if any
		if (cv::waitKey(10) != -1) continue_bool = false;
	}
}