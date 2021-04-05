#include "OCR_image_process.h"
#include "LWOCR.h"
#include "OCR_shared.h"
	

static const int	ocr_tan_list[10]	=	{ 0, 36397, 83910, 173205, 567128, -567128, -173205, -83910, -36397, 0 };			// tan 20, tan 40, tan 80 ... tan 160 的值 放大 10^6



/******************************************************************************
	sampling
*******************************************************************************/
#ifdef _OCR_USE_FLOAT_
void	OCR_Sampling( short* image_GRAY, short* Sampling_Image,int shift_h, int shift_v, int position_h, int position_v, int Image_width, int Image_height )
#else
void	OCR_Sampling( BYTE* image_GRAY, short *Sampling_Image,int shift_h, int shift_v, int position_h, int position_v, int Image_width, int Image_height )
#endif
{
	int		i,	j;

	for( i = 0; i < OCR_MASK_HEIGHT; i++ )
		for( j = 0; j < OCR_MASK_WIDTH; j++ )
		{
			Sampling_Image[ i*OCR_MASK_WIDTH + j ]	=	image_GRAY[ (shift_v*position_v*Image_width) + i*Image_width + j + (shift_h*position_h) ];
		}
}


/******************************************************************************
	Harr
*******************************************************************************/
#ifdef _OCR_USE_FLOAT_
void	OCR_Haar_DWT( short *image_GRAY, float* DWT_image, int Dwidth, int Dheight )
#else
void	OCR_Haar_DWT( short *image_GRAY, int* DWT_image, int Dwidth, int Dheight )
#endif
{
	int		i,	j;

#ifdef _OCR_USE_FLOAT_
	float	v_matrix[300]	=	{0},	h_matrix[300]	=	{0};
#else
	int		v_matrix[300]	=	{0},	h_matrix[300]	=	{0};
#endif

////VERTICAL DWT
	for( i = 0; i < Dheight; i++ )
		for( j = 0; j < 0.5*Dwidth; j++ )
		{
			v_matrix[ i*Dwidth + j]		=	image_GRAY[ i*Dwidth + 2*j ] + image_GRAY[ i*Dwidth + 2*j + 1 ];

			v_matrix[ (int)(0.5*Dwidth) + (i*Dwidth) + j ]	=	image_GRAY[ i*Dwidth + 2*j ] - image_GRAY[ i*Dwidth + 2*j + 1 ];
		}
////HORIZONTAL DWT
	for( i = 0; i < Dwidth; i++ )
		for( j = 0; j < 0.5*Dheight; j++ )
		{
			h_matrix[ i + j*Dwidth ]	=	v_matrix[ i + 2*j*Dwidth ] + v_matrix[ i + (2*j+1)*Dwidth ];

			h_matrix[ (int)(0.5*Dwidth*Dheight) + i + j*Dwidth ]	=	v_matrix[ i + 2*j*Dwidth ] - v_matrix[ i + (2*j+1)*Dwidth ];
		}

	for( i = 0; i < Dheight; i++ )
		for( j = 0; j < Dwidth; j++ )
			DWT_image[ i*Dwidth + j ]	=	h_matrix[ i*Dwidth + j ];
}




/******************************************************************************
	Harr
*******************************************************************************/
#ifdef _OCR_USE_FLOAT_
void	OCR_Haar_DWT_2( float *Harr_image, float* DWT_image, int Dwidth, int Dheight )
#else
void	OCR_Haar_DWT_2( int *Harr_image, int* DWT_image, int Dwidth, int Dheight )
#endif
{
	int		i,	j;

#ifdef _OCR_USE_FLOAT_
	float	v_matrix[300]	=	{0},	h_matrix[300]	=	{0};
#else
	int		v_matrix[300]	=	{0},	h_matrix[300]	=	{0};
#endif

////VERTICAL DWT
	for( i = 0; i < Dheight; i++ )
		for( j = 0; j < 0.5*Dwidth; j++ )
		{
			v_matrix[ i*Dwidth + j]		=	Harr_image[ i*Dwidth + 2*j ] + Harr_image[ i*Dwidth + 2*j + 1 ];

			v_matrix[ (int)(0.5*Dwidth) + (i*Dwidth) + j ]	=	Harr_image[ i*Dwidth + 2*j ] - Harr_image[ i*Dwidth + 2*j + 1 ];
		}
////HORIZONTAL DWT
	for( i = 0; i < Dwidth; i++ )
		for( j = 0; j < 0.5*Dheight; j++ )
		{
			h_matrix[ i + j*Dwidth ]	=	v_matrix[ i + 2*j*Dwidth ] + v_matrix[ i + (2*j+1)*Dwidth ];

			h_matrix[ (int)(0.5*Dwidth*Dheight) + i + j*Dwidth ]	=	v_matrix[ i + 2*j*Dwidth ] - v_matrix[ i + (2*j+1)*Dwidth ];
		}

	for( i = 0; i < Dheight; i++ )
		for( j = 0; j < Dwidth; j++ )
			DWT_image[ i*Dwidth + j ]	=	h_matrix[ i*Dwidth + j ];
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef _OCR_USE_FLOAT_
void	OCR_Calcu_DWT_L_H( float* DWT_image, float* LL_image, float* HL_image, float* LH_image, float* HH_image, int Dwidth, int Dheight )
#else
void	OCR_Calcu_DWT_L_H( int* DWT_image, int* LL_image, int* HL_image, int* LH_image, int* HH_image, int Dwidth, int Dheight )
#endif
{	
	int		i,	j;

	for( i = 0; i < 0.5*Dheight; i++ )
		for( j = 0; j < 0.5*Dwidth; j++ )
		{
			LL_image[ (i*Dwidth/2) + j ]	= 	DWT_image[ i*Dwidth + j ];
			HL_image[ (i*Dwidth/2) + j ]	= 	DWT_image[ i*Dwidth + j + (Dwidth/2) ];
			LH_image[ (i*Dwidth/2) + j ]	= 	DWT_image[ (Dwidth*Dheight/2) + i*Dwidth + j ];
			HH_image[ (i*Dwidth/2) + j ]	= 	DWT_image[ (Dwidth*Dheight/2) + i*Dwidth + j + (Dwidth/2) ];
		}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef _OCR_USE_FLOAT_
void	OCR_HOG( float* HL_image, float* LH_image, float*HOG , int Dwidth, int Dheight )
#else
void	OCR_HOG( int* HL_image, int* LH_image, int *HOG , int Dwidth, int Dheight )
#endif
{
	//atan(x)傳回tan-1 x，傳回值是 0 到 -π/2 到 π/2 之間。
	//atan2(y, x)傳回tan-1 y/x，傳回值是 0 到 -π 到 π 之間。
#ifdef _OCR_USE_FLOAT_
	float	Gra_Ma;
	float	Orie;
#else
	int		Gra_Ma;
#endif

#ifdef _OCR_USE_FLOAT_
	float	MAX_HOG;
	float	MIN_HOG;
#else
	int		MAX_HOG;	
	int		MIN_HOG;	
#endif
	
	int		i,	j;
	int		index2;

#ifndef _OCR_USE_FLOAT_
	int		x,	y;
	int		index;
	static int	multi	=	100000;
#endif

	long long	ui_tmp1,	ui_tmp2;
	

	for( i = 0; i < 9; i++ )
		HOG[i]	=	0;

	// Andrea 修改這邊 避免掉float計算
	for( i = 0; i < Dwidth*Dheight; i++ )
	{	
		Gra_Ma	=	sqrt( pow(HL_image[i],2) + pow(LH_image[i],2) );

//#if 1
#ifdef _OCR_USE_FLOAT_
		Orie	=	abs( DEGREEOF( atan2(LH_image[i],HL_image[i]) ) );
			
		if( Orie >=0 && Orie <=20 )				////degree between 0 to 20
		{
			//HOG[0] += Gra_Ma;
			index2	=	0;
		}
		else if( Orie > 20 && Orie <= 40 )		////degree between 21 to 40
		{
			//HOG[1] += Gra_Ma;
			index2	=	1;
		}
		else if( Orie > 40 && Orie <= 60 )		////degree between 41 to 60
		{
			//HOG[2] += Gra_Ma;
			index2	=	2;
		}		
		else if( Orie > 60 && Orie <= 80)		////degree between 61 to 80
		{
			//HOG[3] += Gra_Ma;
			index2	=	3;
		}		
		else if( Orie > 80 && Orie <= 100 )		////degree between 81 to 100
		{
			//HOG[4] += Gra_Ma;
			index2	=	4;
		}		
		else if( Orie > 100 && Orie <= 120 )	////degree between 101 to 120
		{
			//HOG[5] += Gra_Ma;
			index2	=	5;
		}		
		else if( Orie > 120 && Orie <= 140 )	////degree between 121 to 140
		{
			//HOG[6] += Gra_Ma;
			index2	=	6;
		}		
		else if( Orie > 140 && Orie <= 160 )	////degree between 141 to 160
		{
			//HOG[7] += Gra_Ma;
			index2	=	7;
		}		
		else if( Orie > 160 && Orie <= 180 )	////degree between 161 to 180
		{
			//HOG[8] += Gra_Ma;
			index2	=	8;
		}

		HOG[index2]	+=	Gra_Ma;

#else
		x		=	HL_image[i];
		y		=	LH_image[i];
		index	=	-1;

		// 底下是直接窮舉
		if( x > 0 && y > 0 )
		{
			for( j = 1; j <= 3; j++ )		// { 0, 36, 83, 173, 567, -567, -173, -83, -36, 0 }
			{
				if( ocr_tan_list[j]*x < multi*y && multi*y <= ocr_tan_list[j+1]*x )
				{
					index	=	j;
					break;
				}
			}
			if( index == -1 )
			{
				if( ocr_tan_list[0]*x <= multi*y && multi*y <= ocr_tan_list[1]*x )	// 頭尾分開處理
					index	=	0;
				else 
					index	=	4;
			}
		}
		else if( x < 0 && y < 0 )
		{
			//x	=	-x;		// 直接反轉 然後同上
			y	=	-y;
			for( j = 5; j <= 8; j++ )
			{
				if( ocr_tan_list[j]*x > multi*y && multi*y >= ocr_tan_list[j+1]*x )
				{
					index	=	j;
					break;
				}
			}
			if( index == -1 )		// 頭尾分開處理
			{
				index	=	4;		// 例外狀態
			}
		}
		else if( x < 0 && y > 0 )
		{
			for( j = 5; j <= 8; j++ )
			{
				if( ocr_tan_list[j]*x > multi*y && multi*y >= ocr_tan_list[j+1]*x )
				{
					index	=	j;
					break;
				}
			}
			if( index == -1 )		// 頭尾分開處理
			{
				index	=	4;		// 例外狀態
			}
		}
		else if( x > 0 && y < 0 )
		{
			//x	=	-x;
			y	=	-y;	// 反轉後同上
			for( j = 1; j <= 3; j++ )		// { 0, 36, 83, 173, 567, -567, -173, -83, -36, 0 }
			{
				if( ocr_tan_list[j]*x < multi*y && multi*y <= ocr_tan_list[j+1]*x )
				{
					index	=	j;
					break;
				}
			}
			if( index == -1 )
			{
				if( ocr_tan_list[0]*x <= multi*y && multi*y <= ocr_tan_list[1]*x )	// 頭尾分開處理
					index	=	0;
				else 
					index	=	4;
			}
		}
		else if( x == 0 && y == 0 )
			index	=	0;
		else if( y == 0 && x > 0 )
			index	=	0;
		else if( y == 0 && x < 0 )
			index	=	8;
		else if( x == 0 )
			index	=	4;
		else
			printf("error...\n");

		HOG[index]	+=	Gra_Ma;
#endif
	}

	MAX_HOG		=	HOG[0];
	MIN_HOG		=	HOG[0];
	for( i = 0; i < 9; i++ )
	{
		if( HOG[i] >= MAX_HOG )
			MAX_HOG = HOG[i];
		if( HOG[i] <= MIN_HOG )
			MIN_HOG = HOG[i];
	}
	if( MAX_HOG <= MIN_HOG )
	{
		printf("Error! MAX_HOG<=MIN_HOG\n");
	}
	else
	{
		for( i = 0; i < 9; i++ )
		{
#ifndef _OCR_USE_FLOAT_
			//HOG[i] = ( 65534 * (HOG[i]-MIN_HOG) / (MAX_HOG-MIN_HOG) ) - 32767 ;
			ui_tmp1	=	HOG[i] - MIN_HOG;
			ui_tmp2	=	ui_tmp1 * Scale_Table[MAX_HOG-MIN_HOG];
			HOG[i]	=	(int)(ui_tmp2 - 32767);   //( (HOG[i]-MIN_HOG) * Scale_Table[MAX_HOG-MIN_HOG] ) - 32767 ;
#else
			HOG[i]	=	( 2.0*(HOG[i]-MIN_HOG) / (MAX_HOG-MIN_HOG) ) - 1;//normalized
#endif	
		}
	}
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef _OCR_USE_FLOAT_
void	OCR_LSF( float* HL_image, float* LH_image, float* HH_image, float* LSF, int Dwidth, int Dheight )
#else
void	OCR_LSF( int* HL_image, int* LH_image, int* HH_image, int* LSF, int Dwidth, int Dheight )
#endif
{
	int		i;

	int		MAX_LSF;
	int		MIN_LSF;

	long long	ui_tmp1,	ui_tmp2;

	for( i = 0; i < Dwidth*Dheight; i++ )
#ifdef _OCR_USE_FLOAT_
		LSF[i]	=	fabs(HL_image[i]) + fabs(LH_image[i]) + fabs(HH_image[i]);
#else
		LSF[i]	=	abs(HL_image[i]) + abs(LH_image[i]) + abs(HH_image[i]);
#endif

	MAX_LSF		=	LSF[0];
	MIN_LSF		=	LSF[0];

	for( i = 0; i < Dwidth*Dheight; i++ )
	{	
		if( LSF[i] >= MAX_LSF )
			MAX_LSF	=	LSF[i];
		if( LSF[i] <= MIN_LSF )
			MIN_LSF	=	LSF[i];
	}
	if( MAX_LSF <= MIN_LSF )
	{
		printf("Error! MAX_LSF<=MIN_LSF\n");
	}
	else
	{
		for( i = 0; i < Dwidth*Dheight; i++ )
		{
#ifndef _OCR_USE_FLOAT_
			//LSF[i] = ( 65534 * (LSF[i]-MIN_LSF) / (MAX_LSF-MIN_LSF) ) - 32767 ;
			ui_tmp1	=	LSF[i] - MIN_LSF;
			ui_tmp2	=	ui_tmp1 * Scale_Table[MAX_LSF-MIN_LSF];
			LSF[i]	=	(int)(ui_tmp2 - 32767);							//		( (LSF[i]-MIN_LSF) * Scale_Table[MAX_LSF-MIN_LSF] ) - 32767 ;

#else
			LSF[i]	=	(2.0*(LSF[i]-MIN_LSF) / (MAX_LSF-MIN_LSF)) - 1.0;	//normalized
#endif
		}
	}
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float	OCR_Correlation( short *_source, short *_template, int label_source, int label, int _num_block, int _mask_size )
{
	int		iii;

	int		v_shift_num;
	int		h_shift_num;

	int		i,	j,	k;
	int		shift;
	int		v_shift;
	int		h_shift;

	int		max_id;

	long long	lltmp1,	lltmp2;		// 這邊命名原則不好 以後有空再改
	long long	lltmp3,	lltmp4;
	long long	lltmp_a, lltmp_b;

	int		index_count_value = 16 * 16 * 12 * 4;
	int		block_count_value  = 16 * 16 * 12;
	int		shift_count_value  = 16 * 16;
	int 	index_coutn_ASST = 2 * 12 * 4;
	int 	block_count_ASST = 2 * 12;
	int 	shift_coutn_ASST = 2;


	float	fk,	
			p[10]		=	{0},
			_cor[10]	=	{0};


	// ~~~~~~~~~~~~~~~~~ float 分隔線
#ifdef _OCR_USE_FLOAT_
	float	temp;

	float		shift_template[300]	=	{0};

	float		shift_temp;

	float	shift_sum_source[20]	=	{0}, 
			shift_sum_template[20]	=	{0},
			shift_avg_source[20]	=	{0},
			shift_avg_template[20]	=	{0}, 
			shift_sum_square_source[20]	=	{0}, 
			shift_sum_square_template[20]	=	{0}, 
			shift_cor[20]	=	{0};
	
	
	float		tmep_avg = 0;
	float		temp_square = 0;

	float		shift_cor_de[20]	=	{0};	// 分母
#else
	int		temp;
	int		shift_template[300]	=	{0};
	int		shift_temp;

	int		shift_sum_source[20]	=	{0}, 
			shift_sum_template[20]	=	{0},
			shift_avg_source[20]	=	{0},
			shift_avg_template[20]	=	{0}, 
			shift_sum_square_source[20]	=	{0}, 
			shift_sum_square_template[20]	=	{0}, 
			shift_cor[20]	=	{0};
	
	
	int		tmep_avg = 0;
	int		temp_square = 0;

	int		shift_cor_de[20]	=	{0};	// 分母
#endif

	v_shift_num		=	3;	//垂直移動樣板數
	h_shift_num		=	4;	//水平移動樣板數

	for( i = 0; i < _num_block; i++ )
		p[i]	=	2;
	
	//for particular case to tune p parameters****************************************
	if( (label_source == 10 && label == 20)  || (label_source == 20 && label == 10) )//J, T
	{
		p[0] = 0, p[2] = 0, 
		p[1] = 8, p[3] = 0;
	}
	if( (label_source ==  4 && label == 15)  || (label_source == 15 && label ==  4) )//D, O
	{
		p[0] = 4, p[2] = 0, 
		p[1] = 4, p[3] = 0;
    }
	if( (label_source ==  7 && label == 15)  || (label_source == 15 && label ==  7) )//G, O
	{
		p[0] = 0, p[2] = 6, 
		p[1] = 0, p[3] = 2;
	}
	if( (label_source ==  6 && label == 16)  || (label_source == 16 && label ==  6) )//F, P
	{
		p[0] = 0, p[2] = 8, 
		p[1] = 0, p[3] = 0;
	}
	if( (label_source ==  12 && label ==  12) )//L防止左上髒汙
	{
		p[0] = 3, p[2] = 0, 
		p[1] = 3, p[3] = 4;
	}
	if( (label_source ==  2 && label == 34)  || (label_source == 34 && label ==  2) )//B, 8
	{
		p[0] = 4, p[2] = 0, 
		p[1] = 4, p[3] = 0;
	}
	//************************************************************************************
	

	for( i = 0; i < _num_block; i++ )
	{
		// 歸零
		for( shift = 0; shift < v_shift_num*h_shift_num; shift++ )
		{
			shift_sum_source[shift]				=	0;
			shift_sum_template[shift]			=	0;
			shift_avg_source[shift]				=	0;
			shift_avg_template[shift]			=	0;
			shift_sum_square_source[shift]		=	0;
			shift_sum_square_template[shift]	=	0;
			shift_cor[shift]					=	0;
			shift_cor_de[shift]					=	0;
		}
		
		for( v_shift = 0; v_shift < v_shift_num; v_shift++ )
		{
			for( h_shift = 0; h_shift < h_shift_num; h_shift++ )
			{

				tmep_avg		=	Shift_Template_ASST[( label * index_coutn_ASST ) + ( i * block_count_ASST ) + ((( v_shift * 4) + h_shift ) * shift_coutn_ASST ) ] ;
				temp_square		=	Shift_Template_ASST[( label * index_coutn_ASST ) + ( i * block_count_ASST ) + ((( v_shift * 4) + h_shift ) * shift_coutn_ASST ) + 1 ];

				////計算shift後的template
				for( j = 0; j < 16; j++ )
					for( k = 0; k < 16; k++ )	
						shift_template[ 16*j + k ] = Shift_Template_Value[ ( label * index_count_value) + ( i * block_count_value ) + ((( v_shift * 4) + h_shift ) * shift_count_value ) + (16 * j + k) ]; 

				////計算和
				for( j = 0; j < _mask_size; j++ )
					shift_sum_source[  (v_shift*h_shift_num) + h_shift]		+=	_source[ i*_mask_size + j ];
				
				////計算平均
				shift_avg_source  [ (v_shift*h_shift_num) + h_shift]	=	shift_sum_source[   (v_shift*h_shift_num) + h_shift] / _mask_size;
				temp													=	shift_sum_source[   (v_shift*h_shift_num) + h_shift] ;
				/*
					這邊不除 256 = _mask_size 是為了保持精確度
					sum(XY) - n*X_bar*Y_bar
					希望用 n*sum(XY) - n*n*X_bar*Y_bar 
					所以用temp暫存 真正的平均值要拿去算標準差用
				*/
				shift_avg_template[ (v_shift*h_shift_num) + h_shift]	=   tmep_avg;

				////計算平方和
				for( j = 0; j < _mask_size; j++ )
					shift_sum_square_source[   (v_shift*h_shift_num) + h_shift]		+=	(_source[i*(_mask_size) + j] - shift_avg_source[ (v_shift*h_shift_num) + h_shift] ) * (_source[i*(_mask_size) + j] - shift_avg_source[ (v_shift*h_shift_num) + h_shift] ) ;

				shift_sum_square_source[ (v_shift*h_shift_num) + h_shift ]			/=	256;		// andreas 新增 deviation 除 256
				shift_sum_square_template[   (v_shift*h_shift_num) + h_shift]		=	temp_square;
			
				////計算cor分子
				for( j = 0; j < _mask_size; j++ )
				{
					//shift_cor[ (v_shift*h_shift_num) + h_shift ]	+=	( _source[ i*(_mask_size) + j] - shift_avg_source[ (v_shift*h_shift_num) + h_shift]) * (shift_template[j] - tmep_avg );
																		//   X                                Y
					shift_cor[ (v_shift*h_shift_num) + h_shift ]	+=	_source[ i*(_mask_size) + j] * shift_template[j];
				}
				shift_cor[ (v_shift*h_shift_num) + h_shift ]	*=	256;		// n*sum(XY)
				                                              //   x_bar    y_bar   實際上隱藏了 256*256在裡面 要小心
				shift_cor[ (v_shift*h_shift_num) + h_shift ]	-=	temp * tmep_avg;

				shift_cor[ (v_shift*h_shift_num) + h_shift ]	/=	256;

#ifdef _OCR_USE_FLOAT_
				shift_cor[ (v_shift*h_shift_num) + h_shift]			=	shift_cor[ (v_shift*h_shift_num) + h_shift] / (sqrt(shift_sum_square_source[(v_shift*h_shift_num) + h_shift] * temp_square));
#else
				shift_cor_de[ (v_shift*h_shift_num) + h_shift ]		=	shift_sum_square_source[(v_shift*h_shift_num) + h_shift] * temp_square;
#endif
					
			}//h end
		}//v end
	
		shift_temp	=	shift_cor[0];
		max_id		=	0;

		for( j = 0; j < (v_shift_num * h_shift_num); j++ )
		{
#ifdef _OCR_USE_FLOAT_
			if( shift_temp <= shift_cor[j] )
			{
				shift_temp	=	shift_cor[j];
				max_id		=	j;
			}
#else
			if( shift_temp >= 0 && shift_cor[j] < 0 )
			{
				// 無動作
			}
			else if( shift_temp >= 0 && shift_cor[j] >= 0 )
			{
				lltmp1	=	shift_temp;
				lltmp2	=	shift_cor_de[max_id];
				lltmp3	=	shift_cor[j];
				lltmp4	=	shift_cor_de[j];

				lltmp_a	=	lltmp1 * lltmp1 * lltmp4;		// 這邊要小心 乘錯會有bug  目標是交叉相乘
				lltmp_b	=	lltmp3 * lltmp3 * lltmp2;

				if( lltmp_a <= lltmp_b )
				{
					shift_temp	=	shift_cor[j];
					max_id		=	j;
				}
			}
			else if( shift_temp < 0 && shift_cor[j] >= 0 )
			{
				shift_temp	=	shift_cor[j];
				max_id		=	j;
			}
			else if( shift_temp < 0 && shift_cor[j] < 0 )
			{
				lltmp1	=	shift_temp;
				lltmp2	=	shift_cor_de[max_id];
				lltmp3	=	shift_cor[j];
				lltmp4	=	shift_cor_de[j];

				lltmp_a	=	lltmp1 * lltmp1 * lltmp4;
				lltmp_b	=	lltmp3 * lltmp3 * lltmp2;

				if( lltmp_a >= lltmp_b )		// 都負要變號
				{
					shift_temp	=	shift_cor[j];
					max_id		=	j;
				}
			}
#endif

		}

#ifdef _OCR_USE_FLOAT_
		_cor[i]		=	shift_temp;
#else
		_cor[i]		=	1.0*shift_temp / sqrt( 1.0*shift_cor_de[max_id] );	
#endif
	
	}//block end

	fk	=	0;

	for( i = 0; i < _num_block; i++ )
		fk	+=	p[i] * _cor[i];
	
	return	fk;
}










//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef _OCR_USE_FLOAT_
void	OCR_Pre_BZF( short *Image_Binary, float *BZF, int Image_width, int index )
#else
void	OCR_Pre_BZF( short *Image_Binary, int *BZF, int Image_width, int index )
#endif
{
	int		i,	j;
	int		add_count;
	int		add_count2;
	int		count;

#ifdef _OCR_USE_FLOAT_
	float	diagonal[60]	=	{0};
#else
	int		diagonal[60]	=	{0};
#endif

	for( i = 0; i < ((2*Image_width) - 1) ; i++ )
	{
		diagonal[i] = 0;
	}
	add_count	=	0;
	add_count2	=	0;
	count		=	1;
	
	for( i = 0; i < 2*Image_width - 1; i++)
	{
		if( add_count < Image_width )
		{	
			for( j = 0; j < (add_count + 1); j++ )
			{	
				diagonal[i]	+=	Image_Binary[ add_count + (j*(Image_width - 1)) ];
				
			}
			add_count++;
			add_count2++;
		}
		else
		{
			add_count2--;			
			count++;
			for( j = 0; j < (add_count2); j++ )
			{	
				diagonal[i]		+=	Image_Binary[ (count*Image_width - 1) + (j*(Image_width - 1)) ];
			}
		}
	}
	
	BZF[index]	=	0;
	for( i = 0; i < (2*Image_width) - 1; i++ )
	{
		BZF[index] +=  diagonal[i];
	}
	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef _OCR_USE_FLOAT_
void	OCR_BZF( float*BZF, int BZ_H_Num , int BZ_V_Num )
#else
void	OCR_BZF( int *BZF, int BZ_H_Num , int BZ_V_Num )
#endif
{
	int		i,	j;
	long long	ui_tmp,	ui_tmp2;

#ifdef _OCR_USE_FLOAT_
	float	H_Sum[10]	=	{0},
			V_Sum[10]	=	{0};
#else
	long long	H_Sum[10]	=	{0},
				V_Sum[10]	=	{0};
#endif
	int	max,	min;
	

	for( i = 0; i < BZ_V_Num; i++ )
	{
		H_Sum[i] = 0;
	}
	for( i = 0; i < BZ_H_Num; i++ )
	{
		V_Sum[i] = 0;
	}
	////normalize
	max		=	BZF[0];
	min		=	BZF[0];
	for( i = 0; i < BZ_V_Num*BZ_H_Num; i++ )
	{
		if( max < BZF[i] )
			max	=	BZF[i];
		if( min > BZF[i] )
			min	=	BZF[i];
	}
	for( i = 0; i < BZ_V_Num*BZ_H_Num; i++ )
	{
#ifndef _OCR_USE_FLOAT_
		ui_tmp2	=	BZF[i] - min;
		ui_tmp	=	ui_tmp2 * Scale_Table[max-min];
		BZF[i]	=	(int)(ui_tmp - 32767);
#else
		BZF[i]	=	(2*(BZF[i] - min) / (max-min)) - 1;
#endif 
	}
	
	////水平和與垂直和
	for( i = 0; i < BZ_V_Num; i++ )
	{
		for( j = 0; j < BZ_H_Num; j++ )
		{
			H_Sum[i]	+=	BZF[i*BZ_H_Num + j];
		}
	}
	for( i = 0; i < BZ_H_Num; i++ )
	{
		for( j = 0;j < BZ_V_Num; j++ )
		{
			V_Sum[i]	+=	BZF[ j*BZ_H_Num + i ];
		}
	}
	////存入BZF特徵
	for( i = 0; i < BZ_H_Num + BZ_V_Num; i++ )
	{
		if(i < BZ_V_Num )
		{
			//BZF[ i + (BZ_H_Num*BZ_V_Num)]	=	( 25 * H_Sum[i] ) / 100 ;
			BZF[ i + (BZ_H_Num*BZ_V_Num)]	=	H_Sum[i] / 4 ;
		}  
		else
		{
			//BZF[ i + (BZ_H_Num*BZ_V_Num)]	=	( 20 * V_Sum[i - BZ_V_Num] ) / 100;
			BZF[ i + (BZ_H_Num*BZ_V_Num)]	=	V_Sum[i - BZ_V_Num] / 5;
		}
	}
}





