#ifndef	_MRF_H
#define _MRF_H

//#include "Energy_test.h"
#include "energyStandard.h"
#include "gaussfilter.h"
#include "../tools.h"
#include <vector>
#include <ctime>
#include <iomanip>
#include <QImage>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_01.hpp>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Markov Random Field 物件
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


class	MarkovRandomField
{
	private:


		std::vector<mrfEnergyAbstract*>		vpEnergy;				// Energy pointer vector
		std::vector<bMatrixDouble>			D;						// 紀錄 width * height * label 的 三度空間 map        屬於暫存性質 是核心
																	// 改用vector - matrix 架構  (原本設計會在解構的地方跑很慢)  
																	// E-M 一樣使用此 D

		double				*P_lambda;								// E-M 使用 P(lambda)  論文中說類似weight
		double				*K_lambda;								// E-M 使用 K_lambda   


		bMatrixInt			label_map;								// 存放label資訊
		QImage				image;									// 為了E-M method, 還是加進來了

	public:
		MarkovRandomField();										// 建構
		MarkovRandomField( mrfEnergyAbstract &e );					// 建構
		~MarkovRandomField();										// 解構

		void				ICM_exec( QImage img );					// 執行 ICM mode
		void				ICM8_exec( QImage img);					// 考慮 8-neighbor
		void				SimuAnneal_exec( QImage img );			// 執行模擬退火法
		void				SimuAnneal8_exec( QImage img );			// 考慮 8-neighbor

		void				Gibbs_exec( QImage img );				// 執行 gibbs sampler  (跟EM效果類似)
		void				initial_state( int MAX_LABEL, int *counts, double *sum, double *sumsq );				// gibbs 初始化
		void				remove_observation( double data, int label, int *counts, double *sum, double *sumsq );	// gibbs 移除點
		void				add_observation( double data, int label, int *counts, double *sum, double *sumsq );		// gibbs 增加點
		void				update_estimates( int MAX_LABEL, double *prob, double *mean, double *sigma, int *counts, double *sum , double *sumsq );		// 更新估計結果
		double				dnorm( double x, double mean, double sigma );												// 求機率 (名稱用norm是因為論文寫的)
		double				dmix( int MAX_LABEL, double x, double *probs, double *mean, double *sigma );				// 求機率和 (類似normalized的Z)
		int					sample_group( int MAX_LABEL, double data, double *probs, double *mean, double *sigma );		// 取樣

		double				totalEnergy( bMatrixInt label );		// 根據label結果來計算能量
		double				singleEnergy( int pix, int lab );		// 計算單一點的energy
		double				dataEnergy();							// 計算能量 (E_data)
		double				smoothEnergy();							// 計算能量 (E_smooth)
		double				smoothEnergyOblique();					// 計算能量 (斜向)

		int					ij2pix( int i, int j );					// 將 (i,j) 座標轉成 pixel
		void				push_back( mrfEnergyAbstract &e );		// 增加新的energy

		void				EM_exec( QImage img );					// 執行 E-M 估計參數
		void				EM_init();								// 初始化 (決定幾個label, mean 跟 sigma 的初始值
		void				EM_init_distance();						// 根據距離來做初始

		void				cal_2_clique_hv( int i, int j );		// 計算水平垂直方向的 2-clique
		void				cal_2_clique_oblique( int i, int j );	// 計算斜向的2-clique
		void				update_label_map( int i, int j );		// 搜尋最小值 更新 label map
		void				init_D();								// 初始化 map D  (三度空間的map 算是暫存性質 是核心)

		void				E_Step();								// E-M 的 Expectation 階段
		void				M_Step();								// E-M 的 Maximization 階段

		void				cal_K_lambda();							// E-M 計算 K_(lambda)
		void				cal_P_lambda();							// E-M 計算 P(lambda)
		void				cal_EM_mean();							// E-M 計算 mean
		void				cal_EM_devia();							// E-M 計算標準差

};


#endif