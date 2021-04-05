/*=============================================================================================================================

	各種工具
	凡是不想做成class
	或者共通的define typedef 
	都放在這邊.

/*=============================================================================================================================*/

#ifndef _TOOLS_H
#define _TOOLS_H

#include <QImage>
#include <QVector>

#include <boost/numeric/ublas/matrix.hpp>



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// point結構  可以決定是整數點還是浮點數點
template<class T>
struct	CCPOINT
{
	T	x;
	T	y;
	CCPOINT<T>( T _x = 0, T _y = 0 ) : x(_x), y(_y) {}
	//POINT( int _x = 0, int _y = 0 ) : x(_x), y(_y) {}
};

// window結構  決定範圍用
// 實際上是用 width*2+1   height*2+1  當成搜尋範圍   中心點左右距離
struct	WINDOW
{
	int		width;
	int		height;
	WINDOW( int _w, int _h ) : width(_w), height(_h) {}
};


// pixel  有點座標(整數 浮點數)  RGB
struct	PIXEL
{
	int		x,	y;
	int		square_size;			// 陰影用的 正方形的大小
	int		direction;				// 決定方向
	double	fx,	fy;
	QRgb	rgb;

	PIXEL( int _x, int _y, double _fx, double _fy, QRgb _rgb )
		:	x(_x), y(_y), fx(_fx), fy(_fy), rgb(_rgb)  {}

	PIXEL()	:	x(0), y(0), fx(0.), fy(0.), rgb(0)  {}
};



namespace M2I
{
	const int	default_value	=	0;			// 預設值
	const int	normalized		=	1;			// 標準化
};



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ typedef ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef		CCPOINT<int>							PointInt;
typedef		CCPOINT<double>							PointDouble;
typedef		boost::numeric::ublas::matrix<int>		bMatrixInt;
typedef		boost::numeric::ublas::matrix<double>	bMatrixDouble;
typedef		boost::numeric::ublas::vector<int>		bVectorInt;
typedef		boost::numeric::ublas::vector<double>	bVectorDouble;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QImage				Matrix2Image( bMatrixDouble map, int m2i = M2I::default_value );								// 將matrix轉成image 自動normalized
bMatrixDouble		matrix_normalized( bMatrixDouble map );															// 對matrix做normalized  (到0~255)
void				m2i_copy( bMatrixDouble &map, QImage &image );													// 將matrix的資料複製到image
bMatrixInt			local_max( bMatrixDouble map , WINDOW win, double threshold );									// 找出local max
QVector<QRgb>		Pixel2Rgb( QVector<PIXEL> pixel );																// 將pixel vector轉成 qrgb
QVector<QPoint>		Pixel2Point( QVector<PIXEL> pixel );
QVector<QPointF>	Pixel2PointF( QVector<PIXEL> pixel );

#endif