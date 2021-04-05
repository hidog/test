#include <QStringList>
#include <QObject>
#include <iostream>
#include <QFileDialog>

#include "settings.h"
#include "datacontainer.h"

using namespace std;



/*=================================================================================
	建構函數
/*=================================================================================*/
DataContainer::DataContainer(Settings *settingsPass, QObject *parent) : QObject(parent)
{
    settings			=	settingsPass;

//	GFilgerImageCreated		=	false;
	pixelList.clear();
}


/*=================================================================================
	解構函數
/*=================================================================================*/
DataContainer::~DataContainer()
{}




/*=================================================================================
	讀取BMP
	Andrea新增
/*=================================================================================*/
void	DataContainer::loadBMP( QImage imagePass )
{
	image	=	imagePass;

	// 初始化map
	map.resize( image.width(), image.height() );
	map.clear();
}



/*=================================================================================
	將處理後的影像丟到mainwindow的顯示視窗上面
	Andrea新增
	輸出到視窗2
/*=================================================================================*/
void	DataContainer::setOutputList( QVector<QPointF> qf )
{
	outputList	=	qf;
}


/*=================================================================================
	將資料用svg的方式輸出  (檔案放在硬碟  從硬碟裡面讀取進來)
/*=================================================================================*/
void	DataContainer::output_svg( QString name )
{
#ifndef _GTEST_
	emit( svgChanged(name) );
#endif
}

/*=================================================================================
	回傳BMP
	Andrea新增
/*=================================================================================*/
QImage	DataContainer::get_image()
{
	if( !image.isNull() )
		return	image;

	else
		return	QImage( 100, 100, QImage::Format_RGB888 );		// 100*100的空白圖

}

/*=================================================================================
	清除pointlist
/*=================================================================================*/
void	DataContainer::clearPixelList()
{
	outputList.clear();
	pixelList.clear();
	map.clear();
}

/*=================================================================================
	回傳outputList
/*=================================================================================*/
QVector<QPointF>	DataContainer::getOutputList()
{
	return	outputList;
}




/*=================================================================================
	回傳Pixel List
/*=================================================================================*/
QVector<PIXEL>	DataContainer::getPixelList()
{
	return	pixelList;
}





/*=================================================================================
	增加點到list清單
/*=================================================================================*/
bool	DataContainer::addPointList( PIXEL pixel )
{
	int		x	=	pixel.x;
	int		y	=	pixel.y;

	if( x<0 || x>=map.size1() || y<0 || y>map.size2() )
		return	false;
	
	// 成功新增點 其他都是失敗
	else if( pixelList.size() < 100 && map(x,y) != 1 )
	{
		map(x,y)	=	1;
		pixelList.append( pixel );
		return	true;
	}

	else if( map(x,y) == 1 )
	{
		//debugSignal("DataContainer::addPointList this point has been added!!");
		return	false;
	}

	else
	{
		//debugSignal("DataContainer::addPointList full list!!");
		return	false;
	}
}



/*=================================================================================
	移除點擊到的項目
/*=================================================================================*/
void	DataContainer::removePointList( int row, int col )
{
	if( row >= pixelList.size() )
		return;

	pixelList.remove( row );
}


/*=================================================================================
	陰影檢測用  回傳square的資訊  
	(主畫面點到的時候)
/*=================================================================================*/
PIXEL	DataContainer::get_square( int row, int col )
{
	return	pixelList.at(row);
	//pixelList.remove( row );
}