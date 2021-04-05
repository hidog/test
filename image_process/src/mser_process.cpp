
#include "stdafx.h"
#include "mser_process.h"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;


#define M_PI  3.14159265358979323846


void drawEllipse(const MSER::Region & region, int width, int height, int depth,
				QImage &final, const BYTE * color)
{
	// Centroid (mean)
	const double x = region.moments_[0] / region.area_;
	const double y = region.moments_[1] / region.area_;
	
	// Covariance matrix [a b; b c]
	const double a = region.moments_[2] / region.area_ - x * x;
	const double b = region.moments_[3] / region.area_ - x * y;
	const double c = region.moments_[4] / region.area_ - y * y;
	
	// Eigenvalues of the covariance matrix
	const double d  = a + c;
	const double e  = a - c;
	const double f  = sqrt(4.0 * b * b + e * e);
	const double e0 = (d + f) / 2.0; // First eigenvalue
	const double e1 = (d - f) / 2.0; // Second eigenvalue
	
	// Desired norm of the eigenvectors
	const double e0sq = sqrt(e0);
	const double e1sq = sqrt(e1);
	
	// Eigenvectors
	double v0x = e0sq;
	double v0y = 0.0;
	double v1x = 0.0;
	double v1y = e1sq;
	
	if (b) {
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
	
	for (double t = 0.0; t < 2.0 * M_PI; t += 0.001) {
		int x2 = x + (cos(t) * v0x + sin(t) * v1x) * 2.0 + 0.5;
		int y2 = y + (cos(t) * v0y + sin(t) * v1y) * 2.0 + 0.5;
		
		if ((x2 >= 0) && (x2 < width) && (y2 >= 0) && (y2 < height))
			final.setPixel( x2, y2, qRgb(0,0,255) );
			//image->pixel_channel( x2, y2, color[0], color[1], color[2] );
			//for (int i = 0; i < std::min(depth, 3); ++i)
				//bits[(y2 * width + x2) * depth + i] = color[i];
	}
}


QImage	mser_process( QImage imagePass )
{
	// Try to load the jpeg image
	int width;
	int height;
	int depth;
	vector<BYTE> original;

	QImage	final	=	imagePass;
	
	width	=	imagePass.width();
	height	=	imagePass.height();
	depth	=	imagePass.depth() / 8;

	original.resize( width*height*depth );

	// Create a grayscale image
	vector<BYTE> grayscale(width * height);
	
	for (int i = 0; i < width * height; ++i) 
	{	
		grayscale[i]	=	qGray( imagePass.pixel( i%width, i/width ) );
	}
	

		//MSER(bool eight = false, int delta = 2, double minArea = 0.0001, double maxArea = 0.5,
		 //double maxVariation = 0.5, double minDiversity = 0.33);

	//MSER mser8(true);
	MSER mser4( false, 2, 0.0001, 0.5, 0.5, 0.33 );
	
	vector<MSER::Region> regions[2];
	vector<MSER::Region> regionlist;
	
	//mser8(&grayscale[0], width, height, regions[0]);
	
	// Invert the pixel values
	//for (int i = 0; i < width * height; ++i)
	//	grayscale[i] = ~grayscale[i];
	
	mser4(&grayscale[0], width, height, regions[1], regionlist );



	//for( int i=0; i<regions[1].size(); i++ )
	//	cout << "region1 = " << regions[1][i] << endl;


	// Draw ellipses in the original image
	const BYTE colors[2][3] = {{255, 0, 0}, {0, 0, 255}};

		
	
	for (int i = 1; i < 2; ++i)
		for (int j = 0; j < regions[i].size(); ++j)
			drawEllipse(regions[i][j], width, height, depth, final, colors[i]);


	

	//for( int i=0; i<width*height; i++ )
	//	image2.gray_channel( i/height, i%height, original[i] );

	//image2.save_image("output.bmp");
	

	return	final;

}
