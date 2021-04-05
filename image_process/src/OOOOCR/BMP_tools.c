#include "BMP_tools.h"


/*************************************************************************************
	開啟檔案
/**************************************************************************************/
BMP_DATA*	BMP_open( char *filename )
{
	FILE		*p;
	BMP_DATA	*bmp_data;

	p	=	fopen( filename, "rb" );

	if( p == NULL )
	{
		printf("error...");
		return	0;
	}

	// 產生資料 
	bmp_data	=	malloc( sizeof(BMP_DATA) );

	// load header
	//fread( &(bmp_data->file_header), sizeof(BMP_FILE_HEADER), 1, p );
	bmp_data->file_header	=	BMP_file_load(p);

	// load infomation
	//fread( &(bmp_data->inf_header), sizeof(BMP_INF_HEADER), 1, p );
	bmp_data->inf_header	=	BMP_inf_load(p);

	// 讀取圖檔資料
	BMP_data_load( p, bmp_data );

	fclose(p);

	return	bmp_data;
}


/*************************************************************************************
	取得寬
/**************************************************************************************/
int		BMP_width( BMP_DATA *bmp_data )
{
	return	bmp_data->inf_header.width;
}


/*************************************************************************************
	取得高
/**************************************************************************************/
int		BMP_height( BMP_DATA *bmp_data )
{
	return	bmp_data->inf_header.height;
}


/*************************************************************************************
	取得深度 (除上8)
/**************************************************************************************/
int		BMP_depth( BMP_DATA *bmp_data )
{
	return	bmp_data->inf_header.bit_count/8;
}



/*************************************************************************************
	讀取圖檔資料
/**************************************************************************************/
void	BMP_data_load( FILE *p, BMP_DATA *bmp_data )
{
	BYTE	*buff;
	BYTE	*data;
	int		width,	height,	depth;
	int		add_4;		// 將每行補滿變成 4 的倍數 	
	int		row_width;	// 每一行的寬度 = width * depth
	int		i,	j;

	width		=	(int)bmp_data->inf_header.width;
	height		=	(int)bmp_data->inf_header.height;
	depth		=	(int)bmp_data->inf_header.bit_count/8;		// 位元深度  除8  轉成幾個byte)  這邊只能處理 32 24 8bit灰階

	add_4		=	width*depth + 4 - (width*depth)%4;			// 讓每行變成四的倍數
	row_width	=	width*depth;								// 每行寬度 = width * depth
	
	// 每行必須是 add_4 的長度
	buff	=	malloc( sizeof(BYTE) * add_4 );

	// 會在這邊 malloc
	bmp_data->data	=	BMP_create_data( width, height, depth );
	data			=	bmp_data->data;		// 必須在取得空間後才能這樣寫 不然會爆炸

	// 開始進行讀取資料 一次一行
	for( j = 0; j < height; j++ )
	{
		fread( buff, sizeof(BYTE), add_4, p );		// 每一行必須讀取 add_4 的寬度 (讓他成為 4 的倍數)

		for( i = 0; i < row_width; i++ )
			          // 資料必須反轉 (因為圖檔格式) 
					  // 每一個 offset = width*depth 每一行的寬度
			*( data + (height-j-1)*row_width + i )	=	*( buff + i );
	}

	free(buff);
}



/*************************************************************************************
	寫入圖檔資料
/**************************************************************************************/
void	BMP_data_save( FILE *p, BMP_DATA *bmp_data )
{
	BYTE	*buff;
	BYTE	*data;
	int		width,	height,	depth;
	int		add_4;		// 將每行補滿變成 4 的倍數 	
	int		row_width;	// 每一行的寬度 = width * depth
	int		i,	j;

	width		=	(int)bmp_data->inf_header.width;
	height		=	(int)bmp_data->inf_header.height;
	depth		=	(int)bmp_data->inf_header.bit_count/8;		// 位元深度  除8  轉成幾個byte)  這邊只能處理 32 24 8bit灰階

	add_4		=	width*depth + 4 - (width*depth)%4;			// 讓每行變成四的倍數
	row_width	=	width*depth;								// 每行寬度 = width * depth
	
	// 每行必須是 add_4 的長度
	buff	=	malloc( sizeof(BYTE) * add_4 );
	data	=	bmp_data->data;		

	// 開始進行寫入資料 一次一行
	for( j = 0; j < height; j++ )
	{

		for( i = 0; i < row_width; i++ )
			          // 資料必須反轉 (因為圖檔格式) 
					  // 每一個 offset = width*depth 每一行的寬度
			*( buff + i )	=	*( data + (height-j-1)*row_width + i );

		fwrite( buff, sizeof(BYTE), add_4, p );		// 每一行必須讀取 add_4 的寬度 (讓他成為 4 的倍數)
	}

	free(buff);
}



/*************************************************************************************
	寫入檔案
/**************************************************************************************/
void	BMP_save( char *filename, BMP_DATA *bmp_data )
{
	FILE	*p;
	
	p	=	fopen( filename, "wb+" );

	if( p == NULL )
	{
		printf("error...");
		return	0;
	}

	// save header
	BMP_file_save( p, bmp_data->file_header );

	// save infomation
	BMP_inf_save( p, bmp_data->inf_header );

	// 寫入圖檔資料
	BMP_data_save( p, bmp_data );

	fclose(p);
}




/*************************************************************************************
	新建一個 BMP 檔案  (資料初始化為0)
/**************************************************************************************/
BMP_DATA*	BMP_new( int width, int height, int depth )
{
	BMP_DATA	*bmp_data;

	bmp_data	=	malloc( sizeof(BMP_DATA) );

	switch( depth )
	{
		case _24_BIT_ :

			bmp_data->file_header.type				=	19778;
			bmp_data->file_header.size				=	2230;
			bmp_data->file_header.reserved1			=	0;
			bmp_data->file_header.reserved2			=	0;
			bmp_data->file_header.off_bits			=	54;

			bmp_data->inf_header.size				=	40;
			bmp_data->inf_header.width				=	width;
			bmp_data->inf_header.height				=	height;
			bmp_data->inf_header.planes				=	1;
			bmp_data->inf_header.bit_count			=	24;			
			bmp_data->inf_header.compression		=	0;
			bmp_data->inf_header.size_image			=	0;
			bmp_data->inf_header.x_pels_per_meter	=	3780;
			bmp_data->inf_header.y_pels_per_meter	=	3780;
			bmp_data->inf_header.clr_important		=	0;
			bmp_data->inf_header.clr_used			=	0;

			break;

		default:
			printf("error");
	}

	// 初始化空間配置
	bmp_data->data	=	BMP_create_data( width, height, depth );
	// 歸零
	memset( bmp_data->data, 0, width*height*depth );

	return	bmp_data;
}




/*************************************************************************************
	將資料從 src 複製到 dest
/**************************************************************************************/
BMP_DATA*	BMP_copy( BMP_DATA *src )
{
	BMP_DATA	*dest;

	int		width;
	int		height;
	int		depth;

	width	=	src->inf_header.width;
	height	=	src->inf_header.height;
	depth	=	src->inf_header.bit_count/8;

	dest	=	malloc( sizeof(BMP_DATA) );

	// 複製 file header
	memcpy( &(dest->file_header), &src->file_header, sizeof(BMP_FILE_HEADER) );

	// 複製 information header
	memcpy( &(dest->inf_header), &src->inf_header, sizeof(BMP_INF_HEADER) );

	// 取得空間
	dest->data	=	BMP_create_data( width, height, depth );

	// 複製資料
	memcpy( dest->data, src->data, sizeof(BYTE)*width*height*depth );

	return	dest;
}


/*************************************************************************************
	寫入一個pixel (rgb三色)
/**************************************************************************************/
void	BMP_pixel( BMP_DATA *bmp_data, int x, int y, int r, int g, int b )
{	
	int		depth	=	bmp_data->inf_header.bit_count/8;
	int		gray;
	
	if( depth >= _24_BIT_ )
	{
		*BMP_red_channel( bmp_data, x, y )		=	r;
		*BMP_green_channel( bmp_data, x, y )	=	g;
		*BMP_blue_channel( bmp_data, x, y )		=	b;
	}
	else
	{
		gray	=	( r*11 + g*16 + b*5 )/32;
		*BMP_gray_channel( bmp_data, x, y )		=	gray;
	}
}



/*************************************************************************************
	讀取/寫入 紅色通道資料
/**************************************************************************************/
BYTE*	BMP_red_channel( BMP_DATA *bmp_data, int x, int y )
{
	BYTE	*r;		// 回傳資料

	BYTE	*data	=	bmp_data->data;
	int		width	=	bmp_data->inf_header.width;
	int		height	=	bmp_data->inf_header.height;
	int		depth	=	bmp_data->inf_header.bit_count/8;

	        // 位移計算 圖片資料假設是 BGR BGR BGR  或   BGRA BGRA BGRA 或  灰階
	r	=	data + y*width*depth + x*depth;
	r	+=	depth >= _24_BIT_ ? RED_OFF_SET : 0;

	return	r;
}



/*************************************************************************************
	讀取/寫入 綠色通道資料
/**************************************************************************************/
BYTE*	BMP_green_channel( BMP_DATA *bmp_data, int x, int y )
{
	BYTE	*g;		// 回傳資料

	BYTE	*data	=	bmp_data->data;
	int		width	=	bmp_data->inf_header.width;
	int		height	=	bmp_data->inf_header.height;
	int		depth	=	bmp_data->inf_header.bit_count/8;

	        // 位移計算 圖片資料假設是 BGR BGR BGR  或   BGRA BGRA BGRA 或  灰階
	g	=	data + y*width*depth + x*depth;
	g	+=	depth >= _24_BIT_ ? GREEN_OFF_SET : 0;

	return	g;
}


/*************************************************************************************
	讀取/寫入 藍色通道資料
/**************************************************************************************/
BYTE*	BMP_blue_channel( BMP_DATA *bmp_data, int x, int y )
{
	BYTE	*b;		// 回傳資料

	BYTE	*data	=	bmp_data->data;
	int		width	=	bmp_data->inf_header.width;
	int		height	=	bmp_data->inf_header.height;
	int		depth	=	bmp_data->inf_header.bit_count/8;

	        // 位移計算 圖片資料假設是 BGR BGR BGR  或   BGRA BGRA BGRA 或  灰階
	b	=	data + y*width*depth + x*depth;
	b	+=	depth >= _24_BIT_ ? BLUE_OFF_SET : 0;

	return	b;
}



/*************************************************************************************
	讀取/寫入 alpha 通道資料
/**************************************************************************************/
BYTE*	BMP_alpha_channel( BMP_DATA *bmp_data, int x, int y )
{
	BYTE	*alpha;		// 回傳資料

	BYTE	*data	=	bmp_data->data;
	int		width	=	bmp_data->inf_header.width;
	int		height	=	bmp_data->inf_header.height;
	int		depth	=	bmp_data->inf_header.bit_count/8;

	if( depth < _32_BIT_ )
		printf( "error..." );

	        // 位移計算 圖片資料假設是 BGR BGR BGR  或   BGRA BGRA BGRA 或  灰階
	alpha	=	data + y*width*depth + x*depth;
	alpha	+=	depth >= _32_BIT_ ? ALPHA_OFF_SET : 0;

	return	alpha;		// 如果格式不對 這邊是隨便丟一個位置回去
}



/*************************************************************************************
	讀取/寫入 灰階 通道資料 (灰階圖專用)
/**************************************************************************************/
BYTE*	BMP_gray_channel( BMP_DATA *bmp_data, int x, int y )
{
	BYTE	*gray;		// 回傳資料

	BYTE	*data	=	bmp_data->data;
	int		width	=	bmp_data->inf_header.width;
	int		height	=	bmp_data->inf_header.height;
	int		depth	=	bmp_data->inf_header.bit_count/8;

	if( depth > _8_BIT_ )
		printf( "error..." );

	        // 位移計算 圖片資料假設是 BGR BGR BGR  或   BGRA BGRA BGRA 或  灰階
	gray	=	data + y*width*depth + x*depth;

	return	gray;		// 如果格式不對 這邊是隨便丟一個位置回去
}



/*************************************************************************************
	讀取 灰階值 
/**************************************************************************************/
int		BMP_gray_channel_read(  BMP_DATA *bmp_data, int x, int y )
{
	int		gray;		// 回傳資料
	int		red,	green,	blue;

	BYTE	*data	=	bmp_data->data;
	int		width	=	bmp_data->inf_header.width;
	int		height	=	bmp_data->inf_header.height;
	int		depth	=	bmp_data->inf_header.bit_count/8;

	if( depth > _8_BIT_ )
	{
		red		=	*BMP_red_channel( bmp_data, x, y );
		green	=	*BMP_green_channel( bmp_data, x, y );
		blue	=	*BMP_blue_channel( bmp_data, x, y );
		gray	=	( red*11 + green*16 + blue*5 )/32;
	}
	else
	{
		gray	=	*BMP_gray_channel( bmp_data, x, y );
	}

	return	gray;
}


/*************************************************************************************
	寫入 灰階值 
/**************************************************************************************/
void	BMP_gray_channel_write( BMP_DATA *bmp_data, int x, int y, int gray )
{
	BYTE	*data	=	bmp_data->data;
	int		width	=	bmp_data->inf_header.width;
	int		height	=	bmp_data->inf_header.height;
	int		depth	=	bmp_data->inf_header.bit_count/8;

	if( depth > _8_BIT_ )
	{
		*BMP_red_channel( bmp_data, x, y )		=	gray;
		*BMP_green_channel( bmp_data, x, y )	=	gray;
		*BMP_blue_channel( bmp_data, x, y )		=	gray;
	}
	else
	{
		*BMP_gray_channel( bmp_data, x, y )		=	gray;
	}

	return	gray;
}





/*************************************************************************************
	讀取 file header
/**************************************************************************************/
BMP_FILE_HEADER		BMP_file_load( FILE *p )
{
	BMP_FILE_HEADER		file_header;

	fread( &file_header.type,      sizeof(unsigned short), 1, p );
	fread( &file_header.size,      sizeof(unsigned int),   1, p );
	fread( &file_header.reserved1, sizeof(unsigned short), 1, p );
	fread( &file_header.reserved2, sizeof(unsigned short), 1, p );
	fread( &file_header.off_bits,  sizeof(unsigned int),   1, p );

	return	file_header;
}





/*************************************************************************************
	寫入 file header
/**************************************************************************************/
void	BMP_file_save( FILE *p, BMP_FILE_HEADER file_header )
{
	fwrite( &file_header.type,      sizeof(unsigned short), 1, p );
	fwrite( &file_header.size,      sizeof(unsigned int),   1, p );
	fwrite( &file_header.reserved1, sizeof(unsigned short), 1, p );
	fwrite( &file_header.reserved2, sizeof(unsigned short), 1, p );
	fwrite( &file_header.off_bits,  sizeof(unsigned int),   1, p );
}




/*************************************************************************************
	讀取 information header
/**************************************************************************************/
BMP_INF_HEADER		BMP_inf_load( FILE *p )
{
	BMP_INF_HEADER	inf_header;

	fread( &inf_header.size,             sizeof(unsigned int),   1, p );
	fread( &inf_header.width,            sizeof(unsigned int),   1, p );
	fread( &inf_header.height,           sizeof(unsigned int),   1, p );
	fread( &inf_header.planes,           sizeof(unsigned short), 1, p );
	fread( &inf_header.bit_count,        sizeof(unsigned short), 1, p );
	fread( &inf_header.compression,      sizeof(unsigned int),   1, p );
	fread( &inf_header.size_image,       sizeof(unsigned int),   1, p );
	fread( &inf_header.x_pels_per_meter, sizeof(unsigned int),   1, p );
	fread( &inf_header.y_pels_per_meter, sizeof(unsigned int),   1, p );
	fread( &inf_header.clr_used,         sizeof(unsigned int),   1, p );
	fread( &inf_header.clr_important,    sizeof(unsigned int),   1, p );

	return	inf_header;
}




/*************************************************************************************
	寫取 information header
/**************************************************************************************/
void	BMP_inf_save( FILE *p, BMP_INF_HEADER inf_header )
{
	fwrite( &inf_header.size,             sizeof(unsigned int),   1, p );
	fwrite( &inf_header.width,            sizeof(unsigned int),   1, p );
	fwrite( &inf_header.height,           sizeof(unsigned int),   1, p );
	fwrite( &inf_header.planes,           sizeof(unsigned short), 1, p );
	fwrite( &inf_header.bit_count,        sizeof(unsigned short), 1, p );
	fwrite( &inf_header.compression,      sizeof(unsigned int),   1, p );
	fwrite( &inf_header.size_image,       sizeof(unsigned int),   1, p );
	fwrite( &inf_header.x_pels_per_meter, sizeof(unsigned int),   1, p );
	fwrite( &inf_header.y_pels_per_meter, sizeof(unsigned int),   1, p );
	fwrite( &inf_header.clr_used,         sizeof(unsigned int),   1, p );
	fwrite( &inf_header.clr_important,    sizeof(unsigned int),   1, p );
}



/*************************************************************************************
	會在這邊 malloc
/**************************************************************************************/
BYTE*	BMP_create_data( int width, int height, int depth )
{
	BYTE	*data;

	data	=	malloc( sizeof(BYTE) * width * height * depth );
	memset( data, 0, sizeof(BYTE)*width*height*depth );

	return	data;
}



/*************************************************************************************
	釋放記憶體空間
/**************************************************************************************/
void	BMP_free( BMP_DATA *bmp_data )
{
	free( bmp_data->data );
	bmp_data->data	=	NULL;

	free( bmp_data );
	bmp_data	=	NULL;

}