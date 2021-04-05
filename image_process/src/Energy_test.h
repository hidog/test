#ifndef _ENERGY_TEST_H
#define _ENERGY_TEST_H

#include "mrfEnergyAbstract.h"
#include "moment.h"
#include <QImage>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	測試用energy function
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

class	EnergyTest : public mrfEnergyAbstract
{
	private:

	public:
		EnergyTest( mrfFeatureAbstract &ft, QImage img );				// 建構
		EnergyTest( const EnergyTest &e );								// 複製建構
		~EnergyTest();													// 解構

		// virtual 區
		double				E_data( int pix, int lab );								// 1-clique
		double				E_smooth( int pix1, int pix2, int lab1, int lab2 );		// 2-clique
		double				Prob( int pix, int lab );								// 回傳機率密度函數值
		MRF_ENERGY_TYPE		type();													// 回傳自己是哪種類型

		// 非 virtual 區
		void	operator = ( const EnergyTest &e );


};



#endif