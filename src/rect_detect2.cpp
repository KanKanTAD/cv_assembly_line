/**
 * @file BackProject_Demo1.cpp
 * @brief Sample code for backproject function usage
 * @author OpenCV team
 */

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <vector>

using namespace cv;
using namespace std;

RNG_MT19937 rng{(unsigned)std::clock()};

/// Global Variables
Mat hue;
Mat to_show;
/// Function Headers
void Hist_and_Backproj(int, void *);

/**
 * @function main
 */
int main(int argc, char *argv[]) {
  //! [Read the image]
  CommandLineParser parser(argc, argv,
                           "{@input |  | input image}{resize|1.0|}");
  parser.printMessage();

  double __resize = parser.get<double>("resize");
  const char *window_image = "Source image";
  namedWindow(window_image);
  int bins = 25;
  createTrackbar("* Hue  bins: ", window_image, &bins, 180);
  int blur_ksize = 1;
  createTrackbar("blur ksize", window_image, &blur_ksize, 128);
  int filter_ksize = 1;
  createTrackbar("laplasian_ddepth", window_image, &filter_ksize, 255);

  VideoCapture cap;
  String __in = parser.get<String>("@input");

  if (__in.empty()) {
    cap.open(0);
  } else {
    cap.open(__in);
  }

  bool is_continue = true;
  while (is_continue) {
    std::unique_ptr<bool, std::function<void(bool *)>> _0x001{
        &is_continue, [](bool *a) {
          if ('\x1b' == waitKey(20)) {
            *a = false;
          }
        }};
    Mat src;
    cap >> src;
    if (src.empty()) {
      cap.open(__in);
      continue;
    }
    blur_ksize = 16;
    blur(src, src, {blur_ksize, blur_ksize});
    // filter2D(src, src, -1,Vec2i {filter_ksize,filter_ksize});
    resize(src, src, {}, __resize, __resize);
    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);
    //! [Transform it to HSV]

    //! [Use only the Hue value]
    hue.create(hsv.size(), hsv.depth());
    int ch[] = {0, 0};
    mixChannels(&hsv, 1, &hue, 1, ch, 1);
    //! [Use only the Hue value]

    //! [Create Trackbar to enter the number of bins]
    to_show = src.clone();
    bins = 3;
    Hist_and_Backproj(bins, 0);
    //! [Create Trackbar to enter the number of bins]

    //! [Show the image]
    imshow(window_image, to_show);
    // Wait until user exits the program
  }

  return 0;
}

/**
 * @function Hist_and_Backproj
 * @brief Callback to Trackbar
 */
void Hist_and_Backproj(int bins, void *) {
  //! [initialize]
  int histSize = MAX(bins, 2);
  float hue_range[] = {0, 180};
  const float *ranges = {hue_range};
  //! [initialize]

  //! [Get the Histogram and normalize it]
  Mat hist;
  calcHist(&hue, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false);
  normalize(hist, hist, 0, 255, NORM_MINMAX, -1, Mat());
  //! [Get the Histogram and normalize it]

  //! [Get Backprojection]
  Mat backproj;
  calcBackProject(&hue, 1, 0, hist, backproj, &ranges, 1, true);
  //! [Get Backprojection]
  threshold(backproj, backproj, 0, 255, THRESH_BINARY_INV | cv::THRESH_OTSU);
  vector<vector<Point2i>> contours;
  findContours(backproj, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  auto it = max_element(contours.begin(), contours.end(),
                        [](const vector<Point2i> &a, const vector<Point2i> &b) {
                          return contourArea(a) < contourArea(b);
                        });
  if (contours.end() != it) {
    auto arc_length = arcLength(*it, true);
    vector<Point2i> appro;
    approxPolyDP(*it, appro, 0.02 * arc_length, true);
    RotatedRect _rect = minAreaRect(appro);
    // drawContours(to_show, contours, it - contours.begin(),
    // Scalar{rng.uniform(0.0, 255.0), rng.uniform(0.0, 255.0),
    // rng.uniform(0.0, 255.0)});

		Mat pts;
		boxPoints(_rect, pts);
		pts.convertTo(pts, CV_32SC1);
    Scalar color{rng.uniform(0.0, 255.0), rng.uniform(0.0, 255.0),
                 rng.uniform(0.0, 255.0)};
    // Point2i center{cvRound(_rect.center.x), cvRound(_rect.center.y)};
		polylines(to_show,pts,true,color,3);
    circle(to_show, _rect.center, 3, color, -1);
  }
  //! [Draw the backproj]
  imshow("BackProj", backproj);
  //! [Draw the backproj]

  //! [Draw the histogram]
  int w = 400, h = 400;
  int bin_w = cvRound((double)w / histSize);
  Mat histImg = Mat::zeros(h, w, CV_8UC3);

  for (int i = 0; i < bins; i++) {
    rectangle(
        histImg, Point(i * bin_w, h),
        Point((i + 1) * bin_w, h - cvRound(hist.at<float>(i) * h / 255.0)),
        Scalar(0, 0, 255), FILLED);
  }

  imshow("Histogram", histImg);
  //! [Draw the histogram]
}
