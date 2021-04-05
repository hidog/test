#ifndef _BLOB_H
#define _BLOB_H

#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include "stdafx.h"
#include "fitting.h"
#include "gaussfilter.h"
#include "NChernov.h"

#include "../MEASURE/facet.h"
#include "../MEASURE/njet.h"
#include "../MEASURE/CV.h"

#include "../timer.h"

#include <QPoint>
#include <QList>
#include <QMutex>
#include <QProcess>

#ifdef _OPENPIV_
#include "../src/andreaengine.h"
#include "../src/settings.h"
#include "../src/progress.h"
#include "../src/datacontainer.h"
#endif


/*--------------           NOTE          -------------------------

	2012/10/15 開始修改成展示版本.

------------------------------------------------------------------*/

// 誤差容許範圍
//#define ERROR 0.00001

// 搜尋邊的門檻值
#define EDGE_THRESHOLD 20

//-----------------------struct區-----------------------------------------------------------------




//-----------------------class區-----------------------------------------------------------------
/*=============================================================
	blob物件
	使用sub pixel precise blob detection
/*=============================================================*/
#ifdef _OPENPIV_
class	SUBPIX_BLOB : public AndreaEngine
#else
class	SUBPIX_BLOB
#endif
{
	private:

		int		scale_w,	scale_l;								// scale 尺度
		
		boost::numeric::ublas::matrix<double>		curva_map;		// 存curvature
		boost::numeric::ublas::matrix<double>		r_x,	r_y;	// 一次微分項
		boost::numeric::ublas::matrix<double>		r_xx,	r_yy;	// 二次微分項
		boost::numeric::ublas::matrix<double>		r_xy;		

		int		_intLengthX, _intLengthY;
		
		QImage		image;						// 處理資料.  原始
		QImage		filter;						// filter後的資料
		//QImage		cur;						// curvature的資料
		QImage		final;						// 完工的資料


#ifdef _OPENPIV_
		QMutex			mutex;
		DataContainer	*filedata;
		Settings		*settings;
		Progress		*progress;					// 負責回報進度用的

	signals:
		void	debugSignal( QString );				// 除錯訊息

	protected:
#endif

	public:

#ifdef _OPENPIV_
		SUBPIX_BLOB( Settings *settingsPass, DataContainer *filedataPass );					// 建構函式

		void			dosomething();														// 都利用這個funtion來做事情. 取代原本的main
		void			dosomething3( QImage graytone );														// 測試用
#else
		SUBPIX_BLOB();
#endif
		
		// 解構
		virtual			~SUBPIX_BLOB();   

		// dosomething 2號 測試用
		void			dosomething2( char *str, int index );

		// 檢查是否是中心點
		bool			is_center( int x, int y, double *subpx, double *subpy );

		// 尋找出blob
		void			find_blob();

		// 尋找出blob  給定範圍
		void			find_blob( PointInt lu, PointInt rd );

		// 產生curvature
		void			curvature();

		// 產生curvature  給定範圍  左上 右下
		void			curvature( PointInt lu, PointInt rd );

		// 計算eigenvector   
		void			eigenvector( int x, int y, double *evx, double *evy, double rx, double ry, double rxx, double ryy, double rxy );

		// 求出polygen
		void			polygen( double subpx, double subpy );

		// 求 rx, ry, rxx, ryy, rxy
		void			diff();

		// 求 rx, ry, rxx, ryy, rxy  給定範圍
		void			diff( PointInt lu, PointInt rd );

		// 求出在該方向, 多少距離有出現 edge. 找不到的話 tmax = -1
		double			find_edge( double nx, double ny, double px, double py, double scale );

		// 畫出橢圓
		void			ellipse_draw();

		// 計算eigenvector   
		void			eigenvector( double A, double B, double C, double &vx, double &vy, double &wx, double &wy );

		// 計算eivenvector用的子function 做一些處理
		void			eigenvector_1( double A, double B, double C, double &nx, double &ny , double eigen );

		void			eigenvector2( int x, int y, double *evx, double *evy, double rx, double ry, double rxx, double ryy, double rxy );

		// 丟入matrix 轉成圖檔 
		void			output( boost::numeric::ublas::matrix<double> map, char *str );
		void			output( boost::numeric::ublas::matrix<int> map );

		// 搜尋local max min  使用 non maximal suppression    pmap = 上一個map   nmap = 下一個map   26個neighbor版本
		int				local_extreme( boost::numeric::ublas::matrix<double> map, boost::numeric::ublas::matrix<double> pmap, boost::numeric::ublas::matrix<double> nmap );

		// 搜尋local max min  使用 non maximal suppression    pmap = 上一個map   nmap = 下一個map	8個neighbor版本
		void			local_extreme( boost::numeric::ublas::matrix<double> map );

		// 計算中心的強度
		double			center_value( int x, int y );

		// 利用non maximal suppression 去得到中心點
		boost::numeric::ublas::matrix<int>	center_detector();

		// 測試效果的corner
		void			corner( int index );


};


#endif