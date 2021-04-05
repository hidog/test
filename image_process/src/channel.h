#ifndef _CHANNEL_H
#define _CHANNEL_H


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	抽取色彩通道
	處理色彩等等的功能
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <QImage>
#include <QList>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfoListIterator>

#include "../tools.h"
//#include "../timer.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#ifdef _OPENPIV_
//#if defined(_OPENPIV_) || defined(_GTEST_)
#include "../src/andreaengine.h"
#include "../src/settings.h"
#include "../src/progress.h"
#include "../src/datacontainer.h"
#endif

#include "shadow.h"

#include <boost/math/distributions/normal.hpp>


#if 0



// square物件 
struct	SquareData
{
	int					direction;						// 方向
	int					x,	y;							// 左上角的座標
	int					size;							// 因為是正方形 只需要記錄一個size資料
	ShadowReturnData	*line_data;						// 將正方形切成很多個 1D 資料 
	SquareData()
	{
		line_data	=	NULL;
		size		=	0;
		x			=	0;
		y			=	0;
		direction	=	0;
	}
	SquareData( const SquareData &sd )
	{
		direction	=	sd.direction;
		x			=	sd.x;
		y			=	sd.y;
		size		=	sd.size >= 0 ? sd.size : 0;

		if( size > 0 )
		{
			line_data	=	new	ShadowReturnData[size];
			for( int i = 0; i < size; i++ )
				line_data[i]	=	sd.line_data[i];
		}
		else
			line_data	=	NULL;
	}
	~SquareData()
	{
		if( line_data )
			delete	[]	line_data;
		size	=	0;
		x		=	0;
		y		=	0;
	}
	void	operator = ( SquareData right )
	{
		direction	=	right.direction;
		x			=	right.x;
		y			=	right.y;
		size		=	right.size >= 0 ? right.size : 0;
		if( size > 0 )
		{
			line_data	=	new ShadowReturnData[size];
			for( int i = 0; i < size; i++ )
				line_data[i]	=	right.line_data[i];
		}
		else
			line_data	=	NULL;
	}
};





// KernelDensity 物件  這邊用來過濾雜訊
class	KernelDensity
{
	private:
		bMatrixInt		data;										// 用來分析的資料
		bMatrixDouble	p_data;										// 計算完的結果
		bMatrixDouble	kernel;										// smooth 的 kernel

	public:
		KernelDensity();											// 建構函數

		bMatrixInt		set_data( SquareData _data );				// 設定資料 會將格式做轉換
		bMatrixDouble	dosomething();								// 執行
		bMatrixDouble	kernel_gauss( int n );						// 產生 Gauss Kernel
		bMatrixDouble	kernel_square( int n );						// 產生基本的方塊狀 kernel
		int				element_count();							// 元素個數計算

};






// channel物件
#ifdef _OPENPIV_
class	Channel	:	public	AndreaEngine
#else
class	Channel
#endif
{
	private:

		int			det( int a, int b, int c, int d );								// cross使用 外積
		double		norm_max( double a, double b, double c );						// maximanorm
		QImage		image;
		QImage		final;

		// 底下是陰影檢測用
		QVector<PIXEL>	square;														// 檢測區塊		
		int				direction;													// 方向
		int				threshold;													
		SquareData		*square_data;												// 檢測區資料.
		SquareData		*square_data2;												// 因為斜向的必須做兩次 (因為起點應該是鋸齒狀)


#ifdef _OPENPIV_
		DataContainer	*filedata;
		Settings		*settings;
		QMutex			mutex;

		// 負責回報進度用的
		Progress		*progress;

	signals:
		void	debugSignal( QString );												// 除錯訊息

	protected:
#endif


	public:
#ifdef _OPENPIV_
		Channel( Settings *settingsPass, DataContainer *filedataPass );				// 建構
		void				dosomething();											// 都利用這個funtion來做事情. 取代原本的main
#else
		Channel();																	// 建構 無傳入值
#endif

		virtual				~Channel();												// 解構


		// 陰影檢測用的
		bVectorInt			loadimage( PIXEL p1, PIXEL p2 );						// 讀取圖片資料
		SquareData			shadow_square( PIXEL square_roi );						// 一次檢查一個區塊
		ShadowReturnData	shadow_line( PIXEL start, PIXEL end );					// 一次檢查一條線
		void				mark_on_image( SquareData square );						// 將結果標記在圖片上 顯示 
		void				mark_on_image( SquareData square, QImage &img );		// 將結果標記在圖片上 顯示 
		double				centroid( bVectorInt deep, int start, int end );		// 計算質心位置


};
















#endif







#endif