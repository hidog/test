
#include <opencv2/opencv.hpp>
#include <stdio.h>



using namespace std;
using namespace cv;




unsigned char yuv_buf[4147200] = {0};
    

FILE *out_fp = NULL;
    

void yuv2bgr()
{
    cv::Size szSize(1920, 1080);
    cv::Size reSize(720,480);

    cv::Mat mSrc(szSize,CV_8UC2, yuv_buf);
    cv::Mat mBGR(szSize, CV_8UC3);
    cv::Mat reBGR; //(reSize, CV_8UC3);
    
    cvtColor(mSrc, mBGR, cv::COLOR_YUV2BGR_UYVY );
    cv::resize( mBGR, reBGR, reSize );
    

    fwrite( reBGR.ptr(), 1, 1036800, out_fp );
    
    
    //imshow( "Image BGR", reBGR);
    //cv::waitKey(1);
}




int main( int argc, char** argv )
{    
    FILE *fp = fopen64( "/home/hidog/dst1080.yuv", "rb+" );
    out_fp = fopen( "/home/hidog/480p_rgb.yuv", "w+" );

    for( int index = 0; ; index++ )
    {
        printf( "index = %d ", index );
        int read = fread( yuv_buf, 1, 4147200, fp );
        printf( "read = %d\n", read );
        if( read <= 0 )
            break;
        
        yuv2bgr();
        //fwrite( buf, 1, 4147200, fp2 );
    }

    fclose(out_fp);    
    fclose(fp);

    return 0;
}



// 底下是要加入的範例


#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2\core\hal\interface.h>


void view_yuv( char *ptr )
{
    const int FRAME_SIZE = 1920 * 1080 * 2;   //一副圖所含的畫素個數  

    static cv::Mat yuv_img;    
    static bool first = false;
    if( first == false )
    {
        yuv_img.create( 1080, 1920, CV_8UC2 ); //建立新的圖片的大小    
        first = true;
    }
    memcpy( yuv_img.data, ptr, FRAME_SIZE*sizeof(unsigned char) );

    static cv::Mat rgb_img;
    cvtColor( yuvImg, rgbImg, CV_YUV2BGR_UYVY );    
    //cv::resizeWindow("decode", 1280, 720);
    imshow("decode", rgb_img);
    cv::waitKey(1);    
}


#endif



#if 0
	steady_clock
	
	time_point<system_clock,milliseconds> start, end;	
	start = time_point_cast<milliseconds>(system_clock::now());
	view_yuv( (char*)frame->ptr() );
	end = time_point_cast<milliseconds>(system_clock::now());
    auto diff = duration_cast<milliseconds>( end - start ).count();  
	char str[1000];
	sprintf( str, "diff = %ld\n", diff );
	OutputDebugStringA(str);
#endif






#include <chrono>
#include <ctime>
#include <iostream>

int main() {

  // std::chrono only
  auto now = std::chrono::system_clock::now();
  std::cout << "std::chrono only: " << std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() << std::endl;

  std::cout << "with std::ctime: " << std::chrono::seconds(std::time(NULL)).count() << std::endl;
}



