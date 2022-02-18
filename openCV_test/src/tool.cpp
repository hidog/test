#include "tool.h"

#include <opencv2/core.hpp>


std::string get_jpg_path()
{
#ifdef WIN32    
    std::string filename = cv::samples::findFile( "D:\\code\\P1060197.JPG" );
#elif defined(MACOS)
    std::string filename = cv::samples::findFile( "/Users/hidog/Downloads/test.jpg" );
#elif defined(UNIX)
    std::string filename = cv::samples::findFile( "/home/hidog/Pictures/test.jpg" );
#else
#error not defined.
#endif

    return filename;
}



std::string get_mp4_path()
{
#ifdef MACOS
    std::string filepath = cv::samples::findFile( "/Users/hidog/Downloads/test.jpg" );
#elif defined(UNIX)
    std::string filepath = "/home/hidog/Videos/test.mp4";
#elif defined(WIN32)
    std::string filepath = "D:\\code\\test.mp4";
#else
#error not defined
#endif

    return filepath;
}
