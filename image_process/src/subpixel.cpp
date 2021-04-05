#include "stdafx.h"

#include "subpixel.h"

using namespace std;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~這邊是 Edge 的function~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*=============================================================================================
	建構函數
/*=============================================================================================*/
#ifdef _OPENPIV_
Edge::Edge(  Settings *settingsPass, DataContainer *filedataPass  )
	:	AndreaEngine( )
{
	progress	=	settingsPass->getProgress();	
	settings	=	settingsPass;
	filedata	=	filedataPass;

    mutex.lock();

	X		=	settings->get_measure_X();
	Y		=	settings->get_measure_Y();

	N		=	1;			// window size   2*N+1

    mutex.unlock();
}
#else
Edge::Edge() {}
#endif


/*=============================================================================================
	解構函數
/*=============================================================================================*/
Edge::~Edge()
{}


/*=============================================================================================
	畫subpixel的點上去
/*=============================================================================================*/
void	Edge::draw_subpixel_point( double x, double y, double xx, double yy )
{
#ifdef _OPENPIV_
	QPointF		qf,	qf2;
	QString		path	=	"temp.svg";

	// 將bmp轉svg 範例
 
    QSvgGenerator	generator;
    generator.setFileName(path);

	generator.setSize( QSize(image.width(), image.height()) );
	generator.setViewBox( QRect(0, 0, image.width(), image.height()) );

    QPainter painter(&generator);
	painter.drawImage( QPoint(0,0), image  );

	painter.setPen( QPen(Qt::red, 0.02 ) );
	
	qf.setX( static_cast<qreal>(xx) );
	qf.setY( static_cast<qreal>(yy) );
	
	qf2.setX( static_cast<qreal>(x) );
	qf2.setY( static_cast<qreal>(y) );
	painter.drawLine( qf, qf2 );

    painter.begin(&generator);
    painter.end();
#endif
}


/*=============================================================================================
	畫subpixel的點上去
	bf = breakList  爆掉的點清單
/*=============================================================================================*/
void	Edge::draw_subpixel_point( QVector<QPointF> qf, QVector<QPointF> bf )
{
#ifdef _OPENPIV_
	QString		path	=	"temp.svg";
	QPointF		breaklist[1000];

	// 將bmp轉svg 範例
 
    QSvgGenerator	generator;
    generator.setFileName(path);

	generator.setSize( QSize(image.width(), image.height()) );
	generator.setViewBox( QRect(0, 0, image.width(), image.height()) );


    QPainter painter(&generator);
	painter.drawImage( QPoint(0,0), image  );

	// 設定風格
	painter.setPen( QPen(Qt::yellow, 1) );
	// 將資料存到陣列
	for( int i=0; i<bf.size(); i++ )
		breaklist[i]	=	bf.at(i);
	// 畫爆掉的點
	painter.drawPoints( breaklist, bf.size() );


	// 設定風格
	painter.setPen( QPen(Qt::red, 0.1 ) );
	// 開始畫線 
	painter.drawLines( qf );

    painter.begin(&generator);
    painter.end();
#endif

}

/*=============================================================================================
	內插用的多項式
	3*3  會使用  1 + x + y + xy + x^2 + y^2 + x^2y + xy^2 + x^2y^2 這樣的多項式
	跟facet model不太一樣
	dx, dy 是 偏移量
/*=============================================================================================*/
double	Edge::ite_poly( int dx, int dy, double x, double y, double xx, double yy )
{
	double	r	=	1;
	//double	xx	=	X + 0.5;
	//double	yy	=	Y + 0.5;

	int		i,	j;


	// 底下做法是為了產生上面給的那個model
	for( i=-N; i<=N; i++ )
	{
		if( i != dx )
		{
			r	*=	( x - xx - i );
			r	/=	( i - dx );
		}
	}

	for( j=-N; j<=N; j++ )
	{
		if( j != dy )
		{
			r	*=	( y - yy - j );
			r	/=	( j - dy );
		}
	}

	return	r;

}


/*=============================================================================================
	測試產生SVG  (使用QWT)
/*=============================================================================================*/
void	Edge::testSVG()
{
	QString			path("QWT.svg");
	QSvgGenerator	generator;
	QImage			img		=	image;

	generator.setFileName(path);
	generator.setSize( QSize(img.width(), img.height()) );
	generator.setViewBox( QRect(0, 0, img.width(), img.height()) );
	generator.setTitle( "SVG Generator Example Drawing" );
	generator.setDescription( "An SVG drawing created by the SVG Generator Example provided with Qt." );

	//generator.setResolution(10);

	QPainter	painter( &generator );
	QwtPainter	qwtPainter;

	QRect		cf( 0,0, img.width(),img.height() );

	qwtPainter.drawImage( &painter, cf, img );

	QwtScaleMap		xMap;
    QwtScaleMap		yMap;
	xMap.setScaleInterval( 0, img.width() );
	yMap.setScaleInterval( 0, img.height() );


	/*QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->setMajPen( QPen( Qt::white, 0, Qt::DotLine ) );
    grid->setMinPen( QPen( Qt::gray, 0 , Qt::DotLine ) );
	grid->draw( &painter, xMap, yMap, cf );*/




	QwtPlotCurve	curve;
	int				Size	=	0,	a;
	double			xval[1000],	yval[1000];

	ifstream		in("subpix.txt");
	while( (in >> a) != NULL )
	{
		in >> xval[Size] >> yval[Size];
		Size++;
	}
	in.close();



	curve.setPen( QPen( QColor( 255, 0, 0 ), 0.02 ) );
    curve.setStyle( QwtPlotCurve::Lines );
	curve.setCurveAttribute( QwtPlotCurve::Fitted, true );
	
	QwtSplineCurveFitter* curveFitter;
	curveFitter = new QwtSplineCurveFitter();
    curveFitter->setSplineSize( 200 );
    
	curve.setCurveFitter( curveFitter );
	
    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::XCross );
    symbol->setPen( QPen( Qt::yellow, 0.1 ) );
    symbol->setSize( 0.1 );
    curve.setSymbol( symbol );
    //const QRect cr = contentsRect();

	curve.setRawSamples( xval, yval, Size );

	//QwtPlotItem::Rtti_PlotGrid

	xMap.setPaintInterval( cf.left(), cf.right() );
    yMap.setPaintInterval( cf.top(), cf.bottom() );

	//curve.setRenderHint( (QwtPlotItem::RenderHint)1, false );


	curve.draw( &painter, xMap, yMap, cf );



	painter.begin(&generator);
	//painter.setBrush( Qt::SolidPattern );

	painter.end();

}


/*=============================================================================================
	提取edge 
	這邊採用的是沿著edge方向去搜尋的策略

	利用gradient去決定edge的方向,沿著方向找出點
	再把這些點link成edge

	lu, rd 是給定範圍
/*=============================================================================================*/
QVector<QPointF>	Edge::extract_edge( PointInt lu, PointInt rd )
{
	int				i,		j;
	int				max_i,	max_j;
	const double	threshold	=	10000000;
	double			max,	min;
	double			dx,		dy;
	double			dtmp1,	dtmp2;
	double			normal;
	QImage			img;
	PointInt		start( X, Y );				// edge的搜尋起點
	//PointInt		start( 770, 55 );
	//PointInt		start( 785, 263 );
	bMatrixDouble	corner;

	QVector<QPointF>	outputList;
	outputList.clear();

	// 先搜尋corner強的點
	Njet	njet(image);
	/*corner	=	njet.pix_corner( lu, rd );
	img		=	Matrix2Image( corner );			// 轉成圖檔 方便測試

	// non maximal suppression   搜尋出local maxima  (實際上還是會有重複觸發 要加threshold
	bMatrixInt		lm	=	local_max( corner, WINDOW(3,3), threshold );*/

	// 底下是輸出 方便測試
	/*for( i = 0;  i < lm.size1(); i++ )
		for( j = 0; j < lm.size2(); j++ )
		{
			if( lm(i,j) == 1 )
				img.pixel_channel( i, j, 255, 0, 0 );
		}
	img.save("corner.bmp");*/


	// 產生curvature
	curvature.resize( image.width(), image.height() );
	curvature.clear();
	for( i=0; i<image.width(); i++ )
		for( j=0; j<image.height(); j++ )
			curvature( i, j )	=	njet.get_cur( i, j );
	/*max		=	0;
	min		=	12345678;
	for( i = 0;  i < image.width();  i++ )
		for( j = 0;  j < image.height();  j++ )
		{
			if( max < curvature( i, j ) )
				max		=	curvature(i,j);
			if( min > curvature( i, j ) )
				min		=	curvature(i,j);
		}
	for( i=0; i<image.width(); i++ )
		for( j=0; j<image.height(); j++ )
		{
			double	mag	=	(255.0)*(curvature(i,j)-min) / (max-min);
			curvature(i,j)	=	mag;
		}
	img	=	Matrix2Image(curvature);
	img.save("curvature.bmp");*/


	
	// 從某個起點開始  沿著邊的方向  去找出邊
	ofstream		out("subpix.txt");

	PointDouble		now( start.x + 0.5, start.y + 0.5 );
	//PointInt		pre		=	now;
	PointInt		step;
	//PointDouble		direction;
	PointDouble		delta;
	int				itr		=	0;
	while(1)
	{
		img.pixel_channel( now.x, now.y, 255, 0, 0 );

		i		=	now.x;
		j		=	now.y;

		// 先計算direction
		dtmp1	=	0.5 * ( image.gray_channel( i+1, j ) - image.gray_channel( i-1, j ) );
		dtmp2	=	0.5 * ( image.gray_channel( i, j+1 ) - image.gray_channel( i, j-1 ) );

		//dtmp1	=	image.gray_channel( i+1,j-1 ) - image.gray_channel( i-1, j-1 )  +  image.gray_channel( i+1, j+1) - image.gray_channel( i-1, j+1 )  +  5.0*(image.gray_channel(i+1,j) - image.gray_channel(i-1,j) );
		//dtmp2	=	image.gray_channel( i+1,j+1 ) - image.gray_channel( i+1, j-1 )  +  image.gray_channel( i-1, j+1) - image.gray_channel( i-1, j-1 )  +  5.0*(image.gray_channel(i,j+1) - image.gray_channel(i,j-1) );

		dx		=	dtmp2;
		dy		=	-dtmp1;
		normal	=	sqrt( dx*dx + dy*dy + 0.00001);
		dx		/=	normal;
		dy		/=	normal;
		dtmp1	/=	normal;
		dtmp2	/=	normal;

		if( dtmp1 == 0 && dtmp2 == 0 )
			dtmp1	=	0.5;

		delta	=	itepolation( now.x, now.y, dtmp1, dtmp2 );

		out << itr << " " << delta.x << " " << delta.y << endl;

		outputList.append( QPointF(delta.x, delta.y)  );

		// 下一個點
		now.x	=	delta.x + dx;
		now.y	=	delta.y + dy;

		itr++;
		if( itr >= 1000 )
			break;

	}

	out.close();

	img.save("edge.bmp");

	return	outputList;
}



/*=============================================================================================
	測試用 
	使用過點的內插法 
	去內插curvature 
	再去搜尋max當成edge subpixel
	dir_x, dir_y 是方向  (edge的方向)
/*=============================================================================================*/
PointDouble		Edge::itepolation( int _x, int _y, double dir_x, double dir_y )
{
	double		rtx,	rty;
	double		xx	=	_x + 0.5;		// 4.5這樣的 浮點數中心點座標
	double		yy	=	_y + 0.5;
	double		sum	=	0;
	double		x,	y,	t;
	int			i,	j;

	// 進行內插  並且找出最大值
	double		max		=	0;
	double		max_x	=	-1;
	double		max_y	=	-1;


	// 搜尋最大值
	for( t = 0; ; t += 0.01 )
	{
		x	=	t*dir_x;
		y	=	t*dir_y;

		// 超出邊界
		if( fabs(x) > 1.0 /*+ N*/ || fabs(y) > 1.0 /*+ N*/ )	break;

		sum		=	0;
		for( i = -N;  i <= N;  i++ )
			for( j = -N;  j <= N;  j++ )
				sum		+=	curvature( i + (int)xx, j + (int)yy ) * ite_poly( i, j, x + xx, y + yy, xx, yy  );

		if( sum > max )
		{
			max		=	sum;
			max_x	=	x + xx;
			max_y	=	y + yy ;
		}
	}

	// 反向再一次
	for( t = 0; ; t -= 0.01 )
	{
		x	=	t*dir_x;
		y	=	t*dir_y;

		// 超出邊界
		if( fabs(x) > 1.0 /*+ N*/ || fabs(y) > 1.0 /*+ N*/ )	break;

		sum		=	0;
		for( i = -N;  i <= N;  i++ )
			for( j = -N;  j <= N;  j++ )
				sum		+=	curvature( i + (int)xx, j + (int)yy ) * ite_poly( i, j, x + xx, y + yy, xx, yy );

		if( sum > max )
		{
			max		=	sum;
			max_x	=	x + xx;
			max_y	=	y + yy ;
		}
	
	}

	if(max_x == -1 && max_y == -1 )
	{
		max_x	=	xx;
		max_y	=	yy;
	}



	return	PointDouble( max_x, max_y );

}

/*=============================================================================================
	OPENPIV的程式進入點
/*=============================================================================================*/
void	Edge::dosomething()
{
#ifdef _OPENPIV_
	progress->setTabText( 2, "vector pic" );

	image	=	filedata->get_image();

	//PointInt	lu( 1009, 82 ),	rd( 1095, 155 );
	PointInt	lu( 1, 1 ),	rd( image.width()-1, image.height()-1 );

	X		=	settings->get_measure_X();
	Y		=	settings->get_measure_Y();


	QVector<QPointF>	outputList;

	outputList	=	extract_edge( lu, rd );

	//testSVG();		// 測試產生SVG  使用QWT

	//itepolation();
	//return;

// 判斷是處理單點  還是PointList
#ifdef _SINGLE_

	double		rtx,	rty;
	
	double	xx	=	X + 0.5;		// 4.5這樣的 浮點數中心點座標
	double	yy	=	Y + 0.5;

	// 初始動作
	image	=	filedata->get_image();

	// 利用facet物件來處理subpixel
	Facet	facet(3);

	facet.edge( (0.1+X), (0.1+Y), image, rtx, rty );		// 為了避免浮點數造成錯誤

	draw_subpixel_point( rtx, rty, xx, yy );
	
#ifdef _OPENPIV_
	progress->debuger( QString("subpixel edge = %1, %2").arg( rtx ).arg(rty ), QColor(24,0,240) );
#endif

#else
	int		i,		j;
	int		tmp;
	int		x,		y;
	int		count	=	0;
	double	rtx,	rty;

	// 初始動作

	image	=	filedata->get_image();


	// 利用facet物件來處理subpixel
	Facet	facet(3);

	QVector<QPointF>::iterator		itr;

	//QVector<QPointF>				PointList	=	filedata->getPointList();

	QVector<PIXEL>					pixel	=	filedata->getPixelList();
	QVector<QPointF>				PointList;

	for( i=0; i<pixel.size(); i++ )
		PointList.append( QPointF(pixel[i].fx, pixel[i].fy) );






	QVector<QPointF>				/*outputList,*/	pairList;
	QVector<QPointF>				breakList;						// 爆掉的點清單
	QPointF							qf,	qf2;


	//outputList.clear();
	pairList.clear();
	breakList.clear();

	// 進行subpixel的計算
	for( itr = PointList.begin();        itr != PointList.end();         itr++ )
	{
		break;

		progress->setGress( (count++)%100 );

		qf	=	*itr;
		x	=	floor( qf.x() );
		y	=	floor( qf.y() );

		if( facet.edge( (0.1+x), (0.1+y), image, rtx, rty ) )		// 為了避免浮點數造成錯誤
			outputList.append(   QPointF( rtx, rty ) );
		else
		{
			breakList.append( QPointF( x, y ) );
			outputList.append( QPointF(-1. , -1.) );
		}
	}

	// 整理點 產出pair
	pairList.clear();
	for( i=0; i<outputList.size()-1; i++ )
	{
		qf	=	outputList.at(i);
		if( qf.x() >= 0 && qf.y() >= 0 )
			pairList.append( qf );
		else
		{	
			qf2	=	QPointF( floor(PointList.at(i).x()) + 0.5, floor(PointList.at(i).y()) + 0.5 );
			pairList.append( qf2 );
		}

		qf	=	outputList.at(i+1);
		if( qf.x() >= 0 && qf.y() >= 0 )
			pairList.append( qf );
		else
		{	
			qf2	=	QPointF( floor(PointList.at(i+1).x()) + 0.5, floor(PointList.at(i+1).y()) + 0.5 );
			pairList.append( qf2 );
		}
	}

	// 畫出資料
	draw_subpixel_point( pairList , breakList );
	filedata->setOutputList( outputList );				// 資料用這個方式存到filedata
	progress->outputSubpixelEdge( outputList );			// 用這邊做畫面更新  要留意是否已經把資料更新上去了  如果會交給不同thread要留意同步問題.
	

	// 清除資料
	PointList.clear();
	outputList.clear();
	pairList.clear();

#endif
	
	progress->debuger("<font color=\"Red\"> finish Subpixel Edge detector... </font>");

	//emit( filedata->output3(image) );
	emit( filedata->output_svg("temp.svg"));

#endif		// _OPENPIV_
}











//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~這邊是 Corner 的function~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



/*=============================================================================================
	建構函數
/*=============================================================================================*/
#ifdef _OPENPIV_
Corner::Corner(  Settings *settingsPass, DataContainer *filedataPass  )
	:	AndreaEngine()
{
	progress	=	settingsPass->getProgress();	
	settings	=	settingsPass;
	filedata	=	filedataPass;

    mutex.lock();

	X		=	settings->get_measure_X();
	Y		=	settings->get_measure_Y();

    mutex.unlock();
}
#else
Corner::Corner() {}
#endif


/*=============================================================================================
	解構函數
/*=============================================================================================*/
Corner::~Corner()
{}


/*=============================================================================================
	畫subpixel的點上去
/*=============================================================================================*/
void	Corner::draw_subpixel_point( double x, double y, double xx, double yy )
{
#ifdef _OPENPIV_
	QPointF		qf,	qf2;
	QString		path	=	"temp.svg";

	// 將bmp轉svg 範例
 
    QSvgGenerator	generator;
    generator.setFileName(path);

	generator.setSize( QSize(image.width(), image.height()) );
	generator.setViewBox( QRect(0, 0, image.width(), image.height()) );

    QPainter painter(&generator);
	painter.drawImage( QPoint(0,0), image  );

	painter.setPen( QPen(Qt::red, 0.1 ) );
	
	qf.setX( static_cast<qreal>(xx) );
	qf.setY( static_cast<qreal>(yy) );
	
	qf2.setX( static_cast<qreal>(x) );
	qf2.setY( static_cast<qreal>(y) );
	painter.drawLine( qf, qf2 );

    painter.begin(&generator);
    painter.end();
#endif
}


/*=============================================================================================
	畫subpixel的點上去
/*=============================================================================================*/
void	Corner::draw_subpixel_point( QVector<QPointF> qf )
{
#ifdef _OPENPIV_
	QString		path	=	"temp.svg";

	// 將bmp轉svg 範例
 
    QSvgGenerator	generator;
    generator.setFileName(path);

	generator.setSize( QSize(image.width(), image.height()) );
	generator.setViewBox( QRect(0, 0, image.width(), image.height()) );


    QPainter painter(&generator);
	painter.drawImage( QPoint(0,0), image  );

	// 設定風格
	painter.setPen( QPen(Qt::red, 0.1 ) );

	// 開始畫線 
	painter.drawLines( qf );

    painter.begin(&generator);
    painter.end();

#endif
}




/*=============================================================================================
	OPENPIV的程式進入點
/*=============================================================================================*/
void	Corner::dosomething()
{
#ifdef _OPENPIV_
	progress->setTabText( 2, "vector pic" );

	// 初始動作
	image	=	filedata->get_image();

// 判斷要不要單點計算 還是多點
#if 1
	if( X < 0 || X >= image.width() )		X	=	0;			// 簡單的防呆
	if( Y < 0 || Y >= image.height() )		Y	=	0;

	// 利用 njet 來做 sub-pixel 的 corner 計算
	Njet	njet(image);

	double	dx,	dy;
	
	// openCV的code
	cornerSubPix( image, 0.00000000000000001 + X, 0.0000000000000000001 + Y, dx, dy );

	//int		ii	=	njet.get_corner( X, Y, dx, dy );

	int		ii;
	if( fabs(dx) > 0.5 || fabs(dy) > 0.5 )
		ii	=	-1;
	else
		ii	=	0;


	if( ii == -1 )
		progress->debuger( QString("out of point!!").arg(dx).arg(dy) );
	else if( ii == 0 )
		progress->debuger( QString("over max iter!!").arg(dx).arg(dy) );

	//progress->debuger( QString("Subpixel Corner = ( %1 , %2 )").arg(dx+X).arg(dy+Y) , QColor(33,9,253) );
	progress->debuger( QString("Subpixel Corner (delta) = ( %1 , %2 )").arg(dx).arg(dy) , QColor(33,9,253) );
	progress->debuger( QString("finish Subpixel Corner detector...") , QColor(255,0,0) );

	draw_subpixel_point( dx, dy, 0.5+X, 0.5+Y );
#else
	int		i,		j;
	int		tmp;
	int		x,		y;
	int		count	=	0;
	double	rtx,	rty;
	double	ip_x,	ip_y;

	// 利用njet 物件來處理subpixel
	Njet	njet(image);

	QVector<QPointF>::iterator		itr;
	QVector<QPointF>				PointList	=	filedata->getPointList();
	QVector<QPointF>				outputList,	pairList;
	QVector<QPointF>				breakList;						// 爆掉的點清單
	QPointF							qf,	qf2;

	outputList.clear();
	pairList.clear();
	breakList.clear();

	// 進行subpixel的計算
	for( itr = PointList.begin();        itr != PointList.end();         itr++ )
	{
		progress->setGress( (count++)%100 );

		qf	=	*itr;
		x	=	floor( qf.x() );
		y	=	floor( qf.y() );

		if( njet.get_corner( x, y, rtx, rty ) == 1 )
			outputList.append( QPointF( rtx, rty ) );
		else
		{
			breakList.append( QPointF( x, y ) );
			outputList.append( QPointF(-1. , -1.) );
		}
	}

	// 整理點 產出pair
	pairList.clear();
	for( i=0; i<outputList.size()-1; i++ )
	{
		qf	=	outputList.at(i);
		if( qf.x() >= 0 && qf.y() >= 0 )
		{
			pairList.append( qf );
			ip_x	=	floor(qf.x()) + 0.5;
			ip_y	=	floor(qf.y()) + 0.5;
			pairList.append( qf );
		}

	}

	// 畫出資料
	draw_subpixel_point( pairList );
	filedata->setOutputList( outputList );				// 資料用這個方式存到filedata
	progress->outputSubpixelEdge( outputList );			// 用這邊做畫面更新  要留意是否已經把資料更新上去了  如果會交給不同thread要留意同步問題.
	

	// 清除資料
	PointList.clear();
	outputList.clear();
	pairList.clear();



#endif


	// 把圖丟去顯示
	//emit( filedata->output_svg("temp.svg"));


#endif			// _OPENPIV_
}