#ifndef _MRF_ENERGY_ABSTRACT_H
#define _MRF_ENERGY_ABSTRACT_H

#include "mrfEnergyAbstract.h"
#include "mrfFeatureAbstract.h"
#include "moment.h"
#include "distance.h"
#include "Gabor.h"
#include "COCCM.h"
#include "MRSAR.h"
#include <QImage>
#include <assert.h>



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	抽象Energy型別
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



enum	MRF_ENERGY_TYPE
{
	MRF_ENERGY_TEST,
	MRF_ENERGY_STANDARD,
};



class	mrfEnergyAbstract
{
	private:

	protected:
		mrfFeatureAbstract			*feature;
		QImage						image;

	public:
		mrfEnergyAbstract();																		// 建構
		mrfEnergyAbstract( mrfFeatureAbstract &ft, QImage img );									// 建構
		mrfEnergyAbstract( const mrfEnergyAbstract &e );											// copy construct
		virtual		~mrfEnergyAbstract();															// 解構

		virtual double				E_data( int pix, int lab )							=	0;		// data Energy
		virtual double				E_smooth( int pix1, int pix2, int lab1, int lab2 )	=	0;		// smooth Energy 
		virtual double				Prob( int pix, int lab )							=	0;		// 回傳機率密度函數值
		virtual MRF_ENERGY_TYPE		type()												=	0;		// 回傳自己是哪種類型

		int							get_img_width();												// 回傳圖片寬
		int							get_img_height();												// 回傳圖片高
		void						set_image( QImage img );										// 設定圖片
		MRF_FEATURE_TYPE			get_feature_type();												// 回傳 feature type
		QImage						get_image();													// 回傳image
		bool						check_image();													// 檢查跟feature的image是否相同
		bool						is_feature();													// 檢查feature是否初始化
		int							dim();															// 回傳feature的dim
		void						get_feature( int pix, double *_f );								// 取得該pixel的feature
		void						set_mean( double *_m );											// 設定平均 (到feature)
		void						get_mean( double *_m );											// 回傳feature平均
		void						set_devia( double *_d );										// 設定標準差				
		void						get_devia( double *_d );										// 回傳標準差
		bool						check_dim();													// 檢查 dimension
		void						cal_feature();													// 計算 feature

		void						output();														// 輸出資料 (測試用)

		void	operator = ( const mrfEnergyAbstract &e );


};





#endif