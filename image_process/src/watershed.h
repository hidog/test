#ifndef _WATERSHED_H
#define _WATERSHED_H

// INT_MAX包含在這邊
#include <limits.h>

#include <boost/pending/queue.hpp>
#include <boost/heap/priority_queue.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include "mser_process.h"
#include "../MEASURE/FFT.h"
#include "gaussfilter.h"

#include <QImage>
#include <QPoint>
#include <QList>
#include <QMutex>


#ifdef _OPENPIV_

#include "../src/andreaengine.h"
#include "../src/settings.h"
#include "../src/progress.h"
#include "../src/datacontainer.h"

#endif




struct	POINT_W
{
	int		x;	
	int		y;
	POINT_W( int _x = 0, int _y = 0 ) : x(_x), y(_y) {};

	//   = operator
	POINT_W	operator	=	( POINT_W b );
};

//   != operator
bool	operator	!=	( POINT_W a, POINT_W b );

//   == operator
bool	operator	==	( POINT_W a, POINT_W b );



/*===================================================================================================
	Watershed的class
/*===================================================================================================*/
#ifdef _OPENPIV_
class	Watershed : public AndreaEngine
#else
class	Watershed
#endif
{
	private:

		int		_intLengthX, _intLengthY;

#ifdef _OPENPIV_
		DataContainer	*filedata;
		Settings		*settings;

		QMutex		mutex;

		// 負責回報進度用的
		Progress	*progress;
#endif

		QImage		image;
		QImage		final;


		// 存放 lower complete的資料 (因為會超過255)
		bMatrixInt		lc;

		// 存放label資料
		bMatrixInt		lab;

		// 存放dist資料
		bMatrixInt		dist;

		// 存放sort後的點資料
		bVectorInt		sort_data;

		// 存放sln的資料
		boost::numeric::ublas::matrix<POINT_W>	sln;

		// 是否有比自己小的鄰居
		bool	has_low_neib( int i, int j );
		
		// 用在4.1
		bool	has_low_neib_2( QImage *image, int i, int j );

		// NG 求有哪些鄰居
		void	NG( POINT_W p, POINT_W *pointlist, int &size, int width, int height );

		// 遞迴使用的 Resolve
		POINT_W	Resolve( POINT_W p );

		// 建立 sln 的 function
		void	sln_maker();

#ifdef _OPENPIV_
		
	signals:
		void	debugSignal( QString );				// 除錯訊息

	protected:

#endif



	public:

#ifdef _OPENPIV_
		// 建構
		Watershed( Settings *settingsPass, DataContainer *filedataPass );
#else
		// 建構 無傳入值
		Watershed();
#endif

		// 解構
		virtual		~Watershed();   

		// 都利用這個funtion來做事情. 取代原本的main
		void		dosomething();

		// lower completion
		void	LowerCompletion();

		// minima detetor
		void	LevelComponents();

		// 執行watershed
		void	doWatershed();

		// 最原始的watershed   alg 4.1
		void	doWatershed_4_1();

		// 反白
		void	highlight();

		// 輸出
		void	output();

		// 產生gradient image    sqrt( fx^2 + fy^2 )
		QImage	gradient();

		// 將圖形做truncate   i+-5 都變成 i 之類的
		QImage	truncate( int n );

		// 將圖形切開來  色標用
		QImage	divide( QImage	imagePass );
};



#endif