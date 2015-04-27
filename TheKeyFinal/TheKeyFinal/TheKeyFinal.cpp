#define UNICODE
#define _UNICODE
#include "stdafx.h"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

using namespace cv;
using namespace std;

double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;

	for (int i = 0; i < corners.size(); i++)
	{
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}
	corners.clear();

	if (top.size() == 2 && bot.size() == 2){
		cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
		cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
		cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
		cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];


		corners.push_back(tl);
		corners.push_back(tr);
		corners.push_back(br);
		corners.push_back(bl);
	}
}
/*	Viết 1 hàm lưu trạng thái ban đầu của 4 điểm góc,
*	việc key sẽ cố định dựa trên 4 tọa độ góc đó!
*/

int main()
{
	Mat frame = imread("E:\\DangDuyChien\\CG Programmer\\Images\\keyTuGiacChuan.png");
	Mat workFrame = imread("E:\\DangDuyChien\\CG Programmer\\Images\\keyTuGiacFail.png");

	/*Mat frame;
	bitwise_or(standarFrame, workFrameC, frame);
	Mat workFrame;
	bitwise_and(standarFrame, workFrameC, workFrame);
	imshow("c",workFrame);*/

	VideoCapture keyCap = VideoCapture("E:\\HDVN\\Over.mxf");
	//VideoCapture cap = VideoCapture(0);

	namedWindow("Control", WINDOW_NORMAL);
	int iLowB = 0;
	int iHighB = 150;

	int iLowG = 100;
	int iHighG = 255;

	int iLowR = 0;
	int iHighR = 150;

	//Create trackbars in "Control" window
	createTrackbar("LowB", "Control", &iLowB, 255);
	createTrackbar("HighB", "Control", &iHighB, 255);

	createTrackbar("LowG", "Control", &iLowG, 255);
	createTrackbar("HighG", "Control", &iHighG, 255);

	createTrackbar("LowR", "Control", &iLowR, 255);
	createTrackbar("HighR", "Control", &iHighR, 255);

	imshow("Source", workFrame);

	//Tao anh cuong do
	Mat thresholdImage, thresholdWorkImage;
	vector<vector<Point>> contours;
	vector<vector<Point>> hull(1);
	vector<Vec4i> lines;
	

	/* Thuật toán chung: lưu trạng thái ban đầu của background (trạng thái trống, chưa có người) làm trạng thái chuẩn,
	** thực hiện key lên trạng thái đó, với các background tiếp theo, thực hiện xóa font xanh rồi dùng bitwise_and để
	** cộng trạng thái này với trạng thái chuẩn đã key, ta luôn được điều mình cần (trong trường hợp góc quay là cố định)
	*/
	while (true)
	{

		Mat keyImg;
		if (!keyCap.read(keyImg))
		{
			keyCap.set(SEEK_CUR, 0);
			keyCap.read(keyImg);
		}

		Mat product = workFrame.clone();
		Mat out = frame.clone();
		//Checks if array elements lie between the elements of two other arrays.
		//Dua anh frame ve thanh anh cuong do qua ham inRange
		inRange(frame, Scalar(iLowB, iLowG, iLowR), Scalar(iHighB, iHighG, iHighR), thresholdImage);
		inRange(workFrame, Scalar(iLowB, iLowG, iLowR), Scalar(iHighB, iHighG, iHighR), thresholdWorkImage);
		//blur(thresholdImage, thresholdImage, Size(5, 5));

		//imshow("Threshold", thresholdImage);
		//imshow("ThreholdWorkImage", thresholdWorkImage);

		Mat tmp = thresholdImage.clone();
		//Tim vien anh cua anh tmp luu vao vecto contours
		findContours(tmp, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
		int maxContourIndex = -1;
		double maxContourSize = 0;
		for (int i = 0; i < contours.size(); i++)
		{
			double contourSize = contourArea(contours[i]);
			if (contourSize > 1000 && contourSize > maxContourSize)
			{
				maxContourSize = contourSize;
				maxContourIndex = i;
			}
		}

		if (maxContourIndex >= 0)
		{
			//Ve vien trang len anh den theo mang contours
			Mat contourImg = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
			drawContours(contourImg, contours, maxContourIndex, Scalar(255), 2);

			//Tim phan "loi" cua anh contours, ve phan loi do vao anh den trang hullImg
			convexHull(contours[maxContourIndex], hull[0]);
			Mat hullImg = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
			drawContours(hullImg, hull, 0, Scalar(255), 2);

			//Ve vien vao addImg
			Mat addImg = Mat(contourImg.size(), contourImg.type());
			bitwise_and(contourImg, hullImg, addImg);

			//Tim duong thang trong addImg, luu vao vector lines
			lines.clear();
			HoughLinesP(addImg, lines, 1, CV_PI / 180, 50, 50, 10);
			addImg = Mat::zeros(contourImg.size(), contourImg.type());

			//Xac dinh 4 diem goc cua TV ao
			for (int j = 0; j < lines.size(); j++)
			{
				Vec4i curLine = lines[j];
				Point p1(0, 0), p2(0, 0);
				if (curLine[0] == curLine[2])
				{
					p1.x = p2.x = curLine[0];
					p1.y = 0;
					p2.y = frame.rows;
				}
				else if (curLine[1] == curLine[3])
				{
					p1.x = 0;
					p2.x = frame.cols;
					p1.y = p2.y = curLine[1];
				}
				else
				{
					int num = curLine[0] * curLine[3] - curLine[2] * curLine[1];
					p1.y = num / (curLine[0] - curLine[2]);
					if (p1.y < 0)
					{
						p1.x = frame.cols;
						p1.y = (curLine[3] * (p1.x - curLine[0]) - curLine[1] * (p1.x - curLine[2])) / (curLine[2] - curLine[0]);
					}

					p2.x = num / (curLine[3] - curLine[1]);
					if (p2.x < 0)
					{
						p2.y = frame.rows;
						p2.x = (curLine[2] * (p2.y - curLine[1]) - curLine[0] * (p2.y - curLine[3])) / (curLine[3] - curLine[1]);
					}
				}
				//Ve duong thang tu diem p1 den p2
				line(addImg, p1, p2, Scalar(255), 5);
			}

			contours.clear();
			findContours(addImg, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
			vector<Point> square;
			maxContourIndex = -1;
			maxContourSize = 0;
			vector<Point> approx;
			for (int i = 0; i < contours.size(); i++)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				approxPolyDP(contours[i], approx, arcLength(contours[i], true) * 0.02, true);

				// square contours should have 4 vertices after approximation
				// relatively large area (to filter out noisy contours)
				// and be convex.
				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				if (approx.size() == 4 && isContourConvex(Mat(approx)))
				{
					double contourSize = fabs(contourArea(Mat(approx)));
					if (contourSize > 1000 && contourSize > maxContourSize)
					{
						double maxCosine = 0;

						for (int j = 2; j < 5; j++)
						{
							// find the maximum cosine of the angle between joint edges
							double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
							maxCosine = MAX(maxCosine, cosine);
						}

						// if cosines of all angles are small
						// (all angles are ~90 degree) then write quandrange
						// vertices to resultant sequence
						if (maxCosine < 0.6)
						{
							maxContourIndex = i;
							square = approx;
						}
					}
				}
			}

			addImg = Mat::zeros(contourImg.size(), contourImg.type());
			if (maxContourIndex >= 0)
			{
				vector<Point2f> pts1;
				pts1.push_back(Point2f(0, 0));
				pts1.push_back(Point2f(keyImg.cols, 0));
				pts1.push_back(Point2f(keyImg.cols, keyImg.rows));
				pts1.push_back(Point2f(0, keyImg.rows));

				Point2f center(0, 0);
				vector<cv::Point2f> corners;
				int index = 0;

				// Get mass center
				for (int i = 0; i < square.size(); i++)
				{
					corners.push_back(Point2f(square[i].x, square[i].y));
					center.x += square[i].x;
					center.y += square[i].y;
				}
				center *= (1. / square.size());
				sortCorners(corners, center);

				try
				{
					Mat warp_matrix = cv::getPerspectiveTransform(pts1, corners);

					Mat neg_image;
					warpPerspective(keyImg, neg_image, warp_matrix, frame.size());

					Mat maskImg = Mat::zeros(thresholdImage.size(), thresholdImage.type());
					Point pts[1][4] = { corners[0], corners[1], corners[2], corners[3] };// luu lai 4 diem goc

					const Point *ptt[1] = { pts[0] };
					int npt[] = { 4 };

					fillPoly(maskImg, ptt, npt, 1, Scalar(255));

					bitwise_and(maskImg, thresholdImage, maskImg);

					Mat blackImg = Mat::zeros(frame.size(), frame.type());
					bitwise_and(out, blackImg, out, maskImg);

					bitwise_not(maskImg, maskImg);

					bitwise_and(neg_image, blackImg, neg_image, maskImg);
					//imshow("neg_image", out);

					out = out + neg_image;

					//Xong phần key cho ảnh standar, tiếp theo đè ảnh real time lên nó

					Mat maskWorkImg = Mat::zeros(thresholdWorkImage.size(), thresholdWorkImage.type());

					fillPoly(maskWorkImg, ptt, npt, 1, Scalar(255));

					bitwise_and(maskWorkImg, thresholdWorkImage, maskWorkImg);

					Mat blackImg2 = Mat::zeros(workFrame.size(), workFrame.type());

					bitwise_and(product, blackImg, product, maskWorkImg);

					//bitwise_not(maskWorkImg, maskWorkImg);
					//imshow("mask", maskWorkImg);
					//bitwise_or(product, blackImg2, product, maskWorkImg);
					Mat test;
					out.copyTo(test, maskWorkImg);
					//imshow("preProduct", test);
					product = product + test;

				}
				catch (...)
				{
				}
				index++;
			}
		}

		imshow("Product", product);

		if (waitKey(30) == 27)
			break;

	}
}