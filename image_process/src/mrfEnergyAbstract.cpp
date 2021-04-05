#include "mrfEnergyAbstract.h"




/********************************************************************
	建構
/*********************************************************************/
mrfEnergyAbstract::mrfEnergyAbstract()
{
	feature		=	NULL;
}



/********************************************************************
	建構
	這邊的模式是supervised
	ft會先算好樣張,在這邊去計算目標的feature
/*********************************************************************/
mrfEnergyAbstract::mrfEnergyAbstract( mrfFeatureAbstract &ft, QImage img )
{
	//assert( ft.get_image() == img );

	image	=	img;

	// 根據type決定feature類型
	switch( ft.type() )
	{
		case	MRF_FEATURE_MOMENT:
			feature		=	new	Moment( (Moment&)ft );
			break;

		case	MRF_FEATURE_DISTANCE:
			feature		=	new	Distance( (Distance&)ft );
			break;

		case	MRF_FEATURE_GABOR:
			feature		=	new	Gabor( (Gabor&)ft );
			break;

		case	MRF_FEATURE_COCCM:
			feature		=	new	COCCM( (COCCM&)ft );
			break;

		case	MRF_FEATURE_MRSAR:
			feature		=	new MRSAR( (MRSAR&)ft );
			break;

		default:
			assert(false);
	}

	//feature->cal_feature(image);	// 計算feature  移動到外面去計算
}


/********************************************************************
	計算 feature
/*********************************************************************/
void	mrfEnergyAbstract::cal_feature()
{
	assert( feature != NULL );

	feature->cal_feature(image);
}


/********************************************************************
	copy construct
/*********************************************************************/
mrfEnergyAbstract::mrfEnergyAbstract( const mrfEnergyAbstract &e )
{
	*this	=	e;
}



/********************************************************************
	解構
/*********************************************************************/
mrfEnergyAbstract::~mrfEnergyAbstract()
{
	delete	feature;
	feature		=	NULL;
}


/********************************************************************
	operator =
/*********************************************************************/
void	mrfEnergyAbstract::operator = ( const mrfEnergyAbstract &e )
{
	assert( e.feature != NULL );	// 如果走預設建構  不能走這邊

	switch( e.feature->type() )
	{
		case	MRF_FEATURE_MOMENT:
			feature		=	new	Moment( (Moment&)(*e.feature) );
			break;
		case	MRF_FEATURE_DISTANCE:
			feature		=	new	Distance( (Distance&)(*e.feature) );
			break;

		case	MRF_FEATURE_GABOR:
			feature		=	new Gabor( (Gabor&)(*e.feature) );
			break;

		case	MRF_FEATURE_COCCM:
			feature		=	new	COCCM( (COCCM&)(*e.feature) );
			break;

		case	MRF_FEATURE_MRSAR:
			feature		=	new MRSAR( (MRSAR&)(*e.feature) );
			break;

		default:
			assert(false);
	}

	image		=	e.image;
}



/********************************************************************
	回傳image
/*********************************************************************/
QImage		mrfEnergyAbstract::get_image()
{
	return	image;
}



/********************************************************************
	回傳圖片寬
/*********************************************************************/
int		mrfEnergyAbstract::get_img_width()
{
	return	image.width();
}


/********************************************************************
	回傳圖片高
/*********************************************************************/
int		mrfEnergyAbstract::get_img_height()
{
	return	image.height();
}


/********************************************************************
	設定圖片
/*********************************************************************/
void	mrfEnergyAbstract::set_image( QImage img )
{
	image	=	img;

	feature->set_image(image);
}




/********************************************************************
	回傳 feature type
/*********************************************************************/
MRF_FEATURE_TYPE	mrfEnergyAbstract::get_feature_type()
{
	assert( feature != NULL );

	return	feature->type();
}


/********************************************************************
	檢查跟feature的image是否相同
/*********************************************************************/
bool	mrfEnergyAbstract::check_image()
{
	assert( feature != NULL );

	bool	is;
	is	=	(image == feature->get_image());

	return	is;
}


/********************************************************************
	檢查feature是否初始化
/*********************************************************************/
bool	mrfEnergyAbstract::is_feature()
{
	if( feature == NULL )	return	false;
	else					return	true;
}



/********************************************************************
	回傳feature的dim
/*********************************************************************/
int		mrfEnergyAbstract::dim()
{
	assert( feature != NULL );

	return	feature->dim();
}


/********************************************************************
	取得該pixel的feature
/*********************************************************************/
void	mrfEnergyAbstract::get_feature( int pix, double *_f )
{
	assert( feature != NULL );

	feature->get_feature( pix, _f );
}



/********************************************************************
	設定平均 (到feature)
/*********************************************************************/
void	mrfEnergyAbstract::set_mean( double *_m )
{
	assert( feature != NULL );

	feature->set_mean( _m );
}


/********************************************************************
	回傳feature平均
/*********************************************************************/
void	mrfEnergyAbstract::get_mean( double *_m )
{
	assert( feature != NULL );

	feature->get_mean(_m);
}



/********************************************************************
	設定標準差
/*********************************************************************/
void		mrfEnergyAbstract::set_devia( double *_d )
{
	assert( feature != NULL );

	feature->set_devia(_d);
}


/********************************************************************
	回傳標準差
/*********************************************************************/
void	mrfEnergyAbstract::get_devia( double *_d )
{
	assert( feature != NULL );

	feature->get_devia(_d);
}



/********************************************************************
	檢查 dimension
/*********************************************************************/
bool	mrfEnergyAbstract::check_dim()
{
	assert( feature != NULL );

	return	feature->check_dim();
}



/********************************************************************
	輸出資料 (測試用)
/*********************************************************************/
void	mrfEnergyAbstract::output()
{
	QImage	img(640, 640, QImage::Format_RGB888);

	for( int i = 0; i < 640; i++ )
		for( int j = 0; j < 640; j++ )
			img.gray_channel( i, j, ((Moment*)feature)->get_data(i,j) );
	img.save("../TEMP/test.bmp");
}
