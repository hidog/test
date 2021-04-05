#include "stdafx.h"
#include "../h/zernike_tool.h"
using namespace std;

	//const double	PI	=	3.1415926;
#define PI 3.1415926



/*=================================================
	產生階層的表
	只能做到 12!   
	超過的話要做特殊處理,不然會overflow
/*=================================================*/
void	factorial_list(){

	ofstream	out( "factorial.txt" );

	int			i;
	int			fac;
	const int	n	=	N_MAX;	// 預設做到12階層

	out << "1\n" ;				// 0! = 1
	for( i=1, fac=1; i<=n; i++ ){
		fac *= i;
		out << fac << endl;
	}

	out.close();
}



/*=================================================
	讀取階層的資料到記憶體中
/*=================================================*/
void	factorial_load( int *a ){

	ifstream	in( "factorial.txt" );

	int		i;

	for( i=0; i<=N_MAX; i++ )
		in >> a[i];


	in.close(); 
}


/*=================================================
	產生r的表
	r = (-1)^l (n-l)! / (...)
	for n-m 偶數
/*=================================================*/
void	r_list( int *fac ){

	ofstream	out("r_list.txt");

	int		n,	m,	l;
	int		PN	=	1;			// 決定正負   -1^l

	for( n=0; n<=N_MAX; n++ )
		for( m=0; m<=N_MAX; m++ ){

			if( (n-m)%2 == 1 )	continue;

			PN	=	1;						// 利用l初始值 = 0
			for( l=0; l<=(n-m)/2; l++ ){
			
				//cout << "(" << n << "," << m << "," << l << ") = " ;
				out << PN * fac[n-l] / fac[l] / fac[ (n+m)/2 - l ] / fac[ (n-m)/2 - l ] << "\n";
				PN	*=	-1;
			}


		}

	out.close();
}


/*=================================================
	讀取 r 的資料
/*=================================================*/
void	r_load( int *r_data ){

	ifstream	in("r_list.txt");

	int		n,	m,	l;
	int		tmp;
	//int		PN	=	1;			// 決定正負   -1^l

	for( n=0; n<=N_MAX; n++ )
		for( m=0; m<=N_MAX; m++ ){

			if( (n-m)%2 == 1 )	continue;

			//PN	=	1;						// 利用l初始值 = 0
			for( l=0; l<=(n-m)/2; l++ ){
			
				//cout << "(" << n << "," << m << "," << l << ") = " ;
				//out << PN * fac[n-l] / fac[l] / fac[ (n+m)/2 - l ] / fac[ (n-m)/2 - l ] << "\n";
				//PN	*=	-1;
				in >> tmp;
				*( r_data + n*(N_MAX+1)*(N_MAX+1) + m*(N_MAX+1) + l )	=	tmp;
			}

		}

	in.close();
}



/*=================================================
	回傳R_nm(x,y)的值
	x^2 + y^2 = radios^2    radios = rho
	這邊要留意, 如果中心點不是(0,0)
	要做座標轉換
	x' = x-Rx
	y' = y-Ry  
	R 是圓心
/*=================================================*/
double	R_nm( int n, int m, double x, double y, int *r_data ){

	const double	rho	=	sqrt(x*x + y*y);
	double			r;
	double			sum;

	int		l;

	// 這邊使用的是 n-m 奇數 Rnm = 0的定義
	if( (n-m)%2 == 1 )
		return	0;

	l	=	(n-m)/2;
	r	=	pow( rho, n-2*l );


	for( sum=0; l>=0; l-- ){
		
		//cout << *( r_data + n*(N_MAX+1)*(N_MAX+1) + m*(N_MAX+1) + l ) << "*r^" << (n-2*l) << " ";
		sum	+=	*( r_data + n*(N_MAX+1)*(N_MAX+1) + m*(N_MAX+1) + l ) * r;
		r	*=	(rho*rho);

	}
	
	return	sum;
}




/*=================================================
	回傳V_nm(x,y)的值
	x^2 + y^2 = radios^2    radios = rho
	這邊要留意, 如果中心點不是(0,0)
	要做座標轉換
	x' = x-Rx
	y' = y-Ry  
	R 是圓心
/*=================================================*/
void	V_nm( int n, int m, double x, double y, int *r_data, complex<double> &v ){


	double	theta;   //	=	atan( y/x );
	double	real,	image;

	if( x == 0 && y == 0 ){
		v	=	complex<double> (0, 0);
		return;
	}

	else if( x == 0 && y > 0 )
		theta	=	PI/2;

	else if( x == 0 && y < 0 )
		theta	=	-PI/2;

	else
		theta	=	atan( y/x );
	


	real	=	R_nm( n, m, x, y, r_data ) * cos( m*theta );
	image	=	R_nm( n, m, x, y, r_data ) * sin( m*theta );

	v	=	complex<double> ( real, image );
}