// njet.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "njet.h"
#include "../BLOB/gaussfilter.h"

using namespace std;


/*=============================================================================
	建構函數
/*=============================================================================*/
Njet::Njet( QImage imagePass )
{
	image		=	imagePass;

	win_width	=	5;			// 1 表示 3*3 的 window
	win_height	=	5;

	GaussFilter_2D	gfilter(win_width, win_height);

	mask		=	gfilter.get_map();

	diff();
}



/*=============================================================================
	實際上是 1-jet
/*=============================================================================*/
double	Njet::jet1( int x, int y, double dx, double dy )
{
	double	r	=	image.gray_channel( x, y );

	r	+=	dx * r_x(x,y);
	r	+=	dy * r_y(x,y);

	return	r;
}



/*=============================================================================
	2-jet
/*=============================================================================*/
double	Njet::jet2( int x, int y, double dx, double dy )
{
	boost::numeric::ublas::matrix<double>	H;			// Hessian
	boost::numeric::ublas::vector<double>	X,	XT;		// X & X^T
	boost::numeric::ublas::vector<double>	tmp;

	double	r;

	H.resize(2,2);
	X.resize(2);
	XT.resize(2);


	H(0,0)	=	r_xx( x, y );
	H(0,1)	=	r_xy( x, y );
	H(1,0)	=	r_xy( x, y );
	H(1,1)	=	r_yy( x, y );

	X(0)	=	dx;
	X(1)	=	dy;

	XT		=	X;//trans(X);

	tmp		=	prod( H, X );

	r		=	jet1(x,y,dx,dy) + 0.5 * inner_prod( XT, tmp );		// 利用呼叫 1-jet的方式.
	

	return	r;
}




/*=============================================================================
	3-jet
/*=============================================================================*/
double	Njet::jet3( int x, int y, double dx, double dy )
{
	double	r;

	//cout << "\n\n~~~" << r_xxx(x,y) << " " << r_xyx(x,y) << " " << r_xyy(x,y) << " " << r_xxy(x,y) << " " << r_yyx(x,y) << " " << r_yyy(x,y) << endl;

	r		=	jet2(x,y,dx,dy) + (1.0/6.0) * ( r_xxx(x,y)*dx*dx*dx + 2*r_xyx(x,y)*dx*dx*dy + 2*r_xyy(x,y)*dx*dy*dy + r_xxy(x,y)*dx*dx*dy + r_yyx(x,y)*dx*dy*dy + r_yyy(x,y)*dy*dy*dy )	;	// 利用呼叫 2-jet的方式.
	
	return	r;
}



/*=============================================================================
	得到 ew     gradient direction 
/*=============================================================================*/
void	Njet::get_ew( int x, int y, double &nx, double &ny )
{
	double	normal	=	sqrt( r_x(x,y) * r_x(x,y) + r_y(x,y) * r_y(x,y) );

	if( normal == 0 )
	{
		//cout << "normal is zero." ;
		nx	=	0;
		ny	=	0;
	}

	nx	=	1.0 * r_x(x,y) / normal;
	ny	=	1.0 * r_y(x,y) / normal;
}


/*=============================================================================
	得到 norm of gradient  (類似curvature)
/*=============================================================================*/
double		Njet::get_cur( int x, int y )
{
	return	sqrt( r_x(x,y)*r_x(x,y) + r_y(x,y)*r_y(x,y) );
}


/*=============================================================================
	得到corner
/*=============================================================================*/
void		Njet::corner()
{
	int		i,	j;
	double	fvv,	fvw,	fww,	fv,	fw,	tmp;

	QImage	final	=	QImage("test.bmp");

	for( i=1; i<image.width()-1; i++ )
		for( j=1; j<image.height()-1; j++ )
		{
			fv	=	0;
			tmp	=	sqrt( r_x(i,j)*r_x(i,j) + r_y(i,j)*r_y(i,j) );
			fw	=	tmp;

			tmp	=	r_x(i,j)*r_x(i,j) + r_y(i,j)*r_y(i,j);
			fvv	=	r_x(i,j)*r_x(i,j)*r_yy(i,j) - 2.0*r_x(i,j)*r_y(i,j)*r_xy(i,j) + r_y(i,j)*r_y(i,j)*r_xx(i,j);
			fvw	=	( r_y(i,j)*r_y(i,j) - r_x(i,j)*r_x(i,j) )*r_xy(i,j) + r_x(i,j)*r_y(i,j)*(r_xx(i,j) - r_yy(i,j));
			fww	=	r_x(i,j)*r_x(i,j)*r_xx(i,j) + 2.0*r_x(i,j)*r_y(i,j)*r_xy(i,j) + r_y(i,j)*r_y(i,j)*r_yy(i,j);


			fvv	/=	tmp;
			fvw	/=	tmp;
			fww	/=	tmp;

			if( i == 308 && j == 122 )
				cout << "???";

			
			if( tmp != 0 )
			{
				//cout << -fvv*fw*fw*fw << endl;


				//if( fabs(fvv*fw*fw*fw) > 300000000 )
				if( fabs(fvv*fw*fw*fw) > 1000000 )
				{
					final.pixel_channel( i, j, 255, 0, 0 );
					cout << i << " " << j << " ... ";
					cout << fvv << "," << fw << "," << -fvv*fw*fw*fw << endl;
				}

			}

		}

	final.save("output.bmp");
	//final.save_image("output.bmp");
}

/*=============================================================================
	對稱矩陣求eigenvector  
	(取eigenvalue絕對值較大的方向)
/*=============================================================================*/
void		Njet::eigenvector(  double A, double B, double C, double &vx, double &vy )
{
	double		dtmp;
	double		eigen1,		eigen2;						// 存 eigenvalue
	double		eigen;
	double		D;
	double		nx,			ny;							// normalized vector
	double		a,		c;

	// 底下是求eigen value的部分
	D		=	(A+C)*(A+C) - 4.*(A*C-B*B);
	
	eigen1	=	0.5 * ((A+C) + sqrt( D )) ;
	eigen2	=	0.5 * ((A+C) - sqrt( D )) ;

	if( fabs( eigen1 ) > fabs( eigen2 ) )	
		eigen	=	eigen1;
	else
		eigen	=	eigen2;



	// 求第一組eigenvector
	a		=	A - eigen;
	c		=	C - eigen;
	
	// 底下在求 eigen vector
	dtmp	=	B*B - (eigen-A)*(eigen-A);
	if( dtmp > 0 )
	{
		nx		=	B / sqrt(dtmp) ;
		ny		=	(eigen-A) / sqrt(dtmp) ;
	}
	else
	{
		dtmp	=	B*B - (eigen-C)*(eigen-C);
		if( dtmp <= 0 )
		{

			// 這邊是特例區	 小心這邊的判斷  有問題的時候回來檢查這邊是否造成bug
			/*
				case 1   
				 a -a
				-a  a
			*/
			if( fabs(a-c) < 0.0001 && fabs(a+B) < 0.0001 && fabs(a) > 0.0001 )
			{
				nx	=	1.0 / sqrt(2.0);
				ny	=	1.0 / sqrt(2.0);
			}
			/*
				case 2
				 a  0
				 0  0
			*/
			else if( fabs(B) < 0.0001 && fabs(a) >= 0.0001 && fabs(c) < 0.0001 )
			{
				nx	=	0;
				ny	=	1;
			}
			/*
				case 3
				 0  0
				 0  a
			*/
			else if( fabs(B) < 0.0001 && fabs(c) >= 0.0001 && fabs(a) < 0.0001 )
			{
				nx	=	1;
				ny	=	0;
			}
			/*
				case 4 
				 a a
				 a a
			*/
			else if( fabs(a-c) < 0.0001 && fabs(a-B) < 0.0001 && fabs(a) > 0.0001 )
			{
				nx	=	 1.0 / sqrt(2.0);
				ny	=	-1.0 / sqrt(2.0);
			}
			else
			{
				/*nx	=	0;
				ny	=	0;*/
				// 實際上 應該要給0,0   為了計算方便先給(1,0)測試
				nx	=	1;
				ny	=	0;
			}
		}
		else
		{
			nx		=	(eigen-C) / sqrt( dtmp );
			ny		=	B / sqrt( dtmp );
		}
	}

	vx	=	nx;
	vy	=	ny;

}



/*=============================================================================
	傳回a b c d 的最小值
/*=============================================================================*/
double		Njet::min( double a, double b, double c, double d )
{
	a	=	fabs(a);
	b	=	fabs(b);
	c	=	fabs(c);
	d	=	fabs(d);

	double	min	=	100000000;
	if( a < min )
		min	=	a;
	if( b < min )
		min	=	b;
	if( c < min )
		min	=	c;
	if( d < min )
		min	=	d;

	return	min;
}




/*=============================================================================
	pixel 等級的 corner detector
	參考論文, 用fw fv來組合
/*=============================================================================*/
bMatrixDouble	Njet::pix_corner( PointInt lu, PointInt rd )
{
	int					i,	j;
	double				mag;
	double				fvv,	fw;
	bMatrixDouble		corner;

	corner.resize( image.width(), image.height() );
	corner.clear();

	for( i = lu.x;  i <= rd.x;  i++ )
		for( j = lu.y;  j <= rd.y;  j++ )
		{
			fvv				=	get_fvv( i, j );
			fw				=	get_fw( i, j );
			mag				=	-fvv * pow( fw, 3 );

			if( mag <= 0 )
				corner( i, j )	=	0;
			else
				corner( i, j )	=	mag;
		}

	return	corner;
}



/*=============================================================================
	openCV的作法
/*=============================================================================*/
void	Njet::cornet_gradient2( bMatrixDouble &dst, bMatrixDouble buffer )
{
    /*int		dst_width, buffer_step = 0;
    int		x, y;

    dst_width = src_size.width - 2;

    for( y = 0; y <= src_size.height; y++, src += src_step,
                                           dst += dst_step,
                                           buffer += buffer_step )
    {
        float* src2 = src + src_step;
        float* src3 = src + src_step*2;
        for( x = 0; x < src_size.width; x++ )
        {
            buffer[x] = (float)(ky[0]*src[x] + ky[1]*src2[x] + ky[2]*src3[x]);
        }

        for( x = 0; x < dst_width; x++ )
        {
            dst[x] = (float)(kx[0]*buffer[x] + kx[1]*buffer[x+1] + kx[2]*buffer[x+2]);
        }
    }*/
}


/*=============================================================================
	得到corner的subpixel位置
	演算法概念是
	理想化的model
	sum (p-c) * grad(p) = 0      p是鄰近點   c是中心
	所以設計一個迭代法去計算
/*=============================================================================*/
int		Njet::get_corner( int x, int y, double &dx, double &dy )
{
	// 超出邊界檢查
	if( x<win_width || x>=image.width()-win_width || y<win_height || y>=image.height() )
		return	0;

    const int	MAX_ITERS	=	100000;		// 最高迭代次數
	
	int		iter	=	0;
	int		i,	j;
	int		count;
	double	m;
	double	ip_x	=	0.5 + x;
	double	ip_y	=	0.5 + y;
	double	err;
	double	c1_x,	c1_y,	c2_x,	c2_y;	// 中心點 跟前一個step的中心點

	double	tgx,	tgy;
	double	gxx,	gxy,	gyy;;
	double	det;

    double	a, b, c, bb1, bb2;				// 計算用暫存  a,b,c放矩陣   bb1,bb2 放向量 b

	//boost::numeric::ublas::matrix<double>	mask;									// 存放 Gauss filter
	boost::numeric::ublas::matrix<double>	gx_buffer,	gy_buffer,	src_buffer;		// 存放暫存的資料  ( 原始圖像偏移後內插值, 微分值)
	boost::numeric::ublas::matrix<double>	A,	InvA;
	//boost::numeric::ublas::vector<double>	bb;
	A.resize(2,2);
	InvA.resize(2,2);

    // 將資料暫存到 buffer  為了要做微分 size 要更大一點
	src_buffer.resize( image.width(), image.height() );
	for( i = 0;   i < image.width();    i++  )
		for(   j = 0;    j < image.height();    j++   )
			src_buffer( x, y )	=	image.gray_channel( x , y  );			// 將資料複製進來
	/*for( i = -win_width - 1;   i <= win_width + 1;    i++  )
		for(   j = -win_height - 1;    j <= win_height + 1;    j++   )
			src_buffer( x+i, y+j )	=	image.gray_channel( x+i , y+j  );			// 將資料複製進來*/

    gx_buffer	=	src_buffer;		// 微分用
    gy_buffer	=	src_buffer;


	// 開始進行迭代
    iter	=	0;
	c1_x	=	ip_x;		// 初始值  用1.5 2.5這樣的點
	c1_y	=	ip_y;
	do
	{
		c2_x	=	c1_x;	// 上一個step的值
		c2_y	=	c1_y;

		// 計算每個點偏移後的值  (用內插)   因為用njet了  所以這邊省略
		//corner_subpixel( image,  src_buffer, c2_x, c2_y );

		// 計算偏移後的 一階微分  gx, gy     原始做法是利用偏移後的原始值去內插  這邊改成直接用n-jet來求該點的一階微分
		cornet_gradient( gx_buffer, gy_buffer, c2_x, c2_y );
		//cornet_gradient( src_buffer,  gx_buffer, gy_buffer, c2_x, c2_y );

		a	=	b	=	c	=	bb1	=	bb2	=	0;	// 歸零

		// 計算矩陣 向量
		count	=	0;
		for(  i = -win_width;  i <= win_width;  i++  )
			for(  j = -win_height;  j <= win_height;  j++  )
			{
				m		=	mask( i + win_width,  j + win_height  );
				tgx 	=	gx_buffer( i + x ,  j + y );
				tgy 	=	gy_buffer( i + x ,  j + y );
				gxx 	=	tgx * tgx * m;
				gxy 	=	tgx * tgy * m;
				gyy 	=	tgy * tgy * m;

				a		+=	gxx;
				b		+=	gxy;
				c		+=	gyy;

				bb1		+=	gxx * i + gxy * j;
				bb2		+=	gxy * i + gyy * j;

				count++;
			}

		bb1	/=	count;
		bb2	/=	count;

		A(0,0)	=	a;
		A(0,1)	=	b;
		A(1,0)	=	b;
		A(1,1)	=	c;

		det		=	A(0,0)*A(1,1) - A(1,0)*A(0,1);
		if( det == 0 )		det	+=	0.000000000001;		// 避免除 0

		// 計算 InvA
		InvA(0,0)	=	A(1,1) / det;
		InvA(0,1)	=	-A(0,1) / det;
		InvA(1,0)	=	-A(1,0) / det;
		InvA(1,1)	=	A(0,0) / det;

		// 計算下一個點的資料    c(i+1) = c(i) + G^-1 * bb    實際上是矩陣運算
		c2_x	=	c1_x + InvA(0,0)*bb1 + InvA(0,1)*bb2;
		c2_y	=	c1_y + InvA(1,0)*bb1 + InvA(1,1)*bb2;

		// 計算誤差項
		err		=	sqrt(   (c2_x - c1_x) * (c2_x - c1_x)  +  (c2_y - c1_y) * (c2_y - c1_y)   );

		// 超出點  這個點不是corner!!
		if( fabs( c2_x - ip_x ) > 0.5 || fabs( c2_y - ip_y ) > 0.5 )
		{
			while( fabs( c2_x - ip_x ) > 0.5 || fabs( c2_y - ip_y ) > 0.5  )
			{
				c2_x	=	0.5 * (c1_x + c2_x);
				c2_y	=	0.5 * (c1_y + c2_y);
			}
			continue;
		}

		// 更新中心點
		c1_x	=	c2_x;
		c1_y	=	c2_y;

		//cout << "error = " << err << endl;

	}while( ++iter < MAX_ITERS && err > 0.001 );


	dx	=	c1_x;
	dy	=	c1_y;


	// 超出點
	if( fabs( c1_x - ip_x ) > 0.5 || fabs( c1_y - ip_y ) > 0.5 )
		return	-1;
	// 迭代次數爆炸
	else if( iter >= MAX_ITERS )
		return	0;
	else
		return	1;

}

/*=============================================================================
	回傳 fx	
/*=============================================================================*/
double	Njet::get_fx( int x, int y )
{
	return	r_x( x, y );
}

/*=============================================================================
	回傳 fy	
/*=============================================================================*/
double	Njet::get_fy( int x, int y )
{
	return	r_y( x, y );
}

/*=============================================================================
	得到corner
	方法二
	採用Hessian的作法
	這裡面記得有加入 non-maxima suppression
/*=============================================================================*/
void		Njet::corner2()
{
	int		i,	j,	c,	r;
	double	fvv,	fvw,	fww,	fv,	fw; //,	tmp;

	double	tmp1,	tmp2,	tmp3,	tmp4,	tmp5,	tmp6,	tmp7,	tmp8;

	QImage	final	=	image;//QImage("test.bmp");

	boost::numeric::ublas::matrix<int>	mask,	skip;
	mask.resize( final.width(), final.height() );
	skip.resize( 2,  final.height() );

	double	threshold	=	1000;

	int		tmp;
	int		cur		=	0;
	int		next	=	1;
	int		h		=	final.height();
	int		w		=	final.width();

	boost::numeric::ublas::matrix<double>	&im	=	det_H;

	for( c=1; c<w-1; c++ )
	{
		r	=	1;
		while( r < h-1 )
		{
			if( skip( cur, r ) == 1 )	{ r++; continue; }
			if( im(c,r) <= im(c,r+1) )
			{
				r++;
				while( r<h && im(c,r) <= im(c,r+1) )	r++;
				if( r == h-1 )	break;
			}
			else
			{
				if( im(c,r) <= im(c,r-1) )		{ r++; continue; }
			}
			skip( cur, r+1 )	=	1;

			if( im(c,r) <= im(c+1,r-1) )	{ r++; continue; }
			skip( next, r-1  )	=	1;
			if( im(c,r) <= im(c+1,r) )		{ r++; continue; }
			skip( next, r )		=	1;
			if( im(c,r) <= im(c+1,r+1) )	{ r++; continue; }
			skip( next, r+1 )	=	1;
			if( im(c,r) <= im(c-1,r-1) )	{ r++; continue; }
			if( im(c,r) <= im(c-1,r) )		{ r++; continue; }
			if( im(c,r) <= im(c-1,r+1) )	{ r++; continue; }

			mask(c,r)	=	1;
			//cout << c << " " << r  << endl;
			r++;

		}

		tmp		=	cur;
		cur		=	next;
		next	=	tmp;
		
		for( i=0; i<skip.size2(); i++ )
			skip( next, i )	=	0;

	}

	for( i=0; i<final.width(); i++ )
		for( j=0; j<final.height(); j++ )
			if( mask(i,j) == 1 && im(i,j) > threshold  )
				final.pixel_channel( i, j, 255, 0, 0 );



	/*for( i=1; i<image.width()-1; i++ )
		for( j=1; j<image.height()-1; j++ )
		{
			tmp		=	r_xx(i,j)*r_yy(i,j) - r_xy(i,j)*r_xy(i,j);

			tmp1	=	r_xx(i,j-1)*r_yy(i,j-1) - r_xy(i,j-1)*r_xy(i,j-1);		// 上
			tmp2	=	r_xx(i,j+1)*r_yy(i,j+1) - r_xy(i,j+1)*r_xy(i,j+1);		// 下
			tmp3	=	r_xx(i-1,j)*r_yy(i-1,j) - r_xy(i-1,j)*r_xy(i-1,j);		// 左
			tmp4	=	r_xx(i+1,j)*r_yy(i+1,j) - r_xy(i+1,j)*r_xy(i+1,j);		// 右
			tmp5	=	r_xx(i-1,j-1)*r_yy(i-1,j-1) - r_xy(i-1,j-1)*r_xy(i-1,j-1);		// 左上
			tmp6	=	r_xx(i-1,j+1)*r_yy(i-1,j+1) - r_xy(i-1,j+1)*r_xy(i-1,j+1);		// 左下
			tmp7	=	r_xx(i+1,j-1)*r_yy(i+1,j-1) - r_xy(i+1,j-1)*r_xy(i+1,j-1);		// 右上
			tmp8	=	r_xx(i+1,j+1)*r_yy(i+1,j+1) - r_xy(i+1,j+1)*r_xy(i+1,j+1);		// 右下

			
			if( tmp != 0 )
			{
				if( tmp >= tmp1 &&
					tmp >= tmp2 &&
					tmp >= tmp3 &&
					tmp >= tmp4 &&
					tmp >= tmp5 &&
					tmp >= tmp6 &&
					tmp >= tmp7 &&
					tmp >= tmp8 )
						final.pixel_channel(i,j,255,0,0);

			}
		}*/

	//final.save_image("output.bmp");
	final.save("output.bmp");
}

/*=============================================================================
	計算Hessain的值   
	( det  tr 等等  測試用)
/*=============================================================================*/
double		Njet::get_Hessian( int x, int y )
{
	double		rxx		=	r_xx( x, y );
	double		rxy		=	r_xy( x, y );
	double		ryy		=	r_yy( x, y );

	double		det		=	rxx*ryy - rxy*rxy;
	double		tr		=	rxx + ryy;

	double		alpha	=	0.04;

	return		det;
}




/*=============================================================================
	得到 ev   跟 ew 垂直
/*=============================================================================*/
void		Njet::get_ev( int x, int y, double &tx, double &ty )
{
	double	normal;
	normal	=	sqrt( r_x(x,y)*r_x(x,y) + r_y(x,y)*r_y(x,y) );

	if( normal == 0 )
	{
		tx	=	0;
		ty	=	0;
	}

	tx	=	r_y(x,y)/normal;
	ty	=	-r_x(x,y)/normal;
}



/*=============================================================================
	得到 fvv (想成是旋轉後的Hessian)
/*=============================================================================*/
double		Njet::get_fvv( int x, int y )
{
	double	normal	=	get_fw(x,y);

	if( normal == 0 )
	{
		//cout << "normal is...zero";
		return	0;
	}

	double	tmp	=	r_x(x,y)*r_x(x,y)*r_yy(x,y) - 2*r_x(x,y)*r_y(x,y)*r_xy(x,y) + r_y(x,y)*r_y(x,y)*r_xx(x,y) ;

	return	tmp / normal;
}

/*=============================================================================
	得到 fww (想成是旋轉後的Hessian)
/*=============================================================================*/
double		Njet::get_fww( int x, int y )
{
	double	normal	=	get_fw(x,y);

	if( normal == 0 )
	{
		//cout << "normal is...zero";
		return	0;
	}

	double	tmp	=	r_x(x,y)*r_x(x,y)*r_xx(x,y) + 2*r_x(x,y)*r_y(x,y)*r_xy(x,y) + r_y(x,y)*r_y(x,y)*r_yy(x,y) ;

	return	tmp / normal;
}


/*=============================================================================
	得到 fw  (v,w)平面上  旋轉過  一個分量 = 0  
	相當於回傳  sqrt(  r_x^2 + r_y^2 )
/*=============================================================================*/
double		Njet::get_fw( int x, int y )
{
	return	sqrt( r_x(x,y)*r_x(x,y) + r_y(x,y)*r_y(x,y) );
}




/*=============================================================================
	硬算出來求邊 (subpixel等級) 的做法
	
	f(x+a) = f + Df*a
	D2 f(x+a) = D2 f + D3 f * a
	a = -(D3 f)^-1 * D2 f   但是不知道 (D3 f)^-1的長相
	所以改寫成

	[0 0] = [ fxx fxy ]  +   [  fxxx*a + fxyx*b,    fyxx*a + fyyx*b ]
	[0 0]   [ fxy fyy ]      [  fxxy*a + fxyy*b,    fyxy*a + fyyy*b ]    但是  /  對角線的兩個不知道要不要交換
/*=============================================================================*/
void		Njet::get_edge( int x, int y, double &px, double &py )
{
	boost::numeric::ublas::matrix<double>	A,	A_inv,	B;
	boost::numeric::ublas::vector<double>	v,	b;

	double	normal;

	A.resize( 2, 2 );
	A_inv.resize( 2, 2 );
	v.resize(2);
	b.resize(2);

	A(0,0)	=	r_xxx(x,y);
	A(0,1)	=	r_xyx(x,y);
	A(1,0)	=	r_xyy(x,y);
	A(1,1)	=	r_yyy(x,y);

	normal	=	A(0,0)*A(1,1) - A(0,1)*A(1,0);

	A_inv(0,0)	=	A(1,1);
	A_inv(0,1)	=	-A(0,1);
	A_inv(1,0)	=	-A(1,0);
	A_inv(1,1)	=	A(0,0);
	A_inv		/=	normal;

	b(0)	=	-r_xx(x,y);
	b(1)	=	-r_yy(x,y);

	v	=	prod( A_inv, b );

	// 求出第一組向量
	for( int i=0; i<2; i++ )
	{
		//cout << v(i) << " ";
	}
	//px	=	v(0);
	//py	=	v(1);


	A(0,0)	=	r_xyx(x,y);
	A(0,1)	=	r_yyx(x,y);
	A(1,0)	=	r_xxy(x,y);
	A(1,1)	=	r_xyy(x,y);

	normal	=	A(0,0)*A(1,1) - A(0,1)*A(1,0);

	A_inv(0,0)	=	A(1,1);
	A_inv(0,1)	=	-A(0,1);
	A_inv(1,0)	=	-A(1,0);
	A_inv(1,1)	=	A(0,0);
	A_inv		/=	normal;

	b(0)	=	-r_xy(x,y);
	b(1)	=	-r_xy(x,y);

	v	=	prod( A_inv, b );

	// 印出第二組解

	px	=	v(0);
	py	=	v(1);
	//cout << "\n";
	//for( int i=0; i<2; i++ )
	//	cout << v(i) << " ";
	
}



/*=============================================================================
	最大值 
/*=============================================================================*/
double	Njet::MAX( double a, double b )
{
	return	a>b ? a : b;
}

/*=============================================================================
	最小值 
/*=============================================================================*/
double	Njet::MIN( double a, double b )
{
	return	a<b ? a : b;
}

/*=============================================================================
	得到旋轉後的 Hessian
/*=============================================================================*/
void	Njet::get_RH( int x, int y )
{
	double	fvv,	fww,	fvw;

	fvv	=	r_x(x,y)*r_x(x,y)*r_yy(x,y) - 2*r_x(x,y)*r_y(x,y)*r_xy(x,y) + r_y(x,y)*r_y(x,y)*r_xx(x,y);

	fvw	=	(r_y(x,y)*r_y(x,y) - r_x(x,y)*r_x(x,y))*r_xy(x,y) + r_x(x,y)*r_y(x,y)*(r_xx(x,y) - r_yy(x,y));

	fww	=	r_x(x,y)*r_x(x,y)*r_xx(x,y) + 2*r_x(x,y)*r_y(x,y)*r_xy(x,y) + r_y(x,y)*r_y(x,y)*r_yy(x,y);

	cout << fvv << " " << fvw << "\n" << fvw << " " << fww;
}




/*=============================================================================
	將資料normalized後輸出到圖檔
/*=============================================================================*/
void	Njet::output( boost::numeric::ublas::matrix<double>	map, char *str )
{
	int		i,	j;
	double	max	=	0;
	double	min	=	12345678;
	double	mag;

	QImage	img	=	QImage( map.size1(), map.size2(), QImage::Format_RGB888 );

	for( i=0; i<map.size1(); i++ )
		for( j=0; j<map.size2(); j++ )
		{
			if( max < map(i,j) )
				max	=	map(i,j);
			if( min > map(i,j) )
				min	=	map(i,j);
		}

	for( i=0; i<map.size1(); i++ )
		for( j=0; j<map.size2(); j++ )
		{
			mag		=	255.0*(map(i,j) - min)/(max-min);
			img.gray_channel(i,j,mag);
		}

	img.save(str);

}


/*=============================================================================
	計算window裡面 
	偏移後的一階微分
	用原始資料偏移後再去內插
/*=============================================================================*/
void	Njet::cornet_gradient( boost::numeric::ublas::matrix<double> src, boost::numeric::ublas::matrix<double> &gx, boost::numeric::ublas::matrix<double> &gy, double cx, double cy )
{
	int		i,	j;
	double	x,	y;
	double	delta_x,	delta_y;		// 偏移量

	x		=	floor( cx );
	y		=	floor( cy );

	delta_x	=	cx - x - 0.5;
	delta_y	=	cy - y - 0.5;

	
	for( i = -win_width;  i <= win_width;  i++ )
		for(  j = -win_height;   j <= win_height;   j++ )
		{
			//gx( x+i, y+j )	=	r_x( x+i, y+j ) + r_xx( x+i, y+j )*delta_x + r_xy( x+i, y+j )*delta_y;
			//gy( x+i, y+j )	=	r_y( x+i, y+j ) + r_xy( x+i, y+j )*delta_x + r_yy( x+i, y+j )*delta_y;

			gx( x+i, y+j )	=	0.5 * ( src( x+i+1, y+j) - src( x+i-1, y+j) );		//舊的方式  利用偏移後的原始資料來做內插
			gy( x+i, y+j )	=	0.5 * ( src( x+i, y+j+1) - src( x+i, y+j-1) );
		}

}




/*=============================================================================
	計算window裡面 
	偏移後的一階微分
/*=============================================================================*/
void	Njet::cornet_gradient( boost::numeric::ublas::matrix<double> &gx, boost::numeric::ublas::matrix<double> &gy, double cx, double cy )
{
	int		i,	j;
	double	x,	y;
	double	delta_x,	delta_y;		// 偏移量

	x		=	floor( cx );
	y		=	floor( cy );

	delta_x	=	cx - x - 0.5;
	delta_y	=	cy - y - 0.5;

	
	for( i = -win_width;  i <= win_width;  i++ )
		for(  j = -win_height;   j <= win_height;   j++ )
		{
			gx( x+i, y+j )	=	r_x( x+i, y+j ) + r_xx( x+i, y+j )*delta_x + r_xy( x+i, y+j )*delta_y;
			gy( x+i, y+j )	=	r_y( x+i, y+j ) + r_xy( x+i, y+j )*delta_x + r_yy( x+i, y+j )*delta_y;

			//gx( ip.x+i, ip.y+j )	=	0.5 * ( src( ip.x+i+1, ip.y+j) - src( ip.x+i-1, ip.y+j) );		舊的方式  利用偏移後的原始資料來做內插
			//gy( ip.x+i, ip.y+j )	=	0.5 * ( src( ip.x+i, ip.y+j+1) - src( ip.x+i, ip.y+j-1) );
		}

}


/*=============================================================================
	計算windows裡面 
	偏移後的pixel資料 
/*=============================================================================*/
void	Njet::corner_subpixel( QImage src,  boost::numeric::ublas::matrix<double> &dst, double cx, double cy )
{
#if 0
// 我的版本
	int		i,	j;
	int		x,	y;
	double	delta_x,	delta_y;	// 偏移量

	x	=	floor( cx );		// 找回pixel的值
	y	=	floor( cy );

	delta_x	=	cx - x - 0.5;		// 用0.5當中心
	delta_y	=	cy - y - 0.5;

	for( i = -win_width-1; i <= win_width+1; i++ )
		for( j = -win_height-1; j <= win_height+1; j++ )
			dst( x+i, y+j )		=	jet3( x+i, y+j, delta_x, delta_y );
			//dst( ip.x+i, ip.y+j )	=	src.gray_channel(ip.x+i,ip.y+j) + delta.x * r_x( ip.x+i, ip.y+j ) + delta.y * r_y( ip.x+i, ip.y+j );
#else
// openCV的版本

	int		i,	j;
	int		x,	y;
	double	delta_x,	delta_y;	// 偏移量

	double	a12, a22, b1, b2;
    double	a, b;
    double	s = 0;

	cx	-=	(13-1) * 0.5;
	cy	-=	(13-1) * 0.5;

	x	=	floor( cx );		// 找回pixel的值
	y	=	floor( cy );

    a 	= 	cx - x;
    b 	= 	cy - y;
    //a 	= 	MAX(a,0.0001f);
    a12 = 	a*(1.-b);
    a22 = 	a*b;
    b1	= 	1. - b;
    b2	= 	b;
    s	= 	(1. - a)/a;

	//if( 0 <= ip.x && ip.x + win_size.width < src_size.width &&
    //    0 <= ip.y && ip.y + win_size.height < src_size.height )
    {
        // extracted rectangle is totally inside the image
        //src += ip.y * src_step + ip.x;
		//src = index
		//src[0]		=	image.gray_channel( x, y );
		//src[1]		=	image.gray_channel( x+1, y );
		//src[src_step]	=	image.gray_channel(x,y+1);

		// win = 5*5    win_w = 11   win_size = 13

        //for( ; win_size.height--; src += src_step, dst += dst_step )
		for(i=0; i<13; i++ )   
        {
            //float prev = (1 - a)*(b1*CV_8TO32F(src[0]) + b2*CV_8TO32F(src[src_step]));
			double	prev = (1-a)*b1*src.gray_channel( x, y+i ) + b2*src.gray_channel( x, y+i+1 );
            //for( j = 0; j < win_size.width; j++ )
			for( j=0; j<13; j++ )  
            {
                //float t = a12*CV_8TO32F(src[j+1]) + a22*CV_8TO32F(src[j+1+src_step]);
				double	t = a12*src.gray_channel( x+j+1, y+i ) + a22*src.gray_channel(x+j+1,y+i+1);
                //dst[j] = prev + t;
				dst(x+j,y+i) = prev+t;
                prev = (double)(t*s);
            }
        }
    }






#endif

}



/*=============================================================================
	求微分項
/*=============================================================================*/
void	Njet::diff()
{
	int		i,	j;
	int		x,	y;
	double	dtmp;
	double	dtmp2;

	r_x.resize ( image.width(), image.height() );			r_x.clear();
	r_y.resize ( image.width(), image.height() );			r_y.clear();
	r_xx.resize( image.width(), image.height() );			r_xx.clear();
	r_yy.resize( image.width(), image.height() );			r_yy.clear();
	r_xy.resize( image.width(), image.height() );			r_xy.clear();
	r_xxx.resize( image.width(), image.height() );
	r_xxy.resize( image.width(), image.height() );
	r_xyx.resize( image.width(), image.height() );
	r_xyy.resize( image.width(), image.height() );
	r_yyx.resize( image.width(), image.height() );
	r_yyy.resize( image.width(), image.height() );
	det_H.resize( image.width(), image.height() );

		
	// 以下是假設 h = 1   (之後看要不要normalize  因為怕誤差過大)
	cout << "\nfind diff...\n";
	for( x=1; x<image.width()-1; x++ )
	{
		//cout << ".";
		for( y=1; y<image.height()-1; y++ )
		{
		//	cout << x << " " << y << endl;

			//dtmp2			=	0.5 * ( image.gray_channel( x+1, y) - image.gray_channel( x-1, y ) );

			r_x( x, y )		=	0.5 * ( image.gray_channel( x+1, y) - image.gray_channel( x-1, y ) );
			r_y( x, y )		=	0.5 * ( image.gray_channel( x, y+1) - image.gray_channel( x, y-1 ) );

			dtmp			=	image.gray_channel( x+1, y ) + image.gray_channel( x-1, y ) - 2.0 * image.gray_channel( x, y );
			r_xx( x, y )	=	dtmp;
	
			dtmp			=	image.gray_channel( x, y+1 ) + image.gray_channel( x, y-1 ) - 2.0*image.gray_channel(x,y);
			r_yy( x, y )	=	dtmp;

			dtmp			=	image.gray_channel( x+1, y+1 ) + image.gray_channel( x-1, y-1 ) - image.gray_channel( x+1, y-1 ) - image.gray_channel( x-1, y+1 );
			r_xy( x, y )	=	dtmp / 4;

			det_H( x, y )	=	fabs(r_xx(x,y) * r_yy(x,y) - r_xy(x,y)*r_xy(x,y));

		}
	}
	cout << endl;

	for( x=2; x<image.width()-2; x++ )
		for( y=2; y<image.height()-2; y++ )
		{
			r_xxx( x, y )	=	0.5 * ( r_xx( x+1, y ) - r_xx( x-1, y ) );
			r_xxy( x, y )	=	0.5 * ( r_xx( x, y+1 ) - r_xx( x, y-1 ) );
			r_xyx( x, y )	=	0.5 * ( r_xy( x+1, y ) - r_xy( x-1, y ) );
			r_xyy( x, y )	=	0.5 * ( r_xy( x, y+1 ) - r_xy( x, y-1 ) );
			r_yyx( x, y )	=	0.5 * ( r_yy( x+1, y ) - r_yy( x-1, y ) );
			r_yyy( x, y )	=	0.5 * ( r_yy( x, y+1 ) - r_yy( x, y-1 ) );
		}

	//output( r_x, "r_x.bmp" );
	//output( r_y, "r_y.bmp" );

}











