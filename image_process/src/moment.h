#ifndef _MOMENT_H
#define _MOMENT_H



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	texture
	moment
	目前使用 m00 m01 m10 m02 m11 m20   六維向量
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/




#include "mrfFeatureAbstract.h"

#include "../tools.h"
#include <assert.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>


// --------------------------------------------------------------- moment 物件 -----------------------------------------------------------------------------
class	Moment : public mrfFeatureAbstract
{
	private:	
		static const int			DIMENSION	=	6;					// m00 m01 m10 m02 m11 m20 共六個
		static const int			win_size	=	25;					// sliding window size

		std::vector<bMatrixDouble>	moment;								// 存放moment資料  (vector放前面 避免效能問題)  

		// virtual 區
		void				cal_mean();									// 計算平均值 這兩個必須由feature來呼叫
		void				cal_devia();								// 計算標準差

	public:
		Moment();														// 建構
		Moment( QImage imagePass );										// 建構
		Moment( const Moment &mt );										// copy construct
		~Moment();

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
		void				set_mean( sVectorDouble _m );				// 設定平均值
		sVectorDouble		get_mean();									// 回傳平均值
		void				set_devia( sVectorDouble _d );				// 設定標準差
		sVectorDouble		get_devia();								// 回傳標準差
		void				get_feature( int pix, double *_f );			// 回傳該pixel的feature
		bool				check_dim();								// 檢查維度
		void				cal_sample_feature( QImage img );			// 計算樣張 跟cal_feature內容會很像 差在這邊會算平均標準差

		// 非virtual 區
		void				cal_moment( int p, int q );					// 計算 moment p q
		int					MomentStep( int p, int q );					// 將 p, q 轉成 Moment[i] 的轉換公式

		double				get_data( int i, int j );					// 測試回傳資料

		// operator = 
		void	operator	=	( const Moment &mt );
		
};




#endif