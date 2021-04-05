/************************************************************************************************************************************

	pivdisplay
	mainwindow底下的顯示視窗
	負責各種圖片的顯示

	QGraphicsView分成三個部分
	View   
	Scene  (model)
	item   

*************************************************************************************************************************************/

#ifndef PIVDISPLAY_H
#define PIVDISPLAY_H



#include <QMouseEvent>
#include <QCoreApplication>
#include <QGraphicsView>
#include <QGraphicsSvgItem>
#include <QGraphicsEffect>
#include <QStyleOptionGraphicsItem>
#include <QBrush>
//#include <QSvgGenerator>
//#include <QSvgRenderer>
#include <QLabel>

#include "datacontainer.h"
#include "displaywidget.h"
#include "imagepaint.h"
#include "settings.h"



class PivDisplay : public QGraphicsView
{
    Q_OBJECT

	private:

		QLabel				*positionLabel;											// 顯示視窗底下 顯示座標的label
		QLabel				*RGBLabel;												// 顯示視窗底下 顯示RGB的label

		PIXEL				pix_1;													// 用兩個point紀錄按下滑鼠 放開滑鼠的座標
		PIXEL				pix_2;

        bool				mouseIsPressed;
		bool				isSVG;

        QImage				image;													// 圖片資料
		QRect				rect;													// 原始圖片大小
		QRectF				rectF;													// 原始圖片大小
        QRectF				viewRectF;												// 顯示視窗大小  可以用在縮放
        bool				viewRectFSet;		
		
        QGraphicsScene		*scene;													// 顯示用 不能刪   scene = model   
        ImagePaint			*imPaint;												// item
		QPointF				centerPoint;

		QPointF				preMarkPoint;											// 存上一次標記點的位置
		QRgb				preMarkRgb;												// 存上一次標記點的質

        Settings			*settings;
        DataContainer		*filedata;

		PIXEL				MouseEvent2Pixel( QMouseEvent *event );					// 將滑鼠訊息轉成 PIXEL 資訊

    public:
        PivDisplay(QWidget* parent = 0);
        ~PivDisplay();
		
        QPointF				getPoint1();											// 目前無作用
        QPointF				getPoint2();											// 目前無作用

        void				setSettings( Settings *settingsPass );					
        void				setData( DataContainer *filedataPass );
		void				setPositionLabel( QLabel *_position );
		void				setRGBLabel( QLabel *_rgb );

		void				mark_point( int x, int y );								// 標記點在畫面上.
		QRgb				getRGB( int i, int j );									// 取得該點座標的 RGB 值

		void				mark_square( PIXEL pixel );								// 標記正方形區域

    signals:
        void				mouseMoved( PIXEL pixel );								// openPIV原本的
		void				mousePressed(PIXEL pixel);								// 點擊畫面 送出PIXEL資料

    public slots:
        void				refreshView();
		void				displaySVG( QString name );								// 顯示SVG檔案 檔案暫存在硬碟
		void				displayImage(QImage imagePass);							// 顯示圖片

    protected:
        void				resize();												// resize

        void 				wheelEvent(QWheelEvent* event);							// 縮放 (滾輪)
        void				zoomIn();
        void				zoomOut();
        void				zoomFit();

        void 				mouseReleaseEvent( QMouseEvent* event );				// 滑鼠放開動作
		void 				mouseMoveEvent( QMouseEvent* event );					// 滑鼠移動動作
		void				updateMoveData( PIXEL pixel );							// 更新顯示資訊 (顯示畫面底下 座標 RGB資料)
        void 				mousePressEvent(QMouseEvent* event);
        void 				resizeEvent(QResizeEvent* event);						// 目前無作用





};
#endif // PIVDISPLAY_H
