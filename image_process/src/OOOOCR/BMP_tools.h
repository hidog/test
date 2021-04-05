#ifndef BMP_TOOLS_H
#define BMP_TOOLS_H

#include <stdio.h>


#define	_32_BIT_	4		// 32 bit
#define _24_BIT_	3		// 24 bit
#define _8_BIT_		1		// 8  bit

#define BLUE_OFF_SET	0	// 讀取顏色的 offset
#define	GREEN_OFF_SET	1
#define RED_OFF_SET		2
#define	ALPHA_OFF_SET	3



typedef	unsigned char	BYTE;


//#pragma pack(1)
typedef struct BMP_FILE_HEADER_DEFINE
{
   unsigned short 	type;
   unsigned int   	size;
   unsigned short 	reserved1;
   unsigned short 	reserved2;
   unsigned int   	off_bits;
} BMP_FILE_HEADER ;
//#pragma pack()


//#pragma pack(1)
typedef struct	BMP_INF_HEADER_DEFINE
{
   unsigned int   	size;					// header size
   unsigned int   	width;
   unsigned int   	height;
   unsigned short 	planes;
   unsigned short 	bit_count;
   unsigned int   	compression;
   unsigned int   	size_image;				// width * height
   unsigned int   	x_pels_per_meter;
   unsigned int   	y_pels_per_meter;
   unsigned int   	clr_used;
   unsigned int   	clr_important;
} BMP_INF_HEADER ;
//#pragma pack()



typedef struct	BMP_DATA_DEFINE
{
	BMP_FILE_HEADER		file_header;
	BMP_INF_HEADER		inf_header;

	BYTE				*data;

} BMP_DATA;













BMP_DATA*			BMP_open( char *filename );													// 開啟檔案
void				BMP_save( char *filename, BMP_DATA *bmp_data );								// 寫入檔案
void				BMP_free( BMP_DATA *bmp_data );												// 釋放記憶體空間
BMP_DATA*			BMP_new( int width, int height, int depth );								// 新建一個 BMP 檔案  (資料初始化為0)

BMP_FILE_HEADER		BMP_file_load( FILE *p );													// 讀取 file header
void				BMP_file_save( FILE *p, BMP_FILE_HEADER file_header );						// 寫入 file header
BMP_INF_HEADER		BMP_inf_load( FILE *p );													// 讀取 information header
void				BMP_inf_save( FILE *p, BMP_INF_HEADER inf_header );							// 寫入 information header
void				BMP_data_load( FILE *p, BMP_DATA *bmp_data );								// 讀取圖檔資料 
void				BMP_data_save( FILE *p, BMP_DATA *bmp_data );								// 寫入圖檔資料

BYTE*				BMP_create_data( int width, int height, int depth );						// 會在這邊 malloc, 建立存放圖檔資料的空間

BYTE*				BMP_red_channel(   BMP_DATA *bmp_data, int x, int y );						// 讀取/寫入 紅色通道資料
BYTE*				BMP_green_channel( BMP_DATA *bmp_data, int x, int y );						// 讀取/寫入 綠色通道資料
BYTE*				BMP_blue_channel(  BMP_DATA *bmp_data, int x, int y );						// 讀取/寫入 藍色通道資料
BYTE*				BMP_alpha_channel( BMP_DATA *bmp_data, int x, int y );						// 讀取/寫入 alpha 通道資料
BYTE*				BMP_gray_channel(  BMP_DATA *bmp_data, int x, int y );						// 讀取/寫入 灰階值 (灰階圖使用)

int					BMP_gray_channel_read(  BMP_DATA *bmp_data, int x, int y );					// 讀取 灰階值 
void				BMP_gray_channel_write( BMP_DATA *bmp_data, int x, int y, int gray );		// 寫入 灰階值 
void				BMP_pixel( BMP_DATA *bmp_data, int x, int y, int r, int g, int b );			// 寫入一個pixel (rgb三色)

int					BMP_width( BMP_DATA *bmp_data );											// 取得寬
int					BMP_height( BMP_DATA *bmp_data );											// 取得高
int					BMP_depth( BMP_DATA *bmp_data );											// 取得深度 (位元/8)
				
BMP_DATA*			BMP_copy( BMP_DATA *src );													// 將資料從 src 複製到 dest



#endif