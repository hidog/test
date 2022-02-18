#include "filter_test.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "tool.h"




void laplace_test()
{
    cv::VideoCapture vcap( get_mp4_path() );
    if ( false == vcap.isOpened())
        return;

    //
    cv::Mat frame, src, src_gray, dst;
    cv::Mat abs_dst;

    constexpr int kernel_size = 3;
    constexpr int scale = 1;
    constexpr int delta = 0;
    constexpr int ddepth = CV_16S;

    while(true)
    {
        vcap >> frame;
        if( frame.ptr() == NULL )
            break;

        GaussianBlur( frame, src, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT );
        cvtColor( src, src_gray, cv::COLOR_BGR2GRAY );
        
        Laplacian( src_gray, dst, ddepth, kernel_size, scale, delta, cv::BORDER_DEFAULT );

        convertScaleAbs( dst, abs_dst );

        imshow( "frame", frame );
        imshow( "laplace", abs_dst );

        cv::waitKey(1);
    }

    vcap.release();
}



void sobel_test()
{
    cv::VideoCapture vcap( get_mp4_path() );
    if ( false == vcap.isOpened())
        return;

    cv::Mat frame, src, src_gray, gray, sobel, grad;
    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y;

    constexpr int ddepth = CV_16S;
    constexpr int ksize = 1;
    constexpr int scale = 1;
    constexpr int delta = 0;
    
    // 利用第一張圖的資訊來做初始化, 沒做這個動作會造成crash.
    vcap >> frame;
    sobel.create( frame.size(), frame.type() );
    
    int count = 0;
    while (true)
    {
        std::cout << count++ << std::endl;

        vcap >> frame;

        if( frame.ptr() == nullptr )
            break;

        // Remove noise by blurring with a Gaussian filter ( kernel size = 3 )
        cv::GaussianBlur( frame, src, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

        // Convert the image to grayscale
        cvtColor( src, src_gray, cv::COLOR_BGR2GRAY );

        Sobel( src_gray, grad_x, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT );
        Sobel( src_gray, grad_y, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT );

        // converting back to CV_8U
        convertScaleAbs( grad_x, abs_grad_x );
        convertScaleAbs( grad_y, abs_grad_y );
        addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

        imshow( "frame", frame );
        imshow( "gradiant", grad );

        cv::waitKey(1);
    }

    vcap.release();
}





void opencv_tutorial()
{
    cv::VideoCapture vcap( get_mp4_path() );
    if( vcap.isOpened() == false )
        return;

    cv::Mat frame, edges;
    cv::namedWindow("edges", cv::WINDOW_AUTOSIZE);
    
    int count = 0;

    while(true)
    {
        std::cout << count++ << "\n";
    
        vcap >> frame;
        if( frame.ptr() == nullptr )
            break;
        
        cvtColor( frame, edges, cv::COLOR_BGR2GRAY );
        
        cv::GaussianBlur( edges, edges, cv::Size(7,7), 1.5, 1.5 );
        cv::Canny( edges, edges, 0, 30, 3 );
        
        imshow("edges", edges);
        
        cv::waitKey(1);
    }

    vcap.release();
}

