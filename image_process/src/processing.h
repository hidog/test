#ifndef PROCESSING_H
#define PROCESSING_H

#include <QStringList>
#include <QPoint>
#include <QObject>

#include <QSemaphore>
#include <QThread>
#include <QMutex>

#include "datacontainer.h"
#include "settings.h"
#include "batchwindow.h"
#include "output.h"
#include "pivthread.h"
#include "progress.h"

//#include "../BLOB/gaussfilter.h"

class Processing : public QObject 
{
    Q_OBJECT

    public:
        explicit	Processing(Settings *settingsPass, DataContainer *filedataPass, QObject *parent = 0);
        virtual		~Processing();

    public slots:
        void	processBatch();

		void	progress_add( int value );				// 設定進度條
		void	setTabText( int index, QString str );	// 設定主畫面的tab文字
		void	finish( int id );						// 結束process   目前只做single thread


    signals:
        void	currentProcessed();
        void	batchProcessed();

		void	debugSignal( QString );					// 除錯訊息

		void	imageThrow(QImage);						// 丟出圖片
		void	signalsGress(int);						// 控制進度條
		void	signalTabText( int, QString );			// 修改主畫面的tab文字說明


    protected slots:
        void	launchBatchWindow();
        void	emitBatchProcessed();
        void	stopBatch();

    protected:
        void	processBatchSerial();
        void	processBatchParallel();
        void	initializeThreads();
        void	deleteThreads();


    private:
        Settings				*settings;
        DataContainer			*filedata;
        BatchWindow				*batchWindow;

        QList<PivThread*>		threadList;
        bool					threadsCreated;

		//GaussFilter_2D			gauss_filter;			// Andrea  Gauss Filter
		Progress				*progress;				// 進度條控制

		bool					isProcess;				// 紀錄是否正在執行process  是的話要鎖住

};

#endif // PROCESSING_H
