#include "andreaimage.h"


/***************************************************************************************
	建構
/****************************************************************************************/
AndreaImage::AndreaImage()
{
	ptr		=	0;
	w		=	0;
	h		=	0;
	size	=	0;
	format	=	BGR;		// 預設值
}


/***************************************************************************************
	建構
	輸入指標 寬 高
/****************************************************************************************/
AndreaImage::AndreaImage( unsigned char *_ptr, int _w, int _h, FORMAT _format )
{
	assert( _ptr != 0 && _w > 0 && _h > 0 );

	set( _ptr, _w, _h, _format );
}


int fff(int a, int b )
{
	return	a+b;
}

/***************************************************************************************
	複製建構
/****************************************************************************************/
AndreaImage::AndreaImage( AndreaImage &a )
{
	*this	=	a;
}


/***************************************************************************************
	等於運算
/****************************************************************************************/
void	AndreaImage::operator = ( AndreaImage &a )
{
	assert( a.ptr != 0 && a.w > 0 && a.h > 0 );

	ptr		=	a.ptr;
	w		=	a.w;
	h		=	a.h;
	format	=	a.format;

	if( format == BGR || format == RGB )
		size	=	w * h * 3;		// 深度24bit
	else if( format == GRAY )
		size	=	w * h;			// 深度8bit
	else
		assert(false);

}



/***************************************************************************************
	解構
	注意這邊不做刪除ptr的動作
/****************************************************************************************/
AndreaImage::~AndreaImage()
{
	ptr		=	0;
	w		=	0;
	h		=	0;
	size	=	0;
	format	=	(FORMAT)0;
}


/***************************************************************************************
	設定資料
/****************************************************************************************/
void	AndreaImage::set( unsigned char *_ptr, int _w, int _h, FORMAT _format )
{
	assert( _ptr != 0 && _w > 0 && _h > 0 );

	ptr		=	_ptr;
	w		=	_w;
	h		=	_h;
	size	=	w * h * 3;		// 深度 24 bit
	format	=	_format;
}


/***************************************************************************************
	回傳寬
/****************************************************************************************/
int		AndreaImage::width()
{
	return	w;
}


/***************************************************************************************
	回傳高
/****************************************************************************************/
int		AndreaImage::height()
{
	return	h;
}


/***************************************************************************************
	回傳格式 
	(RGB BGR gray)
/****************************************************************************************/
AndreaImage::FORMAT		AndreaImage::get_format()
{
	return	format;
}


/***************************************************************************************
	回傳紅色通道
/****************************************************************************************/
int		AndreaImage::red_channel( int i, int j ) const
{
	assert( j >= 0 && j < h && i >= 0 && i < w );

	if( format == AndreaImage::BGR )
		return	*(ptr + j*3*w + i*3 + 2);
	else if( format == AndreaImage::RGB )
		return	*(ptr + j*3*w + i*3);
	else
		return	*(ptr + j*w + i);
}


/***************************************************************************************
	回傳綠色通道
/****************************************************************************************/
int		AndreaImage::green_channel( int i, int j ) const
{
	assert( j >= 0 && j < h && i >= 0 && i < w );
	if( format == AndreaImage::GRAY )
		return	*(ptr + j*3 + i);
	else
		return	*(ptr + j*3*w + i*3 + 1);
}


/***************************************************************************************
	回傳藍色通道
/****************************************************************************************/
int		AndreaImage::blue_channel( int i, int j ) const
{
	assert( j >= 0 && j < h && i >= 0 && i < w );
	if( format == AndreaImage::BGR )
		return	*(ptr + j*3*w + i*3 + 0);
	else if( format == AndreaImage::RGB )
		return	*(ptr + j*3*w + i*3 + 2);
	else
		return	*(ptr + j*w + i);
}


/***************************************************************************************
	回傳灰階
/****************************************************************************************/
int		AndreaImage::gray_channel( int i, int j ) const
{
	assert( j >= 0 && j < h && i >= 0 && i < w );
	if( format == AndreaImage::GRAY )
		return	*(ptr + j*w + i);
	else
		return	(red_channel(i,j)*11 + green_channel(i,j)*16 + blue_channel(i,j)*5)/32;
}

/***************************************************************************************
	寫入紅色通道
/****************************************************************************************/
void		AndreaImage::red_channel( int i, int j, int r )
{
	assert( j >= 0 && j < h && i >= 0 && i < w );
	if( format == AndreaImage::BGR )
		*(ptr + j*3*w + i*3 + 2)	=	r;
	else if( format == AndreaImage::RGB )
		*(ptr + j*3*w + i*3 + 0)	=	r;
	else
		*(ptr + j*w + i)			=	r;
}


/***************************************************************************************
	寫入綠色通道
/****************************************************************************************/
void		AndreaImage::green_channel( int i, int j, int g )
{
	assert( j >= 0 && j < h && i >= 0 && i < w );
	if( format == AndreaImage::GRAY )
		*(ptr + j*w + i )			=	g;
	else
		*(ptr + j*3*w + i*3 + 1)	=	g;
}



/***************************************************************************************
	寫入藍色通道
/****************************************************************************************/
void		AndreaImage::blue_channel( int i, int j, int b )
{
	assert( j >= 0 && j < h && i >= 0 && i < w );
	if( format == AndreaImage::BGR )
		*(ptr + j*3*w + i*3 + 0)	=	b;
	else if( format == AndreaImage::RGB )
		*(ptr + j*3*w + i*3 + 2 )	=	b;
	else
		*(ptr + j*w + i )			=	b;
}


/***************************************************************************************
	寫入灰階
/****************************************************************************************/
void 	AndreaImage::gray_channel( int i, int j, int gray )
{
	assert( j >= 0 && j < h && i >= 0 && i < w );
	if( format == AndreaImage::GRAY )
		*(ptr + j*w + i )	=	gray;
	else
	{
		red_channel( i, j, gray );
		green_channel( i, j, gray );
		blue_channel( i, j, gray );
	}
}


/***************************************************************************************
	寫入RGB
/****************************************************************************************/
void	AndreaImage::pixel_channel( int i, int j, int r, int g, int b )
{
	assert( j >= 0 && j < h && i >= 0 && i < w );
	if( format == AndreaImage::GRAY )
		*(ptr + j*w + i)	=	(r*11 + g*16 + b*5)/32;
	else
	{
		red_channel( i, j, r );
		green_channel( i, j, g );
		blue_channel( i, j, b );
	}
}

/***************************************************************************************
	寫入檔案
/****************************************************************************************/
void	AndreaImage::save( char *filename )
{
	assert(false);
}


/***************************************************************************************
	將資料全部設成指定的值
/****************************************************************************************/
void	AndreaImage::fill( const unsigned char _value )
{
	assert( ptr != 0 );
	if( format == AndreaImage::GRAY )
		memset( ptr, _value, w*h );
	else
		memset( ptr, _value, w*h*3 );
}








