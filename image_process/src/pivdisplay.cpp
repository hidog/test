#include "pivdisplay.h"



/************************************************************************************************
	建構
/*************************************************************************************************/
PivDisplay::PivDisplay(QWidget* parent) 
	: QGraphicsView(parent)
{
    mouseIsPressed	=	false;
    this->setMouseTracking(true);
    this->setCursor(Qt::OpenHandCursor);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene					=	new QGraphicsScene;
	imPaint					=	NULL;

    viewRectFSet			=	false;
	isSVG					=	false;
}


/************************************************************************************************
	解構
/*************************************************************************************************/
PivDisplay::~PivDisplay()
{
	delete	imPaint;
	delete 	scene;			// delete順序不能顛倒
}


/************************************************************************************************
	設定setting
/*************************************************************************************************/
void	PivDisplay::setSettings( Settings *settingsPass )
{
    settings	=	settingsPass;
}

/************************************************************************************************
	設定filedata
/*************************************************************************************************/
void	PivDisplay::setData( DataContainer *filedataPass )
{
    filedata	=	filedataPass;
}

/************************************************************************************************
	設定position label
/*************************************************************************************************/
void	PivDisplay::setPositionLabel( QLabel *_position )
{
	positionLabel	=	_position;
}

/************************************************************************************************
	設定rgb label
/*************************************************************************************************/
void	PivDisplay::setRGBLabel( QLabel *_rgb )
{
	RGBLabel	=	_rgb;
}



/************************************************************************************************
	縮放  
/*************************************************************************************************/
void PivDisplay::wheelEvent(QWheelEvent* event)
{
	// 放大
    if( event->delta() > 0 ) 
		zoomIn();
	// 縮小
    else 
		zoomOut();
}

/************************************************************************************************
	縮放  
	放大
/*************************************************************************************************/
void PivDisplay::zoomIn()
{
    this->scale(1.05,1.05);
    viewRectF.setWidth(viewRectF.width()/1.05);
    viewRectF.setHeight(viewRectF.height()/1.05);

    viewRectFSet	=	true;
}

/************************************************************************************************
	縮放  
	縮小
/*************************************************************************************************/
void PivDisplay::zoomOut()
{
    this->scale(1.0/1.05,1.0/1.05);
    viewRectF.setWidth(viewRectF.width()*1.05);
    viewRectF.setHeight(viewRectF.height()*1.05);

    viewRectFSet	=	true;
}

/************************************************************************************************
	縮放  
	fit
/*************************************************************************************************/
void PivDisplay::zoomFit()
{
    viewRectF		=	rectF;
    viewRectFSet	=	true;
    resize();
}


/************************************************************************************************
	滑鼠放開動作
/*************************************************************************************************/
void PivDisplay::mouseReleaseEvent( QMouseEvent* event )
{
    mouseIsPressed	=	false;

	QPointF		point;
	QRgb		rgb;
	int			x,	y;

	pix_2	=	MouseEvent2Pixel( event );

    this->setCursor(Qt::OpenHandCursor);
}



/************************************************************************************************
	將滑鼠訊息轉成 PIXEL 資訊
/*************************************************************************************************/
PIXEL	PivDisplay::MouseEvent2Pixel( QMouseEvent *event )
{
	const QPointF	point	=	mapToScene( QPoint(event->x(), event->y()) );		// 座標轉換成螢幕對應的座標
	const double	dx		=	point.x();
	const double	dy		=	point.y();
	const int		x		=	floor( dx );
	const int		y		=	floor( dy );
	const QRgb		rgb		=	getRGB( x, y );

	return	PIXEL( x, y, dx, dy, rgb );
}




/************************************************************************************************
	滑鼠移動動作
/*************************************************************************************************/
void	PivDisplay::mouseMoveEvent(QMouseEvent* event)
{
    double		dx,				dy;
    double		xCenter,		yCenter;
    int			xCenterView,	yCenterView;
	int			x,				y;
	QRgb		rgb;
	QPointF		point;

	// 拖曳滑鼠狀態
    if (mouseIsPressed)
    {
        xCenterView		=	int(double(this->width() / 2.0));
        yCenterView		=	int(double(this->height() / 2.0));

		pix_2			=	MouseEvent2Pixel( event );

        dx				=	double( pix_2.x - pix_1.x );
        dy				=	double( pix_2.y - pix_1.y );

        xCenter			=	QPointF( this->mapToScene(QPoint(xCenterView,yCenterView)) ).x();
        yCenter			=	QPointF( this->mapToScene(QPoint(xCenterView,yCenterView)) ).y();
        centerPoint		=	QPointF( double(xCenter-dx),double(yCenter-dy) );

        this->centerOn(centerPoint);
        viewRectFSet	=	true;
    }
    else
	{
		pix_1			=	MouseEvent2Pixel( event );
	}

	// 更新顯示資訊  到顯示畫面底下 座標資訊 RGB資訊
	updateMoveData( pix_1 );


	// 陰影檢測 畫方塊上去標示
	if( settings->set_square )
	{
		// 直接畫圖在image上面的範例
		QImage		image2		=	QImage( image.width(), image.height(), QImage::Format_RGB888 );

		// 用painter描點
		QPainter	painter;		
		painter.begin(&image2);
		painter.setPen( QPen(Qt::green, 1) );
		painter.drawImage( 0, 0, image );
		//painter.drawPoint( pix_1.x, pix_1.y );
		painter.drawRect( pix_1.x, pix_1.y, settings->square_size, settings->square_size );
		painter.end();

		// 將資料畫上去
		scene->removeItem( imPaint );
		delete	imPaint;
		imPaint	=	new	ImagePaint( image2 );
		scene->addItem( imPaint );

		// 更新畫面
		//resize();
		refreshView();	
	}


}


/************************************************************************************************
	滑鼠移動動作
/*************************************************************************************************/
void	PivDisplay::updateMoveData( PIXEL pixel )
{
	const int	x		=	pixel.x;
	const int	y		=	pixel.y;
	const QRgb	rgb		=	pixel.rgb;
	const int	r		=	qRed( rgb );
	const int	g		=	qGreen( rgb );
	const int	b		=	qBlue( rgb );

	const QString	str1	=	QString( "( %1, %2, %3 )" ).arg(r).arg(g).arg(b);
	const QString	str2	=	QString( "( %1, %2 )" ).arg(x).arg(y);

	positionLabel->setText( str2 );		// 更新到座標
	RGBLabel->setText( str1 );			// 更新到顏色
}



/************************************************************************************************
	目前無作用
/*************************************************************************************************/
void PivDisplay::resizeEvent(QResizeEvent *event)
{
    resize();
}


/************************************************************************************************
	resize
/*************************************************************************************************/
void PivDisplay::resize()
{
    this->fitInView(viewRectF, Qt::KeepAspectRatio);
    this->centerOn(centerPoint);
}


/************************************************************************************************
	目前無作用 
/*************************************************************************************************/
QPointF PivDisplay::getPoint1()
{
	QPointF	point1	=	QPointF( pix_1.fx, pix_1.fy );

    return	point1;
}

/************************************************************************************************
	目前無作用
/*************************************************************************************************/
QPointF PivDisplay::getPoint2()
{
	QPointF	point2	=	QPointF( pix_2.fx, pix_2.fy );

    return point2;
}


/************************************************************************************************
	更新顯示畫面
/*************************************************************************************************/
void PivDisplay::refreshView()
{
    this->setSceneRect(rectF);
    this->setScene( scene );
    if (viewRectFSet)
    {
        //this->fitInView(viewRectF, Qt::KeepAspectRatio);
        //this->centerOn(centerPoint);
    }
    else
    {
        //this->fitInView(rectF, Qt::KeepAspectRatio);
        //centerPoint = QPointF(rectF.width()/2,rectF.height()/2);
        viewRectFSet = true;
    }
    this->show();
}



/************************************************************************************************
	畫圖上去
/*************************************************************************************************/
void PivDisplay::displayImage( QImage imagePass )
{
	image			=	imagePass;
	rect			=	image.rect();
	rectF			=	QRectF( rect );
	viewRectF		=	rectF;
	viewRectFSet	=	true;
	
	// 把圖片放上去
	scene->removeItem( imPaint );						// 刪除順序要注意!!  不然會當機
	delete	imPaint;
	imPaint		=	new	ImagePaint(image);
	scene->addItem( imPaint );

	isSVG	=	false;

	// 更新畫面
	//resize();
	refreshView();
}





/*======================================================================================
	顯示SVG檔案
	檔案暫存在硬碟
	用檔名來呼叫
/*======================================================================================*/
void	PivDisplay::displaySVG( QString name )
{
    if (1/*imPaintAdded*/)
    {
        image	=	filedata->get_image();
        rect	=	image.rect();
        rectF	=	QRectF(rect);

        viewRectF = rectF;
        viewRectFSet = true;

		// 從檔案讀取svg 在把svg丟到mainwindows的顯示畫面上.
		//imPaint	=	new QGraphicsSvgItem(name);
		QGraphicsSvgItem	*imPaint2	=	new QGraphicsSvgItem(name);
		imPaint	=	new ImagePaint(image);

		scene->removeItem(imPaint);
        scene->addItem(imPaint);
		scene->removeItem( imPaint2 );
		scene->addItem( imPaint2 );

//        imPaintAdded = true;
    }
    else
    {
        image	=	filedata->get_image();
        rect	=	image.rect();
        rectF	=	QRectF(rect);
        if (viewRectF.width() == 0)
        {
            viewRectF = rectF;
            viewRectFSet = true;
        }

		// 從檔案讀取svg 在把svg丟到mainwindows的顯示畫面上.
		QGraphicsSvgItem	*imPaint2	=	new QGraphicsSvgItem(name);
		imPaint	=	 new ImagePaint(image);
		//QPixmap buffer(image.size());
		//QGraphicsPixmapItem item(buffer);
		/*QGraphicsBlurEffect effect;
		effect.setBlurHints(QGraphicsBlurEffect::QualityHint);
		effect.setBlurRadius(0);
		imPaint2->setGraphicsEffect(&effect);*/ 
		//item.pixmap().toImage().save("blur.png");
		//imPaint	=	new ImagePaint(*image);
        scene->addItem(imPaint);
        scene->addItem(imPaint2);
//        imPaintAdded = true;
    }

	isSVG	=	true;

	resize();
	refreshView();
}


/*==================================================================================================
	取得該點座標的 RGB 值
/*==================================================================================================*/
QRgb	PivDisplay::getRGB( int i, int j )
{
	QRgb	rgb;

	if( i < 0 || i >= image.width() || j < 0 || j >= image.height() )
		rgb		=	qRgb( 0, 0, 0 );
	else
		rgb		=	image.pixel( i, j );

	return	rgb;
}





/*==================================================================================================
	點擊滑鼠event
	會在畫面上面標記點 (預設用紅色)
	這邊改成標記檢測區的框框在圖片上 
/*==================================================================================================*/
void PivDisplay::mousePressEvent(QMouseEvent* event)
{
	pix_1	=	MouseEvent2Pixel( event );

    this->setCursor(Qt::ClosedHandCursor);
    mouseIsPressed = true;

	int		x	=	pix_1.x;
	int		y	=	pix_1.y;

	QRgb	rgb		=	getRGB( x, y );

	settings->setRGB( rgb );					// 紀錄該點的顏色資訊
	settings->set_measure_X( x );
	settings->set_measure_Y( y );

	PIXEL	pixel( x+1, y+1, pix_1.fx, pix_1.fy, rgb );
	pixel.square_size	=	settings->square_size-1;

	if( event->button() == Qt::LeftButton && settings->set_square )
		emit(mousePressed(pixel));					// 送出pixel資訊到mainwindow 再從mainwindow送到point list

	// 非svg才做mark point
	if( !isSVG && settings->set_square && event->button() == Qt::LeftButton )
		mark_point( x, y );

}



/*==================================================================================================
	在畫面上標記點
/*==================================================================================================*/
void	PivDisplay::mark_point( int x, int y )
{
	// 直接畫圖在image上面的範例
	QImage		image2		=	QImage( image.width(), image.height(), QImage::Format_RGB888 );

	// 用painter描點
	QPainter	painter;		
	painter.begin(&image2);
	painter.setPen( QPen(Qt::red, 1) );
	painter.drawImage( 0, 0, image );
	painter.drawPoint( x, y );
	painter.end();

    // 將資料畫上去
	scene->removeItem( imPaint );
	delete	imPaint;
	imPaint	=	new	ImagePaint( image2 );
	scene->addItem( imPaint );

	// 更新畫面
	//resize();
	refreshView();
}

/*==================================================================================================
	標記正方形區域
/*==================================================================================================*/
void	PivDisplay::mark_square( PIXEL pixel )
{
	// 直接畫圖在image上面的範例
	QImage		image2		=	QImage( image.width(), image.height(), QImage::Format_RGB888 );

	int		x	=	pixel.x;
	int		y	=	pixel.y;

	int		square_size	=	pixel.square_size;

	// 用painter描點
	QPainter	painter;		
	painter.begin(&image2);
	painter.setPen( QPen(Qt::blue, 1) );
	painter.drawImage( 0, 0, image );
	//painter.drawPoint( x, y );
	painter.drawRect( x-1, y-1, square_size+1, square_size+1 );
	painter.end();

    // 將資料畫上去
	scene->removeItem( imPaint );
	delete	imPaint;
	imPaint	=	new	ImagePaint( image2 );
	scene->addItem( imPaint );

	// 更新畫面
	//resize();
	refreshView();
}

