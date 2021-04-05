
#include "OCR_shared.h"




void Pre_Load_Shift_Template(int _index, int _block, int _v_shift, int _h_shift)
{
	int 	j, k;
	int		index_count_value = 16 * 16 * 12 * 4;
	int		block_count_value  = 16 * 16 * 12;
	int		shift_count_value  = 16 * 16;
	int 	index_coutn_ASST = 2 * 12 * 4;
	int 	block_count_ASST = 2 * 12;
	int 	shift_coutn_ASST = 2;

#ifdef _OCR_USE_FLOAT_
	float 	Shift_Sum_Template = 0;
	float 	Shift_Avg_Template = 0;
	float 	Shift_Sum_Square_Template = 0;
	float	temp;
#else
	int 	Shift_Sum_Template = 0;
	int 	Shift_Avg_Template = 0;
	int 	Shift_Sum_Square_Template = 0;
	int		temp;
#endif

	const int	multi	=	1;		// 小心這邊會overflow


	if( _block == 0 )
	{
		for( j = 0; j < 16; j++ )
			for( k = 0; k < 16; k++ )
			{
				Shift_Template_Value[ ( _index * index_count_value) + ( _block * block_count_value ) + ((( _v_shift * 4) + _h_shift ) * shift_count_value ) + (16 * j + k) ] 
									= template_original[ ( _index * 22 * 32 ) + 22 * ( _v_shift + j ) + k + _h_shift ];
				Shift_Sum_Template += template_original[ ( _index * 22 * 32 ) + 22 * ( _v_shift + j ) + k + _h_shift ];
			}
			temp				=	multi * Shift_Sum_Template;
			Shift_Avg_Template	=	multi * Shift_Sum_Template / 256; 
			/*
				這邊拿掉 除 256
				是為了將 n*X_bar*Y_bar 變成 n*n*X_bar*Y_bar
				原本correlation = sum(XY) - n*X_bar*Y_bar
				修改成 n*sum(XY) - n*n*X_bar*Y_bar  
				為了修正精確度問題
			*/
		
		for( j = 0; j < 16; j++ )
			for( k = 0; k < 16; k++ )
			{
				Shift_Sum_Square_Template +=	 ( multi * template_original[ ( _index * 22 * 32 ) + 22 * ( _v_shift + j ) + k + _h_shift ] - Shift_Avg_Template) * (  multi * template_original[ ( _index * 22 * 32 ) + 22 * ( _v_shift + j ) + k + _h_shift ] - Shift_Avg_Template) ; 
			}

		Shift_Template_ASST[( _index * index_coutn_ASST ) + ( _block * block_count_ASST ) + ((( _v_shift * 4) + _h_shift ) * shift_coutn_ASST ) ]		=	temp;
		Shift_Template_ASST[( _index * index_coutn_ASST ) + ( _block * block_count_ASST ) + ((( _v_shift * 4) + _h_shift ) * shift_coutn_ASST ) + 1 ]	=	Shift_Sum_Square_Template / (multi*multi) / 256;		// andrea 增加 deviation 除分母

	}
	else if( _block == 1 )
	{
		for( j = 0; j < 16; j++ )
			for( k = 0; k < 16; k++ )
			{
				Shift_Template_Value[ ( _index * index_count_value) + ( _block * block_count_value ) + ((( _v_shift * 4) + _h_shift ) * shift_count_value ) + (16 * j + k) ] = template_original[ ( _index * 22 * 32 ) + 22 * ( 16 - _v_shift + j ) + k + _h_shift ];
				Shift_Sum_Template += template_original[ ( _index * 22 * 32 ) + 22 * ( 16 - _v_shift + j ) + k + _h_shift ];
			}
			temp				=	Shift_Sum_Template;
			Shift_Avg_Template	=	multi * Shift_Sum_Template / 256;
		
		for( j = 0; j < 16; j++ )
			for( k = 0; k < 16; k++ )
			{
				Shift_Sum_Square_Template +=	 (multi * template_original[ ( _index * 22 * 32 ) + 22 * ( _v_shift + j ) + k + _h_shift ] - Shift_Avg_Template) * ( multi * template_original[ ( _index * 22 * 32 ) + 22 * ( _v_shift + j ) + k + _h_shift ] - Shift_Avg_Template) ;
			}

		Shift_Template_ASST[( _index * index_coutn_ASST ) + ( _block * block_count_ASST ) + ((( _v_shift * 4) + _h_shift ) * shift_coutn_ASST ) ]		=	temp;
		Shift_Template_ASST[( _index * index_coutn_ASST ) + ( _block * block_count_ASST ) + ((( _v_shift * 4) + _h_shift ) * shift_coutn_ASST ) + 1 ]	=	Shift_Sum_Square_Template / (multi*multi) / 256;

	}
	else if( _block == 2 )
	{
		for( j = 0; j < 16; j++ )
			for( k = 0; k < 16; k++ )
			{
				Shift_Template_Value[ ( _index * index_count_value) + ( _block * block_count_value ) + ((( _v_shift * 4) + _h_shift ) * shift_count_value ) + (16 * j + k) ]
									= template_original[ ( _index * 22 * 32 ) + 6 + ( 22 * ( _v_shift+j ) ) + k - _h_shift ];
				Shift_Sum_Template += template_original[ ( _index * 22 * 32 ) + 6 + ( 22 * ( _v_shift+j ) ) + k - _h_shift ];
			}
			temp				=	Shift_Sum_Template;
			Shift_Avg_Template	=	multi * Shift_Sum_Template / 256 ;
		
		for( j = 0; j < 16; j++ )
			for( k = 0; k < 16; k++ )
			{
				Shift_Sum_Square_Template +=	 ( multi * template_original[ ( _index * 22 * 32 ) + 22 * ( _v_shift + j ) + k + _h_shift ] - Shift_Avg_Template) * ( multi * template_original[ ( _index * 22 * 32 ) + 22 * ( _v_shift + j ) + k + _h_shift ] - Shift_Avg_Template) ;
			}

		Shift_Template_ASST[( _index * index_coutn_ASST ) + ( _block * block_count_ASST ) + ((( _v_shift * 4) + _h_shift ) * shift_coutn_ASST ) ]		=	temp;
		Shift_Template_ASST[( _index * index_coutn_ASST ) + ( _block * block_count_ASST ) + ((( _v_shift * 4) + _h_shift ) * shift_coutn_ASST ) + 1 ]	=	Shift_Sum_Square_Template / (multi*multi) / 256;

	}
	else if( _block == 3 )
	{
		for( j = 0; j < 16; j++ )
			for( k = 0; k < 16; k++ )
			{
				Shift_Template_Value[ ( _index * index_count_value) + ( _block * block_count_value ) + ((( _v_shift * 4) + _h_shift ) * shift_count_value ) + (16 * j + k) ]
									= template_original[ ( _index * 22 * 32 ) + 6 + ( 22 * ( 16 - _v_shift + j ) ) + k - _h_shift ];
				Shift_Sum_Template += template_original[ ( _index * 22 * 32 ) + 6 + ( 22 * ( 16 - _v_shift + j ) ) + k - _h_shift ];
			}
			temp				=	Shift_Sum_Template;
			Shift_Avg_Template	=	multi * Shift_Sum_Template / 256;
		
		for( j = 0; j < 16; j++ )
			for( k = 0; k < 16; k++ )
			{
				Shift_Sum_Square_Template +=	 ( multi * template_original[ ( _index * 22 * 32 ) + 22 * ( _v_shift + j ) + k + _h_shift ] - Shift_Avg_Template) * ( multi * template_original[ ( _index * 22 * 32 ) + 22 * ( _v_shift + j ) + k + _h_shift ] - Shift_Avg_Template) ;
			}

		Shift_Template_ASST[( _index * index_coutn_ASST ) + ( _block * block_count_ASST ) + ((( _v_shift * 4) + _h_shift ) * shift_coutn_ASST ) ]		=	temp;
		Shift_Template_ASST[( _index * index_coutn_ASST ) + ( _block * block_count_ASST ) + ((( _v_shift * 4) + _h_shift ) * shift_coutn_ASST ) + 1 ]	=	Shift_Sum_Square_Template / (multi*multi) / 256;

	}					
}


