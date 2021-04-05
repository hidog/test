#ifndef _MRF_FEATURE_ABSTRACT_H
#define _MRF_FEATURE_ABSTRACT_H

#include "../tools.h"
#include <QImage>
#include <boost/numeric/ublas/matrix.hpp>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	抽象Feature型別
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


enum	MRF_FEATURE_TYPE
{
	MRF_FEATURE_MOMENT,
	MRF_FEATURE_DISTANCE,
	MRF_FEATURE_GABOR,
	MRF_FEATURE_COCCM,
	MRF_FEATURE_MRSAR,
};



class	mrfFeatureAbstract
{
	private:

	protected:
		QImage						sample;											// 樣張
		QImage						image;											// 當前計算feature的圖片

		int							dimension;										// 維度 (由衍生物件傳入)
		double						*mean;											// 平均值 (用pointer 為了加速)
		double						*devia;

		// 純虛擬函數
		virtual void				cal_mean()							=	0;		// 計算平均值 這兩個必須由cal_feature來呼叫  因為如果沒有feature,沒辦法計算平均值跟標準差
		virtual void				cal_devia()							=	0;		// 計算標準差

	public:
		mrfFeatureAbstract( int dim );
		mrfFeatureAbstract( QImage imagePass, int dim );							// 建構
		mrfFeatureAbstract( const mrfFeatureAbstract &ft );							// copy construct
		virtual		~mrfFeatureAbstract();											// 解構

		// 純虛擬函數
		virtual void				segmentation( QImage img )			=	0;		// 抽象切割 用來測試feature的鑑別度
		virtual	void				cal_feature( QImage img )			=	0;		// 計算feature 同時會算平均值 標準差
		virtual	void				output_txt()						=	0;		// 將feature輸出成檔案 (用txt格式)
		virtual double				get_norm( int pix )					=	0;		// 計算自己的 norm |u|
		virtual double				get_norm( int pix1, int pix2 )		=	0;		// 計算兩個pixel的norm |u-v|
		virtual	MRF_FEATURE_TYPE	type()								=	0;		// 回傳目前的feature type
		virtual int					dim()								=	0;		// 回傳維度
		virtual double				get_norm_squ( int pix )				=	0;		// 回傳 norm 的平方
		virtual double				get_devia_det()						=	0;		// 回傳deviation的determine  (因為對照的論文是加上 |Sigma| 不確定是norm還是det)  需要用到dimension,無法現在決定
		virtual void				get_feature( int pix, double *_f )	=	0;		// 回傳該pixel的feature
		virtual bool				check_dim()							=	0;		// 檢查維度
		virtual	void				cal_sample_feature( QImage img )	=	0;		// 計算樣張 跟cal_feature內容會很像 差在這邊會算平均標準差

		// 非純虛擬函數
		void						set_image( QImage img );						// 設定圖片
		QImage						get_image();									// 回傳圖片
		void						set_mean( double *_m );							// 設定平均值
		void						get_mean( double *_m );							// 回傳平均值
		void						set_devia( double *_d );						// 設定標準差
		void						get_devia( double *_d );						// 回傳標準差

		void	operator	=	( const mrfFeatureAbstract &ft );					// = operator

};





#endif