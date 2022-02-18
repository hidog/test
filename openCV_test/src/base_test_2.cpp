#include "base_test_2.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "tool.h"




void generate_test_image()
{
    constexpr int w = 400;

    auto draw_ellipse = [w] ( cv::Mat img, double angle )
    {
        int thickness = 2;
        int line_type = 8;
        ellipse( img, cv::Point( w/2, w/2 ), cv::Size( w/4, w/16 ),
                 angle, 0, 360, cv::Scalar( 255, 0, 0 ), thickness, line_type );
    };

    auto draw_filled_circle = [w] ( cv::Mat img, cv::Point center )
    {
        circle( img, center, w/32, cv::Scalar( 0, 0, 255 ), cv::FILLED, cv::LINE_8 );
    };

    auto draw_polygon = [w]( cv::Mat img )
    {
        int line_type = cv::LINE_8;
        cv::Point rook_points[1][20];
        rook_points[0][0]  = cv::Point(    w/4,   7*w/8 );        rook_points[0][1]  = cv::Point(  3*w/4,   7*w/8 );
        rook_points[0][2]  = cv::Point(  3*w/4,  13*w/16 );       rook_points[0][3]  = cv::Point( 11*w/16, 13*w/16 );
        rook_points[0][4]  = cv::Point( 19*w/32,  3*w/8 );        rook_points[0][5]  = cv::Point(  3*w/4,   3*w/8 );
        rook_points[0][6]  = cv::Point(  3*w/4,     w/8 );        rook_points[0][7]  = cv::Point( 26*w/40,    w/8 );
        rook_points[0][8]  = cv::Point( 26*w/40,    w/4 );        rook_points[0][9]  = cv::Point( 22*w/40,    w/4 );
        rook_points[0][10] = cv::Point( 22*w/40,    w/8 );        rook_points[0][11] = cv::Point( 18*w/40,    w/8 );
        rook_points[0][12] = cv::Point( 18*w/40,    w/4 );        rook_points[0][13] = cv::Point( 14*w/40,    w/4 );
        rook_points[0][14] = cv::Point( 14*w/40,    w/8 );        rook_points[0][15] = cv::Point(    w/4,     w/8 );
        rook_points[0][16] = cv::Point(    w/4,   3*w/8 );        rook_points[0][17] = cv::Point( 13*w/32,  3*w/8 );
        rook_points[0][18] = cv::Point(  5*w/16, 13*w/16 );       rook_points[0][19] = cv::Point(    w/4,  13*w/16 );

        const cv::Point* ppt[1] = { rook_points[0] };
        int npt[] = { 20 };
        //fillPoly( img, ppt, npt, 1, cv::Scalar( 255, 255, 255 ), line_type ); // ¶ñº¡
        constexpr int thinkness = 9;
        constexpr int shift = 0;
        polylines( img, ppt, npt, 1, true, cv::Scalar( 255, 255, 255 ), thinkness, line_type, shift );  // ¤£¶ñº¡
    };

    auto draw_line = [w] ( cv::Mat img, cv::Point start, cv::Point end )
    {
        int thickness = 2;
        int line_type = cv::LINE_8;
        line( img, start, end, cv::Scalar( 0, 0, 0 ), thickness, line_type );
    };

    char atom_window[] = "Drawing 1: Atom";
    char rook_window[] = "Drawing 2: Rook";

    cv::Mat atom_image = cv::Mat::zeros( w, w, CV_8UC3 );
    cv::Mat rook_image = cv::Mat::zeros( w, w, CV_8UC3 );

    draw_ellipse( atom_image, 90 );
    draw_ellipse( atom_image, 0 );
    draw_ellipse( atom_image, 45 );
    draw_ellipse( atom_image, -45 );

    draw_filled_circle( atom_image, cv::Point( w/2, w/2) );
    draw_polygon( rook_image );

    rectangle( rook_image, cv::Point( 0, 7*w/8 ), cv::Point( w, w ), cv::Scalar( 0, 255, 255 ), cv::FILLED, cv::LINE_8 );

    draw_line( rook_image, cv::Point( 0, 15*w/16 ),   cv::Point( w, 15*w/16 ) );
    draw_line( rook_image, cv::Point( w/4, 7*w/8 ),   cv::Point( w/4, w )     );
    draw_line( rook_image, cv::Point( w/2, 7*w/8 ),   cv::Point( w/2, w )     );
    draw_line( rook_image, cv::Point( 3*w/4, 7*w/8 ), cv::Point( 3*w/4, w )   );
    
    imshow( atom_window, atom_image );
    cv::moveWindow( atom_window, 0, 200 );

    imshow( rook_window, rook_image );
    cv::moveWindow( rook_window, w, 200 );

    cv::waitKey( WAIT_TIME );
}





void blur_test()
{
    cv::Mat src, dst;

    constexpr int MAX_KERNEL_LENGTH = 31;
    std::string filename = get_jpg_path();
    src = imread( filename, cv::IMREAD_COLOR );

    char text[300];

    dst = cv::Mat::zeros( src.size(), src.type() );

    auto display_caption = [ &dst, &text ] ( int wait )
    {
        putText( dst, text, cv::Point( dst.cols/8, dst.rows/2 ),
                 cv::FONT_HERSHEY_COMPLEX, 2, cv::Scalar( 0, 0, 255 ) );
        imshow( "test", dst );
        cv::waitKey(wait);
    };
    sprintf( text, "original image" );
    display_caption(0);

    //
    for( int i = 1; i < MAX_KERNEL_LENGTH; i += 2 )
    {
        blur( src, dst, cv::Size( i, i ), cv::Point(-1,-1) );
        sprintf( text, "blur, i = %d", i );
        display_caption(1);
    }
    sprintf( text, "finish" );
    display_caption(0);

    //
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
    {
        GaussianBlur( src, dst, cv::Size( i, i ), 0, 0 );
        sprintf( text, "g blur, i = %d", i );
        display_caption(1);
    }
    sprintf( text, "finish" );
    display_caption(0);

    //
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
    {
        medianBlur ( src, dst, i );
        sprintf( text, "m blur, i = %d", i );
        display_caption(1);
    }
    sprintf( text, "m finish" );
    display_caption(0);

    //
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
    {
        bilateralFilter ( src, dst, i, i*2, i/2 );
        sprintf( text, "bi, i = %d", i );
        display_caption(1);
    }
    sprintf( text, "bilateral finish" );
    display_caption(0);
}




