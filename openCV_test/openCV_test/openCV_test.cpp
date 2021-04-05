// openCV_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
//#include "opencv2/opencv.hpp"

#include <QtGui\QImage>


void drawEllipse( QImage &img )
{
	int moment[6] = {0};  // m[0] = m00, m[1] = m10, m[2] = m01, m[3] = m20, m[4] = m11, m[5] = m02 
	int i, j;
	for( i = 0; i < img.width(); i++ )
	{
		for( j = 0; j < img.height(); j++ )
		{
			QRgb rgb = img.pixel( i, j );
			int gray = qGray(rgb);
			
			if( gray > 0 )
			{
				moment[0]++;
				moment[1] += i;
				moment[2] += j;
				moment[3] += i*i;
				moment[4] += i*j;
				moment[5] += j*j;
			}

		}
	}

	// Centroid (mean)
	const double x = moment[1] / moment[0];  // m10 / m00
	const double y = moment[2] / moment[0];  // m01 / m00
	
	// Covariance matrix [a b; b c]
	const double a = moment[3] / moment[0] - x * x;   // m20/m00 - x0^2
	const double b = moment[4] / moment[0] - x * y;
	const double c = moment[5] / moment[0] - y * y;

	std::cout << "x = " << x << ", y = " << y << "\na = " << a << ", b = " << b << ", c = " << c << "\n";
	
	// Eigenvalues of the covariance matrix
	const double A  = a + c;
	const double B  = a - c;
	const double C  = sqrt(4.0*b*b + B*B);
	const double e0 = (A + C) / 2.0; // First eigenvalue
	const double e1 = (A - C) / 2.0; // Second eigenvalue
	
	// Desired norm of the eigenvectors
	const double e0sq = sqrt(e0);
	const double e1sq = sqrt(e1);
	
	// Eigenvectors
	double v0x = e0sq;
	double v0y = 0.0;
	double v1x = 0.0;
	double v1y = e1sq;
	
	if( b != 0 ) 
	{
		v0x = e0 - c;
		v0y = b;
		v1x = e1 - c;
		v1y = b;
		
		// Normalize the eigenvectors
		const double n0 = e0sq / sqrt(v0x * v0x + v0y * v0y);
		v0x *= n0;
		v0y *= n0;
		
		const double n1 = e1sq / sqrt(v1x * v1x + v1y * v1y);
		v1x *= n1;
		v1y *= n1;
	}
	
  	const double M_PI = 3.1415926;
	for ( double t = 0.0; t < 2.0*M_PI; t += 0.001 ) 
	{
		int x2 = x + (cos(t) * v0x + sin(t) * v1x) * 2.0 + 0.5;
		int y2 = y + (cos(t) * v0y + sin(t) * v1y) * 2.0 + 0.5;
		
		if ( x2 >= 0 && x2 < img.width() && y2 >= 0 && y2 < img.height() )
			img.setPixel( x2, y2, qRgb(255,0,0) );
	}

	img.save("D:\\code\\ttttt.bmp");
}



int _tmain(int argc, _TCHAR* argv[])
{
	QImage img("D:\\code\\QualidiaCode_CutData_026.jpg");
	drawEllipse(img);

#if 0
	std::cout << "test";

	cv::VideoCapture vcap("D:\\code\\test\\test.mp4");

	if ( false == vcap.isOpened())
		return -1;

    cv::Mat frame, gray;
    while (true)
    {
		vcap >> frame;

		if( frame.ptr() == nullptr )
			break;

		//cv::cvtColor( frame, gray, CV_RGB2GRAY );
		cv::Sobel( frame, frame, cv::Size( 3, 3 ), 0, 0);

        imshow( "test", gray );
        if( cv::waitKey(60) > 0 )
            break;
    }
#endif

	system("PAUSE");
	return 0;
}

