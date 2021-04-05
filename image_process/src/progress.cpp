

#include "progress.h"



Progress::Progress() {}

Progress::~Progress() {}



/*===================================================================
	設定進度條的值
/*===================================================================*/
void	Progress::setGress( int value )
{
#ifndef _GTEST_
	emit( signalsGress(value) );
#endif
}



/*===================================================================
	設定主畫面 tab 上的文字說明
/*===================================================================*/
void		Progress::setTabText( int index, QString str )
{
#ifndef _GTEST_
	emit( signalTabText(index, str) );
#endif
}

/*===================================================================
	輸出edge subpixel的資料
/*===================================================================*/
void		Progress::outputSubpixelEdge( QVector<QPointF> qf )
{
#ifndef _GTEST_
	emit( updateWidgeB() );
#endif
}


/*===================================================================
	顯示訊息到主畫面的debug_text上面
/*===================================================================*/
void		Progress::debuger( QString str )
{
#ifndef _GTEST_
	emit( debugSignal(str) );
#endif
}


/*===================================================================
	顯示訊息到主畫面的debug_text上面
/*===================================================================*/
void	Progress::debuger( QString str, QColor color  )
{
	int	r	=	color.red();
	int	g	=	color.green();
	int	b	=	color.blue();

	char	tmp[100];

	sprintf( tmp, "\"#%02x%02x%02x\"", r, g, b);

	//str		=	"<font color = " + tmp + " > " + str + "</font>";
	QString		str2	=	"<font color=";
	str2.append( tmp );
	str2	+=	" > " + str + "</font>";

#ifndef _GTEST_
	emit( debugSignal(str2) );
#endif
	//progress->debuger("<font color=\"Red\"> finish subpixel blob detector... </font>");
}

/*===================================================================
	輸出image到顯示視窗
/*===================================================================*/
void	Progress::outputImage( int index, QImage image )
{
#ifndef _GTEST_
	emit( signalImage( index, image ) );
#endif
}




