/******************************************************************************
Copyright 1994 by the Massachusetts Institute of Technology.  All
rights reserved.

Developed by Thomas P. Minka and Rosalind W. Picard at the Media
Laboratory, MIT, Cambridge, Massachusetts, with support from BT, PLC,
Hewlett-Packard, and NEC.

Contributor to early versions of this software:
Fang Liu

This distribution is approved by Nicholas Negroponte, Director of
the Media Laboratory, MIT.

Permission to use, copy, or modify this software and its documentation
for educational and research purposes only and without fee is hereby
granted, provided that this copyright notice and the original authors'
names appear on all copies and supporting documentation.  If individual
files are separated from this distribution directory structure, this
copyright notice must be included.  For any other uses of this software,
in original or modified form, including but not limited to distribution
in whole or in part, specific prior permission must be obtained from
MIT.  These programs shall not be used, rewritten, or adapted as the
basis of a commercial software or hardware product without first
obtaining appropriate licenses from MIT.  MIT. makes no representations
about the suitability of this software for any purpose.  It is provided
"as is" without express or implied warranty.
******************************************************************************/

/* Compile this program with
 * cc mr_sar.c lapack.c basic.c -llapack -lblas -lf2c -lm
 * (Leave off -lf2c if you didn't use f2c to compile lapack.)
 */

/******************************************************************************
This program implements the MRSAR feature extraction described in

J. Mao and A.K. Jain, 
"Texture classification and segmentation using 
multiresolution simultaneous autoregressive models", 
Pattern Recognition, vol. 25, no. 2, pp. 173-188, 1992.

It works by computing MRSAR features (with resolution levels 2,3,4) on
overlapping 21x21 subwindows of the image. It outputs the mean and inverse
covariance matrix of those feature vectors. Similarity between images can then
be computed using Mahalanobis distance, Gaussian divergence, etc.
******************************************************************************/
#include "../tools.h"
#include "mr_sar.h"

#include <stdio.h>
#include <math.h>
#include <cstring>
#include <cstdlib>
#include "matrix.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


#define MODEL_ORDER 2
#define LSE_SIZE (MODEL_ORDER*2)
#define NFEATURES (LSE_SIZE+1)
#define WINDOW_INC 2     /* windows will overlap by window_size - WINDOW_INC */

double sar_std(Matrix s, double *x);
double MatrixCorrMN(Matrix a, int m, int n);
void MatrixCorr(Matrix src, Matrix dest);
void lse_neighbor_sum(Matrix win, int d, Matrix sum);
Matrix MRSAR(Matrix image, int *res_levels, int num_levels, int window_size);

/* Dumps the feature vectors over the whole image to a file. */
void DumpFeatureImages(Matrix features)
{
  int i;
  int f;
  FILE *fp;

  fp = fopen("features", "w");
  for(i=0;i<features->height;i++) {
 // for( i = 0; i < feature.height(); i++ )
    for(f=0;f<features->width;f++) {
//	for( f = 0; f < feature.width(); f++ )
      fprintf(fp, "%g ", features->data[i][f]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}

int		mr_sar_cal( QImage img, std::vector<bMatrixDouble> &map )
{
	int	DIMENSION	=	15;

  int res_levels[] = { 2,3,4 };
  int num_levels = 3;
  int window_size = 21;

  int width, height;
  Matrix image;
  Matrix features;
  Matrix cov, mean;
  FILE *fp;


  //width = atoi(argv[1]);
  //height = atoi(argv[2]);
  width		=	img.width();
  height	=	img.height();

  /* Read in the image */
  image = MatrixCreate(height, width);
  //fp = fopen(argv[3], "r");
  //fp	=	fopen( "../DATA/texture_1.bmp", "r" );
  //if(!fp) { perror(""); exit(1); }
  //MatrixReadImage(image, fp);
  //fclose(fp);

  for( int i = 0; i < img.width(); i++ )
	  for( int j = 0; j < img.height(); j++ )
		  image->data[i][j]	=	img.gray_channel( i,  j );

  /* Compute the matrix of features. */
  features = MRSAR(image, res_levels, num_levels, window_size);

  img	=	QImage(width - window_size + 1 , height - window_size + 1 ,QImage::Format_RGB888);


  map.resize( 15 );

  for( int i = 0; i < DIMENSION; i++ )
  {
	  map[i].resize( img.width(), img.height() );
	  map[i].clear();
  }



  double	sum;

  //ofstream	out("../TEMP/1.txt");

  for( int i = 0; i < img.width(); i++ )
  {
	  for( int j = 0; j < img.height(); j++ )
	  {
			for( int k = 0; k < DIMENSION; k++ )
				map[k](i,j)	=	features->data[i*img.height() + j][ k ];

	  }
  }



  return	0;




  for( int i = 0; i < img.width(); i++ )
	  for( int j = 0; j < img.height(); j++ )
	  {
		  sum	=	/*features->data[5][ i*img.height() + j ] + 
					features->data[6][ i*img.height() + j ] +
					features->data[7][ i*img.height() + j ] +
					features->data[8][ i*img.height() + j ] +*/
					features->data[i*img.height() + j][ 9 ];

		  img.gray_channel( i, j, sum );
	  }
  img.save("../TEMP/2.bmp");

  for( int i = 0; i < img.width(); i++ )
	  for( int j = 0; j < img.height(); j++ )
	  {
		  sum	=	/*features->data[10][ i*img.height() + j ] + 
					features->data[11][ i*img.height() + j ] +
					features->data[12][ i*img.height() + j ] +
					features->data[13][ i*img.height() + j ] +*/
					features->data[ i*img.height() + j][ 14 ];

		  img.gray_channel( i, j, sum );
	  }
  img.save("../TEMP/3.bmp");



  MatrixFree(image);





  /* Compute the covariance matrix by subtracting out the mean of each
   * coefficient and calculating the correlation.
   */
  mean = MatrixColMean(features);
  MatrixSubtractRow(features, mean->data[0]);
  cov = MatrixCreate(features->width, features->width);
  MatrixCorr(features, cov);

  /* Invert the covariance, for use in the Mahalanobis distance */
  MatrixInvert(cov);






  /* Output the mean and covariance of the features */
//  fp = fopen(argv[4], "w");
  fp	=	fopen( "../TEMP/out1.txt", "w+" );
  MatrixWriteVector(mean, fp);
  fclose(fp);
  //fp = fopen(argv[5], "w");
  fp	=	fopen( "../TEMP/out2.txt", "w+" );
  MatrixWriteVector(cov, fp);
  fclose(fp);
/*
  DumpFeatureImages(features);
*/
  return	1;
}

/* Computes the SAR parameters on the image by iterating
 * over overlapping windows of size <window_size> and 
 * estimating the AR model at each of the specified resolution levels.
 * The output matrix is a set of row vectors, where each row is
 * the estimated AR parameters (4 numbers) and the std dev of the error
 * (1 number) for each resolution level. For example, for the three
 * resolution levels 2,3,4, each row will have 15 numbers, 5 for each level:
 * | AR 2 | dev 2 | AR 3 | dev 3 | AR 4 | dev 4 |
 */
Matrix MRSAR(Matrix image, int *res_levels, int num_levels, int window_size)
{
  Matrix window;                 /* work window */
  double *p;
  int win_y, win_x, i,j,k, lvl;
  Matrix sum,                    /* work matrix for estimation */
         A;                      /* correlation of the sum matrix */
  double *B;                     /* estimated parameters */
  Matrix features;               /* output array */
  int eff_width, eff_height;     /* dimensions of the useful image */

  /* ignore the left and bottom borders of the image */
  eff_height = image->height - window_size + 1;
  eff_width = image->width - window_size + 1;

  /* create the matrices */
  features = MatrixCreate(eff_height*eff_width, NFEATURES * num_levels);
  window = MatrixCreate(window_size, window_size);
  sum = MatrixCreate(window_size*window_size, NFEATURES);
  A = MatrixCreate(LSE_SIZE, LSE_SIZE);
  B = Allocate(NFEATURES, double);

  /* loop across windows of the image */
  for(win_y = 0; win_y < eff_height; win_y += WINDOW_INC) {
    //printf("Row: %d\n", win_y);
    printf("*");
    for(win_x = 0; win_x < eff_width; win_x += WINDOW_INC) {

      /* copy image data into the window */
      p = window->data[0];
      for(i = 0; i < window->height; i++)
	for(j = 0; j < window->width; j++)
	  *p++ = image->data[win_y+i][win_x+j];

      /* debias the window */
      MatrixAddValue(window, -MatrixMean(window));

      /* loop resolution levels */
      for(lvl = 0; lvl < num_levels; lvl++) {

	/* compute the features over the window for this resolution */
	/* sum[i][LSE_SIZE] is the pixel value itself */
	lse_neighbor_sum(window, res_levels[lvl], sum);

	/* let A be the correlation matrix */
	MatrixCorr(sum, A);

	/* let B be the correlations with the pixel value */
	for(i=0; i<LSE_SIZE; i++) {
	  B[i] = MatrixCorrMN(sum, i, LSE_SIZE);
	}

	/* estimate the parameters; place in B */
	MatrixSolve(A, B);
	/* let the last element be the std dev of the error */
	B[LSE_SIZE] = sar_std(sum, B);

	/* put into the feature array */
	for(i=0;i<WINDOW_INC;i++) {
	  for(j=0;j<WINDOW_INC;j++) {
	    k = (win_y+i)*eff_width + (win_x+j);
	    memcpy(&features->data[k][lvl*NFEATURES],
		   B, NFEATURES*sizeof(double));
	  }
	}
      }
    }
  }
  MatrixFree(window);
  MatrixFree(sum);
  MatrixFree(A);
  free(B);
  return features;
}

/* Compute the std dev of the error from the estimated parameters
 * to the true pixel values.
 */
double sar_std(Matrix s, double *x)
{
  int i;
  double sum = 0.0, v;

  for(i=0;i<s->height;i++) {
    v = dot_product(s->data[i], x, LSE_SIZE) - s->data[i][LSE_SIZE];
    sum += v*v;
  }
  v = sqrt(sum / s->height);
  return v;
}

/* Computes the correlation between columns m and n in a */
double MatrixCorrMN(Matrix a, int m, int n)
{
  int i;
  double r = 0.0;

  for(i=0;i<a->height;i++) {
    r += a->data[i][m] * a->data[i][n];
  }
  return r / a->height;
}

/* Computes a matrix of correlations of src, placing it in dest */
void MatrixCorr(Matrix src, Matrix dest)
{
  int i,j;

  for(i=0;i<dest->height;i++) {
    for(j=i;j<dest->width;j++) {
      dest->data[j][i] = dest->data[i][j] = MatrixCorrMN(src, i, j);
    }
  }
}

/* Computes symmetric, noncausal AR features over all points in win */
void lse_neighbor_sum(Matrix win, int d, Matrix sum)
{
  int i,j, i1,i2, j1,j2;
  double *p;

  p = sum->data[0];
  for(i=0;i<win->height;i++) {
    i1 = (i - d + win->height) % win->height;
    i2 = (i + d) % win->height;
    for(j=0;j<win->width;j++) {
      j1 = (j - d + win->width) % win->width;
      j2 = (j + d) % win->width;
      *p++ = win->data[i1][j] + win->data[i2][j];
      *p++ = win->data[i][j1] + win->data[i][j2];
      *p++ = win->data[i1][j1] + win->data[i2][j2];
      *p++ = win->data[i1][j2] + win->data[i2][j1];
      *p++ = win->data[i][j];    /* last entry is true pixel value */
    }
  }
}
