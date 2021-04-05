#ifndef _ANDREAIMAGE_H
#define _ANDREAIMAGE_H

#include <cassert>
#include <string>




// class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class	AndreaImage
{
	public:
		typedef int (AndreaImage::*AndreaChannel)(int,int) const;									// 定義讀取通道

		enum FORMAT
		{
			BGR		=	0,		// 預設值
			RGB,
			GRAY
		};

		AndreaImage();
		AndreaImage( unsigned char *_ptr, int _w, int _h, FORMAT _format = BGR );					// 建構 輸入寬 高
		AndreaImage( AndreaImage &a );																// 複製建構
		~AndreaImage();

		void	operator = ( AndreaImage &a );														// 等於運算
	
		void			set( unsigned char *_ptr, int _w, int _h, FORMAT _format = BGR );			// 設定
		int				width();
		int				height();

		int				red_channel( int i, int j ) const;											// 回傳紅色通道 
		int				green_channel( int i, int j ) const;										// 回傳綠色通道
		int				blue_channel( int i, int j ) const;											// 回傳藍色通道
		int				gray_channel( int i, int j ) const;											// 回傳灰階
		void			red_channel( int i, int j, int r );											// 寫入紅色通道
		void			green_channel( int i, int j, int g );										// 寫入綠色通道
		void			blue_channel( int i, int j, int b );										// 寫入藍色通道
		void 			gray_channel( int i, int j, int gray ); 									// 寫入灰階
		void			pixel_channel( int i, int j, int r, int g, int b );							// 寫入RGB
		void			fill( const unsigned char _value );											// 將資料全部設成指定的值
		FORMAT			get_format();																// 回傳格式 (RGB BGR gray)

		void			save( char *filename );														// 寫入檔案  目前沒做事情


	private:
		unsigned char	*ptr;
		int				w;
		int				h;
		int				size;
		FORMAT			format;


};







#endif