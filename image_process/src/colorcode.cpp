#include "colorcode.h"




#ifdef _OPENPIV_
/*=======================================================================================================
	建構
	OPENPIV 底下使用
/*=======================================================================================================*/
ColorCode::ColorCode(Settings *settingsPass, DataContainer  *filedataPass)	
	: AndreaEngine()
{
	progress	=	settingsPass->getProgress();	
	settings	=	settingsPass;
	filedata	=	filedataPass;

    mutex.lock();
    mutex.unlock();
}
#else
/*=======================================================================================================
	建構
	無傳入值
/*=======================================================================================================*/
ColorCode::ColorCode()	
{
	image.load("test.bmp");
	final.load("test.bmp");
}
#endif


/*=================================================================================
	預設function
===================================================================================*/
ColorCode::~ColorCode()
{}



#ifdef _OPENPIV_
/*=================================================================================
	都利用這個funtion來做事情. 取代原本的main
===================================================================================*/
void	ColorCode::dosomething()
{
	progress->debuger("Start color code detector...");

	image	=	filedata->get_image();

	// 用watershed 4.8 把色標切開來
	Watershed	watershed( settings, filedata );
	label	=	watershed.divide( image );
	label.save("label.bmp");

	// 產生gray tone的圖
	/*Channel		channel( settings, filedata );
	graytone	=	channel.gray_tone( image, label );
	graytone.save("graytone.bmp");*/


	SUBPIX_BLOB		blob( settings, filedata );
	//blob.dosomething3( graytone );



	// 輸出到主畫面
	//emit( filedata->output(graytone) );
	//progress->outputImage( 0, graytone );
	progress->debuger( "finish color code detector...", QColor(255,0,0) );
}
#endif