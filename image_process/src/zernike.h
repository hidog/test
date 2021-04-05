#ifndef _ZERNIKE
#define _ZERNIKE

#include <complex>
#include <fstream>
#include <boost/numeric/ublas/matrix.hpp>

#include <QImage>

using namespace std;



//----------------------------------------------typedef------------------------------------------------------
typedef		boost::numeric::ublas::matrix<double>	bMatrixDouble;
typedef		boost::numeric::ublas::matrix<int>		bMatrixInt;
typedef		boost::numeric::ublas::vector<int>		bVectorInt;

//----------------------------------------------struct------------------------------------------------------
struct	POINT
{
	int		x;
	int		y;
	POINT( int _x, int _y ) : x(_x), y(_y) {}
};


//----------------------------------------------class------------------------------------------------------
class	Zernike
{
	private:
		int				N;											// size
		double			PI;			
		
		QImage			image;

		bVectorInt		fac_list;									// 階層的資料

		boost::numeric::ublas::matrix<bVectorInt>		r_data;		// r lsit 的資料
		boost::numeric::ublas::matrix<complex<double>>	Z;			// Zernike moment


	public:
		// 建構函數
		Zernike();
		
		// 產生階層的表
		void				factorial_list();

		// 讀取階層的表
		void				factorial_load();

		// 產生 r 的表
		void				r_list();

		// 讀取 r 的資料
		void				r_load();

		// 回傳 R_nm(x,y) 的值
		double				R_nm( int n, int m, double x, double y );

		// 回傳 V_nm(x,y) 的值
		complex<double>		V_nm( int n, int m, double x, double y );

		// 產生資料  階層  r table 這兩個
		void				generate_pdata();

		// 讀取資料  階層 r table 這兩個
		void				load_pdata();

		// 讀取圖檔資料
		void				loadBMP( QImage passImage );

		// 產生zernike moment 檔案
		void				calculate_zernike( POINT cenater, double radius );

		// 計算 Anm   ( zernike moment)    給定中心 半徑 去做積分 (半徑會拿來normalized)
		complex<double>		Z_nm( int n, int m, POINT center, double radius );

		// 重建圖像
		void				reconstruct( POINT center, double radius );

		// 重建某階的moment圖像
		void				recon_nm( int n, int m, POINT center, double radius );

		// 測試用函數
		void				test();

};






#endif