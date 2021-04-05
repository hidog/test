/*================================================================================================================================

	主視窗 底下的兩個list  
	目前是拿去放point list

/*================================================================================================================================*/

#ifndef IMAGELIST_H
#define IMAGELIST_H

#include <QObject>
#include <QVector>
#include <QTableWidget>
#include <QHeaderView>

#include "datacontainer.h"
#include "../tools.h"

class PointList : public QTableWidget
{
    Q_OBJECT

    private:

    public:
        PointList( QWidget* parent = 0 );
        ~PointList();
	
		void				updatePointList( const PIXEL pixel );							// 接收到資料 更新顯示在list上面
		void				clearData();													// 清除顯示資料
		void				removePointList( int row, int col );							// 移除選取的pointlist
		void				setupConnections();

    signals:
        void 				fileClicked(QString fileName, int rowSelected);
        void 				fileClicked(int rowSelected);
        void 				itemEntered(QTableWidgetItem* item);
};

#endif // IMAGELIST_H
