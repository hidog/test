#include "MRSAR.h"

using namespace std;


/**********************************************************************
	建構
/***********************************************************************/
MRSAR::MRSAR( QImage imagePass )
	:	mrfFeatureAbstract( imagePass, dim() )
{}


/**********************************************************************
	建構
/***********************************************************************/
MRSAR::MRSAR()
	:	mrfFeatureAbstract( dim() )
{}


/**********************************************************************
	copy construct
/***********************************************************************/
MRSAR::MRSAR( const MRSAR &mr )
	:	mrfFeatureAbstract(mr)
{
	*this	=	mr;
}


/**********************************************************************
	解構
/***********************************************************************/
MRSAR::~MRSAR()
{}



/**********************************************************************
	切割
/***********************************************************************/
void	MRSAR::segmentation( QImage img )
{
	assert(false);
}


/**********************************************************************
	計算樣張 
	跟cal_feature內容會很像 
	差在這邊會算平均標準差
/***********************************************************************/
void	MRSAR::cal_sample_feature( QImage img )
{
	cal_feature( img );		// 進去提取特徵

	cal_mean();				// 計算平均
	cal_devia();			// 計算標準差
}




/**********************************************************************
	計算feature
/***********************************************************************/
void	MRSAR::cal_feature( QImage img )
{
	mr_sar_cal( img, map );

	// 因為mr_sar的做法會扣掉邊框 所以這邊要再做回轉換

	int		i,	j,	k;

	std::vector<bMatrixDouble>	map_tmp(DIMENSION);//( img.width(), img.height() );
	//map_tmp.clear();
	for( i = 0; i < DIMENSION; i++ )
	{
		map_tmp[i].resize( img.width(), img.height() );
		map_tmp[i].clear();
	}

	for( i = 0; i < map[0].size1(); i++ )
		for( j = 0; j < map[0].size2(); j++ )
			for( k = 0; k < DIMENSION; k++ )
				map_tmp[k]( i+10, j+10 )	=	map[k]( i, j );		// 10是寫死的數字 對應在mr_sar = 21

	map		=	map_tmp;


	for( i = 0; i < DIMENSION; i++ )
	{
		char	filename[100];
		sprintf( filename, "../TEMP/%d.bmp", i );

		img		=	Matrix2Image( map[i], M2I::normalized );
		img.save(filename);
	}

}



/**********************************************************************
	測試輸出
/***********************************************************************/
void	MRSAR::output_txt()
{
	assert(0);
}



/**********************************************************************
	計算 norm
	這邊會開根號
/***********************************************************************/
double	MRSAR::get_norm( int pix )
{
	double	value	=	get_norm_squ(pix);

	return	sqrt(value);
}


/**********************************************************************
	計算 |u-v|
/***********************************************************************/
double	MRSAR::get_norm( int pix1, int pix2 )
{
	assert(0);
	return	0;
}

/**********************************************************************
	回傳型別
/***********************************************************************/
MRF_FEATURE_TYPE	MRSAR::type()
{
	return	MRF_FEATURE_MRSAR;
}


/**********************************************************************
	等於運算
/***********************************************************************/
void	MRSAR::operator	=	( const MRSAR &mr )
{
	mrfFeatureAbstract::operator = (mr);

	map	=	mr.map;
}


/**********************************************************************
	回傳維度
/***********************************************************************/
int		MRSAR::dim()
{
	return	DIMENSION;
}


/**********************************************************************
	計算平均值
/***********************************************************************/
void	MRSAR::cal_mean()
{
	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	size	=	(width-2*10) * (height-2*10);		// 這邊 10先用常數

	double	sum	=	0;

	cout << endl;
	for( int k = 0; k < DIMENSION; k++ )
	{
		sum	=	0;
		for( int i = 10; i < width - 10; i++ )
			for( int j = 10; j < height - 10; j++ )
				sum	+=	map[k](i,j);
		mean[k]		=	sum / size;
		cout << "mean[" << k << "] = " << mean[k] << endl;
	}

	//mean[0]		=	sum / size;					// 因為目前維度 = 1
	//cout << "mean = " << mean[0] << endl;
}


/**********************************************************************
	計算標準差
/***********************************************************************/
void	MRSAR::cal_devia()
{
	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	size	=	(width-2*10) * (height-2*10);

	double	sum	=	0;

	for( int k = 0; k < DIMENSION; k++ )
	{
		sum	=	0;
		for( int i = 10; i < width - 10; i++ )
			for( int j = 10; j < height - 10; j++ )
				sum	+=	(map[k](i,j) - mean[k]) * (map[k](i,j) - mean[k]);
		devia[k]	=	sqrt( sum/size );
		cout << "devia[" << k << "] = " << devia[k] << endl;
	}

	//devia[0]	=	sqrt( sum / size );			// 因為維度 = 1
	//cout << "devia = " << devia[0] << endl;
}


/**********************************************************************
	回傳 norm^2
/***********************************************************************/
double		MRSAR::get_norm_squ( int pix )
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	assert( width > 0 && height > 0 );

	int		i,	j;
	double	value;
	//double	r;
	int		nth;
	double	v1,	v2;

	i	=	pix / height;
	j	=	pix % height;

	assert( i >= 0 && i < width && j >= 0 && j < height );

	// 實際上有除上deviation!!
	//value	=	map( i, j ) - mean[0];					// 因為維度 = 1
	//r		=	pow(value,2) / (2.0*pow(devia[0],2));	// 因為維度 = 1

	double	sum;

	sum		=	0;
	for( nth = 0; nth < DIMENSION; nth++ )
	{
		v1		=	map[nth](i,j) - mean[nth];
		v2		=	pow( v1, 2 ) / 2.0 / pow( devia[nth], 2 );
		sum		+=	v2;
	}
	//sum		*=	0.5;

	return	sum;
}


/**********************************************************************
	回傳 devia^2
/***********************************************************************/
double		MRSAR::get_devia_det()
{
	double	mul	=	1;

	for( int i = 0; i < DIMENSION; i++ )
		mul	*=	devia[i]*devia[i];

	//return	devia[0] * devia[0];
	return	mul;
}


/**********************************************************************
	取得特徵
/***********************************************************************/
void	MRSAR::get_feature( int pix, double *_f )
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	int		i	=	pix / height;
	int		j	=	pix % height;
	int		k;

	assert( i >= 0 && i < width && j >= 0 && j < height );

	//_f[0]	=	map( i, j );			// 因為DIMENSION = 1


	for( k = 0; k < DIMENSION; k++ )
		_f[k]	=	map[k](i,j);

}

/**********************************************************************
	檢查維度
/***********************************************************************/
bool	MRSAR::check_dim()
{
	return	true;
}




/**********************************************************************
	檢查維度
/***********************************************************************/
void	MRSAR::output()
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	


}