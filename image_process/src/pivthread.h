#ifndef PIVTHREAD_H
#define PIVTHREAD_H

#include <fstream>
#include <assert.h>

#include <QThread>
#include <QSemaphore>
#include <QVector>
#include <QMutex>
#include <QList>

#include "settings.h"
#include "datacontainer.h"

// blob detector
#include "../BLOB/blob.h"				// 自行撰寫的sub pixel blob detector
#include "../BLOB/watershed.h"			// 自行撰寫的watershed

// measure
#include "../MEASURE/FFT.h"				// 利用快速傅立葉轉換來做junction detector
#include "../MEASURE/Steer.h"			// 利用Steer filter來做junction detector
#include "../MEASURE/subpixel.h"		// 進行各種subpixel的動作 Edge Corner等等
#include "../MEASURE/channel.h"			// 抽取色彩通道
#include "../MEASURE/colorcode.h"		// 色標檢測

#include "../MEASURE/emboss.h"

#include "progress.h"


class PivThread : public QThread
{
    Q_OBJECT

    public:
        explicit PivThread( int _id, QSemaphore *freePass, QSemaphore *usedPass, QMutex *mutexPass/*, QList<int> listPass*/, QObject *parent = 0);
        virtual ~PivThread();

        void setSettings(Settings *settingsPass);
        void setFileData(DataContainer *filedataPass);

        int process();
        void stopProcess();

		void				setProgress( Progress *passGress );			// 設定Progress
		emboss::EMBOSS_DIRECTION	int2direction( int a );						// 將整數轉成direction
		emboss::EMBOSS_DARK_LIGHT	int2dark( int a );							// 將整數轉成dark_light

		int		id;		// 此thread的id


    protected:
        void	initializeProcessor();
		void	run();

		void	run2();

	signals:
		
		void	finished(int);

    private:


        QSemaphore *free;
        QSemaphore *used;
        QMutex *mutex;
        QList<int> list;

        Settings		*settings;
        bool			settingsSet;
        int				_processor;
        DataContainer	*filedata;
        bool			filedataSet;

        bool filter;
        bool analysisCreated;

        bool abort;

        //Processors
		AndreaEngine		*engine;						// 運算核心 決定要做怎樣的動作
		Progress			*progress;						// 負責回報進度用

};

#endif // PIVTHREAD_H
