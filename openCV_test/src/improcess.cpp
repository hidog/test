#include "improcess.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "tool.h"



namespace my_canny {


cv::Mat src, src_gray;
cv::Mat dst, detected_edges;

int lowThreshold = 0;
const int max_lowThreshold = 100;
const int ratio = 3;
const int kernel_size = 3;
const char* window_name = "Edge Map";

static void CannyThreshold(int, void*)
{
    blur( src_gray, detected_edges, cv::Size(3,3) );
    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
    dst = cv::Scalar::all(0);
    src.copyTo( dst, detected_edges);
    imshow( window_name, dst );
}



int canny_test_2()
{
    src = cv::imread( get_jpg_path(), cv::IMREAD_COLOR ); 

    dst.create( src.size(), src.type() );
    cvtColor( src, src_gray, cv::COLOR_BGR2GRAY );

    cv::namedWindow( window_name, cv::WINDOW_AUTOSIZE );
    cv::createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

    CannyThreshold(0, 0);
    cv::waitKey(WAIT_TIME);

    return 0;
}



void canny_test()
{
    cv::Mat src, src_gray;
    cv::Mat dst, detected_edges;

    int lowThreshold = 60;
    const int ratio = 3;
    const int kernel_size = 3;

    //
    cv::VideoCapture vcap( get_mp4_path() );
    if ( false == vcap.isOpened())
        return;

    while(true)
    {
        vcap >> src;
        if( src.ptr() == nullptr )
            break;
      
        dst.create( src.size(), src.type() );
        cvtColor( src, src_gray, cv::COLOR_BGR2GRAY );

        cv::blur( src_gray, detected_edges, cv::Size(3,3) );
        Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

        dst = cv::Scalar::all(0);
        src.copyTo( dst, detected_edges);
        imshow( "edge map", dst );

        cv::waitKey(1);
    }
}


} // end namespace canny_test












void histogram_test()
{
    cv::VideoCapture cap( get_mp4_path() );
    if( cap.isOpened() == false )
        return;

    cv::Mat frame;
    cv::Mat rframe, gframe, bframe;
    cv::Mat rdst, gdst, bdst;
    cv::Mat dst;

    int i, j;

    cap >> frame;
    rframe.create( frame.size(), CV_8UC1 );
    gframe.create( frame.size(), CV_8UC1 );
    bframe.create( frame.size(), CV_8UC1 );
    dst.create( frame.size(), frame.type() );


    while(true)
    {
        cap >> frame;
        if( frame.ptr() == nullptr )
            break;

        for( j = 0; j < frame.rows; j++ )
        {
            for( i = 0; i < frame.cols; i++ )
            {
                bframe.at<uchar>(j,i) = frame.at<cv::Vec3b>(j,i)[0];
                gframe.at<uchar>(j,i) = frame.at<cv::Vec3b>(j,i)[1];
                rframe.at<uchar>(j,i) = frame.at<cv::Vec3b>(j,i)[2];
            }
        }

        cv::equalizeHist( rframe, rdst );
        cv::equalizeHist( gframe, gdst );
        cv::equalizeHist( bframe, bdst );

        for( j = 0; j < frame.rows; j++ )
        {
            for( i = 0; i < frame.cols; i++ )
            {
                dst.at<cv::Vec3b>(j,i)[0] = bdst.at<uchar>(j,i);
                dst.at<cv::Vec3b>(j,i)[1] = gdst.at<uchar>(j,i);
                dst.at<cv::Vec3b>(j,i)[2] = rdst.at<uchar>(j,i);
            }
        }

        imshow( "source", frame );
        imshow( "equalized`", dst );

        cv::waitKey(1);
    }

    cap.release();
}






namespace my_harris {


cv::Mat src, src_gray;

int thresh = 200;
int max_thresh = 255;

const char* source_window = "Source image";
const char* corners_window = "Corners detected";



void harris_corner_test()
{
    src = cv::imread( get_jpg_path() );
    cvtColor( src, src_gray, cv::COLOR_BGR2GRAY );

    cv::namedWindow( source_window );
    cv::createTrackbar( "Threshold: ", source_window, &thresh, max_thresh, harris_func );

    imshow( source_window, src );
    harris_func( 0, 0 );

    cv::waitKey(WAIT_TIME);
}





void harris_func( int, void* )
{
    int blockSize = 2;
    int apertureSize = 3;
    double k = 0.04;

    cv::Mat dst = cv::Mat::zeros( src.size(), CV_32FC1 );
    cornerHarris( src_gray, dst, blockSize, apertureSize, k );

    cv::Mat dst_norm, dst_norm_scaled;
    normalize( dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat() );
    convertScaleAbs( dst_norm, dst_norm_scaled );

    for( int i = 0; i < dst_norm.rows ; i++ )
    {
        for( int j = 0; j < dst_norm.cols; j++ )
        {
            if( dst_norm.at<float>(i,j) > thresh )            
                circle( dst_norm_scaled, cv::Point(j,i), 5,  cv::Scalar(0), 2, 8, 0 );            
        }
    }

    cv::namedWindow( corners_window );
    imshow( corners_window, dst_norm_scaled );
}



}; // end namespace my_harris
