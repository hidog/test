#include "GABOR.h"

using namespace std;



/***************************************************************************************
	建構
/****************************************************************************************/
Gabor::Gabor( QImage imagePass )
	:	mrfFeatureAbstract( imagePass, dim() )
{
	sigma	=	sqrt( 2.0*win_size*win_size/12.46882145143674291132554261896 );		// 12.46882145143674291132554261896 參考網路資料

	sample	=	imagePass;
	image	=	imagePass;

	cal_wh( sample );	// 計算樣張 求出參數 wave, theta
}
	


/***************************************************************************************
	計算樣張 跟cal_feature內容會很像 差在這邊會算平均標準差

/****************************************************************************************/
void	Gabor::cal_sample_feature( QImage img )
{
	assert(false);
}


/***************************************************************************************
	建構
/****************************************************************************************/
Gabor::Gabor()
	:	mrfFeatureAbstract( dim() )
{
	sigma	=	sqrt( 2.0*win_size*win_size/12.46882145143674291132554261896 );		// 12.46882145143674291132554261896 參考網路資料
}


/**********************************************************************
	copy construct
/***********************************************************************/
Gabor::Gabor( const Gabor &gb )
	:	mrfFeatureAbstract(gb)
{
	sigma	=	sqrt( 2.0*win_size*win_size/12.46882145143674291132554261896 );		// 12.46882145143674291132554261896 參考網路資料
	*this	=	gb;
}



/***************************************************************************************
	解構
/****************************************************************************************/
Gabor::~Gabor()
{}




/***************************************************************************************
	產生real part的filter
/****************************************************************************************/
bMatrixDouble	Gabor::RealPart( double wave, double theta )
{
	bMatrixDouble	map( win_size*2+1, win_size*2+1 );

	int		i,	j;

	double	x_theta,	y_theta;
	double	cos_part,	exp_part;
	double	value;
	double	sum;

	for( i = -win_size; i <= win_size; i++ )
		for( j = -win_size; j <= win_size; j++ )
		{
			x_theta		=	i * cos(theta) + j * sin(theta);		
			y_theta		=	-i * sin(theta) + j * cos(theta);
			cos_part	=	cos( /*2.0*PI**/x_theta/wave );		// 參考別人的作法 拿掉2*PI
			value		=	- ( x_theta*x_theta + y_theta*y_theta ) / ( 2.0*sigma*sigma );
			exp_part	=	exp( value );
	
			map( i + win_size, j + win_size )	=	cos_part*exp_part;
		}
	return	map;
}

/***************************************************************************************
	產生image part的filter
/****************************************************************************************/
bMatrixDouble	Gabor::ImagePart( double wave, double theta )
{
	bMatrixDouble	map( win_size*2+1, win_size*2+1 );

	int		i,	j;

	double	x_theta,	y_theta;
	double	sin_part,	exp_part;
	double	value;
	double	sum;

	for( i = -win_size; i <= win_size; i++ )
		for( j = -win_size; j <= win_size; j++ )
		{
			x_theta		=	i * cos(theta) + j * sin(theta);		
			y_theta		=	-i * sin(theta) + j * cos(theta);
			sin_part	=	sin( /*2.0*PI**/x_theta/wave );		// 參考別人的作法 拿掉2*PI
			value		=	- ( x_theta*x_theta + y_theta*y_theta ) / ( 2.0*sigma*sigma );
			exp_part	=	exp( value );
	
			map( i + win_size, j + win_size )	=	sin_part*exp_part;
		}
	return	map;
}




/***************************************************************************************
	計算平均
/****************************************************************************************/
void	Gabor::cal_mean()
{
	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	size	=	(width-2*win_size) * (height-2*win_size);

	double	sum	=	0;
	int		i,	j;

	for( i = win_size; i < width-win_size; i++ )
		for( j = win_size; j < height-win_size; j++ )
			sum		+=	map_tmp(i,j);
	mean_tmp	=	sum / size;
}


/***************************************************************************************
	計算標準差
/****************************************************************************************/
void	Gabor::cal_devia()
{
	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	size	=	(width-win_size*2) * (height-win_size*2);

	double	sum	=	0;
	int		i,	j;

	for( i = win_size; i < width-win_size; i++ )
		for( j = win_size; j < height-win_size; j++ )
			sum		+=	pow(   map_tmp(i,j) - mean_tmp , 2 );
	devia_tmp	=	sqrt( sum / size );
}



/***************************************************************************************
	給定 wave, theta 去計算 feature
/****************************************************************************************/
void	Gabor::cal_feature2( double _wave, double _theta )
{
	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	size	=	(win_size*2+1) * (win_size*2+1);

	int		i,	j;
	int		dx,	dy;
	double	value,	sum;

	bMatrixDouble	real_map;

	// 產生real part
	real_map	=	RealPart( _wave, _theta );

	for( i = 0; i < width; i++ )
	{
		for( j = 0; j < height; j++ )
		{
			sum		=	0;

			for( dx = -win_size; dx <= win_size; dx++ )
				for( dy = -win_size; dy <= win_size; dy++ )
				{
					if( dx + i >= 0 && dx + i < width && dy + j >= 0 && dy + j < height )
					{
						value	=	real_map( dx+win_size, dy+win_size ) * image.gray_channel( dx+i, dy+j );
						sum		+=	value;
					}
				}

			map_tmp( i, j )		=	sum / size; //100*tanh(0.5*sum);  //0.5*(1 + tanh((sum-200)/300));

		}
	}

	cal_mean();
	cal_devia();
	//cal_max();
}





/**********************************************************************
	計算 |u|
	會開根號
/***********************************************************************/
double		Gabor::get_norm( int pix )
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
double		Gabor::get_norm_squ( int pix )
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
	value	=	map(i,j) - mean[0];						// 因為維度 = 1
	r		=	pow(value,2) / (2.0*pow(devia[0],2));	// 因為維度 = 1

	return	r;
}

/**********************************************************************
	回傳該pixel的feature
/***********************************************************************/
void	Gabor::get_feature( int pix, double *_f )
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	assert( width > 0 && height > 0 );

	int		i,	j;

	i	=	pix / height;
	j	=	pix % height;

	assert( i >= 0 && i < width && j >= 0 && j < height );

	// 實際上有除上deviation!!
	_f[0]	=	map( i, j );
}


/**********************************************************************
	檢查維度
/***********************************************************************/
bool	Gabor::check_dim()
{
	return	true;		// 因為改成指標
}




/**********************************************************************
	回傳deviation的determine  
	(因為對照的論文是加上 |Sigma| 不確定是norm還是det)
	注意這邊是回傳平方!!
/***********************************************************************/
double		Gabor::get_devia_det()
{
	return	devia[0] * devia[0];
}


/**********************************************************************
	回傳維度
/***********************************************************************/
int		Gabor::dim()
{
	return	DIMENSION;
}


/**********************************************************************
	回傳目前的feature type
/***********************************************************************/
MRF_FEATURE_TYPE	Gabor::type()
{
	return	MRF_FEATURE_GABOR;
}


/**********************************************************************
	計算兩個pixel的norm |u-v|
/***********************************************************************/
double		Gabor::get_norm( int pix1, int pix2 )
{
	// 目前無內容
	assert(false);
	return	0;
}


/***************************************************************************************
	輸出檔案
/****************************************************************************************/
void	Gabor::output_txt()
{
	assert(0);
}



/***************************************************************************************
	計算feature
/****************************************************************************************/
void	Gabor::cal_feature( QImage img )
{
	image	=	img;

	const int	width	=	image.width();
	const int	height	=	image.height();

	map_tmp.resize(width,height);

	cout << "energy !! wave = " << wave << " theta = " << theta << endl;
	cal_feature2( wave, theta );

	map		=	map_tmp;	// 計算完的資料暫存在map_tmp


	/*img		=	Matrix2Image( map, M2I::normalized );
	char	filename[100];
	sprintf( filename, "../TEMP/%d_%d.bmp", (int)(wave*100), (int)(theta*100) );
	img.save(filename);*/
}


/***************************************************************************************
	計算feature
/****************************************************************************************/
void	Gabor::cal_wh( QImage img )
{
	image	=	img;

	const int	width	=	image.width();
	const int	height	=	image.height();

	double	_wave;
	double	_theta;

	//double	inf_max		=	999999;		// 因為我們要找的是 每一次的max 中最小的  (要小心這邊的邏輯)
	double	min_devia	=	999999;

	map.resize( width, height );
	map_tmp.resize( width, height );

	for( _theta = 0; _theta < PI; _theta += 0.25*PI )		
		for( _wave = 0.1; _wave <= 3; _wave += 0.5 )
		{
			cout << "*";
			cal_feature2( _wave, _theta );

			//if( inf_max > max_tmp )
			if( min_devia > devia_tmp )
			{
				//inf_max		=	max_tmp;
				min_devia	=	devia_tmp;
				//cout << min_devia << endl;
				mean[0]		=	mean_tmp;
				devia[0]	=	devia_tmp;
				map			=	map_tmp;
				theta		=	_theta;
				wave		=	_wave;
			}
		}

	if( devia[0] < 0.00001 )
		devia[0]	=	0.00001;		// 強制一定要給 deviation

	cout << "\ntheta = " << theta << " wave = " << wave << endl;
	cout << "mean = " << mean[0] << " devia = " << devia[0] << endl;

	/*img		=	Matrix2Image( map, M2I::normalized );
	char	filename[100];
	sprintf( filename, "../TEMP/%d_%d_abc.bmp", (int)(wave*100), (int)(theta*100) );
	img.save(filename);*/
}



/***************************************************************************************
	計算最大值 (絕對值)
/****************************************************************************************/
void	Gabor::cal_max()
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	double	max	=	0;
	int		i,	j;

	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			if( max < fabs( map_tmp(i,j) ) )
				max	=	fabs( map_tmp(i,j) );
		}
}



/***************************************************************************************
	測試切割
/****************************************************************************************/
void	Gabor::segmentation( QImage img )
{
	assert(0);
}


/**********************************************************************
	operator = 
/***********************************************************************/
void	Gabor::operator	=	( const Gabor &gb )
{
	mrfFeatureAbstract::operator = (gb);

	wave	=	gb.wave;	
	theta	=	gb.theta;	
	map		=	gb.map;	
	sigma	=	gb.sigma;
}





