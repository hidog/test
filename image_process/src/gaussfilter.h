#ifndef _AMATH_H
#define _AMATH_H

#include <vector>
#include <cmath>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/math/distributions/normal.hpp>

#include "../tools.h"

#include <QImage>
#include <QColor>
#include <QProgressBar>

#include "../src/Progress.h"
#include <fstream>
#include <iostream>


using namespace std;





/*======================================================================================
	typedef區
========================================================================================*/


/*======================================================================================
	struct區
========================================================================================*/




/*======================================================================================
	class區
========================================================================================*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    用來做Gauss Filter
	未來可能會擴充成做各種filter
	這個是 1D 的filter,未來希望用兩個1D做成一個2D的filter
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
class	GaussFilter_1D
{

	private:
		int					scale;					// 測量尺度 會影響mean deviation

		double				mean;					// 平均
		double				deviation;				// 標準差

		vector<double>		filter_map;				// filter的矩陣
		vector<double>		DoG;					// Laplacian of Gaussian的矩陣

		void				generate_map();			// 產生filter map


	public:
		// 建構函式
		GaussFilter_1D( int _scale );

		// 取 map 值用, 會自動將index 轉成 以 0 為中心
		double&		operator [] ( int index );

		// 取 map 值用, 會自動將index 轉成 以 0 為中心
		double&		operator () ( int index );

		// 產生filter map  固定 k = 2
		void	generate_DoG_map();		

		// 取出Dog的Map
		double	get_DoG( int index );
	

};


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	利用兩個 Gauss filter 1D 來做 2D 的filter
	未來會考慮可以處理 w != l 的情況
	目前先做 scale = max(w,l) 的情況.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
class	GaussFilter_2D
{
	private:
		int				w,	l;										// 兩個方向的尺度

		double			mean_w,	mean_l;								// 平均值  同樣區分成兩個方向  跟均一平均值
		double			mean;

		double			deviation_w;								// 標準差
		double			deviation_l;
		double			deviation;

		double			normalized;									// normalized係數

		bVectorDouble	filter_map_w;								// 橫向的filter
		bVectorDouble	filter_map_l;								// 縱向的filter
		bMatrixDouble	filter_map;									// filter的矩陣
		bMatrixDouble	DOG;										// DoG的矩陣  用大寫是為了避免跟1D搞混

	public:
		// 建構函式
		GaussFilter_2D();
		GaussFilter_2D( int _w, int _l );

		// 產生filter map
		void	generate_map();

		// 取 map 值用, 會自動將index 轉成 以 0 為中心
		// 未來看是否能改成 [] 的方式. (因為boost的matrix沒提供這樣的作法...得自己寫)
		//double&		get_map ( int i, int j );
		double&		operator	()	( int i, int j );

#ifdef _OPENPIV_
		// 將圖檔讀進來, 做filter的動作
		QImage	do_filter( QImage passImage, Progress *progress );
#else
		QImage	do_filter( QImage passImage );
#endif


		// 執行filter, 只限固定區間   lu = left up    rd = right down
		QImage	do_filter( QImage passImage, PointInt lu, PointInt rd );

		
		// 取出Dog的Map
		double	get_DoG( int i, int j );


		// 取出filter_map
		double	get( int i, int j );

		// 設定 w
		void	set_w( int _w );

		// 設定 l
		void	set_l( int _l );

		// 產生DoG map
		void	generate_DoG_map();

		// 回傳filter矩陣
		boost::numeric::ublas::matrix<double>	get_map();

};












/*======================================================================================
	function區
========================================================================================*/



#endif