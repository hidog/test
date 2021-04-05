#include "COCCM.h"


#include <omp.h>


using namespace std;


/**********************************************************************
	建構
/***********************************************************************/
COCCM::COCCM( QImage imagePass )
	:	mrfFeatureAbstract( imagePass, dim() )
{
	A	=	NULL;
}


/**********************************************************************
	建構
/***********************************************************************/
COCCM::COCCM()
	:	mrfFeatureAbstract( dim() )
{
	A	=	NULL;
}


/**********************************************************************
	copy construct
/***********************************************************************/
COCCM::COCCM( const COCCM &cc )
	:	mrfFeatureAbstract(cc)
{
	A		=	NULL;
	*this	=	cc;
}


/**********************************************************************
	解構
/***********************************************************************/
COCCM::~COCCM()
{
	delete	[] A;	A	=	NULL;
}



/**********************************************************************
	切割
/***********************************************************************/
void	COCCM::segmentation( QImage img )
{
	assert(false);
}


/**********************************************************************
	計算樣張 
	跟cal_feature內容會很像 
	差在這邊會算平均標準差
/***********************************************************************/
void	COCCM::cal_sample_feature( QImage img )
{
	cal_feature( img );		// 進去提取特徵

	cal_mean();				// 計算平均
	cal_devia();			// 計算標準差
}




/**********************************************************************
	計算feature
/***********************************************************************/
void	COCCM::cal_feature( QImage img )
{
	sample	=	img;
	image	=	img;

	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	size	=	width * height;

	int		i;

	map.resize( width, height );

	// 初始化 暫存資料 A
	A	=	new std::vector<PIXEL> [vec_size*vec_size];
	for( i = 0; i < vec_size*vec_size; i++ )
		A[i].resize( size );

	for( i = 0; i < vec_size*vec_size; i++ )
		pre_Pd_data( i );		// 預先產生向量資料 (存在640,640的 MAP)

	for( i = 0; i < width; i++ )
	{
		cout << i << " ";
#pragma omp parallel for
		for( int j = 0; j < height; j++ )
			map( i, j )		=	get_feature_pix( i, j );
	}

	cout << endl;

	delete	[] A;	A	=	NULL;
}



/**********************************************************************
	測試輸出
/***********************************************************************/
void	COCCM::output_txt()
{
	assert(0);
}



/**********************************************************************
	計算 norm
	這邊會開根號
/***********************************************************************/
double	COCCM::get_norm( int pix )
{
	double	value	=	get_norm_squ(pix);

	return	sqrt(value);
}


/**********************************************************************
	計算 |u-v|
/***********************************************************************/
double	COCCM::get_norm( int pix1, int pix2 )
{
	assert(0);
	return	0;
}

/**********************************************************************
	回傳型別
/***********************************************************************/
MRF_FEATURE_TYPE	COCCM::type()
{
	return	MRF_FEATURE_COCCM;
}


/**********************************************************************
	等於運算
/***********************************************************************/
void	COCCM::operator	=	( const COCCM &cc )
{
	mrfFeatureAbstract::operator = (cc);

	map	=	cc.map;
}


/**********************************************************************
	回傳維度
/***********************************************************************/
int		COCCM::dim()
{
	return	DIMENSION;
}


/**********************************************************************
	計算平均值
/***********************************************************************/
void	COCCM::cal_mean()
{
	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	size	=	(width-2*sl_size) * (height-2*sl_size);

	double	sum	=	0;

	for( int i = sl_size; i < width - sl_size; i++ )
		for( int j = sl_size; j < height - sl_size; j++ )
			sum	+=	map(i,j);

	mean[0]		=	sum / size;					// 因為目前維度 = 1
	cout << "mean = " << mean[0] << endl;
}


/**********************************************************************
	計算標準差
/***********************************************************************/
void	COCCM::cal_devia()
{
	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	size	=	(width-2*sl_size) * (height-2*sl_size);

	double	sum	=	0;

	for( int i = sl_size; i < width - sl_size; i++ )
		for( int j = sl_size; j < height - sl_size; j++ )
			sum	+=	(map(i,j) - mean[0]) * (map(i,j) - mean[0]);

	devia[0]	=	sqrt( sum / size );			// 因為維度 = 1
	cout << "devia = " << devia[0] << endl;
}


/**********************************************************************
	回傳 norm^2
/***********************************************************************/
double		COCCM::get_norm_squ( int pix )
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	assert( width > 0 && height > 0 );

	int		i,	j;
	double	value;
	double	r;

	i	=	pix / height;
	j	=	pix % height;

	assert( i >= 0 && i < width && j >= 0 && j < height );

	// 實際上有除上deviation!!
	value	=	map( i, j ) - mean[0];					// 因為維度 = 1
	r		=	pow(value,2) / (2.0*pow(devia[0],2));	// 因為維度 = 1

	return	r;
}


/**********************************************************************
	回傳 devia^2
/***********************************************************************/
double		COCCM::get_devia_det()
{
	return	devia[0] * devia[0];
}


/**********************************************************************
	取得特徵
/***********************************************************************/
void	COCCM::get_feature( int pix, double *_f )
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	int		i	=	pix / height;
	int		j	=	pix % height;

	assert( i >= 0 && i < width && j >= 0 && j < height );

	_f[0]	=	map( i, j );			// 因為DIMENSION = 1
}

/**********************************************************************
	檢查維度
/***********************************************************************/
bool	COCCM::check_dim()
{
	return	true;
}






/***************************************************************************************
	計算Pd矩陣
/****************************************************************************************/
void	COCCM::cal_Pd( PIXEL d_vector, int x, int y, bMatrixInt &Pd )
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	//bMatrixInt	Pd;
	//Pd.resize( 256, 256 );		// 因為目前考慮灰階 256色
	//Pd.clear();

	//std::vector< std::vector<int> >		Pd(2);

	int		sl_size		=	25;		// 這邊出現兩個windows size  一個是以中心點為範圍的windows, 一個是向量 d 的size  (之後命名需要修改)

	int		i,		j;
	int		off_x,	off_y;
	int		count;
	int		v1,		v2;


	// 這邊有修改過  這樣的做法速度會快很多.
	for( i = x - sl_size; i <= x + sl_size; i++ )
		for( j = y - sl_size; j <= y + sl_size; j++ )
		{
			off_x	=	i + d_vector.x;
			off_y	=	j + d_vector.y;

			if( off_x < 0 || off_x >= width || off_y < 0 || off_y >= height || i < 0 || i >= width || j < 0 || j >= height )
				continue;

			v1	=	image.gray_channel( i, j );
			v2	=	image.gray_channel( off_x, off_y );

			//omp_set_lock(&writelock);

			Pd( v1, v2 )++;

			//omp_unset_lock(&writelock);

			//Pd[0].push_back( v1 );
			//Pd[1].push_back( v2 );

		}

	//return	Pd;
}


/***************************************************************************************
	輸出到檔案
/****************************************************************************************/
void	COCCM::output()
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	int			i,	j;
	int			v;
	double		value;

	bMatrixDouble	map_tmp( width, height );

	/*for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			value	=	map(i,j); //pow( map( i, j ) - mean[0], 2 ) / 2.0 / pow( devia[0], 2 );
			map_tmp( i, j )	=	value;
		}

	ofstream	out("../TEMP/test.txt");
	for( i = 0; i < width; i++ )
	{
		for( j = 0; j < height; j++ )
		{
			out << map_tmp(i,j) << " ";
		}
		out << endl;
	}
	out.close();*/


	ifstream	in("../TEMP/test.txt");
	for( i = 0; i < width; i++ )
	{
		for( j = 0; j < height; j++ )
		{
			in >> map_tmp(i,j) ;
		}
		//out << endl;
	}
	in.close();


	QImage	img	=	Matrix2Image( map_tmp, M2I::normalized );
	img.save("../TEMP/output.bmp");


	bMatrixDouble	map1,	map2,	map3,	map4;
	map1.resize( width, height );
	map2.resize( width, height );
	map3.resize( width, height );
	map4.resize( width, height );

	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			value	=	map_tmp(i,j);
			v		=	(int)value;

			map1(i,j)	=	1.0* ((v / 256 / 256 / 256) % 256);
			map2(i,j)	=	1.0* ((v / 256 / 256) % 256);
			map3(i,j)	=	1.0* ((v / 256) % 256);
			map4(i,j)	=	1.0* (v % 256);

		}


	img	=	Matrix2Image( map1);
	img.save("../TEMP/1.bmp");

	img	=	Matrix2Image( map1 );
	img.save("../TEMP/2.bmp");

	img	=	Matrix2Image( map1 );
	img.save("../TEMP/3.bmp");

	img	=	Matrix2Image( map1 );
	img.save("../TEMP/4.bmp");

}




/***************************************************************************************
	計算能量
/****************************************************************************************/
double	COCCM::Energy( bMatrixInt Pd )
{
	// 這邊為了方便先隨便亂改

	int		i;
	int		j;

	double	sum		=	0;

	for( i = 0; i < Pd.size1(); i++ )
		for( j = 0; j < Pd.size2(); j++ )
			sum	+=	i * j * pow( 1.0*Pd(i,j) , 2 );

	//cout << sum << endl;

	return	sqrt(sum) / 1000000;
}


/***************************************************************************************
	計算熵
/****************************************************************************************/
double	COCCM::Entropy( bMatrixInt Pd )
{
	const int	width	=	Pd.size1();
	const int	height	=	Pd.size2();
	const int	size	=	width * height;

	int		i;
	int		j;

	double	sum		=	0;

	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
			sum	+=	- Pd(i,j) * log( 1.0*Pd(i,j) + 1 );

	sum	/=	size;

	return	sum;
}


/***************************************************************************************
	計算對比
/****************************************************************************************/
double	COCCM::Contrast( bMatrixInt Pd )
{
	const int	width	=	Pd.size1();
	const int	height	=	Pd.size2();
	const int	size	=	width * height;

	int		i;
	int		j;

	double	sum		=	0;

	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
			sum	+=	(i-j) * (i-j) * Pd(i,j);

	sum	/=	size;

	return	sum;
}


/***************************************************************************************
	計算均勻度
/****************************************************************************************/
double	COCCM::Homogeneity( bMatrixInt Pd )
{
	const int	width	=	Pd.size1();
	const int	height	=	Pd.size2();
	const int	size	=	width * height;

	int		i;
	int		j;

	double	sum		=	0;

	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
			sum	+=	1.0 * Pd(i,j) / ( 1.0 + abs(i-j) );

	//sum	/=	size;

	return	sum/80;
}





/***************************************************************************************
	預先產生向量資料
	(主要目的是加速 才多一個中繼介面)
/****************************************************************************************/
void	COCCM::pre_Pd_data( int vec )
{
	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	size	=	width * height;

	const int	off_x	=	vec / vec_size;
	const int	off_y	=	vec % vec_size;

	int			n,	x,	y;
	int			v1,	v2;
	PIXEL		pix;

	for( n = 0; n < size; n++ )
	{
		x	=	n / height; 
		y	=	n % height;

		if( x + off_x >= 0 && x + off_x < width && y + off_y >= 0 && y + off_y < height )
		{
			v1			=	image.gray_channel( x, y );
			v2			=	image.gray_channel( x + off_x, y + off_y );		// 計算 自己 跟位移過的值

			pix			=	PIXEL( v1, v2 );
			A[vec][n]	=	pix;
		}
	}
}







/***************************************************************************************
	計算單點的 feature
/****************************************************************************************/
double	COCCM::get_feature_pix( int x, int y )
{
	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	size	=	width * height;

	int		i,	j,	k,	v1,	v2;

	double	sum;

	sum		=	0;
	for( i = x - sl_size; i <= x + sl_size; i++ )
		for( j = y - sl_size; j <= y + sl_size; j++ )
		{
			if( i < 0 || i >= width || j < 0 || j >= height )
				continue;	// 越界的跳過

			for( k = 0; k < vec_size*vec_size; k++ )
			{
				v1		=	A[k][ i*height + j ].x;
				v2		=	A[k][ i*height + j ].y;

				sum		+=	1.0 * v1 * v2 / size;		// 這邊是修改重點  有試過其他的feature取法
			}
		}

	return	sum;
}



/***************************************************************************************
	測試用
/****************************************************************************************/
void	COCCM::test()
{
	image	=	QImage("../DATA/scratches.bmp");

	double		max,	min;
	double		value;
	int			i,		j;

	max		=	0;
	min		=	300;
	for( i = 1762 - 100; i < 1962 + 100; i++ )
		for( j = 1721 - 100; j < 1921 + 100; j++ )
		{
			if( max < image.gray_channel( i, j ) )
				max	=	image.gray_channel( i, j );
			if( min > image.gray_channel( i, j ) )
				min	=	image.gray_channel( i, j );
		}

	cout << "max = " << max << " min = " << min << endl;

	for( i = 0; i < image.width(); i++ )
		for( j = 0; j < image.height(); j++ ) 
		{
			value	=	255.0 * (image.gray_channel(i,j) - min)	/ (max - min);
			image.gray_channel( i, j, value );
		}
	image.save("../TEMP/output.bmp");


	// 去產生Pd矩陣
	//int			v1,		v2;
	//bMatrixInt	Pd;
	//Pd.resize( 256, 256 );



	QImage	img	=	image;
	bool	is;

	//omp_init_lock(&writelock);

//#pragma omp parallel for
for( int x = 1762; x < 1962; x++ )
	for( int y = 1721; y < 1921; y++ )
//for( int x = 1800; x < 1810; x++ )
//for( int y = 1800; y < 1810; y++ )

	{
		cout << "*";

		//bMatrixInt	sum_ma(256,256);
		//sum_ma.clear();

		bMatrixInt	Pd(256,256);
		Pd.clear();
	
//#pragma omp parallel for
		for( int v1 = -25; v1 <= 25; v1++ )
		{
			for( int v2 = -25; v2 <= 25; v2++ )
			{
				if( v1 == 0 && v2 == 0 )
					continue;
				cal_Pd( PIXEL(v1,v2), x, y, Pd );
			}
		}



		//img.gray_channel( x, y, 0 );
		//for( i = 0; i < 256; i++ )
		//	for( j = 0; j < 256; j++ )
		img.gray_channel( x, y, Energy(Pd) );


		// 輸出
		/*char	filename[100];
		sprintf( filename, "../TEMP/Pd.txt" );

		FILE	*p	=	fopen(filename,"w+");
		for( i = 0; i < 256; i++ )
		{
			for( j = 0; j < 256; j++ )
			{
				fprintf( p, "%4d", Pd(i,j) );
				if( Pd(i,j) != 0 )
					cout << Pd(i,j) << endl;
			}
			fprintf( p, "\n" );
		}
		fclose(p);*/


	}

	img.save("../TEMP/output.bmp");

}