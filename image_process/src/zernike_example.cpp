// zernike)example.cpp   測試 zernike的程式片段
//



#include "stdafx.h"


/*
#include "./h/bmp.h"
#include "./h/zernike_tool.h"
#include "./h/matrix.h"
#include "./h/kriging.h"

#include <boost/thread.hpp>

using namespace std;


// 測試檔案數.  1~12
#define	FILE_MAX	2
*/




/*======================================================================================
	測試Zernike moment的程式
	2012.08.20
========================================================================================*/
#ifdef _USE_ZERNIKE
int _tmain(int argc, _TCHAR* argv[])
{

	int		i,	j,	k;
	int		n,	m;
	int		count;

	int		fac[ N_MAX+1 ];									// 儲存階層的資料  
	int		r_data[N_MAX+1][N_MAX+1][N_MAX+1]	=	{0};	// 儲存小r的資料

	double	tmp1,	tmp2,	tmp3;
	double	norm_count;
	double	dtmp;

	string				str;
	complex<double>		c;
	ofstream			out;
	ifstream			in;

	factorial_load( fac );									// 讀取 階層
	r_load( &r_data[0][0][0] );								// 讀取 r_nm


#if 0

	factorial_list();   產生階層的表
	r_list( fac );      產生小r的資料


	R_nm( 9, 1, 1,0, &r_data[0][0][0]  );  回傳  R_nm(x,y)

	/*complex<double> C;
	V_nm( 6, 2, 0.5, -0.12, &r_data[0][0][0], C );
	cout << C;*/

	// 給定圓心 擷取出圓
	BMP	bmp("1.bmp");
	bmp.circle( 300,225, 100, "c1.bmp" );

#endif

	//BMP	bmp("1.bmp");
	//bmp.circle( 300,225, 200, "c1.bmp" );



#if 1

	// 產生Zernike Moment檔案
	for( i=1; i<=FILE_MAX; i++ ){

		str	=	"";
		str	+=	"./input/d";
		str +=	i+'0';
		str +=	".bmp";
		BMP	bmp( str );

		str	=	"";
		str	+=	"./input/d";
		str +=	i+'0';
		str +=	".txt";
		//ofstream	out( str.c_str() );
		out.open( str.c_str() );

		count	=	1;

		for( n=0; n<=N_MAX; n++ )
			for( m=0; m<=n; m++ ){

				if( (n-m)%2 == 1 )	continue;

				cout << i << "-" << n << "-" << m << endl;

				//out << "A( " << n << " , " << m << " )\t=\t";
				c	=	bmp.zernick_moment_nm(n, m ,&r_data[0][0][0] );
				//out << bmp.zernick_moment_nm(n, m ,&r_data[0][0][0] ) << endl;

				out << count << " " << c.real() << " " << c.imag() << endl;
				count ++;
			}

		out.close();
	}


	// 讀取Zernike Moment檔案並且計算 1-norm, 2-norm, inf-norm
	// 讀取資料
	vector<double>	a[FILE_MAX];
	for( i=0; i<FILE_MAX; i++ )
		a[i].resize(49);

	for( i=0; i<FILE_MAX; i++ ){
		
		str	=	"";
		str	+=	"./input/d";
		str	+=	i+1+'0';
		str	+=	".txt";

		in.open( str.c_str() );

		for( j=0; j<49; j++ ){
			in >> tmp1 >> tmp2 >> tmp3;
			a[i][j]	=	sqrt( (double)tmp2*tmp2 + tmp3*tmp3 );
		}

		in.close();
	}

	ofstream	out2("output.txt");
		
	// 1-norm
	cout << "1-norm : \n";
	for( i=1; i<FILE_MAX; i++ ){
		norm_count	=	0;

		for( j=0; j<49; j++ ){
			dtmp		=	a[0][j]	-	a[i][j];
			dtmp		=	(dtmp>0) ? dtmp : -dtmp;
			norm_count	+=	dtmp;
		}
		cout << 1 << "-" << (i+1) << " = " << norm_count << endl;
	}

	// 2-norm
	cout << "\n2-norom : \n";
	for( i=1; i<FILE_MAX; i++ ){
		norm_count	=	0;

		for( j=0; j<49; j++ ){
			dtmp		=	(a[0][j]-a[i][j]) * (a[0][j]-a[i][j]);
			norm_count	+=	dtmp;
		}
		cout << 1 << "-" << (i+1) << " = " << sqrt(norm_count) << endl;
	}

	// inf-norm
	cout << "\ninf-norom : \n";
	for( i=1; i<FILE_MAX; i++ ){
		norm_count	=	0;

		for( j=0; j<49; j++ ){
			dtmp		=	a[0][j] - a[i][j];
			dtmp		=	(dtmp>0) ? dtmp : -dtmp;
			if( dtmp > norm_count )
				norm_count	=	dtmp;

			cout << "The " << j << " = " << dtmp << endl;
		}
		cout << 1 << "-" << (i+1) << " = " << norm_count << endl;
	}

	out2.close();
#endif



#if 0
	BMP	bmp("./input/d2.bmp");

	ofstream	out("./input/d2.txt");

	for( n=0; n<=N_MAX; n++ )
		for( m=0; m<=n; m++ ){

			if( (n-m)%2 == 1 )	continue;

			cout << n << " @ " << m << endl;

			out << "A( " << n << " , " << m << " )\t=\t";
			out << bmp.zernick_moment_nm(n, m ,&r_data[0][0][0] ) << endl;
		}

	out.close();
	//bmp.bitmap_save("out.bmp");
	//bmp.gray_save("gray_out.bmp");

	//cout << bmp.get_subpix_linear(10.52, 13.223);
#endif




	system("PAUSE");
	return 0;
}
#endif