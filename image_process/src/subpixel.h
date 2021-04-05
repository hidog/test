#ifndef _SUBPIXEL_H
#define _SUBPIXEL_H

#include "facet.h"

#include <QImage>
#include <QVector>

#include "./njet.h"
#include "./CV.h"
#include "../tools.h"

#include <qwt_painter.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_map.h>
#include <QGraphicsSvgItem>
#include <QRect>
#include <qframe.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_map.h>
#include <QGraphicsSvgItem>
#include <QPainter>
#include <qapplication.h>
#include <qpainter.h>
#include <qwt_math.h>
#include <qwt_symbol.h>
#include <qwt_curve_fitter.h>
#include <qwt_plot_grid.h>
#include <qwt_plot.h>



#include <iostream>
#include <fstream>

#ifdef _OPENPIV_
#include "../src/andreaengine.h"
#include "../src/settings.h"
#include <QSvgGenerator>
#include <QPainter>
#endif


/*=================================================================================================================
	Edge 
	可以做到subpixel的edge 
/*=================================================================================================================*/
#ifdef _OPENPIV_
class	Edge : public AndreaEngine
#else
class	Edge
#endif
{
	private:
		int						X,	Y;			// 針對此座標來做 subpixel的edge計算
		int						N;				// window size
		QImage					image;
		//QImage					curvature;		// 存放curvature的資料
		bMatrixDouble			curvature;

#ifdef _OPENPIV_
		DataContainer			*filedata;
		Settings				*settings;
		QMutex					mutex;
		Progress				*progress;		// 負責回報進度用的

	signals:
		void					debugSignal( QString );				// 除錯訊息
#endif

	public:

#ifdef _OPENPIV_
		Edge(  Settings *settingsPass, DataContainer *filedataPass  );									// 建構函數
#else
		Edge();
#endif

		virtual					~Edge();																// 解構
	
		void					dosomething();															// OPENPIV的程式進入點

		void					draw_subpixel_point( double x, double y, double xx, double yy );		// 畫subpixel的點上去
		void					draw_subpixel_point( QVector<QPointF> qf, QVector<QPointF> bf );		// 畫subpixel的點上去

		PointDouble				itepolation( int _x, int _y, double dir_x, double dir_y );				// 測試用 使用過點的內插法 去內插curvature 再去搜尋max當成edge subpixel
		double					ite_poly( int dx, int dy, double x, double y, double xx, double yy );	// 內插用的多項式

		QVector<QPointF>		extract_edge( PointInt lu, PointInt rd );								// 提取edge 這邊採用的是沿著edge方向去搜尋的策略

		void					testSVG();																// 測試產生SVG  (使用QWT)

};



/*=================================================================================================================
	corner
	可以做到subpixel的corner
/*=================================================================================================================*/
#ifdef _OPENPIV_
class	Corner : public AndreaEngine
#else
class	Corner 
#endif
{
	private:
		int						X,	Y;			// 針對此座標來做 subpixel的 corner 計算
		QImage					image;

#ifdef _OPENPIV_
		DataContainer			*filedata;
		Settings				*settings;
		QMutex					mutex;
		Progress				*progress;		// 負責回報進度用的

	signals:
		void					debugSignal( QString );				// 除錯訊息
#endif

	public:

#ifdef _OPENPIV_
		Corner(  Settings *settingsPass, DataContainer *filedataPass  );						// 建構函數
#else
		Corner();
#endif
		virtual			~Corner();																// 解構
	
		void			dosomething();															// OPENPIV的程式進入點

		void			draw_subpixel_point( double x, double y, double xx, double yy );		// 畫subpixel的點上去
		void			draw_subpixel_point( QVector<QPointF> qf );								// 畫subpixel的點上去

};




#endif