#ifndef _ENERGY_STANDARD_H
#define _ENERGY_STANDARD_H

#include "mrfEnergyAbstract.h"
#include "moment.h"
#include <QImage>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	標準 Energy  (參考基礎的MRF做法)
	neighbor 採用 +-Betta
	single   採用 Gaussian
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

class	EnergyStandard : public mrfEnergyAbstract
{
	private:

	public:
		EnergyStandard( mrfFeatureAbstract &ft, QImage img );				// 建構
		EnergyStandard( const EnergyStandard &e );							// 複製建構
		~EnergyStandard();													// 解構

		// virtual 區
		double				E_data( int pix, int lab );								// 1-clique
		double				E_smooth( int pix1, int pix2, int lab1, int lab2 );		// 2-clique
		double				Prob( int pix, int lab );								// 回傳機率密度函數值
		MRF_ENERGY_TYPE		type();													// 回傳自己是哪種類型

		// 非 virtual 區
		void	operator = ( const EnergyStandard &e );

};



#endif