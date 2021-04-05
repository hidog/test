#ifndef _GABOR_H
#define _GABOR_H




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	texture
	Gabor
	利用sin, cos, exp產生filter
	透過此filter來萃取feature
	關於feature的設計還有討論空間
	(測試版本沒有對feature的計算去做調整  只有初步了解特性 速度過慢)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



#include <QImage>
#include "../tools.h"
#include "mrfFeatureAbstract.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include <boost/numeric/ublas/matrix.hpp>





// --------------------------------------------------------------- Gabor 物件 -----------------------------------------------------------------------------
class	Gabor : public mrfFeatureAbstract
{
	private:
		static const int	DIMENSION	=	1;

		static const int	win_size	=	10;								// sliding windows size
		double				sigma;											// Gauss function 參數									
		double				wave;											// 需要wave, theta這兩個參數  (挑一個讓標準差最小的組合)		
		double				theta;				

		bMatrixDouble		map;											// 將經過Gabor filter的資料存在這邊
		bMatrixDouble		map_tmp;										// 暫存用 map
		double				mean_tmp;										// 暫存用 mean
		double				devia_tmp;										// 暫存用 devia
		double				max_tmp;										// 暫存用最大值

		// virtual 區
		void				cal_mean();										// 計算平均值 這兩個必須由feature來呼叫
		void				cal_devia();									// 計算標準差

	public:
		Gabor();															// 建構 無傳入值
		Gabor( QImage imagePass );
		Gabor( const Gabor &gb );											// copy construct
		~Gabor();															// 解構

		// virtual 區
		void				segmentation( QImage img );						// 測試切割
		void				cal_feature( QImage img );						// 計算feature
		void				output_txt();									// 輸出檔案
		double				get_norm( int pix );							// 計算 |u|
		double				get_norm( int pix1, int pix2 );					// 計算兩個pixel的norm |u-v|
		MRF_FEATURE_TYPE	type();											// 回傳目前的feature type
		int					dim();											// 回傳維度
		double				get_norm_squ( int pix );						// 回傳 norm 的平方
		double				get_devia_det();								// 回傳deviation的determine  (因為對照的論文是加上 |Sigma| 不確定是norm還是det)
		void				get_feature( int pix, double *_f );				// 回傳該pixel的feature
		bool				check_dim();									// 檢查維度
		void				cal_sample_feature( QImage img );				// 計算樣張 跟cal_feature內容會很像 差在這邊會算平均標準差

		// 非virtual 區
		bMatrixDouble		RealPart( double wave, double theta );			// 產生real part的filter
		bMatrixDouble		ImagePart( double wave, double theta );			// 產生image part的filter
		void				cal_wh( QImage img );							// 計算 wave, theta 兩個參數  (在這邊會同時計算出標準差)
		void				cal_feature2( double _wave, double _theta );	// 給定 wave, theta 去計算 feature   回傳的是標準差
		void				cal_max();										// 計算最大值 (絕對值)

		// operator = 
		void	operator	=	( const Gabor &gb );

};



#endif