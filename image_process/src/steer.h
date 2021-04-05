#ifndef _STEER_H
#define _STEER_H

#include <boost/numeric/ublas/matrix.hpp>
#include <fstream>

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


using namespace std;

// 定義boost的 double matrix
typedef boost::numeric::ublas::matrix<double> boost_matrix;

// 定義boost的 double vector
typedef boost::numeric::ublas::vector<double> boost_vector;





#ifdef _OPENPIV_
class Steer : public AndreaEngine
#else
class Steer
#endif
{
	private:
		int						X,	Y;									// 對此座標去做運算
		int						N;										// 表示目前是第 N 階
		int						g_size;									// g_map 的 size.    利用 2*g_size +1  = 實際size  的作法
		int						o_size;									// g_map原始的size 之後再down sample到 g_size
		static const int		MAX_THETA	=	100;
		double					pi;
		double					*w;										// weight

		QImage					image;
		QImage					polar;

		boost_matrix			g_map;									// g(r)   radial function
		boost_matrix			he[MAX_THETA];							// even part
		boost_matrix			ho[MAX_THETA];							// odd part
		boost_matrix			E[MAX_THETA];							// energy

		boost_vector			theta_data;								// 拿來做顯示用的資料 polar 的 theta
		boost_vector			value_data;								// 拿來做顯示用的資料 polar 的 value

		double					exp(int);								// 內部使用的exp  可以傳入整數值
		double					SMAX( double a, double b );				// 回傳最大值


#ifdef _OPENPIV_
		DataContainer			*filedata;
		Settings				*settings;

		QMutex					mutex;

		// 負責回報進度用的
		Progress				*progress;

	signals:
		void					debugSignal( QString );				// 除錯訊息
#endif

	public:
#ifdef _OPENPIV_
		Steer(  Settings *settingsPass, DataContainer *filedataPass  );							// 建構函數
#else
		Steer();																				// 建構函數
#endif

		// 解構
		virtual			~Steer(); 
	
		boost_matrix	downsample( boost_matrix map );											// down sample 動作在這邊

		void			output_map( boost::numeric::ublas::matrix<double> map );				// 丟入map資料 印出圖形 (自動normalized)
		void			output_map( boost::numeric::ublas::matrix<double> map, char *name );	// 丟入map資料 印出圖形 (自動normalized)  指定檔名

		double			g_radial( double r );													// radial 函數
		void			generate_map();															// 產生 g_map

		double			get_Energy( int ii, int x, int y );										// 取得該點的 energy

		void			draw_filter( int ii );													// 測試用 畫filter
		void			polar_draw();															// 用產生好的資料畫出polar圖形

		void			local_max( double &L, double &T, double &X );							// 尋找local max
	
#ifdef _OPENPIV_
		void			dosomething();															// OPENPIV的程式進入點
#endif
	
		//double&		get_g_map( int i, int j );												// 取得 g_map   (必須做到能讀取跟寫入)
};


























#endif