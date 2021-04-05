#include "stdafx.h"

#include "zernike.h"




/*=================================================
	建構函數
/*=================================================*/
Zernike::Zernike()
{
	PI	=	3.14159265358979323846;
	N	=	12;

	// 產生資料
	generate_pdata();
}




/*=================================================
	產生階層的表
	只能做到 12!   
	超過的話要做特殊處理,不然會overflow
/*=================================================*/
void	Zernike::factorial_list()
{
	ofstream	out( "factorial.txt" );

	int			i;
	int			fac;
	const int	n	=	N;	// 預設做到12階層

	fac_list.resize( n+1 );
	fac_list.clear();

	out << "1\n" ;				// 0! = 1
	fac_list(0)		=	1;

	for( i=1, fac=1; i<=n; i++ )
	{
		fac				*=	i;
		fac_list( i )	=	fac;
		out << fac << endl;
	}

	out.close();
}



/*=================================================
	讀取階層的資料到記憶體中
/*=================================================*/
void	Zernike::factorial_load()
{
	ifstream	in( "factorial.txt" );
	int			i;

	fac_list.resize( N+1 );

	for( i=0; i<=N; i++ )
		in >> fac_list(i);

	in.close(); 
}


/*=================================================
	產生r的表
	r = (-1)^l (n-l)! / (...)
	for n-m 偶數
/*=================================================*/
void	Zernike::r_list()
{
	ofstream	out("r_list.txt");

	int			n,	m,	l;
	int			i,	j;
	int			PN	=	1;			// 決定正負   -1^l
	int			mag;

	// 初始化 r_data
	r_data.resize( N+1, N+1 );
	for( i = 0; i < r_data.size1(); i++ )
		for( j = 0; j < r_data.size2(); j++ )
			r_data( i, j ).resize( N+1 );

	for( n = 0; n <= N; n++ )
		for( m = 0; m <= N; m++ )
		{
			if( (n-m)%2 == 1 )	continue;

			PN	=	1;						// 利用l初始值 = 0
			for( l=0; l<=(n-m)/2; l++ )
			{
				//out << PN * fac_list[n-l] / fac_list[l] / fac_list[ (n+m)/2 - l ] / fac_list[ (n-m)/2 - l ] << "\n";
				//cout << "(" << n << "," << m << "," << l << ") = " ;
				mag		=	PN * fac_list[n-l] / fac_list[l] / fac_list[ (n+m)/2 - l ] / fac_list[ (n-m)/2 - l ];		// 記得這邊只會是整數
				PN		*=	-1;

				r_data( n, m )( l )		=	mag;
				out << mag << endl;
			}

		}

	out.close();
}


/*=================================================
	讀取 r 的資料
/*=================================================*/
void	Zernike::r_load()
{
	ifstream	in("r_list.txt");

	int		n,	m,	l;
	int		i,	j;
	int		tmp;
	//int		PN	=	1;			// 決定正負   -1^l

	// 初始化 r_data
	r_data.resize( N+1, N+1 );
	for( i = 0; i < r_data.size1(); i++ )
		for( j = 0; j < r_data.size2(); j++ )
			r_data( i, j ).resize( N+1 );


	for( n=0; n<=N; n++ )
		for( m=0; m<=N; m++ )
		{
			if( (n-m)%2 == 1 )	continue;

			//PN	=	1;						// 利用l初始值 = 0
			for( l=0; l<=(n-m)/2; l++ )
			{
				//cout << "(" << n << "," << m << "," << l << ") = " ;
				//out << PN * fac[n-l] / fac[l] / fac[ (n+m)/2 - l ] / fac[ (n-m)/2 - l ] << "\n";
				//PN	*=	-1;
				in >> tmp;
				r_data( n, m )( l )		=	tmp;

				//*( r_data + n*(N+1)*(N+1) + m*(N+1) + l )	=	tmp;
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
double	Zernike::R_nm( int n, int m, double x, double y )
{
	const double	rho		=	sqrt(x*x + y*y);
	double			r;
	double			sum;

	int		l;

	// 這邊使用的是 n-m 奇數 Rnm = 0的定義
	if( (n-m)%2 == 1 )
		return	0;

	l	=	(n-m)/2;
	r	=	pow( rho, n-2*l );


	for( sum=0; l>=0; l-- )
	{	
		//cout << *( r_data + n*(N_MAX+1)*(N_MAX+1) + m*(N_MAX+1) + l ) << "*r^" << (n-2*l) << " ";
		//sum	+=	*( r_data + n*(N+1)*(N+1) + m*(N+1) + l ) * r;
		sum		+=	r * r_data( n, m )( l );
		r		*=	(rho*rho);

	}
	
	return	sum;
}



/*=================================================
	產生資料  
	階層  r table 這兩個
/*=================================================*/
void	Zernike::generate_pdata()
{
	factorial_list();
	r_list();
}

/*=================================================
	讀取資料  
	階層 r table 這兩個
/*=================================================*/
void	Zernike::load_pdata()
{
	factorial_load();
	r_load();
}



/*=================================================
	回傳 V_nm(x,y) 的值
	x^2 + y^2 = radios^2    radios = rho
	這邊要留意, 如果中心點不是(0,0)
	要做座標轉換
	x' = x-Rx
	y' = y-Ry  
	R 是圓心
/*=================================================*/
complex<double>		Zernike::V_nm( int n, int m, double x, double y )
{
	double	theta;   //	=	atan( y/x );
	double	real,	image;

	if( x == 0 && y == 0 )
	{
		//v	=	complex<double> (0, 0);
		return	complex<double> (0, 0);;
	}

	/*else if( x == 0 && y > 0 )
		theta	=	PI/2;

	else if( x == 0 && y < 0 )
		theta	=	-PI/2;

	else
		theta	=	atan( y/x );*/

	theta	=	atan2( y, x );
	

	real	=	R_nm( n, m, x, y ) * cos( m*theta );
	image	=	R_nm( n, m, x, y ) * sin( m*theta );

	//v	=	complex<double> ( real, image );
	return		complex<double> ( real, image );
}


/*=================================================
	讀取圖檔資料
/*=================================================*/
void	Zernike::loadBMP( QImage passImage )
{
	image	=	passImage;
}


/*=================================================
	產生zernike moment 檔案
/*=================================================*/
void	Zernike::calculate_zernike( POINT center, double radius )
{
	//ofstream	out( "Zernike.txt");

	//int					n,	m;
	int					i,	j;
	//complex<double>		c;

	Z.resize( N+1, N+1 );
	for( i = 0; i < Z.size1(); i++ )
		for( j = 0; j < Z.size2(); j++ )
			Z( i, j )	=	complex<double>(0,0);

	// 產生Zernike Moment檔案
#pragma omp parallel for
	for( int n = 0; n <= N; n++ )
	{
		cout << "*";
		for( int m = 0; m<=n; m++ )
		{
			if( (n-m)%2 == 1 )	continue;

			complex<double>		c	=	Z_nm( n, m, center, radius );

			Z( n, m )		=	c;

			//out << n << " " << m  << "\t" << c.real() << "\t\t" << c.imag() << endl;
		}
	}

	//out.close();
}



/*=================================================
	計算 Anm   ( zernike moment)    
	某些地方會寫Znm
	Anm
	給定中心 半徑 去做積分 
	(半徑會拿來normalized)
/*=================================================*/
complex<double>		Zernike::Z_nm( int n, int m, POINT center, double radius )
{
	int					i,		j;
	double				theta,	r;
	double				x,		y;
	double				xx,		yy;
	double				real_p,	image_p;

	complex<double>		v,	c2(0,0);
	
	bMatrixInt			count_map;									// 計算每個pixel被計算了幾次
	count_map.resize( image.width(), image.height() );		// 實際上有很多個點的資料是用不到的 為了方便計算而已
	count_map.clear();

	boost::numeric::ublas::matrix< complex<double> >	integral;
	integral.resize( image.width(), image.height() );

	for( i = 0; i < image.width(); i++ )
		for( j  = 0; j < image.height(); j++ )
			integral( i, j )	=	complex<double>(0,0);


	// 用極座標來做積分   0不用管  因為zernike moment在r=0   Vnm = 0
	for( r = 0.001; r <= 1; r += 0.001 )
		for( theta = 0; theta < 2*PI; theta += 0.001 )
		{
			// 這邊的計算是為了降低計算誤差
			x		=	r * cos(theta) ;
			y		=	r * sin(theta) ;

			xx		=	fabs( x*radius );			// 用正值去做四捨五入再轉回原本的sign
			yy		=	fabs( y*radius );			// radius: 用來轉換到圖片上的座標
			i		=	(int) (xx + 0.5);
			j		=	(int) (yy + 0.5);

			i		*=	( x>=0 ) ? 1 : -1;
			j		*=	( y>=0 ) ? 1 : -1;

			i		+=	center.x;
			j		+=	center.y;

			if( i < 0 || i >= image.width() || j < 0 || j >= image.height() )		continue;

			v		=	V_nm( n, m, x, y );
	
			real_p	=	image.gray_channel( i, j ) * v.real();
			image_p	=	image.gray_channel( i, j ) * (-v.imag());			// 取共軛

			integral( i, j )	+=	complex<double>( real_p, image_p );
			count_map( i, j )++;
		}


	// 計算完後 將每個點除上對應的count
	for( i = 0; i < image.width(); i++ )
		for( j = 0; j < image.height(); j++ )
		{
			if( count_map(i,j) != 0 )	
				integral( i, j )	/=	count_map( i, j );
		}

	// 換算回 1/radius的比例
	for( i = 0; i < image.width(); i++ )
		for( j = 0; j < image.height(); j++ )
		{
				integral( i, j )	*=	1.0 / (radius*radius);
		}



	// 將全部加起來 做積分
	c2	=	complex<double>(0,0);
	for( i = 0; i < image.width(); i++ )
		for( j = 0; j < image.height(); j++ )
		{
			if( count_map(i,j) != 0 )	
				c2	+=	integral( i, j );
		}

	c2	*=	1.0 * (n+1) / PI;

	return	c2;
}


/*=================================================
	重建某階的moment圖像
/*=================================================*/
void	Zernike::recon_nm( int n, int m, POINT center, double radius )
{
	int					i,		j;
	double				theta,	r;
	double				x,		y;
	double				xx,		yy;
	double				real_p,	image_p;

	complex<double>		v,	c2(0,0);

	QImage				final( image.width(), image.height(), QImage::Format_RGB666 );
	final.fill( 0 );
	
	bMatrixInt			count_map;									// 計算每個pixel被計算了幾次
	count_map.resize( image.width(), image.height() );		// 實際上有很多個點的資料是用不到的 為了方便計算而已
	count_map.clear();

	boost::numeric::ublas::matrix< complex<double> >	integral;
	integral.resize( image.width(), image.height() );

	for( i = 0; i < image.width(); i++ )
		for( j  = 0; j < image.height(); j++ )
			integral( i, j )	=	complex<double>(0,0);


	// 用極座標來做積分   0不用管  因為zernike moment在r=0   Vnm = 0
	for( r = 0.01; r <= 1; r += 0.01 )
		for( theta = 0; theta < 2*PI; theta += 0.01 )
		{
			// 這邊的計算是為了降低計算誤差
			x		=	r * cos(theta) ;
			y		=	r * sin(theta) ;

			xx		=	fabs( x*radius );			// 用正值去做四捨五入再轉回原本的sign
			yy		=	fabs( y*radius );			// radius: 用來轉換到圖片上的座標
			i		=	(int) (xx + 0.5);
			j		=	(int) (yy + 0.5);

			i		*=	( x>=0 ) ? 1 : -1;
			j		*=	( y>=0 ) ? 1 : -1;

			i		+=	center.x;
			j		+=	center.y;

			if( i < 0 || i >= image.width() || j < 0 || j >= image.height() )		continue;

			v		=	V_nm( n, m, x, y );

			c2		=	Z( n, m ) * v;
	
			integral( i, j )	+=	c2;

			count_map( i, j )++;
		}

	// 計算完後 將每個點除上對應的count
	for( i = 0; i < image.width(); i++ )
		for( j = 0; j < image.height(); j++ )
			if( count_map(i,j) != 0 )	
				integral( i, j )	/=	count_map( i, j );


	// 將全部加起來 做積分
	c2	=	complex<double>(0,0);
	for( i = 0; i < image.width(); i++ )
		for( j = 0; j < image.height(); j++ )
			if( count_map(i,j) != 0 )	
				cout << integral( i, j ).real() << " " << integral( i, j ).imag() << endl;


}



/*=================================================
	重建圖像
/*=================================================*/
void	Zernike::reconstruct( POINT center, double radius )
{



}




/*=================================================
	測試用函數
/*=================================================*/
void	Zernike::test()
{
	int			i,	j;
	ofstream	out("test.txt");

	for( i = 0; i <= N; i++ )
	{
		for( j = 0; j <= i; j++ )
		{
			if( (i - j) % 2 != 0 )	continue;
				
			out << i << " " << j << "\t\t" << norm( Z( i, j ) ) << endl;

		}
	}

	out.close();
	
}
