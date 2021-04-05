/*=====================================================================================================
	主視窗
/*=====================================================================================================*/

#include <QtGui>
#include <QVector>
#include <QPointF>
#include <cmath>
#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settings.h"
#include "importimages.h"

#include "pointlist.h"
#include "vectorlist.h"
#include "imagepaint.h"



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    initialize();							// 初始化
    setupWindows();
    setupConnections();
    initializeProcessTab();
    initializeFilterTab();
    setDisplayTab();
    setColourButtons();
    setDoingPIV();
    initializeDataQualityTab();
    initializeViewTab();
}

MainWindow::~MainWindow()
{
    delete importImages;
    delete filedata;
    delete process;
}


/*****************************************************************************************************************
	初始化pivdisplay
******************************************************************************************************************/
void	MainWindow::displayInitialize()
{
	display[0]	=	pivDisplay;
	display[1]	=	pivDisplay_2;
	display[2]	=	pivDisplay_3;
	display[3]	=	pivDisplay_4;
	position[0]	=	positionLabel;
	position[1]	=	positionLabel_2;
	position[2]	=	positionLabel_3;
	position[3]	=	positionLabel_4;
	rgbLabel[0]	=	RGBLabel;
	rgbLabel[1]	=	RGBLabel_2;
	rgbLabel[2]	=	RGBLabel_3;
	rgbLabel[3]	=	RGBLabel_4;

	for( int i = 0; i < displaySize; i++ )
	{
		display[i]->setSettings( settings );
		display[i]->setData( filedata );
		display[i]->setPositionLabel( position[i] );
		display[i]->setRGBLabel( rgbLabel[i] );
	}
}


/*****************************************************************************************************************
	初始化point list
******************************************************************************************************************/
void	MainWindow::pointlistInitialize()
{
	pointList[0]	=	aListWidget;
	pointList[1]	=	bListWidget;
}


/*****************************************************************************************************************
	初始化
******************************************************************************************************************/
void MainWindow::initialize()
{
	progress		=	new Progress;						// 設定 progress  這邊順序不能隨便對調
    settings		=	new Settings( progress );
    filedata		=	new DataContainer( settings );

	pointlistInitialize();

    vectorListWidget->setData(filedata);
    process		=	new Processing(settings,filedata);

	displayInitialize();

}

void MainWindow::setupWindows()
{
    importImages = new ImportImages;
	importImages->setProgress(progress);
    importImages->setData(filedata);
}

void MainWindow::setupConnections()
{
	int		i;

	/*
		Andrea修改
	*/
	//connect( openButton,		  SIGNAL(clicked()), this, SLOT( launchBMPImport() ) );  測試用 

    connect( openBMP,			 	SIGNAL(clicked()),						this,       			SLOT(launchBMPImport()) );						// 按下open BMP  可以讀取圖檔  Andrea
    connect( openBMP_2,			 	SIGNAL(clicked()),						this,       			SLOT(launchBMPImport()) );						// 按下open BMP  可以讀取圖檔  Andrea    2號用在measure
	connect( progress,				SIGNAL(signalImage(int,QImage)),		this,					SLOT(displayImage(int,QImage)) );				// 輸出畫面到顯示視窗

	connect( filedata,			 	SIGNAL(svgChanged(QString)),			pivDisplay_3,			SLOT(displaySVG(QString)) );					// 丟svg的方式顯示圖片 (svg暫存在硬碟)   丟到視窗三去做顯示

	connect( ScaleW,			 	SIGNAL(valueChanged(int)),				settings,    			SLOT(set_scale_w(int)) );						// 設定 Gauss filter scale w
	connect( ScaleL,			 	SIGNAL(valueChanged(int)),				settings,    			SLOT(set_scale_l(int)) );						// 設定 Gauss filter scale l
	connect( measureX,			 	SIGNAL(valueChanged(int)),				settings,    			SLOT(set_measure_X(int)) );						// 設定 Measure的座標X
	connect( measureY,			 	SIGNAL(valueChanged(int)),				settings,    			SLOT(set_measure_Y(int)) );						// 設定 Measure的座標Y
	connect( this,               	SIGNAL(doprocess()),                	process,				SLOT(processBatch()) );							// 進行process的動作
	connect( DoFilter,			 	SIGNAL(clicked()),						this,					SLOT(blob()) );									// 按下按鈕 開始執行blob detector  (包含subblob  watershed)
	connect( ChannelButton,			SIGNAL(clicked()),						this,					SLOT(channel()) );								// 抽取指定顏色的通道 
	connect( FFTJunctionButton,   	SIGNAL(clicked()),						this,					SLOT(FFTJunctionProcess()) );					// 按下按鈕 開始執行 FFT Junction Detector
	connect( SteerJunctionButton, 	SIGNAL(clicked()),						this,					SLOT(SteerJunctionProcess()) );					// 按下按鈕 開始執行 Steer Junction Detector
	connect( EdgeButton,		  	SIGNAL(clicked()),						this,					SLOT(EdgeProcess()) );							// 按下按鈕 開始執行 Subpixel Edge
	connect( CornerButton,		  	SIGNAL(clicked()),						this,					SLOT(CornerProcess()) );						// 按下Corner的按鈕 進行subpixel的Corner計算
	
	//connect( process,			  	SIGNAL(imageThrow(QImage)),				pivDisplay,				SLOT(displayBMP(QImage)) );						// 計算好 Gauss Filter 將資料丟給顯示
	connect( process,			  	SIGNAL(signalsGress(int)),				filterProgress,			SLOT(setValue(int)) );							// 設定進度條
	connect( process,			  	SIGNAL(signalsGress(int)),				measureProgress,		SLOT(setValue(int)) );							// 設定進度條
	connect( process,			  	SIGNAL(signalTabText(int,QString)),		this,					SLOT(setTabText(int,QString)) );				// 設定進度條

	connect( blobCombo,			  	SIGNAL(currentIndexChanged(int)),		this,					SLOT(setBlobmethod(int)) );						// 設定現在執行的是哪一個blob detector
	connect( waterCheck,		  	SIGNAL(stateChanged(int)),				this,					SLOT(setHighlight(int)) );						// 設定是否要反白
	connect( waterCombo,		  	SIGNAL(currentIndexChanged(int)),		this,					SLOT(setWatershed(int)) );						// 設定哪個watershed方法  4.8 4.1 mser


	// 把訊息印到除錯訊息的地方   append  or   setText
	connect( filedata,   			SIGNAL(debugSignal(QString)),     		text_debug,				SLOT(append(QString)) );
	connect( process,    			SIGNAL(debugSignal(QString)),     		text_debug,				SLOT(append(QString)) );
	connect( progress,	 			SIGNAL(debugSignal(QString)),	   		text_debug,				SLOT(append(QString)) );

	// 這邊跟顯示畫面 pointlist有關
	for( i = 0; i < 2; i++ )
		connect( display[i],			SIGNAL(mousePressed(PIXEL)),			this,					SLOT(addPixel(PIXEL)) );					// 點擊畫面 更新資料
	connect( progress,				SIGNAL(updateWidgeB()),					pointList[1],			SLOT(updatePointList()) );						// 將資料顯示到 point list (output)
	//connect( pointList[0],			SIGNAL(cellClicked(int,int)),			this,					SLOT(removePointList(int,int)) );				// 點擊 point list (input) 刪除點資料
	// 陰影檢測用 改成點擊到的時候顯示該區域到顯示區
	connect( pointList[0],			SIGNAL(cellClicked(int,int)),			this,					SLOT(view_square(int,int)) );
	connect( removeButton,			SIGNAL(clicked()),						this,					SLOT(remove_square()) );						// 移除某個square
	connect( clearButton,			SIGNAL(clicked()),						this,					SLOT(clearData()) );							// 按下clear按鈕  清空資料
	connect( inhenceCheck,			SIGNAL(stateChanged(int)),				settings,				SLOT(setInhence(int)) );						// 設定inhencement  判斷是抓單點陰影 還是整段的陰影 (應付noise很強 跟noise不強的case)
	connect( saveCheck,				SIGNAL(stateChanged(int)),				settings,				SLOT(setSaved(int))	);							// 設定是否要將結果輸出到檔案 (image部分)
	connect( batchCheck,			SIGNAL(stateChanged(int)),				settings,				SLOT(setBatch(int)) );							// 設定是否為批次作業
	
    // Importing
   // connect(importButton,			SIGNAL(clicked()),						this,					SLOT(launchImageImport()));
   // connect(filedata,				SIGNAL(imagesImported()),				this,					SLOT(notifyFolderChange()));

    connect(vectorListWidget, SIGNAL(fileClicked(int)), this, SLOT(vectorClicked(int)));

    // The following should be moved into settings
    //
    connect(colourButtonFiltered, SIGNAL(clicked()), this, SLOT(chooseFilteredColour()));
    connect(colourButtonUnfiltered, SIGNAL(clicked()), this, SLOT(chooseUnfilteredColour()));
    connect(scaleSpin, SIGNAL(valueChanged(double)), settings, SLOT(setVectorScale(double)));
    connect(subSpin, SIGNAL(valueChanged(double)), settings, SLOT(setVectorSub(double)));

    // Process tab
    connect(hSizeCombo, SIGNAL(activated(int)), settings, SLOT(setIntLengthX(int)));
    connect(vSizeCombo, SIGNAL(activated(int)), settings, SLOT(setIntLengthY(int)));
    connect(hSpaceSpin, SIGNAL(valueChanged(int)), settings, SLOT(setDeltaX(int)));
    connect(vSpaceSpin, SIGNAL(valueChanged(int)), settings, SLOT(setDeltaY(int)));


    // DoingPIV
    connect(outputFolderEdit, SIGNAL(textEdited(QString)), this, SLOT(setOutput()));
    connect(outputFolderButton, SIGNAL(clicked()), this, SLOT(chooseOutputFolder()));
    connect(outputFormatCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setOutput()));


    connect(batchButton, SIGNAL(clicked()), process, SLOT(launchBatchWindow()));
    connect(process, SIGNAL(batchProcessed()), this, SLOT(batchDone()));


	//for( i = 0; i < displaySize; i++ )
	//	connect(display[i],   	SIGNAL(mousePressed(PIXEL)),			this,			SLOT(updatePositionPixel(PIXEL)));				// Andrea修改  點一下滑鼠,把RGB資訊輸入進去.


	/*connect(pivDisplay,   	SIGNAL(mousePressed(QPointF)),			this,			SLOT(uptdaePositionRGB(QPointF)));				// Andrea修改  點一下滑鼠,把RGB資訊輸入進去.
	connect(pivDisplay_2,  	SIGNAL(mousePressed(QPointF)),			this,			SLOT(uptdaePositionRGB(QPointF)));				// Andrea修改  點一下滑鼠,把RGB資訊輸入進去.   用在視窗2
	connect(pivDisplay,   	SIGNAL(mousePressed(QPointF)),			this,			SLOT(updatePositionXY(QPointF)));				// Andrea修改  點一下滑鼠,把座標輸入進去.
	connect(pivDisplay_2,  	SIGNAL(mousePressed(QPointF)),			this,			SLOT(updatePositionXY(QPointF)));				// Andrea修改  點一下滑鼠,把座標輸入進去.   用在視窗2*/



	//connect(pivDisplay,   	SIGNAL(mousePressed(QPointF)),			pivDisplay,		SLOT(mark_XY(QPointF)));						// Andrea修改  點一下滑鼠, 在上面標記點
	//connect(pivDisplay_2,  	SIGNAL(mousePressed(QPointF)),			pivDisplay_2,	SLOT(mark_XY(QPointF)));						// Andrea修改  點一下滑鼠, 在上面標記點    用在視窗2



	connect(this,			SIGNAL(set_measure_X(int)),				measureX,		SLOT(setValue(int) ) );							// 設定 X 座標的spinbox
	connect(this,			SIGNAL(set_measure_Y(int)),				measureY,		SLOT(setValue(int) ) );							// 設定 Y 座標的spinbox
	

}

/*-------- Setting the tabs -----------------*/

void MainWindow::setDoingPIV()
{
    outputFolderEdit->setText(settings->outputFolder());
    outputFormatCombo->insertItem(OpenPIV::Text,"Text file");
    outputFormatCombo->insertItem(OpenPIV::HDF5,"HDF5");
}

void MainWindow::initializeProcessTab()
{
    int i, index;
    double power2;

    for (i = 0; i < 4; i++)
    {
        power2 = pow(2.0,double(4+i));
        hSizeCombo->insertItem(i,QString("%1").arg(power2));
        vSizeCombo->insertItem(i,QString("%1").arg(power2));
    }

    index = int(log10(double(settings->intLengthX())) / log10(2.0)) - 4;
    index = int(log10(double(settings->intLengthY())) / log10(2.0)) - 4;

    hSpaceSpin->setMinimum(0);
    vSpaceSpin->setMinimum(0);
    hSpaceSpin->setMaximum(100);
    vSpaceSpin->setMaximum(100);
    double value;
    value = double(settings->deltaX()) / double(settings->intLengthX()) * 100;
    hSpaceSpin->setValue(int(value));
    value = double(settings->deltaY()) / double(settings->intLengthY()) * 100;
    vSpaceSpin->setValue(int(value));

    interpolateCombo->insertItem(OpenPIV::GaussianSubPixel,"Three-point Gaussian");
}

void MainWindow::initializeFilterTab()
{
    // Global
    minUedit->setValidator(new QDoubleValidator(-256.0, 256.0, 3, minUedit));
    maxUedit->setValidator(new QDoubleValidator(-256.0, 256.0, 3, maxUedit));
    minVedit->setValidator(new QDoubleValidator(-256.0, 256.0, 3, minVedit));
    maxVedit->setValidator(new QDoubleValidator(-256.0, 256.0, 3, maxVedit));

    // Local
    QString label;
    int i, index;

	// Andrea新增 blobCombo 用來選擇現在要用哪一個方法
	blobCombo->addItem("up");
	blobCombo->addItem("down");
	blobCombo->addItem("left");
	blobCombo->addItem("right");
	blobCombo->addItem("left_up");
	blobCombo->addItem("left_down");
	blobCombo->addItem("right_up");
	blobCombo->addItem("right_down");
	blobCombo->addItem("all_direction");
	blobCombo->addItem("up_down");
	blobCombo->addItem("left_right");
	blobCombo->addItem("left up <-> right down");
	blobCombo->addItem("left down <-> right up");

	waterCombo->addItem( "4.8" );
	waterCombo->addItem( "4.1" );
	waterCombo->addItem( "MSER" );

	//blobCombo->setCurrentIndex( OpenPIV::SubPixBlob );				// andrea 預設值

	
	// 陰影
	ChannelBox->setValue(4);
	saveCheck->setCheckState( Qt::Checked );
	ScaleL->setMaximum(300);



    for (i = 0; i < 5; i++)
    {
        index = (i+1)*2 + 1;
        label = QString("%1x%2").arg(index).arg(index);
        localNxNCombo->insertItem(i,label);
    }
    localUedit->setValidator(new QDoubleValidator(0.0, 256.0, 3, localUedit));
    localVedit->setValidator(new QDoubleValidator(0.0, 256.0, 3, localVedit));

    // Interpolate
//    interpolateMethodCombo->insertItem(OpenPIV::InterpolateMean,"Mean");
    for (i = 0; i < 5; i++)
    {
        index = (i+1)*2 + 1;
        label = QString("%1x%2").arg(index).arg(index);
        interpolateNxNCombo->insertItem(i,label);
    }

    // Smoothing
    for (i = 0; i < 5; i++)
    {
        index = (i+1)*2 + 1;
        label = QString("%1x%2").arg(index).arg(index);
        smoothNxNCombo->insertItem(i,label);
    }
    smoothRadiusEdit->setValidator(new QDoubleValidator(0.0, 1000.0, 3, smoothRadiusEdit));
}



void MainWindow::setDisplayTab()
{
    vectorToggle->setChecked(true);
}

void MainWindow::initializeDataQualityTab()
{
    snrEdit->setValidator(new QDoubleValidator(0.0, 1000.0, 3, snrEdit));
    intensityEdit->setValidator(new QDoubleValidator(0.0, 5000.0, 3, intensityEdit));
}



void MainWindow::initializeViewTab()
{
    scaleSpin->setValue(settings->vectorScale());
    subSpin->setValue(settings->vectorSub());
}


void MainWindow::notifyFolderChange()
{
    QString directory = settings->outputFolder();
    QString boxText = QString(tr("The output folder has been set to:\n %1")).arg(directory);
    boxText.append(tr("\n This folder can be changed on the \"Doing PIV\" tab.\n"));
    QMessageBox::information(this,QString(tr("Output Folder Set")),boxText);
//    pivAclicked(0);
}




void MainWindow::chooseFilteredColour()
{
    QColor colour;
    colour = QColorDialog::getColor(settings->vectorColourFiltered(),this);
    settings->setVectorColourFiltered(colour);
    setColourButtons();
}

void MainWindow::chooseUnfilteredColour()
{
    QColor colour;
    colour = QColorDialog::getColor(settings->vectorColourUnfiltered(),this);
    settings->setVectorColourUnfiltered(colour);
    setColourButtons();
}

void MainWindow::setColourButtons()
{
    QColor colour;
    QString styleItem;
    colour = settings->vectorColourUnfiltered();
    styleItem = QString("background-color: rgb(%1,%2,%3);").arg(colour.red()).arg(colour.green()).arg(colour.blue());
    colourButtonUnfiltered->setStyleSheet(styleItem);
    colour = settings->vectorColourFiltered();
    styleItem = QString("background-color: rgb(%1,%2,%3);").arg(colour.red()).arg(colour.green()).arg(colour.blue());
    colourButtonFiltered->setStyleSheet(styleItem);
}


/* ----------- Output ----------------*/

void MainWindow::setOutput()
{
    settings->setOutputFolder(outputFolderEdit->text());
    settings->setOutputFormat(outputFormatCombo->currentIndex());
}

void MainWindow::chooseOutputFolder()
{
    QString directory = QFileDialog::getExistingDirectory(this);
    outputFolderEdit->setText(directory);
    setOutput();
}




/*============================================================================
	設定要用哪個blob的方法
==============================================================================*/
void MainWindow::setBlobmethod( int	method )
{
	settings->shadow_dir	=	method;
}



/*============================================================================
	設定要用watershed哪個方法
	目前有
	4.8
	4.1
	MSER
==============================================================================*/
void MainWindow::setWatershed( int	method )
{
	settings->watershed	=	method;
}


// Andrea 新增
/*-----------------------------進行measure-------------------------------*/
void	MainWindow::measure( int method )
{
	settings->setProcessor( method );
	emit( doprocess() );
}

/*-----------------------------進行FFT的Junction Detector------------------------------*/
void	MainWindow::FFTJunctionProcess()
{
	measure(OpenPIV::FFT);
}

/*-----------------------------進行Steer的Junction Detector------------------------------*/
void	MainWindow::SteerJunctionProcess()
{

	measure(OpenPIV::Steer);
}

/*-----------------------------進行subpixel Edge------------------------------*/
void	MainWindow::EdgeProcess()
{

	measure(OpenPIV::Edge);
}

/*-----------------------------進行subpixel Corner------------------------------*/
void	MainWindow::CornerProcess()
{

	measure(OpenPIV::Corner);
}


/*-----------------------------進行blob detector------------------------------*/
void	MainWindow::blob()
{
	// 陰影檢測 (偷放在channel底下)
	settings->setProcessor( OpenPIV::Channel );
	emit( doprocess() );
}

/*-----------------------------進行extract channel------------------------------*/
void	MainWindow::channel()
{
	//settings->setProcessor( OpenPIV::Channel );
	settings->square_size	=	ChannelBox->value()+1;
	settings->set_square	^=	true;
	//emit( doprocess() );
}




/*==================================================================================================================
	設定主畫面底下的tab文字說明
/*==================================================================================================================*/
void	MainWindow::setTabText( int index, QString str )
{
	pivTab->setTabText( index, str );
}

/*----------Launching BMP  windows---------------------------------*/
void MainWindow::setHighlight( int is )
{
	settings->isHighlight	=	(is == 0) ? false : true;
	settings->dark_light	=	is ? true : false;
}



/*==================================================================================================================
	更新座標 measure專用  
	點一下滑鼠就能設定XY座標
/*==================================================================================================================*/
/*void	MainWindow::updatePositionXY( QPointF XY )
{
	int		x	=	floor( XY.x() );
	int		y	=	floor( XY.y() );

	settings->set_measure_X( x );
	settings->set_measure_Y( y );

	emit( set_measure_X(x) );
	emit( set_measure_Y(y) );
}*/

/*==================================================================================================================
	取得該點顏色資訊  
	點一下滑鼠
/*==================================================================================================================*/
/*void	MainWindow::uptdaePositionRGB( QPointF XY )
{
	int		x	=	floor( XY.x() );
	int		y	=	floor( XY.y() );

	QRgb	rgb	=	pivDisplay_2->getRGB( x, y );		// 不管怎樣都從二號視窗抓資料 

	settings->setRGB( rgb );

}*/



/*==================================================================================================================
	點擊畫面 
	增加點清單
/*==================================================================================================================*/
void	MainWindow::addPixel( PIXEL pixel )
{
	bool	is_add;

	pixel.direction		=	blobCombo->currentIndex();

	// 更新到 filedata 回傳值表示是否有新增成功
	is_add	=	filedata->addPointList( pixel );

	// 更新到 point list (input )  控制重複點的部分由filedata控制
	if( is_add )
		pointList[0]->updatePointList( pixel );

}


/*==================================================================================================================
	清空資料 
	pointlist
/*==================================================================================================================*/
void	MainWindow::clearData()
{
	filedata->clearPixelList();

	pointList[0]->clearData();
	pointList[1]->clearData();
}



/*==================================================================================================================
	點擊 point list (input) 
	刪除點資料
/*==================================================================================================================*/
void	MainWindow::removePointList( int row, int col )
{
	filedata->removePointList( row, col );
	pointList[0]->removePointList( row, col );
}

/*==================================================================================================================
	會在畫面上把選取區標記出來
/*==================================================================================================================*/
void	MainWindow::view_square( int row, int col )
{
	//filedata->removePointList( row, col );
	//pointList[0]->removePointList( row, col );
	PIXEL	pixel	=	filedata->get_square( row, col );
	for( int i = 0; i < 2; i++ )
		display[i]->mark_square( pixel );
}



/*==================================================================================================================
	輸出畫面到顯示視窗
/*==================================================================================================================*/
void	MainWindow::displayImage( int index, QImage image  )
{
	display[index]->displayImage( image );
}




/*==================================================================================================================
	Andrea 新增
	讀取一個bmp檔案
/*==================================================================================================================*/
void	MainWindow::launchBMPImport()
{
    QString		filename	=	QFileDialog::getOpenFileName( this , tr("Open File"), "./BMP/test", tr("Images (*.bmp)"));
	QImage		image;
	bool		bsucess;

    if( !filename.isEmpty() )
    {
		bsucess		=	image.load( filename, "BMP" );
		if( bsucess )
		{
			filedata->loadBMP( image );
			display[0]->displayImage( image );
			display[1]->displayImage( image );
		}
	}
}



/*==================================================================================================================
	移除某個square
/*==================================================================================================================*/
void	MainWindow::remove_square()
{
	int		index	=	removeBox->value();

	filedata->removePointList( index, 0 );
	pointList[0]->removePointList( index, 0 );
}
