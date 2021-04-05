#include "moment.h"

#include <iostream>
using namespace std;


/**********************************************************************
	建構
/***********************************************************************/
Moment::Moment( QImage imagePass )
	:	mrfFeatureAbstract( imagePass, dim() )
{
	int		width	=	image.width();
	int		height	=	image.height();

	cal_feature(sample);
	cal_mean();	
	cal_devia();

}

/**********************************************************************
	建構
/***********************************************************************/
Moment::Moment()
	:	mrfFeatureAbstract( dim() )
{}

/**********************************************************************
	copy construct
/***********************************************************************/
Moment::Moment( const Moment &mt )
	:	mrfFeatureAbstract( mt )
{
	*this	=	mt;
}



/**********************************************************************
	回傳該 pixel 的 feature
/***********************************************************************/
void	Moment::get_feature( int pix, double *_f )
{
	const int	width		=	image.width();
	const int	height		=	image.height();
	const int	MAX_LABEL	=	moment.size();

	int		i	=	pix / height;
	int		j	=	pix % height;
	int		label;

	assert( i >= 0 && i < width && j >= 0 && j < height );

	sVectorDouble	vec(DIMENSION);

	for( label = 0; label < MAX_LABEL; label++ )
		_f[label]	=	moment[label](i,j);
}



/**********************************************************************
	檢查維度
/***********************************************************************/
bool	Moment::check_dim()
{
	/*assert( DIMENSION == devia.size() );
	assert( DIMENSION == mean.size() );
	assert( DIMENSION == moment.size() );

	bool	is	=	DIMENSION == devia.size() && DIMENSION == mean.size() && DIMENSION == moment.size();

	return	is;*/

	return	true;	// 因為改用指標
}




/**********************************************************************
	解構
/***********************************************************************/
Moment::~Moment()
{}


/**********************************************************************
	計算 moment p q
/***********************************************************************/
void	Moment::cal_moment( int p, int q )
{
	assert( p+q <= 2 );

	cout << "moment " << p << " " << q << endl;

	const int		width	=	image.width();
	const int		height	=	image.height();

	int		i,	j;
	int		nth;
	int		x,	y;
	double	sum;
	double	v1,	v2;

	for( i = 0; i < width; i++ )
	{
		for( j = 0; j < height; j++ )
		{
			sum	=	0;
			for( x = -win_size; x <= win_size; x++ )
				for( y = -win_size; y <= win_size; y++ )
				{
					if( i+x >= 0 && i+x < width && j+y >= 0 && j+y < height )
					{
						v1		=	pow( 1.0*x/win_size, p );
						v2		=	pow( 1.0*y/win_size, q );

						sum		+=	v1 * v2 * image.gray_channel( x+i, y+j );		
						// 故意以 i, j 為 (0,0)  避免overflow 並且方便比較每個區塊的texture (不然同樣texture moment表現不同)
					}
				}
			sum	/=	1.0*(2*win_size+1)*(2*win_size+1);		// normalized gray_channel的值 到 0~255

			nth		=	MomentStep( p, q );
			moment[nth](i,j)	=	sum;
		}
	}
}

/**********************************************************************
	將 p, q 轉成 第幾階的列舉值
/***********************************************************************/
int		Moment::MomentStep( int p, int q )
{
	assert( p + q <= 2 );

	int		r;

	if( p == 0 && q == 0 )			r	=	0;
	else if( p == 0 && q == 1 )		r	=	1;
	else if( p == 1 && q == 0 )		r	=	2;
	else if( p == 0 && q == 2 )		r	=	3;
	else if( p == 1 && q == 1 )		r	=	4;
	else if( p == 2 && q == 0 )		r	=	5;
	else							assert(false);

	return	r;
}



/**********************************************************************
	輸出檔案
/***********************************************************************/
void	Moment::output_txt()
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	int		i,	j,	n;
	char	filename[100];
	FILE	*p;

	// m00
	for( n = 0; n < DIMENSION; n++ )
	{
		sprintf( filename, "../TEMP/m%d.txt", n );
		p	=	fopen( filename, "w+" );
		for( i = 0; i < width; i++ )
		{
			for( j = 0; j < height; j++ )
				fprintf( p, "%3.3lf ", moment[0](i,j) );
			fprintf( p, "\n" );
		}
		fclose(p);
	}
	
}


/**********************************************************************
	計算feature
/***********************************************************************/
void	Moment::cal_feature( QImage img ) 
{
	image	=	img;

	const int	width	=	image.width();
	const int	height	=	image.height();

	int		i;

	// 初始化
	moment.resize(DIMENSION);
	for( i = 0; i < DIMENSION; i++ )
		moment[i].resize(width,height);

	// 計算各階moment
	cal_moment( 0, 0 );	
	cal_moment( 0, 1 );
	cal_moment( 1, 0 );
	cal_moment( 0, 2 );
	cal_moment( 1, 1 );
	cal_moment( 2, 0 );

	//output_txt();	// 輸出資料
}


/**********************************************************************
	測試切割
/***********************************************************************/
void	Moment::segmentation( QImage img )
{
	assert(false);
}


/**********************************************************************
	計算平均值
/***********************************************************************/
void	Moment::cal_mean()
{
	const int	width	=	image.width();
	const int	height	=	image.height();

#ifdef _DEBUG
	int		ii;
	for( ii = 0; ii < DIMENSION; ii++ )
		assert( moment[ii].size1() == width && moment[ii].size2() == height );
#endif

	int		i,	j;
	int		nth;
	int		size	=	(width - 2*win_size) * (height - 2*win_size);
	double	sum;
	
	// 初始化
	//mean.resize(DIMENSION);

	for( nth = 0; nth < DIMENSION; nth++ )
	{
		sum		=	0;
		for( i = win_size; i < width - win_size; i++ )
			for( j = win_size; j < height - win_size; j++ )
				sum		+=	moment[nth](i,j);
		mean[nth]	=	sum / size;
	}

	cout << "The mean is as following:\n";
	nth		=	MomentStep( 0, 0 );
	cout << "m00 = " << mean[nth] << endl;
	nth		=	MomentStep( 0, 1 );
	cout << "m01 = " << mean[nth] << endl;
	nth		=	MomentStep( 1, 0 );
	cout << "m10 = " << mean[nth] << endl;
	nth		=	MomentStep( 0, 2 );
	cout << "m02 = " << mean[nth] << endl;
	nth		=	MomentStep( 1, 1 );
	cout << "m11 = " << mean[nth] << endl;
	nth		=	MomentStep( 2, 0 );
	cout << "m20 = " << mean[nth] << endl;
	cout << "\n";
}


/**********************************************************************
	計算標準差
/***********************************************************************/
void	Moment::cal_devia()
{
	const int	width	=	image.width();
	const int	height	=	image.height();

#ifdef _DEBUG
	int		ii;
	for( ii = 0; ii < DIMENSION; ii++ )
		assert( moment[ii].size1() == width && moment[ii].size2() == height );
#endif

	int		i,	j,	nth;
	int		size	=	(width - 2*win_size) * (height - 2*win_size) - 1;
	double	sum;
	double	value;

	// 初始化
	//devia.resize(DIMENSION);

	for( nth = 0; nth < DIMENSION; nth++ )
	{
		sum		=	0;
		for( i = win_size; i < width - win_size; i++ )
			for( j = win_size; j < height - win_size; j++ )
			{
				value	=	moment[nth](i,j) - mean[nth];
				sum		+=	pow( value, 2 );
			}
		devia[nth]	=	sqrt( sum/size );
	}

	cout << "The deviation is following\n";
	nth		=	MomentStep( 0, 0 );
	cout << "d00 = " << devia[nth] << endl;
	nth		=	MomentStep( 0, 1 );
	cout << "d01 = " << devia[nth] << endl;
	nth		=	MomentStep( 1, 0 );
	cout << "d10 = " << devia[nth] << endl;
	nth		=	MomentStep( 0, 2 );
	cout << "d02 = " << devia[nth] << endl;
	nth		=	MomentStep( 1, 1 );
	cout << "d11 = " << devia[nth] << endl;
	nth		=	MomentStep( 2, 0 );
	cout << "d20 = " << devia[nth] << endl;
	cout << "\n";

}

/**********************************************************************
	計算 |u|
	自己的norm
/***********************************************************************/
double		Moment::get_norm( int pix )
{
	double	value	=	get_norm_squ(pix);

	return	sqrt(value);
}


/**********************************************************************
	回傳 norm 的平方
	有除上deviation
	參考論文
	0.5 * (f-mean)^T Sigma^-1 (f-mean)
	f-mean是向量
	Sigma = sigma_i 的對角矩陣 (所以inverse trivial)
	出來是純量
	對應到 U(x)
/***********************************************************************/
double		Moment::get_norm_squ( int pix )
{	
	const int	width	=	image.width();
	const int	height	=	image.height();
	
	int			i,	j;
	int			nth;
	double		sum;
	double		v1,	v2;

	// 取得座標
	i		=	pix / height;
	j		=	pix % height;

	assert( i >= 0 && i < width && j >= 0 && j < height );


	sum		=	0;
	for( nth = 0; nth < DIMENSION; nth++ )
	{
		v1		=	moment[nth](i,j) - mean[nth];
		v2		=	pow( v1, 2 ) / 2.0 / pow( devia[nth], 2 );
		sum		+=	v2;
	}
	sum		*=	0.5;

	return	sum;
}




/**********************************************************************
	計算兩個pixel的norm |u-v|
	使用 pix1去轉(i,j)的作法 如果讀到不同的圖就會炸掉
/***********************************************************************/
double		Moment::get_norm( int pix1, int pix2 )
{
	assert(false);
	return	0;
}



/**********************************************************************
	計算樣張 跟cal_feature內容會很像 差在這邊會算平均標準差
/***********************************************************************/
void	Moment::cal_sample_feature( QImage img )
{
	assert(false);
}


/**********************************************************************
	norm |a-b|
/***********************************************************************/
MRF_FEATURE_TYPE	Moment::type()
{
	return	MRF_FEATURE_MOMENT;
}


/**********************************************************************
	回傳維度
	m00 m01 m10 m02 m11 m20 
	所以是6
/***********************************************************************/
int		Moment::dim()
{
	return	DIMENSION;
}




/**********************************************************************
	回傳deviation的determine  
	注意要平方!!
/***********************************************************************/
double		Moment::get_devia_det()
{
	double	value	=	1.0;
	int		nth;

	for( nth = 0; nth < DIMENSION; nth++ )
		value	*=	devia[nth];
	value	=	pow(value,2);

	return	value;
}


/**********************************************************************
	operator = 
/***********************************************************************/
void	Moment::operator	=	( const Moment &mt )
{
	mrfFeatureAbstract::operator = (mt);

	moment	=	mt.moment;
}



/**********************************************************************
	測試回傳資料
/***********************************************************************/
double		Moment::get_data( int i, int j )
{
	return	moment[0](i,j);
}