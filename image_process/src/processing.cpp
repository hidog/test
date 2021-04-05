#include <QObject>
#include <QPoint>
#include <QStringList>

#include <QSemaphore>
#include <QThread>
#include <QMutex>
#include <iostream>
#include <fstream>

#include "settings.h"
#include "processing.h"
#include "pivthread.h"
#include "batchwindow.h"


#include "progress.h"

// 負責處理thread同步
QSemaphore	freePairs(10);
QSemaphore	usedPairs;
QMutex		mutex;


/*********************************************************************************************************************
	建構函數
/**********************************************************************************************************************/
Processing::Processing(Settings *settingsPass, DataContainer *filedataPass, QObject *parent) 
	: QObject(parent)
{
    filedata		=	filedataPass;
    settings		=	settingsPass;

	// 批次window
    batchWindow		=	new BatchWindow;
    batchWindow->setSettings(settings);

    connect(batchWindow, SIGNAL(startProcess()), this, SLOT(processBatch()));
    connect(batchWindow, SIGNAL(stopProcess()), this, SLOT(stopBatch()));

    threadsCreated	=	false;

	progress		=	settingsPass->getProgress();
	connect( progress, SIGNAL(signalsGress(int)), this, SLOT(progress_add(int)) );
	connect( progress, SIGNAL(signalTabText(int,QString)), this, SLOT(setTabText(int,QString)) );


	isProcess		=	false;

	// Andrea 搬移到這邊. 因為原本的位置懷疑會當機
	//initializeThreads();
}


/*********************************************************************************************************************
	解構函數
/**********************************************************************************************************************/
Processing::~Processing()
{
    delete batchWindow;
    deleteThreads();
	delete	progress;
}


/*********************************************************************************************************************
	刪除thread
/**********************************************************************************************************************/
void Processing::deleteThreads()
{
	progress->debuger("Processing::deleteThreads...");

    if (threadsCreated)
    {
        while (threadList.size() > 0)
		{
            delete threadList.takeFirst();
		}
		threadList.clear();
        threadsCreated = false;
    }
	isProcess	=	false;
}


/*********************************************************************************************************************
	批次window
/**********************************************************************************************************************/
void Processing::launchBatchWindow()
{
    batchWindow->refresh();
    batchWindow->show();
}



/*********************************************************************************************************************
	停止批次動作
/**********************************************************************************************************************/
void Processing::stopBatch()
{
    for (int i = 0; i < threadList.size(); i++)
    {
        threadList.value(i)->stopProcess();
    }
}



/*********************************************************************************************************************
	送出批次訊息
/**********************************************************************************************************************/
void Processing::emitBatchProcessed()
{
    //std::cout << "processed\n";
    emit(batchProcessed());
}


/*********************************************************************************************************************
	不開thread
	目前無作用
/**********************************************************************************************************************/
void Processing::processBatchSerial()
{}




/*====================================================================================
	Andrea
	mainwindows進入process的進入點
/*====================================================================================*/
void	Processing::processBatch()
{
	//QString		str	=	QString("Processing::processBatch");
	//emit(debugSignal(str) );

	progress->debuger( "Processing::processBatch..." );

	// 正在執行其他process,擋住
	if( isProcess )
	{
		progress->debuger("now is process!!  please wait...", QColor(0,255,0) );
		return;
	}


   // batchWindow->setProgressRange(filedata->size());
	//batchWindow->setProgressRange(1);

    if (settings->batchThreading()) 
		processBatchParallel();
    else 
		processBatchSerial();

}


/*====================================================================================
	Andrea
	設定 Gauss Filter 的 scale w
	未來需要在這邊加上multi thread的判斷
	必須去計算是否每個thread都finished.
/*====================================================================================*/
void	Processing::finish( int id )
{
	progress->debuger( QString("%1 - process finished...").arg(id)   , QColor(0,50,0) );
	deleteThreads();
}




/*====================================================================================
	Andrea
	執行process的進入點 (預設平行化)
/*====================================================================================*/
void Processing::processBatchParallel()
{
	progress->debuger( QString("Processing::processBatchParallel...") );

	// 寫在這邊  跑第二次的時候造成當機...
    initializeThreads();
    for (int i = 0; i < threadList.size(); i++)
	{
		progress->debuger( QString("%1 - process...").arg(i) );
        threadList.value(i)->process();
    }
}





/*====================================================================================
	設定進度條
/*====================================================================================*/
void Processing::progress_add(int value)
{
	emit( signalsGress(value) );
}



/*====================================================================================
	設定主畫面tab的文字
/*====================================================================================*/
void Processing::setTabText(int index, QString str)
{
	emit( signalTabText(index, str) );
}



/*====================================================================================
	process 初始化thread
/*====================================================================================*/
void Processing::initializeThreads()
{
	isProcess			=	true;
	threadsCreated		=	true;

    int		datasize	=	1;


    int i;
    int N = 1;//QThread::idealThreadCount();			// 之後再改成動態


	QString	str	=	"Processing::initializeThreads";
	str.append('0'+N);
	emit(debugSignal(str) );


	// 先將舊的資料清空
	threadList.clear();

    for (i = 0; i < N; i++)
    {
		//pivthread	=	new PivThread(&freePairs,&usedPairs,&mutex ,  splitList(i,N,datasize) ) ;
		
        threadList.append(new PivThread( i, &freePairs,&usedPairs,&mutex)  );
//		threadList.append( pivthread );
		threadList.value(i)->setProgress(progress);			// 將progress丟進去  負責回報進度
        threadList.value(i)->setSettings(settings);
        threadList.value(i)->setFileData(filedata);
		connect( threadList.value(i), SIGNAL(finished(int)), this, SLOT(finish(int)) );
		//connect(   pivthread, finished(), this, stopBatch() );
    }

}