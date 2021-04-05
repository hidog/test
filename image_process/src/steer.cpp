// steer.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"


#include "steer.h"

#include <QImage>

using namespace std;



/*==============================================================================================
	exp 函數
	用來處理傳入值是整數的case
/*==============================================================================================*/
double	Steer::exp( int x )
{
	return	exp( (double)x );
}




/*==============================================================================================
	用來畫polar圖
/*==============================================================================================*/
void	Steer::polar_draw()
{
	int		PolarSizeX	=	501;
	int		PolarSizeY	=	501;
	int		i,	j;
	int		ii,	jj;
	int		x,	y;
	int		x2,	y2;
	int		size	=	1000;
	double	theta,	value;
	double	theta2,	value2;

	polar	=	QImage( PolarSizeX, PolarSizeY, QImage::Format_RGB888 );
	polar.fill( 0 );

	for( i=0; i<MAX_THETA; i++ )
	{
#ifdef _OPENPIV_
		progress->setGress( 100.*i/MAX_THETA );
#endif
		for( j=0; j<size; j++ )
		{
			theta	=	theta_data(i) + pi;
			value	=	value_data(i) * PolarSizeX / 2;
			theta2	=	theta_data( (i+1)%MAX_THETA ) + pi;
			value2	=	value_data( (i+1)%MAX_THETA ) * PolarSizeX / 2;

			x		=	value * cos( theta );
			y		=	value * sin( theta );
			x2		=	value2 * cos( theta2 );
			y2		=	value2 * sin( theta2 );

			x		+=	PolarSizeX/2 + 1;
			y		+=	PolarSizeY/2 + 1;

			x2		+=	PolarSizeX/2 + 1;
			y2		+=	PolarSizeY/2 + 1;

			ii		=	(int)( x*(size-1-j) + x2*j )/size;
			jj		=	(int)( y*(size-1-j) + y2*j )/size;

			polar.pixel_channel( ii, jj, 255, 0, 0 );
		}
	}

}



/*==============================================================================================
	解構
/*==============================================================================================*/
Steer::~Steer() {}



#ifdef _OPENPIV_
/*=======================================================================================================
	建構
	OPENPIV 底下使用
/*=======================================================================================================*/
Steer::Steer(Settings *settingsPass, DataContainer  *filedataPass)	
	: AndreaEngine( )
{	
	progress	=	settingsPass->getProgress();	
	settings	=	settingsPass;
	filedata	=	filedataPass;

    mutex.lock();

	int		i;

	// 放需要lock的動作
	N		=	11;					// 決定幾階
	g_size	=	4;					// 必須比 1 大.
	o_size	=	3;					// 原始 size 參考論文作法 所以取 31   產生63*63 的 map
	pi		=	3.141592653589793;

	X		=	settings->get_measure_X();
	Y		=	settings->get_measure_Y();

	w		=	new double[N];
	char		str[100];

	// 讀取 wight的資料
	sprintf( str, "./Steer/w%d.txt", N );
	ifstream	in(str);

	for( i=0; i<N; i++ )
		in >> w[i] ;
		//w[i]	=	1.0 / 11.0;
	in.close();

    mutex.unlock();
}
#else
/*==============================================================================================
	建構函數
/*==============================================================================================*/
Steer::Steer()
{
	int		i;

	N		=	11;						// 決定幾階
	g_size	=	4;
	o_size	=	3;						// 原始 size 參考論文作法 所以取 31   產生63*63 的 map
	pi		=	3.141592653589793;

	image	=	QImage("test.bmp");		// 預設讀取圖片

	w		=	new double[N];

	char		str[100];

	// 讀取 wight的資料
	sprintf( str, "./Steer/w%d.txt", N );
	ifstream	in(str);

	for( i=0; i<N; i++ )
		in >> w[i] ;
		//w[i]	=	1.0 / 11.0;
	in.close();

}
#endif


/*==============================================================================================
	radial 函數
	這邊可以考慮用遞減函數
/*==============================================================================================*/
double		Steer::g_radial( double r )
{
	if( r < g_size )
		return	1;
		//return	(r<=1) ? 1 : 1 + (1.-r)/(g_size-1) ;
	else
		return	0;
}


/*==============================================================================================
	產生 g_map
/*==============================================================================================*/
void		Steer::generate_map()
{
	int		i,	j,	n;
	int		ii,	jj;
	double	_he,	_ho;
	double	phi,	alpha;
	double	r;

	for( ii=0; ii<MAX_THETA; ii++ )
		draw_filter( ii );
}


/*==============================================================================================
	丟入map資料 印出圖形 
	(自動normalized)
/*==============================================================================================*/
void		Steer::output_map( boost::numeric::ublas::matrix<double> map )
{
	output_map( map, "output.bmp");
	/*int			i,	j;
	double		max,	min;
	double		dtmp;

	QImage	img( map.size1(), map.size2(), QImage::Format_RGB888 );
	//img.setwidth_height( map.size1(), map.size2() );

	max		=	0;
	min		=	1234567;

	for( i=0; i<map.size1(); i++ )
		for( j=0; j<map.size2(); j++ )
		{
			if( max < map(i,j) )
				max		=	map(i,j);
			if( min > map(i,j) )
				min		=	map(i,j);
		}
	
	// 兩個差不多的話 做處理
	if( fabs( max - min ) < 0.0001 )
	{
		cout << "max is near min...!!\n";
		max		=	1;
		min		=	0;
	}

	for( i=0; i<map.size1(); i++ )
		for( j=0; j<map.size2(); j++ )
		{
			dtmp	=	255. * ( map(i,j) - min ) / ( max - min );
			img.gray_channel( i, j, (int)dtmp );
		}

	//img.save_image("output.bmp");
	img.save("output.bmp");*/
}



/*==============================================================================================
	丟入map資料 印出圖形 
	(自動normalized)
	並且指定檔名
/*==============================================================================================*/
void		Steer::output_map( boost::numeric::ublas::matrix<double> map, char *name )
{
	int			i,	j;
	double		max,	min;
	double		dtmp;

	QImage	img( map.size1(), map.size2(), QImage::Format_RGB888 );

	max		=	0;
	min		=	1234567;

	for( i=0; i<map.size1(); i++ )
		for( j=0; j<map.size2(); j++ )
		{
			if( max < map(i,j) )
				max		=	map(i,j);
			if( min > map(i,j) )
				min		=	map(i,j);
		}
	
	// 兩個差不多的話 做處理
	if( fabs( max - min ) < 0.0001 )
	{
		cout << "max is near min...!!\n";
		max		=	1;
		min		=	0;
	}

	for( i=0; i<map.size1(); i++ )
		for( j=0; j<map.size2(); j++ )
		{
			dtmp	=	255. * ( map(i,j) - min ) / ( max - min );
			img.gray_channel( i, j, (int)dtmp );
		}

	img.save(name);
}




/*==============================================================================================
	down sample 動作在這邊
/*==============================================================================================*/
boost_matrix	Steer::downsample( boost_matrix map )
{
	if( map.size1() != o_size*2 + 1 )
		cout << "Steer::downsample error!!\n";

	int			i,	j;
	int			ii,	tmp;
	double		dtmp;

	boost_matrix	dmap1,	dmap2;					// 暫存用的 map 之後用來 down sample
	boost_matrix	rmap;

	dmap1.resize(   o_size * 2 + 1,  o_size * 2 + 1 );
	
	for( i = -o_size;    i <= o_size;    i++   )
	{
		for(  j = -o_size;    j <= o_size;   j++  )
		{
			//dmap1( i + o_size,  j + o_size  )	=	i*i + j*j; //exp( -i*i - j*j );		// 測試用  
			dmap1( i+o_size, j+o_size )		=	map( i+o_size, j+o_size );
		}
	}

	// 進行 downsample的動作
	// 次數是計算出來的  要看case決定
	for( ii=0; ii<56; ii++ )
	{
		tmp		=	dmap1.size1() - 1 ;
		dmap2.resize( tmp, tmp );

		// 進行down sample
		for(   i = 0;     i < dmap1.size1() - 1;     i++    )
			for(     j = 0;     j < dmap1.size2() - 1;    j++     )
			{
				dtmp	=	dmap1( i   , j   );
				dtmp	+=	dmap1( i+1 , j   );
				dtmp	+=	dmap1( i   , j+1 );
				dtmp	+=	dmap1( i+1 , j+1 );
				dtmp	*=	0.25;

				// 這邊可以多做一個判斷 看要不要保持中間 (0,0) 點的值不變
				//if( i != tmp/2+1 && j != tmp/2+1 )
				dmap2( i, j )	=	dtmp;
			}

		// 將資料回存到dmap1
		dmap1	=	dmap2;
	}

	rmap	=	dmap1;

	//output_map( g_map );

	return	rmap;
}



/*==============================================================================================
	測試用 畫filter
/*==============================================================================================*/
void	Steer::draw_filter( int ii )
{
	boost::numeric::ublas::matrix<int>		count_map;				// 計算該map裡面  每個pixel 有多少個點
	boost::numeric::ublas::matrix<double>	&filter_map_even	=	he[ii];
	boost::numeric::ublas::matrix<double>	&filter_map_odd		=	ho[ii];

	count_map.resize( 2*g_size+1, 2*g_size+1 );
	filter_map_even.resize( 2*g_size+1, 2*g_size+1 );
	filter_map_odd.resize( 2*g_size+1, 2*g_size+1 );
	count_map.clear();
	filter_map_even.clear();
	filter_map_odd.clear();

	int			i,	j,	n;
	double		x,	y,	xx,	yy;
	double		r,	theta;
	double		value,	radial;
	double		_he,	_ho;
	double		dsum;
	double		alpha;

	
	alpha	=	2.0 * ii * pi / MAX_THETA;

	// 用極座標來產生filter
	for( r = 0.01; r <= g_size; r += 0.01 )
		for( theta = 0; theta < 2*pi; theta += 0.01 )
		{
			x		=	r * cos(theta) ;
			y		=	r * sin(theta) ;
			xx		=	fabs(x);			// 用正值去做四捨五入再轉回原本的sign
			yy		=	fabs(y);
			i		=	(int) (xx + 0.5);
			j		=	(int) (yy + 0.5);

			i		*=	( x>=0 ) ? 1 : -1;
			j		*=	( y>=0 ) ? 1 : -1;

			i		+=	g_size;
			j		+=	g_size;
			_he		=	0;
			_ho		=	0;
			// 計算 sum cos( n*theta ) sin(n*theta) 的動作
			for( n = 1; n <= N; n++ )
			{
				_he	+=	w[n-1] * cos( n*(theta-alpha) );
				_ho	+=	w[n-1] * sin( n*(theta-alpha) );
			}
			radial	=	g_radial( r );

			count_map(i, j)++;
			filter_map_even(i,j)	+=	_he * radial;
			filter_map_odd(i,j)		+=	_ho * radial;
		}

	for( i=0; i<filter_map_even.size1(); i++ )
		for( j=0; j<filter_map_even.size2(); j++ )
		{
			if( count_map(i,j) != 0 )
			{
				filter_map_even(i,j)	/=	count_map(i,j);	
				filter_map_odd(i,j)		/=	count_map(i,j);
			}
		}

/*	char	str[100];
	sprintf(str,"even_%d.bmp",ii);
	output_map( filter_map_even, str );

		//char	str[100];
	sprintf(str,"odd_%d.bmp",ii);
	output_map( filter_map_odd, str );*/

}



/*==============================================================================================
	尋找local max
/*==============================================================================================*/
void	Steer::local_max( double &L, double &T, double &X )
{
	int		i,	j;
	int		tmax_i;			// 存放index
	int		max_i_1,	max_i_2,	max_i_3,	max_i_4;
	double	tmax,	max1,	max2;

	std::vector<int>		max_index;

	i	=	value_data.size()-1;
	if( value_data(0) > value_data(i) && value_data(0) > value_data(1) )
	{
		progress->debuger(  QString("local max = %1, v=%2").arg(0).arg( value_data(0)) );
		max_index.push_back(0);
	}

	if( value_data(i) > value_data(0) && value_data(i) > value_data(i-1) )
	{
		progress->debuger(  QString("local max = %1, v=%2").arg(i).arg( value_data(i)) );
		max_index.push_back(i);
	}

	for( i=1; i<value_data.size()-1; i++ )
	{
		if( value_data(i) > value_data(i+1) && value_data(i) > value_data(i-1) )
		{
			progress->debuger(  QString("local max = %1, v=%2").arg(i).arg( value_data(i)) );
			max_index.push_back(i);
		}
	}

	// local max 1
	tmax		=	0;
	tmax_i		=	-1;
	for( i=0; i<max_index.size(); i++ )
	{
		j	=	max_index[i];
		if( tmax < value_data(j) )
		{
			tmax	=	value_data(j);
			tmax_i	=	j;
		}
	}
	max_i_1	=	tmax_i;

	// local max 2
	tmax		=	0;
	tmax_i		=	-1;
	for( i=0; i<max_index.size(); i++ )
	{
		j	=	max_index[i];

		if( j == max_i_1 )	continue;

		if( tmax < value_data(j) )
		{
			tmax	=	value_data(j);
			tmax_i	=	j;
		}
	}
	max_i_2	=	tmax_i;

	// local max 3
	tmax		=	0;
	tmax_i		=	-1;
	for( i=0; i<max_index.size(); i++ )
	{
		j	=	max_index[i];

		if( j == max_i_1 || j == max_i_2 )	continue;

		if( tmax < value_data(j) )
		{
			tmax	=	value_data(j);
			tmax_i	=	j;
		}
	}
	max_i_3	=	tmax_i;

	// local max 4
	tmax		=	0;
	tmax_i		=	-1;
	for( i=0; i<max_index.size(); i++ )
	{
		j	=	max_index[i];
		
		if( j == max_i_1 || j == max_i_2 || j == max_i_3 )	continue;

		if( tmax < value_data(j) )
		{
			tmax	=	value_data(j);
			tmax_i	=	j;
		}
	}
	max_i_4	=	tmax_i;

	// max_i_1  max_i_2 ...  是local max的index


	// 計算L T X junction
	double		m1	=	max_i_1 != -1 ? value_data( max_i_1 ) : 0;
	double		m2	=	max_i_2 != -1 ? value_data( max_i_2 ) : 0;
	double		m3	=	max_i_3 != -1 ? value_data( max_i_3 ) : 0;
	double		m4	=	max_i_4 != -1 ? value_data( max_i_4 ) : 0;

	m2	/=	m1;
	m3	/=	m1;
	m4	/=	m1;
	m1	=	1.0;

	L	=	m1 * m2 * ( 1 - m3 ) * (1 - m4 );
	T	=	m1 * m2 * m3 * (1 - m4 );
	X	=	m1 * m2 * m3 * m4;

}


/*==============================================================================================
	回傳最大值
/*==============================================================================================*/
double		Steer::SMAX( double a, double b )
{
	return	a>b ? a : b;
}



/*==============================================================================================
	取得該點的 energy
/*==============================================================================================*/
double		Steer::get_Energy( int ii, int x, int y )
{
	int		i,	j;
	double	dtmp,	dtmp1,	dtmp2;

	dtmp	=	0;
	dtmp1	=	0;
	dtmp2	=	0;
	for( i= -g_size; i<=g_size; i++ )
		for( j= -g_size; j<=g_size; j++ )
		{
			if( i == 0 && j == 0 )
				continue;

			// 這邊的index  為了旋轉  所以才改用 x+i  y+j
			if( x+i >= 0 && x+i < image.width() && y+j >= 0 && y+j < image.height() )
			{
				dtmp1	+=	0.0 + he[ii]( i+g_size, j+g_size ) * image.gray_channel( x+i, y+j );
				dtmp2	+=	0.0 + ho[ii]( i+g_size, j+g_size ) * image.gray_channel( x+i, y+j );
	//			dtmp	+=	E[ii]( i+g_size, j+g_size ) * image.gray_channel( x-i, y-j );
			}
		}

	dtmp	=	dtmp1*dtmp1 + dtmp2*dtmp2;
	//dtmp	=	fabs(dtmp1) + fabs(dtmp2);

	return	dtmp;
}







#ifdef _OPENPIV_
/*=======================================================================
	openPIV
	程序的進入點
/*=======================================================================*/
void	Steer::dosomething()
{
	// 初始動作
	image	=	filedata->get_image();

	// 簡單的防呆.
	if( X < 0 || X >= image.width() || Y < 0 || Y >= image.height() )
	{
		X	=	0;
		Y	=	0;
	}

	int		i;
	double	theta,	value;
	double	max;

	for( i=0; i<100; i++ )
	{
		progress->setGress( i );
		draw_filter( i );
	}

	theta_data.resize( 100 );
	value_data.resize( 100 );

	for( i=0; i<MAX_THETA; i++ )
	{
		theta	=	2.0 * i * pi * 0.01 ;
		value	=	get_Energy( i, X, Y );
		theta_data( i )		=	theta;
		value_data( i )		=	value;
	}

	
	// 尋找local max
	double	L,	T,	X;
	local_max( L, T, X );
	progress->debuger( QString("L = %1, T = %2, X = %3").arg(L).arg(T).arg(X), QColor(0,136,0) );

	// normalized value_data
	max	=	0;
	for( i=0; i<MAX_THETA; i++ )
	{
		progress->setGress( 100.*i/MAX_THETA );
		if( max < value_data(i) )
			max	=	value_data(i);
	}
	progress->debuger( QString("The maximal is %1").arg(max) );
	for( i=0; i<MAX_THETA; i++ )
		value_data(i)	/=	max;

	polar_draw();

	//emit( filedata->output(polar) );
	progress->outputImage( 0, polar );
	progress->debuger("<font color=\"Red\"> finish Steer Junction detector... </font>");

}
#endif

