#ifndef _FACET_H
#define _FACET_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <cmath>
#include <limits.h>

#include <QImage>


class Facet
{
	private:
		int		N;		// 決定是 3*3   5*5

		//boost::numeric::ublas::vector<double>	p;			// 求出來  多項式的係數.   p0 + p1*i + p2*j + p3*i^2 + p4*i*j + p5*j^2   從 0開始  跟paper不太一樣.

		boost::numeric::ublas::matrix<double>	MPA;		// Moore-Penrose inverse  A^+   用這個來推出 p0,p1,....pn
		boost::numeric::ublas::matrix<double>	W;			// weight matrix


	public:

		Facet( int n );

		double	get_subpixel( double x, double y, QImage image );													// 得到 sub pixel 的內插值

		int		power( int i, int n, int j, int m );																// 求 i^n j^m
		double	power( double i, int n, double j, int m );															// 求 i^n j^m

		boost::numeric::ublas::matrix<double>	inverse( boost::numeric::ublas::matrix<double> A );					// 求矩陣inverse  用lu分解
		boost::numeric::ublas::vector<double>	find_p( double x, double y, QImage image );							// 求出係數 p

		bool	edge( double x, double y, QImage image, double &rtx, double &rty );									// 求邊
	
};






#endif