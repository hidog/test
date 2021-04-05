#ifndef _MRSAR_H
#define _MRSAR_H


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
#include <cmath>
#include <vector>
#include "mrfFeatureAbstract.h"
#include <assert.h>
#include "../MRSAR/mr_sar.h"


// --------------------------------------------------------------- co-occurence 物件 -----------------------------------------------------------------------------
class	MRSAR : public mrfFeatureAbstract
{
	private:
		static const int	DIMENSION	=	15;										
   
		std::vector<bMatrixDouble>		map;													// 存放計算好的特徵

		// virtual 區
		void				cal_mean();												// 計算平均值 這兩個必須由cal_feature來呼叫
		void				cal_devia();											// 計算標準差

	public:
		MRSAR();
		MRSAR( QImage imagePass );
		MRSAR( const MRSAR &mr );													// copy construct
		~MRSAR();

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

		// operator = 
		void	operator	=	( const MRSAR &mr );

		void	output();
};



#endif