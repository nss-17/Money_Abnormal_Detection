#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


void GaussianFilter(Mat image) {
	int rows = image.rows;
	int cols = image.cols;
	for (int i = 0; i < rows; i++)
	{
		Vec3b* ptr = image.ptr<Vec3b>(i);
		for (int j = 0; j < cols; j++)
		{
			Vec3b pixel = ptr[j];
		}
	}
	imshow("Truoc Gaussian Filter", image);
	//Mat image_Gauss = image.clone();
	GaussianBlur(image, image, Size(5, 5), 0, 0);
	for (int i = 0; i < rows; i++)
	{
		Vec3b* ptr = image.ptr<Vec3b>(i);
		for (int j = 0; j < cols; j++)
		{
			Vec3b pixel = ptr[j];
		}
	}
	imshow("Sau Gaussian Filter", image);
}

void Abnomarly_Detection(Mat img1, Mat img2) {
	// Subtract the two images to obtain a difference mask
	Mat mask;
	absdiff(img1, img2, mask);

	// Convert the mask to grayscale
	cvtColor(mask, mask, COLOR_BGR2GRAY);

	// Threshold the grayscale mask to obtain a binary mask
	threshold(mask, mask, 30, 255, THRESH_BINARY);

	// Find the contours in the binary mask
	std::vector<std::vector<Point>> contours;
	findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (const auto& contour : contours) {
		//the area of a contour is the number of pixels that are enclosed by the contour
		double area = contourArea(contour);
		if (area > 100) {
			Rect rect = boundingRect(contour);
			rectangle(img2, rect, Scalar(0, 0, 255), 2);
		}

	}

	// Show the original and highlighted images
	imshow("Original Image", img1);
	imshow("Highlighted Image", img2);
}

void sharpen(Mat& image) {
	Mat blurred;
	GaussianBlur(image, blurred, Size(0, 0), 3);
	addWeighted(image, 1.5, blurred, -0.5, 0, image);
	imshow("sharp", image);
}

void diff(Mat img1, Mat img2) {
	Mat mask;
	absdiff(img1, img2, mask);
	imshow("mask", mask);
}

double getRotationAngle(Mat img) {
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	Canny(gray, gray, 50, 200, 3);

	vector<Vec4i> lines;
	HoughLinesP(gray, lines, 1, CV_PI / 180, 50, 50, 10);

	Vec4i longestLine = lines[0];
	double longestLineLength = norm(Point(longestLine[0], longestLine[1]) - Point(longestLine[2], longestLine[3]));
	for (size_t i = 1; i < lines.size(); i++) {
		Vec4i line = lines[i];
		double lineLength = norm(Point(line[0], line[1]) - Point(line[2], line[3]));
		if (lineLength > longestLineLength) {
			longestLine = line;
			longestLineLength = lineLength;
		}
	}

	double angle = atan2(longestLine[3] - longestLine[1], longestLine[2] - longestLine[0]) * 180 / CV_PI;
	return angle;
}

void rotate(Mat& img) {
	double angle = getRotationAngle(img); // Rotation Angle
	Size size = img.size();
	Point2f src_center(size.width / 2.0F, size.height / 2.0F);
	Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
	Mat img_rot;
	warpAffine(img, img_rot, rot_mat, size, INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 255, 255));
	img = img_rot.clone();

	imshow("Xoay anh", img);
}

void adjustImage(Mat& img, Mat& templ, Mat& result, Point& maxLoc) {
	// Find the best match location
	double minVal, maxVal;
	Point minLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

	// Draw a rectangle around the best match
	Rect rect(maxLoc.x, maxLoc.y, templ.cols, templ.rows);

	// Adjust ROI to remove the whitespace around the rectangle
	int border = 0;
	rect.x = max(0, rect.x - border);
	rect.y = max(0, rect.y - border);
	rect.width = min(img.cols - rect.x, rect.width +  border);
	rect.height = min(img.rows - rect.y, rect.height + border);
	img = img(rect);

	// Resize the image to the size of the template
	Size size(templ.cols, templ.rows);
	resize(img, img, size);
}

void tempmatch(Mat templ, Mat& img) {
	Mat result;
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);

	// Do the matching
	matchTemplate(img, templ, result, TM_CCORR_NORMED);

	// Find the best match location
	double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

	// Adjust
	adjustImage(img, templ, result, maxLoc);

	// Display the result
	imshow("Sau khi xoay + resize anh", img);
}



void main() {
	Mat img1 = imread("D:/UET/Nam3_2/LapTrinhUngDung/image/money_o.jpg");
	Mat img2 = imread("D:/UET/Nam3_2/LapTrinhUngDung/image/money_d2.jpg");
	Mat imgGray1, imgGray2;
	imshow("anh goc", img1);
	GaussianFilter(img2);
	sharpen(img2);
	diff(img1, img2);
	//Abnomarly_Detection(img1, img2);

	
	//rotate(img2);
	//tempmatch(img1, img2);
	//Abnomarly_Detection(img1, img2);
	
	waitKey(0);
}