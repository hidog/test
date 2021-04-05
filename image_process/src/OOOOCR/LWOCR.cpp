// LWOCR.cpp : 定義主控台應用程式的進入點。
//

//#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <cstring>

#include "bitmap_image.hpp"

#include "timer.h"

//#include "vld.h"

#define QT_DEF


#ifdef QT_DEF
	#include <QImage>
	#include <QDir>
#endif



extern "C"
{
	#include "src/LWOCR.h"
	#include "src/OCR_Recognition.h"
	#include "src/OCR_shared.h"
	#include "src/BMP_tools.h"
}

//extern int		nr_class;
//extern int		nr_feature;
//extern float	*Omega;
//extern int		*O_label;

//#include <QImage>
using namespace std;


#ifndef QT_DEF

int	main( int argc, char **argv )
{
	FILE			*p;

	int				i,	j,	index; 
	int				id;
	BYTE	tmp;

	short			*img,	*bin;
	int				*Template;
	short			*bg_data;
	int				result;

	char			filename[100];

	img			=	new short[704];
	bin			=	new short[704];
	bg_data		=	new short[40];
	Template	=	new int[704];


	// OCR 初始化
	OCR_init( OCR_CNY_20 );

	Timer	t;
	double	t_sum	=	0;

//for( id = 1; id < 35; id++ )
//{

	p	=	fopen( argv[1], "rb" );
	//p	=	fopen( "5_IC57399111.raw.raw", "rb" );
//	p	=	fopen( "1_OD04779229(1).raw.raw", "rb");
	//p	=	fopen( "6.raw", "rb");

	//QImage	image("5_IC57399111.raw.bmp");

	if( p == NULL )
		printf("error");
		
	// 轉換圖片資料
	for( j = 0; j < 32; j++ )
	{
		for( i = 0; i < 22; i++ )
		{
			//fscanf( p, "%d", &tmp );
			fread( &tmp, sizeof(BYTE), 1, p );
			img[ j*22 + i ]	=	tmp;//image.gray_channel(i,j);//tmp;
			//printf("%d ", tmp );
		}
		//printf("\n");
	}
	fclose(p);


	/*for( j = 0; j < 32; j++ )
	{
		for( i = 0; i < 22; i++ )
		{
			printf( "%d ", a[j*22 + i] );
		}
		printf("\n");
	}*/

	// 二值化
	LWO_binary( img, bin );

	CROP_DATA	crop_data;
	crop_data.img		=	img;
	crop_data.bin		=	bin;
	crop_data.bg_data	=	bg_data;
	//crop_data.id		=	id;
		
	// 進行裁剪 縮放等等
	LWO_crop( &crop_data );

	//bin.save("D:\\TEST\\bin\\output.bmp");

	//p	=	fopen("D:\\_________________output.txt","w+");

	/*for( j = 0; j < 32; j++ )
	{
		for( i = 0; i < 22; i++ )
			fprintf(p,"%d ",a[j*22+i]);
		fprintf(p,"\n");
	}*/	

t.Start();

	// 進行 OCR 辨識
	result	=	OCR_exec( img, bin );

t.Stop();

	t_sum	+=	t.Elapsed();

	//cout << id << " ";
	
	if( result >= 1 && result <= 26 )
		cout << (char)( result + 'A' - 1 ) << endl;
	else
		cout << (char)( result + '0' - 26 ) << endl;

//}

	delete	[]	img;
	delete	[]	bin;
	delete	[]	bg_data;
	delete	[]	Template;
	
	OCR_free();

	getchar();
	//system("PAUSE");
	return 0;
}



#endif


#if 0
#ifdef QT_DEF
int main(int argc, char** argv)
{
	//for( double theta = 0; theta <= 3.1415926; theta += 3.1415926 / 9 )
	//	printf("%10.10lf\n", tan(theta) );
		//cout << tan(theta) << endl;

	FILE			*p;

	int				i,	j,	index; 
	int				id,	tmp;

	char			filename[100];

	QDir			dir;
	QFileInfoList	list;
	QFileInfo		fileInfo;
	QString			str;
	QImage			image;

	BYTE			*img,	*bin;//,	*buffer_clip_img;
	int				result;

	string			c_fiiii	=	"D:/TEST/20130606_crop_error/0.8/7_XL06336116.raw.bmp";   //0_A8I2698760(1).raw.bmp

	string			c;
	int				c_id;
	int				c_cur;


	//BYTE	buffer_clip_img[ (LWO_IMAGE_WIDTH + ADD_VALUE_SIZE*2) * (LWO_IMAGE_HEIGHT + ADD_VALUE_SIZE*2) ];	


	img				=	new BYTE[704];
	bin				=	new BYTE[704];
	//buffer_clip_img	=	new BYTE[ 24 * 34 ];

		

	//OCR_model_to_binary( OCR_CNY_20 );
	//OCR_test();
	//return	1;


	// OCR 初始化
	OCR_init( OCR_CNY_100 );

	// 把字全部混再一起 跑分類

for( id = 1; id <= 36; id++ )
{


	//dir.setPath( QString("D:\\TEST\\CNY20_IMAGE\\CNY20_IMAGE\\") );
	dir.setPath( QString("D:\\TEST\\CNY100_TEST\\%1\\").arg(id) );
	list	=	dir.entryInfoList();

	int		count		=	0;
	int		count_tatol	=	0;

//	Timer	t;
	double	t_sum	=	0;

	//sprintf( filename, "D:\\TEST\\output_%d.txt", id );
	//p	=	fopen(  filename ,"w+");

	printf("start!\n");

		   /* 跳過頭兩個  .  ..   */
	for( index = 2; index < list.size(); index++ )
	//for( index = list.size()-1; index>=2; index-- )
	{
		count_tatol++;

		fileInfo	=	list.at( index );
		str			=	fileInfo.absoluteFilePath();
		//str			=	QString("D:\\TEST\\0_ZH93651424(1).raw.bmp");

		//cout << fileInfo.fileName().toStdString() << endl;

		image		=	QImage( str );
		//image	=	QImage( QString("D:\\TEST\\100_Correct_0527\\1\\0_A00137.bmp") );
		
		// 轉換圖片資料
		for( j = 0; j < image.height(); j++ )
			for( i = 0; i < image.width(); i++ )
				img[ j*22 + i ]	=	image.gray_channel( i, j );

//t.Start();
		// 二值化
		LWO_binary( img, bin );

		CROP_DATA	crop_data;
		crop_data.img		=	img;
		crop_data.bin		=	bin;

		// 進行裁剪 縮放等等
		int		is_I	=	LWO_crop( &crop_data, OCR_CNY_100 );
//t.Stop();
		//t_sum	+=	t.Elapsed();

//t.Start();
		// 進行 OCR 辨識
		result	=	OCR_exec( img, bin );

		//printf("%d\n",result);

		//cout << result << endl;
//t.Stop();
//		t_sum	+=	t.Elapsed();	
		//if(t.Elapsed() > 2.0)
		//	cout << t.Elapsed() << endl;


		c	=	fileInfo.fileName().toStdString();

		c_id	=	c[0];
		c_id	-=	'0';
		c_cur	=	c[c_id+2];        

		//if( c_cur == '0' )		c_cur	=	'O';		// 命名規則問題  o 變成 0
		//if( c_cur == '1' )		c_cur	=	'I';		// 命名規則問題  1 I   大額要有   小額要mark

		if( c_cur >= 'A' && c_cur <= 'Z' )			c_cur	=	c_cur - 'A' + 1;
		else if( c_cur >= '0' && c_cur <= '9' )		c_cur	=	c_cur - '0' + 26;
		
		//image.save( QString("D:\\TEST\\bin\\%1\\%2").arg(result).arg(fileInfo.fileName()) );

		// 轉換圖片資料
		/*for( j = 0; j < image.height(); j++ )
			for( i = 0; i < image.width(); i++ )
				image.gray_channel( i, j, a	[j*22 + i] );
		image.save( QString("D:\\TEST\\output\\%1\\%2").arg(result).arg(fileInfo.fileName()) );*/

		//bin.save( QString("D:\\TEST\\bin\\%1\\%2").arg(result).arg(fileInfo.fileName()) );

		if( result != c_cur )
		{
			
			// 轉換圖片資料
			//for( j = 0; j < image.height(); j++ )
			//	for( i = 0; i < image.width(); i++ )
			//		image.gray_channel( i, j, img[j*22 + i] );
			image.save( QString("D:\\TEST\\output\\%1\\%2").arg(id).arg(fileInfo.fileName()) );

			//cout << result << " " << c << endl;
			count++;
			//image.save( QString("D:\\TEST\\output\\%1\\%2").arg(result).arg(fileInfo.fileName()) );
			//image.save( QString("D:\\TEST\\output\\%1\\%2").arg(id).arg(fileInfo.fileName()) );
			cout << result << " " << fileInfo.fileName().toStdString() << endl;
		}
	}
}



//	cout << count << endl;
	//cout << count_tatol << endl;
//	cout << t_sum / count_tatol << endl;

	OCR_free();

	//fclose(p);


	delete	[]	img;
	delete	[]	bin;
	//delete	[]	buffer_clip_img;

	
	OCR_free();

	//getchar();
	system("PAUSE");
	return 0;
}
#endif
#endif


#if 0
#ifdef QT_DEF
int main(int argc, char** argv)
{
	FILE			*p;

	int				i,	j,	index; 
	int				id,	tmp;

	char			filename[100];

	QDir			dir;
	QFileInfoList	list;
	QFileInfo		fileInfo;
	QString			str;
	QImage			image;

	BYTE			*img,	*bin;
	
	int				result;

	string			c;
	int				c_id;
	int				c_cur;


	img			=	new BYTE[704];
	bin			=	new BYTE[704];


	// OCR 初始化
	OCR_init( OCR_CNY_100 );

	// 把字全部混再一起 跑分類


	//dir.setPath( QString("D:\\TEST\\CNY20_IMAGE\\CNY20_IMAGE\\") );
	dir.setPath( QString("D:\\TEST\\CNY100_ALL\\") );
	list	=	dir.entryInfoList();


	printf("start!\n");

		   /* 跳過頭兩個  .  ..   */
	for( index = 2; index < list.size(); index++ )
	{
		//cout << "*";

		fileInfo	=	list.at( index );
		str			=	fileInfo.absoluteFilePath();

		image		=	QImage( str );
		
		// 轉換圖片資料
		for( j = 0; j < image.height(); j++ )
			for( i = 0; i < image.width(); i++ )
				img[ j*22 + i ]	=	image.gray_channel( i, j );


		// 二值化
		LWO_binary( img, bin );

		CROP_DATA	crop_data;
		crop_data.img		=	img;
		crop_data.bin		=	bin;

		
		// 進行裁剪 縮放等等
		int		is_I	=	LWO_crop( &crop_data, OCR_CNY_100 );


		if( crop_data.crop_type == LWO_WH_SMALL )
			image.save( QString("D:\\TEST\\crop_error_CNY100\\WH_SMALL\\%1").arg(fileInfo.fileName()) );
		else if( crop_data.crop_type == LWO_WH_BIG )
			image.save( QString("D:\\TEST\\crop_error_CNY100\\WH_BIG\\%1").arg(fileInfo.fileName()) );
		else if( crop_data.crop_type == LWO_CENTER_ERROR )
			image.save( QString("D:\\TEST\\crop_error_CNY100\\CENTER\\%1").arg(fileInfo.fileName()) );

		continue;


		// 進行 OCR 辨識
		result	=	OCR_exec( img, bin );

		c	=	fileInfo.fileName().toStdString();

		c_id	=	c[0];
		c_id	-=	'0';
		c_cur	=	c[c_id+2];        

		//if( c_cur == '0' )		c_cur	=	'O';		// 命名規則問題  o 變成 0
		//if( c_cur == '1' )		c_cur	=	'I';		// 命名規則問題  1 I   大額要有   小額要mark

		if( c_cur >= 'A' && c_cur <= 'Z' )			c_cur	=	c_cur - 'A' + 1;
		else if( c_cur >= '0' && c_cur <= '9' )		c_cur	=	c_cur - '0' + 26;

		if( result != c_cur )
		{
			cout << "*";
			// 轉換圖片資料
			//for( j = 0; j < image.height(); j++ )
			//	for( i = 0; i < image.width(); i++ )
			//		image.gray_channel( i, j, img[j*22 + i] );
			image.save( QString("D:\\TEST\\output\\%1\\%2").arg(result).arg(fileInfo.fileName()) );
		}

	}


	OCR_free();

	//fclose(p);


	delete	[]	img;
	delete	[]	bin;

	
	OCR_free();

	//getchar();
	system("PAUSE");
	return 0;
}
#endif
#endif




int	main( int argc, char **argv )
{
	int		i,	j;


	BMP_DATA	*bmp_data,	*bmp_data2;

	//bmp_data	=	BMP_open( "D:\\test.bmp" );
	bmp_data	=	BMP_new( 10, 100, _24_BIT_ );
	

	for( i = 0; i < BMP_width(bmp_data); i++ )
	{
		BMP_pixel( bmp_data, i, 10, 2, 255, 71 );
		BMP_pixel( bmp_data, i, 11, 251, 25, 11 );
		BMP_pixel( bmp_data, i, 12, 15, 15, 16 );
		BMP_pixel( bmp_data, i, 13, 255, 255, 255 );
		BMP_pixel( bmp_data, i, 14, 123, 231, 111 );
	}

	bmp_data2	=	BMP_copy( bmp_data );
	
	BMP_save( "D:\\output.bmp", bmp_data2 );
	


	BMP_free(bmp_data);
	BMP_free(bmp_data2);

	//bitmap_image	bmp("D:\\test.bmp");

	system("PAUSE");
	return	0;
}