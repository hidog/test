#ifndef _OCR_IMAGE_PROCESS_H
#define _OCR_IMAGE_PROCESS_H


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "OCR_shared.h"



#ifndef DEGREEOF
#define DEGREEOF(a) ((a*180.0)/M_PI)
#endif


#define	OCR_MASK_WIDTH	16
#define OCR_MASK_HEIGHT	16




#ifdef _OCR_USE_FLOAT_
void	OCR_Sampling( short* image_GRAY, short* Sampling_Image, int shift_h, int shift_v, int position_h, int position_v, int Image_width, int Image_height );		// sampling
#else
void	OCR_Sampling( BYTE* image_GRAY, short  *Sampling_Image, int shift_h, int shift_v, int position_h, int position_v, int Image_width, int Image_height );		// sampling
#endif

#ifdef _OCR_USE_FLOAT_
void	OCR_Haar_DWT(short* image_GRAY, float* DWT_image, int Dwidth, int Dheight);
#else
void	OCR_Haar_DWT( short* image_GRAY, int* DWT_image, int Dwidth, int Dheight);
#endif

#ifdef _OCR_USE_FLOAT_
void	OCR_Haar_DWT_2( float* Harr_image, float* DWT_image, int Dwidth, int Dheight);
#else
void	OCR_Haar_DWT_2( int* Harr_image, int* DWT_image, int Dwidth, int Dheight);
#endif


#ifdef _OCR_USE_FLOAT_
void	OCR_Calcu_DWT_L_H(float* DWT_image, float* LL_image, float* HL_image, float* LH_image, float* HH_image, int Dwidth, int Dheight);
#else
void	OCR_Calcu_DWT_L_H(int* DWT_image, int* LL_image, int* HL_image, int* LH_image, int* HH_image, int Dwidth, int Dheight);
#endif

#ifdef _OCR_USE_FLOAT_
void	OCR_HOG(float* HL_image, float* LH_image, float*HOG , int Dwidth, int Dheight);
#else
void	OCR_HOG(int* HL_image, int* LH_image, int *HOG , int Dwidth, int Dheight);
#endif

#ifdef _OCR_USE_FLOAT_
void	OCR_LSF(float* HL_image, float* LH_image, float* HH_image, float* LSF, int Dwidth, int Dheight);
#else
void	OCR_LSF(int* HL_image, int* LH_image, int* HH_image, int* LSF, int Dwidth, int Dheight);
#endif


float	OCR_Correlation( short *_source, short *_template, int label_source, int label, int _num_block, int _mask_size );



#ifdef _OCR_USE_FLOAT_
void	OCR_Pre_BZF(short *Sample_Binary, float *BZ_Feature, int DWT_width, int index);
#else
void	OCR_Pre_BZF(short *Sample_Binary, int *BZ_Feature, int DWT_width, int index);
#endif


#ifdef _OCR_USE_FLOAT_
void	OCR_BZF(float *BZF, int width, int height);
#else
void	OCR_BZF( int* BZF, int width, int height);
#endif





#endif