/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	各種資料放在這邊

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <fstream>
#include <iostream>

#include <QStringList>
#include <QObject>
#include <QImage>
#include <QVector>
#include <boost/numeric/ublas/matrix.hpp>

#include "settings.h"
#include "../tools.h"


//! Container class for MetaData objects.
class DataContainer : public QObject
{
#ifndef _GTEST_
    Q_OBJECT
#endif

	private:
		bMatrixInt			map;													// 用來記錄該點是否有被加到list裡面							
        Settings			*settings;

		QVector<PIXEL>		pixelList;												// pixel資訊存在這邊
		QVector<QPointF>	outputList;												// 輸出資訊

		QImage				image;													// Andreaw新增 讀入的圖檔資料.

    public:
        DataContainer(Settings *settingsPass, QObject *parent = 0);
        virtual ~DataContainer();

		// 將圖檔資料讀出
		QImage				get_image();
		QString				filename;												// 讀取的圖檔名稱

		QVector<PIXEL>		getPixelList();											// 回傳 PIXEL List
		QVector<QPointF>	getOutputList();
		
		void				output_svg( QString name );								// 將資料用svg的方式輸出  (檔案放在硬碟  從硬碟裡面讀取進來)
		void				setOutputList( QVector<QPointF> qf );					// 用在widgeB 傳入資料
		bool				addPointList( PIXEL pixel );							// 增加點到list清單
		void				clearPixelList();										// 清除pointlist
		void				removePointList( int row, int col );					// 移除點到的項目
		PIXEL				get_square( int row, int col );							// 陰影檢測用  回傳square的資訊  (主畫面點到的時候)

#ifndef _GTEST_
    public slots:
#endif
		void				loadBMP( QImage imagePass );							// 讀取BMP  Andrea

#ifndef _GTEST_
    signals:
		void				debugSignal( QString );									// 除錯訊息
		void				svgChanged( QString );
#endif



};

#endif // DATACONTAINER_H
