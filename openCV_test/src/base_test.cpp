#include "base_test.h"
#include "tool.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <vector>




void fourier_transform()
{
    // https://docs.opencv.org/4.5.4/d8/d01/tutorial_discrete_fourier_transform.html
    // 有idft可以做逆運算, 原本想用dft做filter後轉回來, 但沒成功, 有空研究
    
    cv::VideoCapture cap{ get_mp4_path() };
    cv::Mat frame, I;

    while( true )
    {
        cap >> frame;
        if( frame.ptr() == nullptr )
            break;
            
        cv::cvtColor( frame, I, cv::COLOR_BGR2GRAY );

        cv::Mat padded;                            //expand input image to optimal size
        int m = cv::getOptimalDFTSize( I.rows );
        int n = cv::getOptimalDFTSize( I.cols );   // on the border add zero values
    
        cv::copyMakeBorder( I, padded, 0, m - I.rows, 0, n - I.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0) );
        cv::Mat planes[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };
        
        cv::Mat complexI;
        cv::merge( planes, 2, complexI );         // Add to the expanded another plane with zeros
        cv::dft( complexI, complexI );            // this way the result may fit in the source matrix

        // compute the magnitude and switch to logarithmic scale
        // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
        cv::split( complexI, planes );                    // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
        cv::magnitude( planes[0], planes[1], planes[0] ); // planes[0] = magnitude
        
        cv::Mat magI = planes[0];
        magI += cv::Scalar::all(1);                    // switch to logarithmic scale
        cv::log(magI, magI);
        
        // crop the spectrum, if it has an odd number of rows or columns
        cv::Rect r( 0, 0, magI.cols & -2, magI.rows & -2 );
        magI = magI( r );
        
        // rearrange the quadrants of Fourier image  so that the origin is at the image center
        int cx = magI.cols / 2;
        int cy = magI.rows / 2;
        
        cv::Mat q0( magI, cv::Rect(  0,  0, cx, cy ) );   // Top-Left - Create a ROI per quadrant
        cv::Mat q1( magI, cv::Rect( cx,  0, cx, cy ) );   // Top-Right
        cv::Mat q2( magI, cv::Rect(  0, cy, cx, cy ) );   // Bottom-Left
        cv::Mat q3( magI, cv::Rect( cx, cy, cx, cy ) );   // Bottom-Right
        cv::Mat tmp;                            // swap quadrants (Top-Left with Bottom-Right)
        
        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);
        
        q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
        q2.copyTo(q1);
        tmp.copyTo(q2);
        
        cv::normalize( magI, magI, 0, 1, cv::NORM_MINMAX ); // Transform the matrix with float values into a
                                                        // viewable image form (float between values 0 and 1).
        imshow( "Input Image"       , I    );           // Show the result
        imshow( "spectrum magnitude", magI );
        
        int k = cv::waitKey(1);
        if( k == 'q' )
            break;
    }

    cap.release();
}






void sharpen_filter( const cv::Mat& input, cv::Mat& output )
{
    CV_Assert( input.depth() == CV_8U );  // accept only uchar images
    
    const int channels = input.channels();
    output.create( input.size(), input.type() );
    
    for( int j = 1 ; j < input.rows - 1; ++j )
    {
        const uchar* previous = input.ptr<uchar>(j - 1);
        const uchar* current  = input.ptr<uchar>(j    );
        const uchar* next     = input.ptr<uchar>(j + 1);
        
        uchar* o = output.ptr<uchar>(j);
        
        for( int i = channels; i < channels*(input.cols - 1); ++i )
        {
            *o++ = cv::saturate_cast<uchar>(  5 * current[i]
                                              - current[ i - channels ] 
                                              - current[ i + channels ] 
                                              - previous[i] 
                                              - next[i] );
        }
    }
    
    output.row(0).setTo( cv::Scalar(0) );
    output.row( output.rows - 1 ).setTo( cv::Scalar(0) );
    
    output.col(0).setTo( cv::Scalar(0) );
    output.col( output.cols - 1 ).setTo( cv::Scalar(0) );
}





int filter_test()
{
    std::string filename = get_jpg_path();
    
    cv::Mat src, dst0, dst1;    
    src = imread( filename, cv::IMREAD_COLOR);

    namedWindow( "Input", cv::WINDOW_AUTOSIZE );
    namedWindow( "Output", cv::WINDOW_AUTOSIZE );
    
    imshow( "Input", src );
    
    sharpen_filter( src, dst0 );
    
    imshow( "Output", dst0 );
    cv::waitKey(WAIT_TIME);
    
    //
    /*cv::Mat kernel = (cv::Mat_<char>(3,3) <<  -1, 0,  1,
                                              -2, 0,  2,
                                              -1, 0,  1);*/
    cv::Mat kernel = (cv::Mat_<char>(3,3) <<  -1, -2, -1,
                                               0,  0,  0,
                                               1,  2,  1);

    cv::filter2D( src, dst1, src.depth(), kernel );

    imshow( "Output 2", dst1 );
    cv::waitKey(WAIT_TIME);
    
    return EXIT_SUCCESS;
}





void image_test_2()
{
    std::string image_path = get_jpg_path();
    cv::Mat img = cv::imread( image_path, cv::IMREAD_GRAYSCALE );

#define USE_SCALAR

    int i, j;
    std::vector<int> table(256);
#ifndef USE_SCALAR
    int gray;
#else
    cv::Scalar gray;  // 可以使用Scalar. 但灰階圖直接用int比較方便
#endif

    for( j = 0; j < img.rows; j++ )
    {
        for( i = 0; i < img.cols; i++ )
        {
            gray = img.at<uchar>( j, i );   // 用在灰階.
#ifndef USE_SCALAR
            table[gray]++;
#else
            table[ gray[0] ]++;
#endif
        }
    }

    for( int i = 0; i < 256; i++ )
        std::cout << "table[" << i << "] = " << table[i] << "\n";

    //
    for( j = 0; j < img.rows; j++ )
    {
        for( i = 0; i < img.cols; i++ )
        {
            //gray = img.at<uchar>( j, i );
            
#ifndef USE_SCALAR
            if( i % 10 == 0 )
                img.at<uchar>( j, i ) = 255;
            else if( j % 20 == 5 )
                img.at<uchar>( j, i ) = 0;
#else
            if( i % 10 == 0 )
                img.at<uchar>( j, i ) = cv::Scalar{255}[0];  // 可以用scalar,但灰階圖直接用int比較方便
            else if( j % 20 == 5 )
                img.at<uchar>( j, i ) = cv::Scalar{0}[0];
#endif
            


        }
    }

    imshow( "Display window", img );
    cv::waitKey(WAIT_TIME); // Wait for a keystroke in the window

#undef USE_SCALAR
}





void image_test_1a()
{
    std::string image_path = get_jpg_path();
    cv::Mat img = cv::imread( image_path, cv::IMREAD_COLOR );

    std::cout << "cols = " << img.cols 
              << "\nrows = " << img.rows 
              << "\ndepth = " << img.depth()
              << "\nchannels = " << img.channels() << "\n";
    
    int i, j;
    
#define USE_VEC3B

#ifndef USE_VEC3B
    int r, g, b;
    uchar* p = img.ptr<uchar>();
#else
    cv::Vec3b rgb;
#endif
       
    for( j = 0; j < img.rows; j++ )
    {
        for( i = 0; i < img.cols; i++ )
        {
#ifndef USE_VEC3B
            b = *(p + (j*img.cols + i)*3 + 0);  // b
            g = *(p + (j*img.cols + i)*3 + 1);  // g
            r = *(p + (j*img.cols + i)*3 + 2);  // r
#else
            rgb = img.at<cv::Vec3b>( j, i );  // 用vec3b做測試
#endif

#ifndef USE_VEC3B
            // 對資料對任意改變的測試
            r = cv::saturate_cast<uchar>(r + 50); // 範圍會限制到 0~255
            g = 100;
            b = cv::saturate_cast<uchar>(b - 50);

            *(p + (j*img.cols + i)*3 + 0) = b;
            *(p + (j*img.cols + i)*3 + 1) = g;
            *(p + (j*img.cols + i)*3 + 2) = r;
#else
            rgb[2] = cv::saturate_cast<uchar>( rgb[2] + 50);
            rgb[1] = 100;
            rgb[0] = cv::saturate_cast<uchar>( rgb[0] - 50);
            img.at<cv::Vec3b>( j, i ) = rgb; // 可以用vec3b做操作
#endif
        }
    }
    
    imshow( "Display window", img );
    cv::waitKey(WAIT_TIME); // Wait for a keystroke in the window
    
#undef USE_VEC3B
}





void image_test_1b()
{
    std::string image_path = get_jpg_path();
    cv::Mat img = cv::imread( image_path, cv::IMREAD_COLOR );
    
    int i, j;
    
#define USE_VEC3B

#ifndef USE_VEC3B
    int r, g, b;
    uchar* p = img.ptr<uchar>();
#else
    cv::Vec3b rgb;
#endif
       
    for( j = 0; j < img.rows; j++ )
    {
        for( i = 0; i < img.cols; i++ )
        {
#ifndef USE_VEC3B
            b = *(p + (j*img.cols + i)*3 + 0);  // b
            g = *(p + (j*img.cols + i)*3 + 1);  // g
            r = *(p + (j*img.cols + i)*3 + 2);  // r
#else
            rgb = img.at<cv::Vec3b>( j, i );  // 用vec3b做測試
#endif


#ifndef USE_VEC3B
            // r, b 對調
            *(p + (j*img.cols + i)*3 + 0) = r;
            *(p + (j*img.cols + i)*3 + 1) = g;
            *(p + (j*img.cols + i)*3 + 2) = b;
#else
            // rgb[2] = r, rgb[1] = g, rgb[0] = b
            img.at<cv::Vec3b>( j, i ) = cv::Vec3b( rgb[2], rgb[1], rgb[0] );  // 不同寫法得到一樣的效果
#endif
        }
    }
    
    imshow( "Display window", img );
    cv::waitKey(WAIT_TIME); // Wait for a keystroke in the window
    
#undef USE_VEC3B
}





void image_convert()
{
    std::string image_path = get_jpg_path();
    cv::Mat img = cv::imread( image_path, cv::IMREAD_COLOR );

    // 可以轉成浮點數, 但無法用img show
    cv::Mat img_32F;
    img.convertTo( img_32F, CV_32F );
    std::cout << "img_32F.at<float>( 100, 100 ) = " << img_32F.at<float>( 100, 100 ) << "\n";
    
    // 轉灰階
    cv::Mat img_gray;
    cvtColor( img, img_gray, cv::COLOR_BGR2GRAY );
    imshow( "gray", img_gray );
    
    // 轉yuv, 用img show無法正常顯示
    cv::Mat img_yuv;
    cvtColor( img, img_yuv, cv::COLOR_BGR2YUV );
    imshow( "yuv", img_yuv );
    
    cv::waitKey(WAIT_TIME);
}







void load_image_1()
{
    try {
        std::string image_path = get_jpg_path();
        cv::Mat img = cv::imread( image_path, cv::IMREAD_COLOR );
        //cv::Mat img = cv::imread( image_path, cv::IMREAD_GRAYSCALE ); // load and convert to gray scale.

        // 複製影像的範例. 直接使用等號, 會是light copy
        // cv::Mat img2(img); // light copy
        // img2 = img; // light copy
        // cv::Mat img2 = img.clone(); // deep copy
        // img.copyTo(img2); // deep copy

        std::cout << "cols = " << img.cols << 
                     "\nrows = " << img.rows << 
                     "\nchannels = " << img.channels() << "\n";

        if( img.empty() )
        {
            std::cout << "Could not read the image: " << image_path << std::endl;
            return;
        }

        cv::Mat sub_img_1( img, cv::Rect( 404, 393, 600, 500) ); // using a rectangle
        // 也有底下的寫法
        // cv::Rect r(404,393,600,500);
        // sub_img_1 = img(r);  
        cv::Mat sub_img_2 = img( cv::Range::all(), cv::Range(200,700) ); // using row and column boundaries

        imshow( "Display window", img );
        imshow( "sub img 1", sub_img_1 );
        imshow( "sub img 2", sub_img_2 );
    
        int k = cv::waitKey(WAIT_TIME); // Wait for a keystroke in the window
        if( k == 's' )    
            imwrite( "output.png", img );
    } 
    catch( std::exception e ) 
    {
        std::cout << "err = " << e.what();
    }
}






void load_image_2()
{
    std::string image_path = get_jpg_path();
    cv::Mat img = cv::imread( image_path, cv::IMREAD_COLOR );    
    cv::resize( img, img, cv::Size(720,480) );
    
    cv::Mat new_img = img.clone();   

    int x, y, c;

    for( y = 0; y < img.rows; y++ ) 
    {
        for( x = 0; x < img.cols; x++ ) 
        {
            // 如何對像素做操作的範例
#if 0
            for( c = 0; c < img.channels(); c++ )             
                new_img.at<cv::Vec3b>(y,x)[c] = cv::saturate_cast<uchar>( 1.5*img.at<cv::Vec3b>(y,x)[c] + 20 );
#else
            int b = img.at<cv::Vec3b>(y,x)[0];
            int g = img.at<cv::Vec3b>(y,x)[1];
            int r = img.at<cv::Vec3b>(y,x)[2];
            b = 0.8 * b + 30;
            g = 1.5 * g - 10;
            r = 2.2 * r + 20;
            new_img.at<cv::Vec3b>(y,x)[0] = cv::saturate_cast<uchar>(b);  // 限制範圍到 0~255
            new_img.at<cv::Vec3b>(y,x)[1] = cv::saturate_cast<uchar>(g);
            new_img.at<cv::Vec3b>(y,x)[2] = cv::saturate_cast<uchar>(r);
#endif
        }
    }    
    
    imshow( "img", img );
    imshow( "new", new_img );
    
    cv::waitKey(WAIT_TIME);
}



void create_image()
{
    cv::Mat M( 2,2,CV_8UC3, cv::Scalar(1,2,255) );
    std::cout << "M = " << std::endl << " " << M << std::endl << std::endl;

    cv::Mat E = cv::Mat::eye(4, 4, CV_64F);
    std::cout << "E = " << "\n" << " " << E << "\n\n";
    cv::Mat O = cv::Mat::ones(2, 2, CV_32F);
    std::cout << "O = " << "\n" << " " << O << "\n\n";
    cv::Mat Z = cv::Mat::zeros(3,3, CV_8UC1);
    std::cout << "Z = " << "\n" << " " << Z << "\n\n";

    cv::Mat C = (cv::Mat_<double>(3,3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
    std::cout << "C = " << "\n" << " " << C << "\n\n";

    //std::cout << "C[2][2] = " C[2][2];

    cv::Mat image( 500, 500, CV_8UC3, cv::Scalar( 255, 20, 10) );
    imshow( "test", image );
    cv::waitKey(WAIT_TIME);

    image.create( 300, 300, CV_8UC3 );
    imshow( "test2", image );
    cv::waitKey(WAIT_TIME);
}



void point_test()
{
    cv::Point2f P(5,1);
    std::cout << "Point (2D) = " << P << "\n\n";

    cv::Point3f P3f(2,6,7);
    std::cout << "Point (3D) = " << P3f << "\n\n";

    std::vector<float> v;
    v.push_back( (float)CV_PI );   
    v.push_back(2);    
    v.push_back(3.01f);

    std::cout << "Vector of floats via Mat = " << cv::Mat(v) << "\n\n";

    
    std::vector<cv::Point2f> vPoints(20);
    
    for( size_t i = 0; i < vPoints.size(); ++i )
        vPoints[i] = cv::Point2f((float)(i * 5), (float)(i % 7));
        
    std::cout << "A vector of 2D Points = " << vPoints << "\n\n";
}




void liner_blend()
{
    cv::VideoCapture vcap( get_mp4_path() );
    std::string image_path = get_jpg_path();

    cv::Mat img = cv::imread( image_path, cv::IMREAD_COLOR );
    cv::Mat frame, mix_img;
    
    vcap >> frame;
    resize( img, img, frame.size() );
    
    double alpha, beta;
    
    int count = 0;

    while(true)
    {
        std::cout << count++ << "\n";
    
        vcap >> frame;
        if( frame.ptr() == nullptr )
            break;
        
        if( (count/100) % 2 == 0 )
            alpha = 0.01 * (count % 100);
        else
            alpha = 0.01 * ( 100 - count % 100 );
        
        beta = 1.0 - alpha;
        cv::addWeighted( frame, alpha, img, beta, 0.0, mix_img );
        imshow("liner blend", mix_img);
        
        cv::waitKey(1);
    }

    vcap.release();
}




/*
Mat mtx(3, 3, CV_32F); // make a 3x3 floating-point matrix
Mat cmtx(10, 1, CV_64FC2); // make a 10x1 2-channel floating-point
// matrix (10-element complex vector)
Mat img(Size(1920, 1080), CV_8UC3); // make a 3-channel (color) image
// of 1920 columns and 1080 rows.
Mat grayscale(image.size(), CV_MAKETYPE(image.depth(), 1)); // make a 1-channel image of
// the same size and same
// channel type as img
*/




/*
Mat a(100, 100, CV_32F);
randu(a, Scalar::all(1), Scalar::all(std::rand()));
cv::log(a, a);
a /= std::log(2.);
*/




/*
// create a big 8Mb matrix
Mat A(1000, 1000, CV_64F);
// create another header for the same matrix;
// this is an instant operation, regardless of the matrix size.
Mat B = A;
// create another header for the 3-rd row of A; no data is copied either
Mat C = B.row(3);
// now create a separate copy of the matrix
Mat D = B.clone();
// copy the 5-th row of B to C, that is, copy the 5-th row of A
// to the 3-rd row of A.
B.row(5).copyTo(C);
// now let A and D share the data; after that the modified version
// of A is still referenced by B and C.
A = D;
// now make B an empty matrix (which references no memory buffers),
// but the modified version of A will still be referenced by C,
// despite that C is just a single row of the original A
B.release();
// finally, make a full copy of C. As a result, the big modified
// matrix will be deallocated, since it is not referenced by anyone
C = C.clone();
*/





/*
Mat mtx(3, 3, CV_32F); // make a 3x3 floating-point matrix
Mat cmtx(10, 1, CV_64FC2); // make a 10x1 2-channel floating-point
// matrix (10-element complex vector)
Mat img(Size(1920, 1080), CV_8UC3); // make a 3-channel (color) image
// of 1920 columns and 1080 rows.
Mat grayscale(image.size(), CV_MAKETYPE(image.depth(), 1)); // make a 1-channel image of
// the same size and same
// channel type as img
*/

