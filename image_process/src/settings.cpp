#include <QSize>
#include <QColor>
#include <cmath>
#include <iostream>
#include "settings.h"

Settings::Settings( Progress *_progress )
{
    // Processing
    _deltaX = 16;
    _deltaY = 16;
    _intLengthX = 32;
    _intLengthY = 32;
    _imageSize.setWidth(-1);
    _imageSize.setHeight(-1);
    _isMask = false;    

	// 這邊負責控制執行什麼動作.
    //_processor = OpenPIV::FFTCorrelator;
	//_processor	=	OpenPIV::GFilter;
	_processor	=	OpenPIV::SubPixBlob;
	//_processor	=	OpenPIV::Watershed;


    _detector = OpenPIV::GaussianSubPixel;
    _roi = QRect(0,0,0,0);
    roiSet = false;

    // Vectors
    _vectorColourUnfiltered = QColor(Qt::cyan);
    _vectorColourFiltered = QColor(Qt::red);
    _vectorScale = 5.0;
    _vectorSub = 0.0;

    // Session
    _expName = "OpenPIV";

    // Output
    _outputFolder = "";
    _outputFormat = OpenPIV::Text;

    // Batch
    _batchFilter = false;
    _batchMask = false;
    _batchShowImage = false;
    _batchShowVectors = false;
    _batchThreading = true;

	// blob detector
	scale_w		=	1;
	scale_l		=	1;
	isHighlight	=	false;			// 預設不要反白

	// 負責回報進度用的
	progress	=	_progress;

	// 陰影檢測用
	square_size				=	5;
	set_square				=	false;
	shadow_dir				=	0;
	shadow_depth_threshold	=	1;
	inhence_mode			=	0;
	file_size				=	1;				
	save_check				=	true;				
	batch_check				=	false;
	dark_light				=	false;
}

Settings::~Settings()
{
}



/* ----------Processing -------------*/

int Settings::deltaX() { return _deltaX; }
void Settings::setDeltaX(int deltaXPass)
{
    xSpin = deltaXPass;
    changeDeltaX();
}

int Settings::deltaY() { return _deltaY; }
void Settings::setDeltaY(int deltaYPass)
{
    ySpin = deltaYPass;
    changeDeltaY();
}

void Settings::changeDeltaX()
{
    _deltaX = int(double(_intLengthX) * double(100-xSpin) / 100);
#ifndef _GTEST_
    emit(processSettingsChanged());
#endif
}

void Settings::changeDeltaY()
{
    _deltaY = int(double(_intLengthY) * double(100-ySpin) / 100);
#ifndef _GTEST_
    emit(processSettingsChanged());
#endif
}

int Settings::intLengthX() { return _intLengthX; }
void Settings::setIntLengthX(int intLengthXPass)
{
    _intLengthX = int(pow(2.0,double(4+intLengthXPass)));
    changeDeltaX();
}

int Settings::intLengthY() { return _intLengthY; }
void Settings::setIntLengthY(int intLengthYPass)
{
    _intLengthY = int(pow(2.0,double(4+intLengthYPass)));
    changeDeltaY();
}

int Settings::processor() { return _processor; }
void Settings::setProcessor(int processPass) { _processor = processPass; }

int Settings::detector() { return _detector; }
void Settings::setDetector(int detectPass) { _detector = detectPass; }

QSize Settings::imageSize() { return _imageSize; }
void Settings::setImageSize(QSize imageSizePass)
{
    bool toEmit = false;
    if (!((imageSizePass.width() == _imageSize.width()) && (imageSizePass.height() == _imageSize.height())))
        toEmit = true;
    _imageSize = imageSizePass;
    if (!roiSet)
    {
        this->setRoi(QRect(0,0,_imageSize.width(),_imageSize.height()));
        roiSet = true;
    }
    else if (roiSet && toEmit)
    {
        if (_roi.left() < 0) _roi.setLeft(0);
        if (_roi.right() > _imageSize.width() || _roi.right() < 0) _roi.setRight(_imageSize.width());
        if (_roi.bottom() > _imageSize.height() || _roi.bottom() < 0) _roi.setBottom(_imageSize.height());
        if (_roi.top() < 0) _roi.setTop(0);
    }
#ifndef _GTEST_
    if (toEmit) emit(imageSizeChanged());
#endif
}

bool Settings::isMask() { return _isMask; }
void Settings::setIsMask(bool isMaskPass)
{
    _batchMask = isMaskPass;
    _isMask = isMaskPass;
#ifndef _GTEST_
    emit(imageSizeChanged());
#endif
}

QRect Settings::roi() { return _roi; }
void Settings::setRoi(QRect roiPass)
{
    _roi = roiPass;
    if (_roi.left() < 0) _roi.setLeft(0);
    if (_roi.right() > _imageSize.width()) _roi.setLeft(_imageSize.width());
    if (_roi.bottom() > _imageSize.height()) _roi.setBottom(_imageSize.height());
    if (_roi.top() < 0) _roi.setTop(0);
}

/*-------Session--------------*/


/*-------Batch Processing-------*/

bool Settings::batchFilter() { return _batchFilter; }
void Settings::setBatchFilter(bool batchFilterPass) { _batchFilter = batchFilterPass; }

bool Settings::batchMask() { return _batchMask; }
void Settings::setBatchMask(bool batchMaskPass) { _batchMask = batchMaskPass; }

bool Settings::batchShowImage() { return _batchShowImage; }
void Settings::setBatchShowImage(bool batchShowImagePass) { _batchShowImage = batchShowImagePass; }

bool Settings::batchShowVectors() { return _batchShowVectors; }
void Settings::setBatchShowVectors(bool batchShowVectorsPass) { _batchShowVectors = batchShowVectorsPass; }

bool Settings::batchThreading() { return _batchThreading; }
void Settings::setBatchThreading(bool batchThreadingPass) { _batchThreading = batchThreadingPass; }

/*-------- Vectors ------------*/

QColor Settings::vectorColourFiltered() { return _vectorColourFiltered; }
void Settings::setVectorColourFiltered(QColor vectorColourPass)
{
    _vectorColourFiltered = vectorColourPass;
#ifndef _GTEST_
    emit(vectorSettingChanged());
#endif
}
QColor Settings::vectorColourUnfiltered() { return _vectorColourUnfiltered; }
void Settings::setVectorColourUnfiltered(QColor vectorColourPass)
{
    _vectorColourUnfiltered = vectorColourPass;
#ifndef _GTEST_
    emit(vectorSettingChanged());
#endif
}
double Settings::vectorScale() { return _vectorScale; }
void Settings::setVectorScale(double vectorScalePass)
{
    _vectorScale = vectorScalePass;
#ifndef _GTEST_
    emit(vectorSettingChanged());
#endif
}

double Settings::vectorSub() { return _vectorSub; }
void Settings::setVectorSub(double vectorSubPass)
{
    _vectorSub = vectorSubPass;
#ifndef _GTEST_
    emit(vectorSettingChanged());
#endif
}



/* ---------Output-----------*/

QString Settings::expName() { return _expName; }
void Settings::setExpName(QString expNamePass)
{
    _expName = expNamePass;
}
QString Settings::outputFolder() { return _outputFolder; }
void Settings::setOutputFolder(QString outputFolderPass)
{
    _outputFolder = outputFolderPass;
}
int Settings::outputFormat() { return _outputFormat; }
void Settings::setOutputFormat(int outputFormatPass)
{
    _outputFormat = outputFormatPass;
}



/* ---------blob detector-----------*/
void	Settings::set_scale( int w, int l ) 
{
	scale_w		=	w;
	scale_l		=	l;
}

void	Settings::set_scale_w( int w ) 
{
	scale_w					=	w;
	shadow_depth_threshold	=	w;
}

void	Settings::set_scale_l( int l ) 
{
	scale_l		=	l;
	file_size	=	l;
}

int		Settings::get_scale_w() 
{
	return		scale_w;
}

int		Settings::get_scale_l() 
{
	return		scale_l;
}



/* ---------measure-----------*/
void	Settings::set_measure_X( int x ) 
{
	measure_x	=	x;
}

void	Settings::set_measure_Y( int y ) 
{
	measure_y	=	y;
}

int		Settings::get_measure_X() 
{
	return		measure_x;
}

int		Settings::get_measure_Y() 
{
	return		measure_y;
}




Progress*	Settings::getProgress(  ) 
{
	return	progress;
}



/* ---------設定顏色資訊-----------*/
void	Settings::setRGB( QRgb _rgb )
{
	rgb		=	_rgb;
}



/* ---------回傳顏色資訊-----------*/
QRgb	Settings::getRGB()
{
	return	rgb;
}


/***************************************************************************************
	設定inhence mode
	陰影檢測用
	決定要抓單點還是整段式的陰影
/****************************************************************************************/
void	Settings::setInhence( int state )
{
	inhence_mode	=	state ? true : false;
}


/***************************************************************************************
	設定是否要輸出結果到檔案 (image部分)
/****************************************************************************************/
void	Settings::setSaved( int state )
{
	save_check	=	state ? true : false;
}



/***************************************************************************************
	設定是否為批次作業
/****************************************************************************************/
void	Settings::setBatch( int state )
{
	batch_check		=	state ? true : false;
}