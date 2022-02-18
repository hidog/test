#ifndef BASE_TEST_H
#define BASE_TEST_H

#include <opencv2/highgui.hpp>


void load_image_1();
void load_image_2();
void create_image();
void image_convert();

void image_test_1a();
void image_test_1b();
void image_test_2();

int filter_test();
void sharpen_filter( const cv::Mat& input, cv::Mat& output ); // 銳化

void point_test();
void liner_blend(); // 線性混合兩張圖
void fourier_transform();

void generate_image();


#endif
