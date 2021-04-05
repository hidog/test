#include "energyStandard.h"



/**********************************************************************
	建構
/***********************************************************************/
EnergyStandard::EnergyStandard( mrfFeatureAbstract &ft, QImage img )
	:	mrfEnergyAbstract( ft, img )
{
	feature->cal_feature(img);

}


/**********************************************************************
	複製建構
/***********************************************************************/
EnergyStandard::EnergyStandard( const EnergyStandard &e )
	:	mrfEnergyAbstract( e )
{
	*this	=	e;
}


/**********************************************************************
	解構
/***********************************************************************/
EnergyStandard::~EnergyStandard()
{}




/**********************************************************************
	= operator
/***********************************************************************/
void	EnergyStandard::operator = ( const EnergyStandard &e )
{
	mrfEnergyAbstract::operator = (e);

	// 目前沒內容
}



/**********************************************************************
	smooth Energy
/***********************************************************************/
double		EnergyStandard::E_smooth( int pix1, int pix2, int lab1, int lab2 )
{
	assert( is_feature() );
	assert( check_image() );

	const double	weight	=	0.25;	// 加權 以後再來思考怎麼修改  目前是調校出來的值

	if( lab1 == lab2 )	return	-weight;
	else				return	weight;
}


/**********************************************************************
	data Energy
/***********************************************************************/
double		EnergyStandard::E_data( int pix, int lab )
{
	//assert( is_feature() );
	//assert( check_image() );

	double	value	=	feature->get_norm_squ( pix );
	return	value ;
}


/**********************************************************************
	回傳自己是哪種類型
/***********************************************************************/
MRF_ENERGY_TYPE		EnergyStandard::type()
{
	return	MRF_ENERGY_STANDARD;
}


/**********************************************************************
	回傳機率密度函數值
/***********************************************************************/
double	EnergyStandard::Prob( int pix, int lab )
{
	assert( is_feature() );
	assert( check_image() );

	double	value	=	feature->get_norm_squ(pix);
	double	devia	=	feature->get_devia_det();	// 注意這邊回傳的是平方
	int		dim		=	feature->dim();
	double	r_data;

	assert( devia > 0 );		// 因為正定...

	r_data	=	exp( -value ) / sqrt( pow(2.0*PI,dim)*devia );	// 放進去根號是因為平方

	return	r_data;
}