#ifndef _DISTANCE_H
#define _DISTANCE_H



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	texture
	distance
	使用image value作為特徵 
	最簡單的特徵
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



#include "mrfFeatureAbstract.h"

#include "../tools.h"
#include <assert.h>
#include <iostream>
#include <cmath>
#include <stdio.h>



// --------------------------------------------------------------- distance 物件 -----------------------------------------------------------------------------
class	Distance : public mrfFeatureAbstract
{
	private:	
		static const int	DIMENSION	=	1;
		int					anchor_x,									// 取樣範圍
							anchor_y,
							anchor_width,
							anchor_height;

		// virtual 區
		void				cal_mean();									// 計算平均值 這兩個必須由cal_feature來呼叫
		void				cal_devia();								// 計算標準差

	public:
		Distance();
		Distance( QImage imagePass );
		Distance( const Distance &dt );									// copy construct
		~Distance();

		// virtual 區
		void				segmentation( QImage img );					// 測試切割
		void				cal_feature( QImage img );					// 計算feature 同時會算平均值 標準差
		void				output_txt();								// 輸出檔案
		double				get_norm( int pix );						// 計算 |u|
		double				get_norm( int pix1, int pix2 );				// 計算兩個pixel的norm |u-v|
		MRF_FEATURE_TYPE	type();										// 回傳目前的feature type
		int					dim();										// 回傳維度
		double				get_norm_squ( int pix );					// 回傳 norm 的平方
		double				get_devia_det();							// 回傳deviation的determine  (因為對照的論文是加上 |Sigma| 不確定是norm還是det)
		void				get_feature( int pix, double *_f );			// 回傳該pixel的feature
		bool				check_dim();								// 檢查維度
		void				cal_sample_feature( QImage img );			// 計算樣張 跟cal_feature內容會很像 差在這邊會算平均標準差


		// 非virtual 區
		void				set_anchor( int x, int y, int w, int h );	// 設定取樣範圍

		// operator = 
		void	operator	=	( const Distance &dt );
};




#endif



