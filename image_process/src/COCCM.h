#ifndef _COCCM_H
#define _COCCM_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	texture
	co-occurence matrix
	C  OCC       M
	用來做統計 分類
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <QImage>
#include <boost/numeric/ublas/matrix.hpp>
#include "../tools.h"
#include <fstream>
#include <omp.h>
#include <cmath>
#include <vector>
#include "mrfFeatureAbstract.h"
#include <assert.h>



// --------------------------------------------------------------- co-occurence 物件 -----------------------------------------------------------------------------
class	COCCM : public mrfFeatureAbstract
{
	private:

		
		//omp_lock_t		writelock;

		static const int	DIMENSION	=	1;
		static const int	vec_size	=	10;										// 向量從 (0,0) ~ (10,10)
		static const int	sl_size		=	15;										// sliding windows size  (因為這邊會有兩個 windows size)

		std::vector<PIXEL>	*A;														// 暫存用 (為了加速 將Pd的資料做轉換放在這邊)  資料結構放PIXEL是為了方便  名稱上意義不對   
		bMatrixDouble		map;													// 存放計算好的特徵

		// virtual 區
		void				cal_mean();												// 計算平均值 這兩個必須由cal_feature來呼叫
		void				cal_devia();											// 計算標準差

	public:
		COCCM();
		COCCM( QImage imagePass );
		COCCM( const COCCM &cc );													// copy construct
		~COCCM();

		// virtual 區
		void				segmentation( QImage img );								// 測試切割
		void				cal_feature( QImage img );								// 計算feature
		void				output_txt();											// 輸出檔案
		double				get_norm( int pix );									// 計算 |u|
		double				get_norm( int pix1, int pix2 );							// 計算兩個pixel的norm |u-v|
		MRF_FEATURE_TYPE	type();													// 回傳目前的feature type
		int					dim();													// 回傳維度
		double				get_norm_squ( int pix );								// 回傳 norm 的平方
		double				get_devia_det();										// 回傳deviation的determine  (因為對照的論文是加上 |Sigma| 不確定是norm還是det)
		void				get_feature( int pix, double *_f );						// 回傳該pixel的feature
		bool				check_dim();											// 檢查維度
		void				cal_sample_feature( QImage img );						// 計算樣張 跟cal_feature內容會很像 差在這邊會算平均標準差



		void			cal_Pd( PIXEL d_vector, int x, int y, bMatrixInt &Pd );		// 計算Pd矩陣
		double				Energy( bMatrixInt Pd );				// 計算能量
		double				Entropy( bMatrixInt Pd );								// 計算熵
		double				Contrast( bMatrixInt Pd );								// 計算對比 
		double				Homogeneity( bMatrixInt Pd );							// 計算均勻度
		//double			Correlation( bMatrixInt Pd )


		void				output();												// 輸出到檔案
		double				get_feature_pix( int x, int y );						// 計算單點的 feature
		void				pre_Pd_data( int vec );									// 預先產生向量資料 
		void				cal_feature_abc( QImage img );

		void				test();


		// operator = 
		void	operator	=	( const COCCM &cc );

};


#endif