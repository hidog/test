//#include "stdafx.h"
#include "tools.h"


/******************************************************************************************************
	將 Matrix 轉成 Image
/*******************************************************************************************************/
QImage		Matrix2Image( bMatrixDouble map, int m2i )
{
	int			i,		j;
	double		max,	min;
	double		mag;
	QImage		image( map.size1(), map.size2(), QImage::Format_RGB888 );

	switch( m2i )
	{
		case	M2I::default_value :
			m2i_copy( map, image );
			break;
		case	M2I::normalized :
			map		=	matrix_normalized( map );
			m2i_copy( map, image );
			break;
		default:
			break;
	}


	return	image;
}

/******************************************************************************************************
	將pixel vector轉成 qrgb
/*******************************************************************************************************/
QVector<QRgb>		Pixel2Rgb( QVector<PIXEL> pixel )
{
	int				i;
	QVector<QRgb>	rgb;

	for( i = 0; i < pixel.size(); i++ )
		rgb.append( pixel[i].rgb );

	return	rgb;
}


/******************************************************************************************************
	將pixel vector轉成 QPoint
/*******************************************************************************************************/
QVector<QPoint>		Pixel2Point( QVector<PIXEL> pixel )
{
	int					i;
	QVector<QPoint>		point;

	for( i = 0; i < pixel.size(); i++ )
		point.append( QPoint( pixel[i].x, pixel[i].y ) );

	return	point;
}


/******************************************************************************************************
	將pixel vector轉成 QPointF
/*******************************************************************************************************/
QVector<QPointF>	Pixel2PointF( QVector<PIXEL> pixel )
{
	int					i;
	QVector<QPointF>	pointF;

	for( i = 0; i < pixel.size(); i++ )
		pointF.append( QPoint( pixel[i].fx, pixel[i].fy ) );

	return	pointF;
}



/******************************************************************************************************
	將matrix的資料複製到image
/*******************************************************************************************************/
void	m2i_copy( bMatrixDouble &map, QImage &image )
{
	int		i,	j;

	image	=	QImage( map.size1(), map.size2(), QImage::Format_RGB888 );

	for( i = 0; i < map.size1(); i++ )
		for( j = 0; j < map.size2(); j++ )
			image.gray_channel( i, j, map(i,j) );
}


/******************************************************************************************************
	對matrix做normalized  
	(到0~255)
/*******************************************************************************************************/
bMatrixDouble	matrix_normalized( bMatrixDouble map )
{
	int		i,		j;
	double	max,	min,	mag;

	// 搜尋max min
	max		=	-12345678;
	min		=	12345678;
	for( i = 0; i < map.size1();   i++ )
		for( j = 0; j < map.size2();   j++ )
		{
			if( max < map( i, j ) )		max		=	map( i, j );
			if( min > map( i, j ) )		min		=	map( i, j );
		}

	// normlized
	for( i = 0; i < map.size1(); i++ )
		for( j = 0; j < map.size2(); j++ )
		{
			mag				=	(255.0) * ( map(i,j) - min ) / ( max - min );
			map( i, j )		=	mag;
		}

	return	map;
}



/******************************************************************************************************
	找出local max
/*******************************************************************************************************/
bMatrixInt	local_max( bMatrixDouble map , WINDOW win, double threshold )
{
	int				i,		j;
	int				ii,		jj;
	bool			is_max;
	bMatrixInt		l_max;

	l_max.resize( map.size1(), map.size2() );
	for( i=0; i<map.size1(); i++ )
		for( j=0; j<map.size2(); j++ )
			l_max( i, j )	=	1;

	// 逐點去搜尋
	for( i = 0;  i < map.size1(); i++ )
		for( j = 0; j < map.size2(); j++ )
		{

			// 代表已經比某個點小了
			if( l_max( i, j ) == 0 )		continue;

			if( map( i, j ) < threshold )
			{
				l_max( i, j )	=	0;
				continue;
			}

			// 跟neighborhood去比較
			is_max	=	true;
			for( ii = i - win.width;  ii <= i + win.width;  ii++ )
			{
				for( jj = j - win.height;  jj <= j + win.height;  jj++ )
				{
					if(  ii < 0 || ii >= map.size1() || jj < 0 || jj >= map.size2() || ii == i || jj == j )
						continue;

					// 不是最大值
					if( map( i, j ) <= map( ii, jj ) )
					{
						l_max( i, j )	=	0;
						is_max			=	false;
						break;
					}

					else 
						l_max( ii, jj )	=	0;
					

				}
				if( is_max == false )
					break;
			}// end 跟neighborhood去比較
		}

	return	l_max;
}