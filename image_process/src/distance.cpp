#include "distance.h"


using namespace std;


/**********************************************************************
	建構
/***********************************************************************/
Distance::Distance( QImage imagePass )
	:	mrfFeatureAbstract( imagePass, dim() )
{
	anchor_x		=	0;		
	anchor_y		=	0;
	anchor_width	=	0;
	anchor_height	=	0;
}


/**********************************************************************
	建構
/***********************************************************************/
Distance::Distance()
	:	mrfFeatureAbstract( dim() )
{
	anchor_x		=	0;		
	anchor_y		=	0;
	anchor_width	=	0;
	anchor_height	=	0;
}


/**********************************************************************
	copy construct
/***********************************************************************/
Distance::Distance( const Distance &dt )
	:	mrfFeatureAbstract(dt)
{
	*this	=	dt;
}


/**********************************************************************
	解構
/***********************************************************************/
Distance::~Distance()
{}


/**********************************************************************
	測試切割
/***********************************************************************/
void	Distance::segmentation( QImage img )
{
	assert(false);
}


/**********************************************************************
	計算feature
/***********************************************************************/
void	Distance::cal_feature( QImage img )
{
	image	=	img;

	const int	width	=	image.width();
	const int	height	=	image.height();

	assert( anchor_x >= 0 && anchor_x + anchor_width < width );
	assert( anchor_y >= 0 && anchor_y + anchor_height < height );
	assert( anchor_width > 0 && anchor_height > 0 );

	// 因為feature值就是圖片本身的RGB值  不用另外算 直接求平均跟標準差

	//cal_mean();		// 計算平均值
	//cal_devia();	// 計算標準差
}



/**********************************************************************
	輸出檔案
/***********************************************************************/
void	Distance::output_txt()
{}


/**********************************************************************
	計算平均值
/***********************************************************************/
void	Distance::cal_mean()
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	assert( anchor_x >= 0 && anchor_x + anchor_width < width );
	assert( anchor_y >= 0 && anchor_y + anchor_height < height );
	assert( anchor_width > 0 && anchor_height > 0 );

	int		i,	j;
	int		size	=	anchor_width * anchor_height;
	double	sum		=	0;

	for( i = anchor_x; i < anchor_x + anchor_width; i++ )
		for( j = anchor_y; j < anchor_y + anchor_height; j++ )
			sum		+=	image.gray_channel( i, j );

	mean[0]	=	sum / size;			// distance維度 = 1
	
	cout << "mean = " << mean[0] << endl;
}



/**********************************************************************
	計算標準差
/***********************************************************************/
void	Distance::cal_devia()
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	assert( anchor_x >= 0 && anchor_x + anchor_width < width );
	assert( anchor_y >= 0 && anchor_y + anchor_height < height );
	assert( anchor_width > 0 && anchor_height > 0 );

	int		i,	j;
	int		size	=	anchor_width * anchor_height - 1;
	double	sum		=	0;
	double	value;

	assert( size > 0 );

	for( i = anchor_x; i < anchor_x + anchor_width; i++ )
		for( j = anchor_y; j < anchor_y + anchor_height; j++ )
		{
			value	=	image.gray_channel( i, j ) - mean[0];
			sum		+=	pow( value, 2 );
		}

	devia[0]	=	sqrt( sum/size );		// 因為維度是 1
	
	cout << "devia = " << devia[0] << endl;
}


/**********************************************************************
	回傳該 pixel 的 feature
/***********************************************************************/
void	Distance::get_feature( int pix, double *_f )
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	int		i	=	pix / height;
	int		j	=	pix % height;

	assert( i >= 0 && i < width && j >= 0 && j < height );

	//sVectorDouble	vec(DIMENSION);

	_f[0]	=	image.gray_channel( i, j );			// 因為DIMENSION = 1
}



/**********************************************************************
	檢查維度
/***********************************************************************/
bool	Distance::check_dim()
{
	//assert( DIMENSION == devia.size() );
	//assert( DIMENSION == mean.size() );

	//bool	is	=	DIMENSION == devia.size() && DIMENSION == mean.size();

	//return	is;
	return	true;		// 因為改成指標
}



/**********************************************************************
	計算樣張 
	跟cal_feature內容會很像 差在這邊會算平均標準差
/***********************************************************************/
void	Distance::cal_sample_feature( QImage img )
{
	assert(0);
}

/**********************************************************************
	計算 |u|
	會開根號
/***********************************************************************/
double		Distance::get_norm( int pix )
{
	double	value	=	get_norm_squ(pix);

	return	sqrt(value);
}



/**********************************************************************
	回傳 norm 的平方  |u|^2
	(有除上deviation)

		(f - m )^2
	_______________
		2 sigma^2


/***********************************************************************/
double		Distance::get_norm_squ( int pix )
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
	value	=	image.gray_channel( i, j ) - mean[0];	// 因為維度 = 1
	r		=	pow(value,2) / (2.0*pow(devia[0],2));	// 因為維度 = 1

	return	r;
}



/**********************************************************************
	計算兩個pixel的norm |u-v|
/***********************************************************************/
double		Distance::get_norm( int pix1, int pix2 )
{
	// 目前無內容
	assert(false);
	return	0;
}


/**********************************************************************
	回傳目前的feature type
/***********************************************************************/
MRF_FEATURE_TYPE	Distance::type()
{
	return	MRF_FEATURE_DISTANCE;
}



/**********************************************************************
	設定取樣範圍
/***********************************************************************/
void	Distance::set_anchor( int x, int y, int w, int h )
{
	anchor_x		=	x;
	anchor_y		=	y;
	anchor_width	=	w;
	anchor_height	=	h;
}






/**********************************************************************
	回傳維度
/***********************************************************************/
int		Distance::dim()
{
	return	DIMENSION;
}


/**********************************************************************
	回傳deviation的determine  
	(因為對照的論文是加上 |Sigma| 不確定是norm還是det)
	注意這邊是回傳平方!!
/***********************************************************************/
double		Distance::get_devia_det()
{
	return	devia[0] * devia[0];
}




/**********************************************************************
	operator = 
/***********************************************************************/
void	Distance::operator	=	( const Distance &dt )
{
	mrfFeatureAbstract::operator = (dt);

	anchor_x		=	dt.anchor_x;
	anchor_y		=	dt.anchor_y;
	anchor_width	=	dt.anchor_width;
	anchor_height	=	dt.anchor_height;
}

