#include "Energy_test.h"



/**********************************************************************
	建構
/***********************************************************************/
EnergyTest::EnergyTest( mrfFeatureAbstract &ft, QImage img )
	:	mrfEnergyAbstract( ft, img )
{}


/**********************************************************************
	複製建構
/***********************************************************************/
EnergyTest::EnergyTest( const EnergyTest &e )
	:	mrfEnergyAbstract( e )
{
	*this	=	e;
}


/**********************************************************************
	解構
/***********************************************************************/
EnergyTest::~EnergyTest()
{}


/**********************************************************************
	operator =
/***********************************************************************/
void	EnergyTest::operator = ( const EnergyTest &e )
{
	mrfEnergyAbstract::operator = (e);

	// 目前沒內容
}



/**********************************************************************
	smooth Energy
/***********************************************************************/
double		EnergyTest::E_smooth( int pix1, int pix2, int lab1, int lab2 )
{
	const double	weight	=	0.25;	// 加權 以後再來思考怎麼放

	//double	value	=	feature->get_norm( pix1, pix2 );

	// 這個是 2 label 版本
	/*if( lab1 == lab2 )
	{
		value	=	0.25*value;
		if( value > 1 )
			value	=	1;
		//value	*=	1.0;
		//value	=	1 - exp(value);
	}
	else
	{
		value	=	-0.25*value + 1;
		if( value < 0 )
			value	=	0;
		//value	*=	-1.0;
		//value	=	exp(value);
	}*/

	//return	weight * value;
	if( lab1 == lab2 )	return	-weight;
	else				return	weight;
}


/**********************************************************************
	data Energy
/***********************************************************************/
double		EnergyTest::E_data( int pix, int lab )
{
	double	value	=	feature->get_norm_squ( pix );

	//if( lab == 1 )
	//	value	=	feature2->get_norm(pix);

	// 先做 2 label
	/*if( lab == 0 )
	{
		value	=	0.25*value;
		if( value > 1 )
			value	=	1;
		//value	*=	1.0;
		//value	=	1 - exp(value);
	}
	else
	{
		value	=	-0.25*value + 1;
		if( value < 0 )
			value	=	0;
		//value	*=	-1.0;
		//value	=	exp(value);
	}*/

	return	value;
}


/**********************************************************************
	回傳自己是哪種類型
/***********************************************************************/
MRF_ENERGY_TYPE		EnergyTest::type()
{
	return	MRF_ENERGY_TEST;
}


/**********************************************************************
	回傳機率密度函數值
/***********************************************************************/
double	EnergyTest::Prob( int pix, int lab )
{
	double	value	=	feature->get_norm_squ(pix);
	double	devia	=	feature->get_devia_det();

	double	r_data;

	r_data	=	exp( -value ) / (devia*sqrt(2.0*PI));

	return	r_data;
}