#include "pointlist.h"


/***************************************************************************************************
	建構
/****************************************************************************************************/
PointList::PointList(QWidget* parent) 
	: QTableWidget(parent)
{
    setupConnections();
}


/***************************************************************************************************
	解構
/****************************************************************************************************/
PointList::~PointList()
{}



/***************************************************************************************************
	設定底下物件的connect  目前無作用
/****************************************************************************************************/
void PointList::setupConnections()
{}





/*========================================================================================
	清除顯示資料
/*========================================================================================*/
void	PointList::clearData()
{
	this->clearContents();
	this->clear();
	setColumnCount(0);		// 將 row  col 歸零
	setRowCount(0);
}





/*========================================================================================
	移除選取的point
/*========================================================================================*/
void	PointList::removePointList( int row, int col )
{
	if( row >= rowCount() )
		return;

	//row + col;
	this->removeRow( row );

	if( rowCount() == 0 )
		clearData();					// 清空資料

	// 更新
	this->resizeColumnsToContents();
}


/*========================================================================================
	接收到資料 
	更新顯示在list上面
/*========================================================================================*/
void	PointList::updatePointList( const PIXEL pixel )
{
	QTableWidgetItem		*pointItem;
	QTableWidgetItem		*rgbItem;

	int		row		=	rowCount();
	int		col		=	2;
	int		x		=	pixel.x;
	int		y		=	pixel.y;

	QRgb	rgb		=	pixel.rgb;
	int		r		=	qRed( rgb );
	int		g		=	qGreen( rgb );
	int		b		=	qBlue( rgb );

	// 增加欄寬
	this->setColumnCount( col );
	this->setRowCount( row + 1 );

	// 產生要顯示的文字
	pointItem	=	new	QTableWidgetItem( QString("( %1 , %2 )").arg(x).arg(y) );
	//rgbItem		=	new	QTableWidgetItem( QString("( %1 , %2 , %3 )").arg(r).arg(g).arg(b) );
	rgbItem		=	new QTableWidgetItem( QString("size = %1").arg(pixel.square_size) );

	// 塞資料到point list
	this->setItem( row , 0, pointItem );
	this->setItem( row , 1, rgbItem );




	//QComboBox		*ttt	=	new	QComboBox;
	//this->setItem( row, 2, ttt );



	// 更新畫面
	this->resizeRowsToContents();
	this->resizeColumnsToContents();
}
