#ifndef PROGRESS_H
#define PROGRESS_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	為了讓其他物件可以控制進度條
	所以寫一個物件丟進去  用這個物件來控制進度條
	未來需要的話再加上其他功能= ="
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <QObject>
#include <QString>
#include <QColor>
#include <QVector>
#include <QImage>
#include <QPointF>

class Progress : public QObject
{
#ifndef _GTEST_
    Q_OBJECT
#endif

    public:
        explicit	Progress();
        virtual		~Progress();

		void		setGress( int value );									// 設定進度條
		void		setTabText( int index, QString str );					// 設定主畫面 tab 上的文字說明
		void		debuger( QString );										// 印debug訊息
		void		debuger( QString , QColor color  );						// 印debug訊息 並且有顏色
		void		outputSubpixelEdge( QVector<QPointF> qf);				// 輸出edge subpixel的資料
		void		outputImage( int index, QImage image );					// 輸出image到顯示視窗

#ifndef _GTEST_
	public slots:
#endif
	
#ifndef _GTEST_
    signals:
		void		signalsGress(int);
		void		debugSignal( QString );
		void		signalTabText(int,QString);
		void		updateWidgeB();											// 輸出訊息到 widge B
		void		signalImage( int, QImage );								// 輸出image到顯示視窗
#endif

    private:


};

#endif // PROGRESS_H
