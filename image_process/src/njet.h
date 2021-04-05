#ifndef _NJET_H
#define _NJET_H

#include <QImage>

#include "../tools.h"

#include <boost/numeric/ublas/matrix.hpp>

//test
typedef boost::numeric::ublas::matrix<double> bMatrixDouble;


struct pointF
{
	double	x;
	double	y;
};

struct point
{
	int		x;
	int		y;
};

struct window
{
	int		width;
	int		height;
};

class	Njet
{
	private:
		QImage		image;															// 讀入的圖檔

		int			win_width,	win_height;											// windows 的 size   在這個windows裡面做內插估計  給 1  表示  2*1 = 3   3*3的window去掃

		bMatrixDouble		mask;								// 產生高斯function
		bMatrixDouble		r_x,	r_y;						// 一階微分項
		bMatrixDouble		r_xx,	r_xy,	r_yy;				// 二階微分項
		bMatrixDouble		r_xxx,	r_xxy,	r_xyx,				// 三階微分項
							r_xyy,	r_yyx,	r_yyy;				// 要小心 r_xyx = r_yxx   會多出兩項
		bMatrixDouble		det_H;								// 存放每個點的 det(H)  H = Hessian

	public:
		Njet( QImage imagePass );													// 建構函數
		void			diff();														// 求微分項
		double			jet1( int x, int y, double dx, double dy );					// 實際上是 1-jet   將 x, y 拆成 dx dy是為了避免誤差   4.663 = 4 + 0.663    x = 4   y = 0.663
		double			jet2( int x, int y, double dx, double dy );					// 2-jet
		double			jet3( int x, int y, double dx, double dy );					// 3-jet
		double			get_Hessian( int x, int y );								// 計算Hessain的值   ( det  tr 等等  測試用)
		void			get_ew( int x, int y, double &nx, double &ny );				// 得到 ew     gradient direction 
		double			get_cur( int x, int y );									// 得到 norm of gradient  (類似curvature)
		void			get_ev( int x, int y, double &tx, double &ty );				// 得到 ev   跟 ew 垂直

		double			get_fx( int x, int y );										// 回傳 fx
		double			get_fy( int x, int y );										// 回傳 fy

		double			get_fw( int x, int y );										// 得到 fw  (v,w)平面上  旋轉過  一個分量 = 0   相當於回傳  sqrt(  r_x^2 + r_y^2 )
		double			get_fww( int x, int y );									// 得到 fww (想成是旋轉後的Hessian)
		double			get_fvv( int x, int y );									// 得到 fvv (想成是旋轉後的Hessian)
		void			get_edge( int x, int y, double &px, double &py );			// 硬算出來求邊 (subpixel等級) 的做法
		void			get_RH( int x, int y );										// 得到旋轉後的 Hessian
		void			corner();													// 得到corner
		void			corner2();													// 得到corner  方法二 採用Hessian的作法
		void			eigenvector( double A, double B, double C, 
											double &vx, double &vy );				// 對稱矩陣求eigenvector  (取eigenvalue絕對值較大的方向)
		double			min( double a, double b, double c, double d );				// 傳回 a, b, c, d 的最小值

		void			output( boost::numeric::ublas::matrix<double>	map, char *str );		// 將資料normalized後輸出到圖檔

		double			MAX( double a, double b );
		double			MIN( double a, double b );

		bMatrixDouble	pix_corner( PointInt lu, PointInt rd );						// pixel 等級的 corner detector.  

		// 用openCV來改的subpixel  應用在  get_corner裡面
		// get_corner是自己修改的 還有問題
		int				get_corner( int x, int y, double &dx, double &dy );																									// 得到corner的subpixel位置   利用 (dx,dy)    回傳值是表示有沒有找到subpixel的位置
		// corner_subpixel是openCV改過來的  也還是有問題
		void			corner_subpixel( QImage src,  boost::numeric::ublas::matrix<double> &dst, double cx, double cy );																				// 計算windows裡面 偏移後的pixel資料
		void			cornet_gradient( boost::numeric::ublas::matrix<double> &gx, boost::numeric::ublas::matrix<double> &gy, double cx, double cy );													// 計算window裡面 偏移後的一階微分
		void			cornet_gradient( boost::numeric::ublas::matrix<double> src, boost::numeric::ublas::matrix<double> &gx, boost::numeric::ublas::matrix<double> &gy, double cx, double cy );		// 使用原始資料偏移後來內插
		void			cornet_gradient2( bMatrixDouble &dst, bMatrixDouble buffer );													// openCV的作法

};



#endif