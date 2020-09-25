#include "opencv2/opencv.hpp"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

#define RND_COLOR() Scalar::all(-1)

int main(int argc, char **argv) {

  CommandLineParser parser{argc, argv,
                           "{@input|rect2.webm|}{@video_dir|/home/tad/Video/"
                           "|}{resize|1.0|}{time_ms|150|}"};
  parser.printMessage();

  double _resize = parser.get<double>("resize");
  String _path =
      parser.get<String>("@video_dir") + parser.get<String>("@input");

  VideoCapture cap;
  cap.open(_path);
  assert(cap.isOpened());

  std::unique_ptr<VideoCapture, std::function<void(VideoCapture *)>> _0x001{
      &cap, [](VideoCapture *a) {
        if (a->isOpened()) {
          a->release();
        }
      }};

  String src_wnd{"source window"};
  namedWindow(src_wnd);
  String res_wnd{"result window"};
  namedWindow(res_wnd);
  String hz_wnd{"he zhi window"};
  namedWindow(hz_wnd);

  int low[3] = {0, 0, 0}, hig[3] = {255, 255, 255};
  for (int i = 0; i < 3; ++i) {
    createTrackbar("low " + to_string(i), res_wnd, (int *)&low[i], 255);
    createTrackbar("hig " + to_string(i), res_wnd, (int *)&hig[i], 255);
  }

  int h_low[3] = {0, 0, 0}, h_hig[3] = {255, 255, 255};
  for (int i = 0; i < 3; ++i) {
    createTrackbar("low " + to_string(i), hz_wnd, (int *)&h_low[i], 255);
    createTrackbar("hig " + to_string(i), hz_wnd, (int *)&h_hig[i], 255);
  }
  int block_size = 1;
  int _C = 1;
  createTrackbar("block_size", res_wnd, &block_size, 128);
  createTrackbar("C", res_wnd, &_C, 100);
  int _min_contoursArea = 0;
  createTrackbar("min contours area", res_wnd, &_min_contoursArea, 2000);

  int jg_thre_min = 0;
  createTrackbar("jg_thre_min", res_wnd, &jg_thre_min, 255);
  int jg_thre_max = 255;
  createTrackbar("jg_thre_max", res_wnd, &jg_thre_max, 255);

  int blur_arg = 1;
  createTrackbar("blur arg", hz_wnd, &blur_arg, 128);
  int filter_arg = 1;
  createTrackbar("filter_arg", hz_wnd, &filter_arg, 64);
  int erode_arg = 1;
  createTrackbar("erode_arg", hz_wnd, &erode_arg, 128);

  int time_ms = parser.get<int>("time_ms");

  auto he_zhi = [&](Mat &src, Mat &res, Mat &draw) mutable {
    Mat s;
    // blur_arg 35
    blur(src, s, {blur_arg, blur_arg});
    // filter_arg 3
    filter2D(s, s, -1, Vec2i{filter_arg, filter_arg});
    Mat gray;
    cvtColor(s, gray, COLOR_BGR2GRAY);
    // threshold(gray, gray, 0, 255, THRESH_BINARY | cv::THRESH_OTSU);
    erode_arg = std::max(1, erode_arg);
    erode(gray, gray, Vec2i{erode_arg, erode_arg});
    res = gray;
    // Mat gray;
    // cvtColor(src, gray, COLOR_BGR2GRAY);
    // Mat mask ;
    // inRange(src, Vec3b{uint8_t(h_low[0]), uint8_t(h_low[1]),
    // uint8_t(h_low[2])}, Vec3b{uint8_t(h_hig[0]), uint8_t(h_hig[1]),
    // uint8_t(h_hig[2])}, mask);

    // adaptiveThreshold(gray, mask, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
    // THRESH_BINARY, 5, 0.01);
    // bitwise_not(mask, mask);
  };

  auto ji_guan = [&](Mat &src, Mat &res, Mat &draw) mutable {
    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    // low bgr { 0 , 0 , 23 }
    // hig bgr {255,255, 255 }
    Mat mask;
    inRange(hsv, Vec3b{uint8_t(low[0]), uint8_t(low[1]), uint8_t(low[2])},
            Vec3b{uint8_t(hig[0]), uint8_t(hig[1]), uint8_t(hig[2])}, mask);
    // threshold(gray, mask, jg_thre_min, jg_thre_max, THRESH_TRUNC);
    res = mask;
    // inRange(src, Vec3b{0, 0, 23}, Vec3b{255, 255, 255}, mask);
    // gray.copyTo(res, mask);
    // adaptiveThreshold(res, res, 255, THRESH_BINARY,
    // ADAPTIVE_THRESH_GAUSSIAN_C, 5, _C * 0.01);
    // bitwise_not(res, res);

    // vector<vector<Point2i>> contours;
    // findContours(res, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // for (int i = 0; i < contours.size(); ++i) {
    // if (contourArea(contours.at(i)) <= _min_contoursArea) {
    // continue;
    //}
    // drawContours(src, contours, i, Scalar::all(128), 3);
    //}
  };

  bool is_continue = true;
  while (is_continue) {
    std::unique_ptr<bool, std::function<void(bool *)>> _0x01{
        &is_continue, [&](bool *a) mutable {
          if ('\x1b' == waitKey(time_ms)) {
            is_continue = false;
          }
        }};

    Mat frame;
    cap >> frame;
    if (frame.empty()) {
      cap.open(_path);
      continue;
    }

    resize(frame, frame, {}, _resize, _resize);

    Mat fm = frame.clone();

    Mat res1;
    ji_guan(fm, res1, frame);
    imshow(res_wnd, res1);

    Mat res2;
    he_zhi(fm, res2, frame);
    imshow(hz_wnd, res2);

    imshow(src_wnd, frame);
  }

  return 0;
}
