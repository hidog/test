#ifndef _FFT_H
#define _FFT_H



#include <fftw3.h>
#include <boost/numeric/ublas/matrix.hpp>

#include "../tools.h"

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


typedef boost::numeric::ublas::matrix<double>	DoubleMatrix;
typedef boost::numeric::ublas::matrix<int>	IntMatrix;



// FFT物件
#ifdef _OPENPIV_
class	FFT : public AndreaEngine
#else
class	FFT
#endif
{
	private:
		DoubleMatrix		R,	I;			// FFT轉換後會產生的 Real part,  Image part

		QImage				image;			// 原始圖形
		QImage				junction;		// 得到的junction  edge  boundary
		QImage				edge;
		QImage				boundray;

		DoubleMatrix		filter_cos;		// 兩個filter
		DoubleMatrix		filter_sin;

		int					w,	h,	N;		// 圖片的長寬 還有大小
		double				sigma;			// scale
		double				pi;
		
#ifdef _OPENPIV_
		DataContainer	*filedata;
		Settings		*settings;

		QMutex		mutex;

		// 負責回報進度用的
		Progress	*progress;

	signals:
		void	debugSignal( QString );				// 除錯訊息

	protected:
		/*bool	crossCorrelate(int topLeftRow, int topLeftColumn);
		void	prepareFFT();
		void	fftCore(ImageData *_imageA, ImageData *_imageB, int topLeftRow, int topLeftColumn);*/
#endif


	public:
#ifdef _OPENPIV_
		// 建構
		FFT( Settings *settingsPass, DataContainer *filedataPass );

		// 都利用這個funtion來做事情. 取代原本的main
		void		dosomething();
#else
		// 建構 無傳入值
		FFT();
#endif

		// 解構
		virtual		~FFT();   

		// 將圖片做FFT轉換
		void	c2_FFT();

		// 丟入陣列 輸出
		void	output( DoubleMatrix out );
		
		// 丟入陣列 輸出  指定檔名
		void	output_map( DoubleMatrix out, char *str );

		// 輸出 Real part
		void	output_Real();

		// 輸出 Image part
		void	output_Image();

		// FFT逆轉換
		void	c2_IFFT();

		// 測試用 做某些動作
		void	doing();

		// 測試用 做某些動作
		void	doing2();

		// 產生filter  (可以有很多不同的濾波器,到時候按照編號來給)   分成 real part (cos)   image part (sin)
		void	filter( int n, boost::numeric::ublas::matrix<double> &FR,  boost::numeric::ublas::matrix<double> &FI );

		// 執行filter的動作
		void	do_filter( int n,  boost::numeric::ublas::matrix<double> FR, boost::numeric::ublas::matrix<double> FI, boost::numeric::ublas::matrix<double> &C1, boost::numeric::ublas::matrix<double> &C2 );

		// 逆轉換回來
		boost::numeric::ublas::matrix<double>	inverse( boost::numeric::ublas::matrix<double> tR, boost::numeric::ublas::matrix<double> tI );

		// 用極座標的方式來產生filter
		void	draw_filter( int n );

		// radial function
		double	Kp( double r );

		// 內插polynomial  (會通過點的)   用來找junction  會自動搜尋出最大值
		void	itepolation( int _x, int _y );

		// 內插polynomail的多項式部分
		double	ite_poly( int dx, int dy, double x, double y, double xx, double yy );

		// subpixel junction  會在這邊搜尋 junction的max  然後再去做計算
		void	subpixel_junction( PointInt lu, PointInt rd );


};




#endif