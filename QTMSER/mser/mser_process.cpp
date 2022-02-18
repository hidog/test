#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <iostream>

#include "mser.h"
#include <QImage>
#include <QDir>

#ifndef M_PI
#define M_PI       3.14159265358979323846   // pi
#endif



using namespace std;




bool loadJpeg( const char * filename, int & width, int & height, int & depth, vector<uint8_t> & bits )
{
    QImage image(filename);
    if( image.isNull() )
        return false;

    width = image.width();
    height = image.height();
    depth = 3; //image.depth();

    bits.resize( width*height*3 );

    int i, j;
    for( i = 0; i < width; i++ )
    {
        for( j = 0; j < height; j++ )
        {
            QRgb rgb = image.pixel( i, j );
            bits[ (j*width + i)*depth + 0 ] = qBlue(rgb);
            bits[ (j*width + i)*depth + 1 ] = qGreen(rgb);
            bits[ (j*width + i)*depth + 2 ] = qRed(rgb);
        }        
    }

    return true;
}





QImage saveJpeg( int width, int height, int depth, vector<uint8_t> & bits)
{
    QImage image( width, height, QImage::Format_RGB888 );
    
    int i, j;
    int r, g, b;
    for( i = 0; i < width; i++ )
    {
        for( j = 0; j < height; j++ )
        {
            b = bits[ (j*width + i)*depth + 0 ];
            g = bits[ (j*width + i)*depth + 1 ];
            r = bits[ (j*width + i)*depth + 2 ];
            
            image.setPixel( i, j, qRgb(r,g,b) );            
        }        
    }
    
    QString path = QDir::currentPath();
    QString file = path + QString("/output.jpg");
    
    image.save(file);
    
	return image;
}




void drawEllipse(const MSER::Region & region, int width, int height, int depth, vector<uint8_t> & bits, const uint8_t * color)
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
			for (int i = 0; i < std::min(depth, 3); ++i)
				bits[(y2 * width + x2) * depth + i] = color[i];
	}
}



QImage mser_process( const char *filepath )
{
    QImage img(100,100,QImage::Format_RGB888);
    
	// Try to load the jpeg image
	int width;
	int height;
	int depth;
	vector<uint8_t> original;
	
    if( !loadJpeg( filepath, width, height, depth, original) )
		return img;
	
	// Create a grayscale image
	vector<uint8_t> grayscale(width * height);
	
    for (int i = 0; i < width * height; ++i)
    {
		int sum = 0;
		
		for (int j = 0; j < depth; ++j)
			sum += static_cast<int>(original[i * depth + j]);
		
		grayscale[i] = sum / depth;
	}
	
	// Extract MSER
	clock_t start = clock();
	
	MSER mser8(2, 0.0005, 0.1, 0.5, 0.5, true);
	MSER mser4(2, 0.0005, 0.1, 0.5, 0.5, false);
	
	vector<MSER::Region> regions[2];
	
	mser8(&grayscale[0], width, height, regions[0]);
	
	// Invert the pixel values
	for (int i = 0; i < width * height; ++i)
		grayscale[i] = ~grayscale[i];
	
	mser4(&grayscale[0], width, height, regions[1]);
	
	clock_t stop = clock();
	
    cout << "Extracted " << (regions[0].size() + regions[1].size()) << " regions from " << filepath
		 << " (" << width << 'x' << height << ") in "
		 << (static_cast<double>(stop - start) / CLOCKS_PER_SEC) << "s." << endl;
	
	// Draw ellipses in the original image
	const uint8_t colors[2][3] = {{255, 0, 0}, {0, 0, 255}};
	
	for (int i = 0; i < 2; ++i)
    {
		for (int j = 0; j < regions[i].size(); ++j)
			drawEllipse(regions[i][j], width, height, depth, original, colors[i]);
    }

	// Save the original image
    img = saveJpeg( width, height, depth, original);
    return img;
}

