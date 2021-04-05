/*===================================================================================================================================================

	主要視窗
	主要物件幾乎都定義在這邊
	物件與物件之間的溝通希望透過mainwindow   物件 <-> mainwindow <-> 物件  而不是物件與物件直接溝通  物件 <-> 物件
	要注意thread的使用  避免顯示跟計算用同一個thread  造成速度拖垮

	目前架構上大概有

	thread   執行運算
	display  執行顯示
	data     各種資料
	setting  各種設定
	output   負責輸出到檔案

	progress 是負責thread內跟外部的溝通

/*===================================================================================================================================================*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "pivdisplay.h"
#include "vectorlist.h"
#include "progress.h"

#include "importimages.h"
#include "batchwindow.h"

#include "processing.h"
#include "datacontainer.h"
#include "output.h"

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

	private:

        Settings			*settings;

        ImportImages		*importImages;

        Processing			*process;
        DataContainer		*filedata;

		Progress			*progress;									// 負責回報進度等等

		static const int	displaySize		=	4;	
		PivDisplay			*display [ displaySize ];					// 主要顯示視窗
		QLabel				*position[ displaySize ];					// 顯示位置的
		QLabel				*rgbLabel[ displaySize ];					// 顯示顏色的

		static const int	pointListSize	=	2;						
		PointList			*pointList[pointListSize];					// 主畫面底下的 point list


    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();

    protected:
        // Initialization
        void	setupWindows();
        void	initialize();
		void	displayInitialize();								// 初始化pivdisplay
		void	pointlistInitialize();								// 初始化point list
        void	setupConnections();

        void	initializeProcessTab();
        void	initializeFilterTab();
        void	initializeDataQualityTab();
        void	initializeViewTab();

		void	measure( int method );									// 進行measure

	signals:
		void	doprocess();											// 開始進行process的動作
		void	set_measure_X( int );									// 設定measure的座標 X Y 到 spinbox
		void	set_measure_Y( int );

    protected slots:

		void 	launchBMPImport();										// Andrea新增 讀取BMP檔案
		void 	setBlobmethod( int method );							// Andrea新增 設定blob要用哪個方法
		void 	setHighlight( int is );									// Andrea新增 是否要反白
		void 	setWatershed( int method );								// Andrea新增 選擇要用哪個watershed的方法  目前有 4.8 4.1 MSER
		void 	blob();													// 進行blob
		void	channel();												// 進行extract channel
		void 	FFTJunctionProcess();									// 進行FFT Junction 
		void 	SteerJunctionProcess();									// 進行Steer Junctioni
		void 	EdgeProcess();											// 進行subpixel Edge 
		void 	CornerProcess();										// 進行subpixel Edge 

		void	setTabText( int index, QString str );					// 設定主畫面底下的tab文字說明

        void 	notifyFolderChange();
        void 	chooseFilteredColour();
        void 	chooseUnfilteredColour();
        void 	setColourButtons();

        void 	setDisplayTab();
		void	clearData();											// 清空資料 pointlist

        void 	setDoingPIV();
        void 	setOutput();
        void 	chooseOutputFolder();

		void	addPixel(PIXEL pixel);									// 點擊畫面 增加點清單
		void	removePointList( int row, int col );					// 點擊 point list (input) 刪除點資料
		void	view_square( int row, int col );						// 陰影用  會在畫面上把選取區標記出來
		void	remove_square();										// 移除某個square
		void	displayImage( int index, QImage image );				// 輸出畫面到顯示視窗

};

#endif // MAINWINDOW_H
