#include "mrf.h"

#include <iostream>
#include <iomanip>

using namespace std;


/********************************************************************
	建構
/*********************************************************************/
MarkovRandomField::MarkovRandomField()
{
	P_lambda	=	NULL;
	K_lambda	=	NULL;
}



/********************************************************************
	建構
/*********************************************************************/
MarkovRandomField::MarkovRandomField( mrfEnergyAbstract &e )
{
	P_lambda	=	NULL;
	K_lambda	=	NULL;

	// 初始化能量
	switch( e.type() )
	{
		//case MRF_ENERGY_TEST:
		//	vpEnergy.push_back( new EnergyTest((EnergyTest&)e) );
		//	break;

		case MRF_ENERGY_STANDARD:
			vpEnergy.push_back( new EnergyStandard((EnergyStandard&)e) );
			break;
		default:
			assert(false);
	}

	const int	width	=	vpEnergy[0]->get_img_width();		// 因為是建構函數 只會有一個物件
	const int	height	=	vpEnergy[0]->get_img_height();

	// 初始化 label_map
	label_map.resize( width, height );
}


/********************************************************************
	解構
/*********************************************************************/
MarkovRandomField::~MarkovRandomField()
{
	delete [] P_lambda;		P_lambda	=	NULL;
	delete [] K_lambda;		K_lambda	=	NULL;
}


/********************************************************************
	計算水平垂直方向的 2-clique
/*********************************************************************/
void	MarkovRandomField::cal_2_clique_hv( int i, int j )
{
	const int	width		=	image.width();
	const int	height		=	image.height();
	const int	MAX_LABEL	=	vpEnergy.size();				// 最大label個數

	int		label;
	int		pix1,	pix2,	lab2;

	// 上 -------------------------------------------------------------------------------------------------------
	if( j-1 >= 0 )
	{
		pix1			=	ij2pix( i, j );
		pix2			=	ij2pix( i, j-1 );
		lab2			=	label_map( i, j-1 );

		for( label = 0; label < MAX_LABEL; label++ )
			D[label](i,j)	+=	vpEnergy[label]->E_smooth( pix1, pix2, label, lab2 );
	}
	// 下 -------------------------------------------------------------------------------------------------------
	if( j+1 < height )
	{
		pix1			=	ij2pix( i, j );
		pix2			=	ij2pix( i, j+1 );
		lab2			=	label_map( i, j+1 );

		for( label = 0; label < MAX_LABEL; label++ )
			D[label](i,j)	+=	vpEnergy[label]->E_smooth( pix1, pix2, label, lab2 );
	}
	// 左 -------------------------------------------------------------------------------------------------------
	if( i-1 >= 0 )
	{
		pix1			=	ij2pix( i, j );
		pix2			=	ij2pix( i-1, j );
		lab2			=	label_map( i-1, j );

		for( label = 0; label < MAX_LABEL; label++ )
			D[label](i,j)	+=	vpEnergy[label]->E_smooth( pix1, pix2, label, lab2 );
	}
	// 右 -------------------------------------------------------------------------------------------------------
	if( i+1 < width )
	{
		pix1			=	ij2pix( i, j );
		pix2			=	ij2pix( i+1, j );
		lab2			=	label_map( i+1, j );

		for( label = 0; label < MAX_LABEL; label++ )
			D[label](i,j)	+=	vpEnergy[label]->E_smooth( pix1, pix2, label, lab2 );
	}
}

/********************************************************************
	搜尋最小值 
	更新 label map
/*********************************************************************/
void	MarkovRandomField::update_label_map( int i, int j )
{
	const int	MAX_LABEL	=	vpEnergy.size();		// 最大label個數

	int			min_id;
	int			label;
	double		min;

	// 搜尋 min
	min_id	=	0;
	min		=	D[0](i,j);    //D(i,j).data[0];
	for( label = 1; label < MAX_LABEL; label++ )
	{
		if( min > D[label](i,j) )
		{
			min_id	=	label;
			min		=	D[label](i,j);
		}
	}
	label_map(i,j)	=	min_id;
}



/********************************************************************
	初始化 map D  
	(三度空間的map 算是暫存性質 是核心)
/*********************************************************************/
void	MarkovRandomField::init_D()
{
	const int	width		=	image.width();
	const int	height		=	image.height();
	const int	MAX_LABEL	=	vpEnergy.size();				// 最大label個數

	int		i,	j;
	int		label;
	int		pix;

	for( i = 0; i < width; i++ )
	{
		for( j = 0; j < height; j++ )
		{
			pix	=	ij2pix( i, j );

			for( label = 0; label < MAX_LABEL; label++ )
				D[label](i,j)	=	vpEnergy[label]->E_data( pix, label );	// 無負號 因為跟自己距離很近 value會很低  (轉換成Probability才需要加上負號)
		}
	}
}



/********************************************************************
	執行 ICM mode
/*********************************************************************/
void	MarkovRandomField::ICM_exec( QImage img )
{
	image	=	img;

	int		i,	j,	n;
	int		label;

	// 重新設定一次圖片
	for( i = 0; i < vpEnergy.size(); i++ )
		vpEnergy[i]->set_image(image);

	const int	MAX_ITER	=	1000;
	const int	width		=	vpEnergy[0]->get_img_width();
	const int	height		=	vpEnergy[0]->get_img_height();
	const int	MAX_LABEL	=	vpEnergy.size();				// 最大label個數

	label_map.resize( width, height );
	label_map.clear();

	bMatrixInt	old_map		=	label_map;		// 舊的 label map, 判斷迭代停止使用

#ifdef _DEBUG		// 檢查圖片是否相同
	for( label = 0; label < MAX_LABEL; label++ )
	{
		assert( vpEnergy[label]->get_image() == image );
		assert( vpEnergy[label]->check_image() );
	}
#endif

	D.resize(MAX_LABEL);							// 總共有幾個label
	for( i = 0; i < MAX_LABEL; i++ )				// 初始化  迭代的時候應該要給初始值
		D[i].resize( width, height );

	for( n = 0; n < MAX_ITER; n++ )
	{	
		cout << "itr = " << setw(3) << n << " energy = " << dataEnergy() + smoothEnergy() << endl;

		// 先產生整個 3D map的資料 每次都需要重新產生
		init_D();

		// 針對2-clique(四向)去求energy
		for( i = 0; i < width; i++ )
		{
			for( j = 0; j < height; j++ )
			{
				cal_2_clique_hv( i, j );		// 計算水平 垂直的 2-clique
				update_label_map( i, j );		// 更新 label map
			}
		}

		// 利用label map決定是否要停止迭代
		if( old_map == label_map )		break;
		old_map		=	label_map;
	
	} // end MAX_ITR

	cout << "end energy = " << dataEnergy() + smoothEnergy() << endl;

	// 輸出結果
	QImage	img( width, height, QImage::Format_RGB888 );
	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			label	=	label_map(i,j);
			img.setPixel( i, j, (label*761838257287)%193707721 );
		}

	char	filename[100];
	sprintf( filename, "%s/output.bmp", TMP_PATH );
	img.save(QString(filename));
	cout << "output finish.\n";
}



/********************************************************************
	計算斜向的2-clique
/*********************************************************************/
void	MarkovRandomField::cal_2_clique_oblique( int i, int j )
{
	const int		width		=	image.width();
	const int		height		=	image.height();
	const int		MAX_LABEL	=	vpEnergy.size();				// 最大label個數
	const double	weight		=	1.0 / sqrt(2.0);				

	int		label;
	int		pix1,	pix2,	lab2;

	// 左上
	if( i-1 >= 0 && j-1 >= 0 )
	{
		pix1			=	ij2pix( i, j );
		pix2			=	ij2pix( i-1, j-1 );
		lab2			=	label_map( i-1, j-1 );

		for( label = 0; label < MAX_LABEL; label++ )
			D[label](i,j)	+=	weight * vpEnergy[label]->E_smooth( pix1, pix2, label, lab2 );
	}
	// 左下
	if( i-1 >= 0 && j+1 < height )
	{
		pix1			=	ij2pix( i, j );
		pix2			=	ij2pix( i-1, j+1 );
		lab2			=	label_map( i-1, j+1 );

		for( label = 0; label < MAX_LABEL; label++ )
			D[label](i,j)	+=	weight * vpEnergy[label]->E_smooth( pix1, pix2, label, lab2 );
	}
	// 右上
	if( i+1 < width && j-1 >= 0 )
	{
		pix1			=	ij2pix( i, j );
		pix2			=	ij2pix( i+1, j-1 );
		lab2			=	label_map( i+1, j-1 );

		for( label = 0; label < MAX_LABEL; label++ )
			D[label](i,j)	+=	weight * vpEnergy[label]->E_smooth( pix1, pix2, label, lab2 );
	}
	// 右下
	if( i+1 < width && j+1 < height )
	{
		pix1			=	ij2pix( i, j );
		pix2			=	ij2pix( i+1, j+1 );
		lab2			=	label_map( i+1, j+1 );

		for( label = 0; label < MAX_LABEL; label++ )
			D[label](i,j)	+=	weight * vpEnergy[label]->E_smooth( pix1, pix2, label, lab2 );
	}
}




/********************************************************************
	執行 ICM mode
	考慮 8-neighbor
/*********************************************************************/
void	MarkovRandomField::ICM8_exec( QImage img )
{
	image	=	img;

	int			i,	j,	n;
	int			label;
	double		min;

	// 重新設定一次圖片
	for( i = 0; i < vpEnergy.size(); i++ )
		vpEnergy[i]->set_image(image);

	const int	MAX_ITER	=	1000;
	const int	width		=	image.width();					// vpEnergy[0]->get_img_width();
	const int	height		=	image.height();					//vpEnergy[0]->get_img_height();
	const int	MAX_LABEL	=	vpEnergy.size();				// 最大label個數

	label_map.resize( width, height );
	label_map.clear();

	bMatrixInt	old_map		=	label_map;		// 上一個狀態的label map  判斷迭代是否停止使用

#ifdef _DEBUG		// 檢查圖片是否相同
	for( label = 0; label < MAX_LABEL; label++ )
	{
		assert( vpEnergy[label]->get_image() == image );
		assert( vpEnergy[label]->check_image() );
	}
#endif

	D.resize(MAX_LABEL);							// 總共有幾個label
	for( i = 0; i < MAX_LABEL; i++ )				// 初始化  迭代的時候應該要給初始值
		D[i].resize( width, height );

	for( n = 0; n < MAX_ITER; n++ )
	{	
		cout << "itr = " << setw(3) << n << endl;//" energy = " << dataEnergy() + smoothEnergy() + smoothEnergyOblique() << endl;

		// 先產生整個 3D map的資料 每次都需要重新產生
		init_D();

		// 針對2-clique(四向)去求energy
		for( i = 0; i < width; i++ )
		{
			for( j = 0; j < height; j++ )
			{
				cal_2_clique_hv( i, j );		// 計算水平 垂直的 2-clique
				cal_2_clique_oblique( i, j );	// 計算斜向的 2-clique
				update_label_map( i, j );		// 更新 label map
			}
		}

		// 利用label map決定是否要停止迭代
		if( old_map == label_map )		break;
		old_map		=	label_map;

	} // end MAX_ITR

	cout << "end energy = " << dataEnergy() + smoothEnergy() + smoothEnergyOblique() << endl;

	// 輸出結果
	QImage	img( width, height, QImage::Format_RGB888 );
	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			label	=	label_map(i,j);
			img.setPixel( i, j, (label*761838257287)%193707721 );
			//img.gray_channel( i, j, label*30 );
		}

	char	filename[100];
	sprintf( filename, "%s/output.bmp", TMP_PATH );
	img.save(QString(filename));
	cout << "output finish.\n";
}




/********************************************************************
	計算能量 (E_data)
/*********************************************************************/
double	MarkovRandomField::dataEnergy()
{
	const int	width	=	vpEnergy[0]->get_img_width();
	const int	height	=	vpEnergy[0]->get_img_height();

	int		i,		j;
	int		pix,	lab;
	double	sum		=	0;

	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			pix		=	ij2pix( i, j );
			lab		=	label_map( i, j );

			sum		+=	vpEnergy[0]->E_data( pix, lab ); 
		}

	return	sum;
}


/********************************************************************
	計算能量 (斜向)
/*********************************************************************/
double	MarkovRandomField::smoothEnergyOblique()
{
	const int		width	=	image.width();		//vpEnergy[0]->get_img_width();
	const int		height	=	image.height();		//vpEnergy[0]->get_img_height();
	const double	weight	=	1.0 / sqrt(2.0);	// 雖然不影響結果 但是要跟cal那邊做同步

	int		i,		j;
	int		pix1,	lab1,	pix2,	lab2;
	double	sum		=	0;

	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			pix1	=	ij2pix( i, j );
			lab1	=	label_map( i, j );

			// 左上
			if( i-1 >= 0 && j-1 >= 0 )
			{
				pix2	=	ij2pix( i-1, j-1 );
				lab2	=	label_map( i-1, j-1 );
				sum		+=	weight * vpEnergy[0]->E_smooth( pix1, pix2, lab1, lab2 );
			}

			// 左下
			if( i-1 >= 0 && j+1 < height )
			{
				pix2	=	ij2pix( i-1, j+1 );
				lab2	=	label_map( i-1, j+1 );
				sum		+=	weight * vpEnergy[0]->E_smooth( pix1, pix2, lab1, lab2 );
			}

			// 右上
			if( i+1 < width && j-1 >= 0 )
			{
				pix2	=	ij2pix( i+1, j-1 );
				lab2	=	label_map( i+1, j-1 );
				sum		+=	weight * vpEnergy[0]->E_smooth( pix1, pix2, lab1, lab2 );
			}

			// 右下
			if( i+1 < width && j+1 < height)
			{
				pix2	=	ij2pix( i+1, j+1 );
				lab2	=	label_map( i+1, j+1 );
				sum		+=	weight * vpEnergy[0]->E_smooth( pix1, pix2, lab1, lab2 );
			}
		}

	return	sum;
}


/********************************************************************
	計算能量 (E_smooth)
/*********************************************************************/
double	MarkovRandomField::smoothEnergy()
{
	const int	width	=	image.width();		//vpEnergy[0]->get_img_width();
	const int	height	=	image.height();		//vpEnergy[0]->get_img_height();

	int		i,		j;
	int		pix1,	lab1,	pix2,	lab2;
	double	sum		=	0;

	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			pix1	=	ij2pix( i, j );
			lab1	=	label_map( i, j );

			// 上
			if( j-1 >= 0 )
			{
				pix2	=	ij2pix( i, j-1 );
				lab2	=	label_map( i, j-1 );
				sum		+=	vpEnergy[0]->E_smooth( pix1, pix2, lab1, lab2 );
			}

			// 下
			if( j+1 < height )
			{
				pix2	=	ij2pix( i, j+1 );
				lab2	=	label_map( i, j+1 );
				sum		+=	vpEnergy[0]->E_smooth( pix1, pix2, lab1, lab2 );
			}

			// 左
			if( i-1 >= 0 )
			{
				pix2	=	ij2pix( i-1, j );
				lab2	=	label_map( i-1, j );
				sum		+=	vpEnergy[0]->E_smooth( pix1, pix2, lab1, lab2 );
			}

			// 右
			if( i+1 < width )
			{
				pix2	=	ij2pix( i+1, j );
				lab2	=	label_map( i+1, j );
				sum		+=	vpEnergy[0]->E_smooth( pix1, pix2, lab1, lab2 );
			}

		}

	return	sum;
}



/********************************************************************
	將 (i,j) 座標轉成 pixel
/*********************************************************************/
int		MarkovRandomField::ij2pix( int i, int j )
{
	const int	width	=	image.width();		//vpEnergy[0]->get_img_width();
	const int	height	=	image.height();		//vpEnergy[0]->get_img_height();

	int		pixel;

	pixel	=	i * height + j ;

	return	pixel;
}



/********************************************************************
	增加新的energy
/*********************************************************************/
void	MarkovRandomField::push_back( mrfEnergyAbstract &e )
{
	// 初始化能量
	switch( e.type() )
	{
		//case MRF_ENERGY_TEST:
		//	vpEnergy.push_back( new EnergyTest((EnergyTest&)e) );
		//	break;

		case MRF_ENERGY_STANDARD:
			vpEnergy.push_back( new EnergyStandard((EnergyStandard&)e) );
			break;

		default:
			assert(false);
	}
}






/********************************************************************
	E-M 的 Expectation 階段
/*********************************************************************/
void	MarkovRandomField::E_Step()
{
	const int	MAX_LABEL	=	vpEnergy.size();
	const int	width		=	image.width();
	const int	height		=	image.height();

	int		i,	j,	pix,	label;
	double	normal;			// 分母 normalized (轉成機率? 0~1 而不是原本會超過1的pdf)

	// Estimation
	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			pix		=	i * height + j;

			// 計算分母
			normal	=	0;
			for( label = 0; label < MAX_LABEL; label++ )
				normal	+=	P_lambda[label] * vpEnergy[label]->Prob(pix,label);

			for( label = 0; label < MAX_LABEL; label++ )
				D[label](i,j)	=	P_lambda[label] * vpEnergy[label]->Prob(pix,label) / normal;
		}
}




/********************************************************************
	E-M 計算 K_(lambdas)
/*********************************************************************/
void	MarkovRandomField::cal_K_lambda()
{
	const int	MAX_LABEL	=	vpEnergy.size();
	const int	width		=	image.width();
	const int	height		=	image.height();

	int		label,	i,	j;

	// 計算 K_lambda
	// 初始化
	for( label = 0; label < MAX_LABEL; label++ )
		K_lambda[label]	=	0;

	// 計算sum   (想說省掉一個變數sum)
	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			for( label = 0; label < MAX_LABEL; label++ )	
				K_lambda[label]	+=	D[label](i,j);
		}
}




/********************************************************************
	E-M 計算 P(lambda)
/*********************************************************************/
void	MarkovRandomField::cal_P_lambda()
{
	const int	MAX_LABEL	=	vpEnergy.size();
	const int	width		=	image.width();
	const int	height		=	image.height();
	const int	size		=	width * height;

	int		label;

	// 計算P_lambda
	for( label = 0; label < MAX_LABEL; label++ )
		P_lambda[label]		=	 K_lambda[label] / size;		// size = 論文的 D	

}








/********************************************************************
	E-M 計算 mean
/*********************************************************************/
void	MarkovRandomField::cal_EM_mean()
{
	const int	MAX_LABEL	=	vpEnergy.size();
	const int	width		=	image.width();
	const int	height		=	image.height();
	const int	dim			=	vpEnergy[0]->dim();

	int		label,	i,	j,	k,	pix;

#ifdef _DEBUG
	for( label = 0; label < MAX_LABEL - 1; label++ )
		assert( vpEnergy[label]->dim() == vpEnergy[label+1]->dim() );
#endif

	//sVectorDouble	vec(dim);
	//sVectorDouble	mean(dim);
	double	*mean	=	new double [dim];
	double	*vec	=	new double [dim];


	// 計算mean
	for( label = 0; label < MAX_LABEL; label++ )
	{
		// 歸零 這邊必須定義vector的operator = (int)
		for( k = 0; k < dim; k++ )
			mean[k]	=	0;
		//set_vector( mean, 0.0 );

		for( i = 0; i < width; i++ )
			for( j = 0; j < height; j++ )
			{
				pix		=	ij2pix( i, j );
				//mean	+=	D[label](i,j) * vpEnergy[label]->get_feature(pix);
				vpEnergy[label]->get_feature( pix, vec );
				for( k = 0; k < dim; k++ )
					mean[k]	+=	D[label](i,j) * vec[k];
			}

		for( k = 0; k < dim; k++ )
			mean[k]	/=	K_lambda[label];
		//mean	/=	K_lambda[label];

		vpEnergy[label]->set_mean(mean);	

#ifdef _DEBUG
		vpEnergy[label]->check_dim();		// 因為dimension不知道怎麼加入抽象物件 所以只好寫一個function特別來檢查
#endif
	}

	delete	[] mean;	mean	=	NULL;
	delete	[] vec;		vec		=	NULL;
}







/********************************************************************
	E-M 計算標準差
/*********************************************************************/
void	MarkovRandomField::cal_EM_devia()
{
	const int	MAX_LABEL	=	vpEnergy.size();
	const int	width		=	image.width();
	const int	height		=	image.height();
	const int	dim			=	vpEnergy[0]->dim();

	int		i,	j,	k,	label,	pix;

#ifdef _DEBUG
	for( label = 0; label < MAX_LABEL - 1; label++ )
		assert( vpEnergy[label]->dim() == vpEnergy[label+1]->dim() );
#endif

	//sVectorDouble	devia(dim);
	//sVectorDouble	value(dim);
	//sVectorDouble	v_tmp(dim);
	double	*devia		=	new double [dim];
	double	*feature	=	new double [dim];
	double	*v_tmp		=	new double [dim];
	double	*mean		=	new double [dim];

	// 計算標準差
	for( label = 0; label < MAX_LABEL; label++ )
	{
		//set_vector( devia, 0.0 );
		for( k = 0; k < dim; k++ )
			devia[k]	=	0;


		for( i = 0; i < width ; i++ )
			for( j = 0 ; j < height; j++ )
			{
				pix		=	ij2pix( i, j );
				vpEnergy[label]->get_feature( pix, feature );
				vpEnergy[label]->get_mean( mean );
		
				for( k = 0; k < dim; k++ )
				{
					v_tmp[k]	=	feature[k] - mean[k];
					devia[k]	+=	D[label](i,j) * pow( v_tmp[k], 2 );
				}

				//v_tmp	=	vpEnergy[label]->get_feature(pix) - vpEnergy[label]->get_mean();
				//devia	+=	D[label](i,j) * ( v_tmp*v_tmp );		// 這邊的乘法是點乘!!
			}

		//devia	/=	K_lambda[label];
		//devia	=	sqrt(devia);		// 論文上用平方後的東西 變異數  自己改成標準差 所以要加 sqrt !!

		for( k = 0; k < dim; k++ )
		{
			devia[k]	/=	K_lambda[label];
			devia[k]	=	sqrt( devia[k] );
		}


		vpEnergy[label]->set_devia(devia);

#ifdef _DEBUG
		vpEnergy[label]->check_dim();		// 因為dimension不知道怎麼加入抽象物件 所以只好寫一個function特別來檢查
#endif
	}
	
	delete	[] devia;		devia	=	0;
	delete	[] feature;		feature	=	0;
	delete	[] v_tmp;		v_tmp	=	0;
	delete	[] mean;		mean	=	0;
}



/********************************************************************
	E-M 的 Maximization 階段
/*********************************************************************/
void	MarkovRandomField::M_Step()
{
	// 計算 K_lambda
	cal_K_lambda();

	// 計算 P(lambda)
	cal_P_lambda();

	// 計算 mean (改成vector形式)
	cal_EM_mean();

	// 計算標準差
	cal_EM_devia();
}





/********************************************************************
	執行 E-M 估計參數
/*********************************************************************/
void	MarkovRandomField::EM_exec( QImage img )
{
	image	=	img;

	const int		MAX_ITR	=	1000;
	const int		width	=	image.width();
	const int		height	=	image.height();
	const int		size	=	width * height;
	const int		DIM		=	vpEnergy[0]->dim();
	const double	end_thr	=	0.00001;

	EM_init();	// init

	// 開始進行 E-M 過程
	const int	MAX_LABEL	=	vpEnergy.size();

	int		i,	j,	n;
	int		label;
	double	sum;

	//std::vector<std::vector<double>>	mean(MAX_LABEL),		devia(MAX_LABEL);
	//std::vector<std::vector<double>>	old_mean(MAX_LABEL),	old_devia(MAX_LABEL);

	double	**mean		=	new double* [MAX_LABEL];	// 為了加速改用指標 小心index不對造成的錯誤
	double	**devia		=	new double* [MAX_LABEL];
	double	**old_mean	=	new double* [MAX_LABEL];
	double	**old_devia	=	new double* [MAX_LABEL];

	for( i = 0; i < MAX_LABEL; i++ )
	{
		mean[i]			=	new double[DIM];
		devia[i]		=	new double[DIM];
		old_mean[i]		=	new double[DIM];
		old_devia[i]	=	new double[DIM];
	}


#ifdef _DEBUG		// 檢查圖片是否相同
	for( label = 0; label < MAX_LABEL; label++ )
	{
		assert( vpEnergy[label]->get_image() == image );
		assert( vpEnergy[label]->check_image() );
	}
#endif

	D.resize(MAX_LABEL);
	for( i = 0; i < MAX_LABEL; i++ )
		D[i].resize( width, height );

	label_map.resize(width,height);		// 原本不需要 為了顯示進度才加進來

	P_lambda	=	new double[MAX_LABEL];
	K_lambda	=	new double[MAX_LABEL];	// 在計算的過程中會給初始值
	for( i = 0; i < MAX_LABEL; i++ )
		P_lambda[i]	=	1.0 / MAX_LABEL;	// 初始化 P_lambda  用均分的方式 (weight sum = 1)

	cout << "initial...\n";

	// 設定舊的mean, devia
	for( i = 0; i < MAX_LABEL; i++ )
	{
		vpEnergy[i]->get_mean( old_mean[i] );
		vpEnergy[i]->get_devia( old_devia[i] );
		//old_mean[d]		=	vpEnergy[d]->get_mean();
		//old_devia[d]	=	vpEnergy[d]->get_devia();
	}
	// 開始進入迭代
	for( n = 0; n < MAX_ITR; n++ )
	{
		cout << "the " << n << " iteration...\n";

		// Estimation
		E_Step();

		// 搜尋最小值  (可以省略 為了看到過程才加進來
		double	max;
		int		max_id;
		for( i = 0; i < width; i++ )
			for( j = 0; j < height; j++ )
			{
				max		=	D[0](i,j);
				max_id	=	0;
				for( label = 1; label < MAX_LABEL; label++ )
				{
					if( max < D[label](i,j) )
					{
						max		=	D[label](i,j);
						max_id	=	label;
					}
				}
				label_map( i, j )	=	max_id;
			}
		// 顯示當前進度圖
		for( i = 0; i < width; i++ )
			for( j = 0; j < height; j++ )
			{
				img.setPixel( i, j, (label_map(i,j)*761838257287)%193707721 );
				//img.gray_channel( i, j, label_map(i,j)*30 );
			}
		img.save("../TEMP/output.bmp");



#ifdef _DEBUG	// 檢查機率合是否等於 1
		for( i = 0; i < width; i++ )
			for( j = 0; j < height; j++ )
			{
				double	tmp_sum	=	0;
				for( label = 0; label < MAX_LABEL; label++ )
					tmp_sum	+=	D[label](i,j);
				assert( fabs(tmp_sum-1.0) < 0.0001 );
			}
#endif

		// Maximization
		M_Step();

		// 檢查是否要結束迴圈
		for( i = 0; i < MAX_LABEL; i++ )
		{
			vpEnergy[i]->get_mean( mean[i] );
			vpEnergy[i]->get_devia( devia[i] );
			//mean[d]		=	vpEnergy[d]->get_mean();
			//devia[d]	=	vpEnergy[d]->get_devia();
		}
		sum		=	0;
		for( i = 0; i < MAX_LABEL; i++ )
			for( j = 0; j < DIM; j++ )
				sum		+=	fabs(mean[i][j] - old_mean[i][j]) + fabs(devia[i][j] - old_devia[i][j] );
		if( sum < end_thr )		break;

		// 輸出結果
		cout << "mean = " << endl;
		for( i = 0; i < MAX_LABEL; i++ )
			for( j = 0; j < DIM; j++ )
				cout << mean[i][j] << " ";
		cout << "\ndevia = " << endl;
		for( i = 0; i < MAX_LABEL; i++ )
			for( j = 0; j < DIM; j++ )
				cout << devia[i][j] << " ";
		cout << endl;

		// 更新
		for( i = 0; i < MAX_LABEL; i++ )
			for( j = 0; j < DIM; j++ )
			{
				old_mean[i][j]		=	mean[i][j];
				old_devia[i][j]		=	devia[i][j];
			}
		//old_mean	=	mean;
		//old_devia	=	devia;

	}

	// 輸出結果
	cout << "end ...\n";
	cout << "mean = " << endl;
	for( i = 0; i < MAX_LABEL; i++ )
		for( j = 0; j < DIM; j++ )
			cout << mean[i][j] << " ";
	cout << "\ndevia = " << endl;
	for( i = 0; i < MAX_LABEL; i++ )
		for( j = 0; j < DIM; j++ )
			cout << devia[i][j] << " ";
	cout << endl;

	delete	[] P_lambda;	P_lambda	=	NULL;
	delete	[] K_lambda;	K_lambda	=	NULL;

//	for( label = 0; label < MAX_LABEL; label++ )
//		cout << ((Distance*)vpEnergy[label]->feature)->get_mean() << " " << ((Distance*)vpEnergy[label]->feature)->get_devia() << endl;

	for( i = 0; i < MAX_LABEL; i++ )
	{
		delete	[] mean[i];			mean[i]			=	0;
		delete	[] devia[i];		devia[i]		=	0;
		delete	[] old_mean[i];		old_mean[i]		=	0;
		delete	[] old_devia[i];	old_devia[i]	=	0;
	}
	delete	[]	mean;
	delete	[]	devia;
	delete	[]	old_mean;
	delete	[]	old_devia;

}


/********************************************************************
	初始化 (決定幾個label, mean 跟 sigma 的初始值
/*********************************************************************/
void	MarkovRandomField::EM_init()
{
	int		i;

	switch( vpEnergy[0]->get_feature_type() )
	{
		case	MRF_FEATURE_MOMENT:
			assert(false);	// 尚未撰寫
			break;

		case	MRF_FEATURE_DISTANCE:
			EM_init_distance();
			break;

		default:
			assert(false);
	}

	// 重新設定一次圖片 放在這邊是因為到這裡才確定vpEnergy的數量
	for( i = 0; i < vpEnergy.size(); i++ )
		vpEnergy[i]->set_image(image);
}



/********************************************************************
	初始化 (決定幾個label, mean 跟 sigma 的初始值
/*********************************************************************/
void	MarkovRandomField::EM_init_distance()
{
	GaussFilter_2D	gf2(7,7);		// 先做 gauss filter 會讓histogram的local maximal明顯很多

	QImage	img	=	gf2.do_filter(image);


	//img.save("../TEMP/output.bmp");

	const int	width	=	img.width();
	const int	height	=	img.height();
	const int	THR		=	width*height/100;	// 粗估

	// 統計 histogram
	int				i,	j;
	int				a[256]	=	{0};
	int				value;
	//sVectorDouble	vec(1);
	double			vec[1];


	for( i = 0; i < image.width(); i++ )
		for( j = 0; j < image.height(); j++ )
		{
			value	=	img.gray_channel( i, j );
			a[value]++;
		}

	// 搜尋local max
	int		map[256];	//  0 or 1

	// 初始化 map 小於thr都砍掉
	for( i = 0; i < 256; i++ )
	{
		if( a[i] < THR )	map[i]	=	0;
		else				map[i]	=	1;
	}

	// 跟周圍比 確定是local max
	for( i = 0; i < 256; i++ )
	{
		if( map[i] == 0 )
			continue;

		for( j = i-3; j <= i+3; j++ )
		{
			if( j >= 0 && j < 256 && j!=i )
			{
				if( a[j] > a[i] )
					map[i]	=	0;
			}
		}
	}

	vpEnergy.clear();	// 將舊資料刪除

	// 利用結果去產生 feature (丟入vector)
	for( i = 0; i < 256; i++ )
	{
		if( map[i] == 1 )
		{
			Distance	dt;
			//assert( vec.size() == dt.dim() );
				
			vec[0]	=	i;					// 為了統一介面改用vector
			dt.set_mean(vec);				// histogram產生的mean  (最大值的index)

			vec[0]	=	2.0;
			dt.set_devia(vec);				// 初始化的deviation (固定丟5.0 以後看要不要從histogram計算)

			EnergyStandard	et( dt , image );	// 不需要 set_anchor
			push_back(et);
		}
	}

	cout << "#Label = " << vpEnergy.size() << endl;

}


/********************************************************************
	執行模擬退火法
/*********************************************************************/
void	MarkovRandomField::SimuAnneal_exec( QImage img )
{
	image	=	img;

	int		i,	j,	n;
	int		label;

	// 重新設定一次圖片
	for( i = 0; i < vpEnergy.size(); i++ )
		vpEnergy[i]->set_image(image);

	const int	width		=	vpEnergy[0]->get_img_width();
	const int	height		=	vpEnergy[0]->get_img_height();
	const int	MAX_ITER	=	1999999999;//width*height*500;
	const int	MAX_LABEL	=	vpEnergy.size();				// 最大label個數

	double		T;				// 溫度
	double		xi;				// 存跟溫度比較的亂數 (機率)
	int			value_tmp;		// 暫存用
	int			pix;
	bMatrixInt	label_tmp;		// 暫存用的label map

	double		energy1,	energy2;		// 暫存能量用  1是原本的  2是修改的

	boost::random::mt19937						gen(time(0));
	boost::random::uniform_int_distribution<>	dist( 0, MAX_LABEL-1 );		// 亂數產生器
	boost::random::uniform_int_distribution<>	dist_x( 0, width-1 );
	boost::random::uniform_int_distribution<>	dist_y( 0, height-1 );
	boost::random::uniform_01<>					dist_01;


	//bMatrixInt	old_map		=	label_map;		// 舊的 label map, 判斷迭代停止使用

#ifdef _DEBUG		// 檢查圖片是否相同
	for( label = 0; label < MAX_LABEL; label++ )
	{
		assert( vpEnergy[label]->get_image() == image );
		assert( vpEnergy[label]->check_image() );
	}
#endif

	D.resize(MAX_LABEL);							// 總共有幾個label
	for( i = 0; i < MAX_LABEL; i++ )				// 初始化  迭代的時候應該要給初始值
		D[i].resize( width, height );

	// 用亂數的方式產生 label map
	label_map.resize( width, height );
	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
			label_map( i, j )	=	dist(gen);

	T	=	100 + 1;//MAX_ITER + 1;
	energy1		=	totalEnergy( label_map );
	label_tmp	=	label_map;	// 為了加速
	for( n = 0; n < MAX_ITER; n++ )
	{	
		//cout << "itr = " << setw(3) << n << " energy = " << dataEnergy() + smoothEnergy() << endl;

		//label_tmp	=	label_map;
		i			=	dist_x(gen);
		j			=	dist_y(gen);	// 亂數產生要改變的點

		while(1)
		{
			value_tmp	=	dist(gen);		// 亂數產生要改變的值
			if( value_tmp != label_map(i,j) )
				break;
		}

		xi				=	dist_01(gen);
		label_tmp(i,j)	=	value_tmp;
		pix				=	i * height + j;

		//energy1			=	totalEnergy( label_map );
		//energy2			=	totalEnergy( label_tmp );
		energy2		=	energy1 - singleEnergy( pix, label_map(i,j) ) + singleEnergy( pix, label_tmp(i,j) );
		                                     /* 小心 label_map                              label _tmp */
		
		if( energy1 >= energy2 ) // 退火成功
		{
			//cout << "*";
			//label_map	=	label_tmp;
			label_map(i,j)	=	value_tmp;		// 減少運算量
			energy1			=	energy2;
		}
		else if( energy1 < energy2 && xi < exp( -(energy2-energy1)/T ) )
		{
			//cout << "*";
			//label_map	=	label_tmp;
			label_map(i,j)	=	value_tmp;
			energy1			=	energy2;
		}
		else
		{
			//cout << "-";
			// 不動
			label_tmp(i,j)	=	label_map(i,j);		// 為了加速
		}

		//T	-=	(100.0/MAX_ITER);
		if( T > 0.1 )	T -= 0.1;

		if( n%10000 == 0 )
		{
			cout << "*";
			// 輸出結果
			QImage	img( width, height, QImage::Format_RGB888 );
			for( i = 0; i < width; i++ )
				for( j = 0; j < height; j++ )
				{
					label	=	label_map(i,j);
					img.setPixel( i, j, (label*761838257287)%193707721 );
				}

			char	filename[100];
			sprintf( filename, "%s/output.bmp", TMP_PATH );
			img.save(QString(filename));
		}


		// 利用label map決定是否要停止迭代
		//if( old_map == label_map )		break;
		//old_map		=	label_map;
	
	} // end MAX_ITR

	cout << "end energy = " << dataEnergy() + smoothEnergy() << endl;

	// 輸出結果
	QImage	img( width, height, QImage::Format_RGB888 );
	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			label	=	label_map(i,j);
			img.setPixel( i, j, (label*761838257287)%193707721 );
		}

	char	filename[100];
	sprintf( filename, "%s/output.bmp", TMP_PATH );
	img.save(QString(filename));
	cout << "output finish.\n";
}


/********************************************************************
	考慮 8-neighbor
/*********************************************************************/
void	MarkovRandomField::SimuAnneal8_exec( QImage img )
{
	// 還沒寫 有需要再寫.
	assert(false);
}



/********************************************************************
	計算單一點的energy
/*********************************************************************/
double	MarkovRandomField::singleEnergy( int pix, int lab )
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	double		sum		=	0;
	int			i,		j;
	int			lab1,	lab2,	pix1,	pix2;

	i		=	pix / height;
	j		=	pix % height;


	lab1	=	lab;
	pix1	=	ij2pix(i,j);

	// 1 clique
	sum		+=	vpEnergy[lab]->E_data( pix, lab );

	// 2 clique
	// 上
	if( j-1 >= 0 )
	{
		pix2	=	ij2pix(i,j-1);
		lab2	=	label_map(i,j-1);
		sum		+=	vpEnergy[lab]->E_smooth( pix1, pix2, lab1, lab2 );
	}

	// 下
	if( j+1 < height )
	{
		pix2	=	ij2pix(i,j+1);
		lab2	=	label_map(i,j+1);
		sum		+=	vpEnergy[lab]->E_smooth( pix1, pix2, lab1, lab2 );
	}

	// 左
	if( i-1 >= 0 )
	{
		pix2	=	ij2pix(i-1,j);
		lab2	=	label_map(i-1,j);
		sum		+=	vpEnergy[lab]->E_smooth( pix1, pix2, lab1, lab2 );
	}

	// 右
	if( i+1 < width )
	{
		pix2	=	ij2pix(i+1,j);
		lab2	=	label_map(i+1,j);
		sum		+=	vpEnergy[lab]->E_smooth( pix1, pix2, lab1, lab2 );
	}


	return	sum;
}



/********************************************************************
	根據label結果來計算能量
/*********************************************************************/
double	MarkovRandomField::totalEnergy( bMatrixInt label )
{
	const int	width	=	image.width();
	const int	height	=	image.height();

	double		sum		=	0;
	int			i,		j;
	int			lab,	pix;
	int			lab1,	lab2,	pix1,	pix2;

	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			lab		=	label(i,j);
			lab1	=	label(i,j);
			pix		=	ij2pix(i,j);
			pix1	=	ij2pix(i,j);

			// 1 clique
			sum		+=	vpEnergy[lab]->E_data( pix, lab );

			// 2 clique
			// 上
			if( j-1 >= 0 )
			{
				pix2	=	ij2pix(i,j-1);
				lab2	=	label(i,j-1);
				sum		+=	vpEnergy[lab]->E_smooth( pix1, pix2, lab1, lab2 );
			}

			// 下
			if( j+1 < height )
			{
				pix2	=	ij2pix(i,j+1);
				lab2	=	label(i,j+1);
				sum		+=	vpEnergy[lab]->E_smooth( pix1, pix2, lab1, lab2 );
			}

			// 左
			if( i-1 >= 0 )
			{
				pix2	=	ij2pix(i-1,j);
				lab2	=	label(i-1,j);
				sum		+=	vpEnergy[lab]->E_smooth( pix1, pix2, lab1, lab2 );
			}

			// 右
			if( i+1 < width )
			{
				pix2	=	ij2pix(i+1,j);
				lab2	=	label(i+1,j);
				sum		+=	vpEnergy[lab]->E_smooth( pix1, pix2, lab1, lab2 );
			}
		}

	return	sum;
}



/********************************************************************
	gibbs 初始化
/*********************************************************************/
void	MarkovRandomField::initial_state( int MAX_LABEL, int *counts, double *sum, double *sumsq )
{
	int		width	=	image.width();
	int		height	=	image.height();
	int		i,	j;
	int		label;
	double	data;

	boost::mt19937								gen(time(0));	
	boost::random::uniform_int_distribution<>	dist( 0, MAX_LABEL-1 );

	// 初始化label map (用亂數)
	label_map.resize( width, height );
	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
			label_map( i, j )	=	dist(gen);

	for( i = 0; i < MAX_LABEL; i++ ) 
	{
		counts[i]	=	0;
		sum[i]		=	0;
		sumsq[i]	=	0;
	}

	for( i = 0; i < width; i++ )
		for( j = 0; j < height; j++ )
		{
			label	=	label_map( i, j );
			data	=	image.gray_channel(i,j);

			counts[label]++;
			sum[label]		+=	data;
			sumsq[label]	+=	pow(data,2);
		}
}


/********************************************************************
	gibbs 移除點
/*********************************************************************/
void	MarkovRandomField::remove_observation( double data, int label, int *counts, double *sum, double *sumsq )
{
	counts[label]--;
	sum[label]		-=	data;
	sumsq[label]	-=	pow( data, 2 );
}




/********************************************************************
	gibbs 增加點
/*********************************************************************/
void	MarkovRandomField::add_observation( double data, int label, int *counts, double *sum, double *sumsq )
{
	counts[label]++;
	sum[label]		+=	data;
	sumsq[label]	+=	pow( data, 2 );
}



/********************************************************************
	gibbs 更新估計值
/*********************************************************************/
void	MarkovRandomField::update_estimates( int MAX_LABEL, double *prob, double *mean, double *sigma, int *counts, double *sum , double *sumsq )
{
	const int	width	=	image.width();
	const int	height	=	image.height();
	const int	n		=	width * height - 1;		// 因為remove掉一個點 所以個數必須 -1

	int		i;

	for( i = 0; i < MAX_LABEL; i++ )
	{
		prob[i]		=	1.0 * counts[i] / n;
		mean[i]		=	sum[i] / counts[i];
		sigma[i]	=	sqrt( (sumsq[i] - mean[i]*mean[i]*counts[i]) / (counts[i]) );
	}
}



/********************************************************************
	gibbs 求機率
	(名稱用norm是因為論文寫的)
/*********************************************************************/
double	MarkovRandomField::dnorm( double x, double mean, double sigma )
{
	const double	T	=	1;	// 溫度 目前預設 = 1

	if( sigma < 0.001 )		// 標準差過小的case  (可能此label無效之類的)
		return	0;
	else
		return	exp( -pow( x-mean, 2 ) / ( 2.0*pow(sigma,2) ) / T )  / sqrt( 2.0*3.1415926) / (sigma)  ;
}



/********************************************************************
	gibbs 求機率和
	(類似normalized的參數Z)
/*********************************************************************/
double	MarkovRandomField::dmix( int MAX_LABEL, double x, double *probs, double *mean, double *sigma )
{
	int		i;
	double	sum	=	0;

	for( i = 0; i < MAX_LABEL; i++ )
		sum		+=	probs[i] * dnorm( x, mean[i], sigma[i] );

	return	sum;
}




/********************************************************************
	gibbs 去逆推機率分布
/*********************************************************************/
int		MarkovRandomField::sample_group( int MAX_LABEL, double data, double *probs, double *mean, double *sigma )
{
	static boost::mt19937						mrf_gen(time(0));		// 要注意必須丟入時間當種子
	static boost::random::uniform_01<>			dist_01;				// 0~1的均勻分布

	int			label;
	double		p	=	dist_01(mrf_gen);		// 隨機產生機率
	double		Z	=	dmix( MAX_LABEL, data, probs, mean, sigma );

	for( label = 0; label < MAX_LABEL - 1; label++ )	// -1 因為最後一個就不用再算了
	{
		double	p_label	=	probs[label] * dnorm( data, mean[label], sigma[label] ) / Z;		// 求出機率分布

		if( p < p_label )
			return	label;	// 落在該區間 就回傳

		p	-=	p_label;		// 想成是累計機率分布 跟 pdf的轉換
	}

	return	MAX_LABEL - 1;
}







/********************************************************************
	執行 gibbs sampler  (跟EM效果類似)
/*********************************************************************/
void	MarkovRandomField::Gibbs_exec( QImage img )
{
	image	=	img;

	const int		width	=	image.width();
	const int		height	=	image.height();

	EM_init_distance();	// init  (這邊還是用EM的程式碼去初始化 以後要改再說) 

	// 進入gibbs sampler
	const int		MAX_LABEL	=	vpEnergy.size();
	const int		MIN_GROUP	=	100;			// 表示區域點數過小就跳過 (某些時候某些區塊會被吃掉)

	int				label;
	int				i,	j,	n;
	int				x,	y;

	int				*counts		=	new int[MAX_LABEL];
	double			*sum		=	new double[MAX_LABEL];
	double			*sumsq		=	new double[MAX_LABEL];
	double			*probs		=	new double[MAX_LABEL];
	double			*mean		=	new double[MAX_LABEL];
	double			*sigma		=	new double[MAX_LABEL];

	boost::mt19937								gen(time(0));	
	boost::random::uniform_int_distribution<>	dist_x( 0, width-1 );
	boost::random::uniform_int_distribution<>	dist_y( 0, height-1 );
	boost::random::uniform_int_distribution<>	dist_label( 0, MAX_LABEL-1 );

	// 初始化資料
	initial_state( MAX_LABEL, counts, sum, sumsq );
	
	for( n = 0; n < 1999999999; n++ )		// 1999999999 任意夠大的數字
	{
		x	=	dist_x(gen);
		y	=	dist_y(gen);

		if( counts[ label_map(x,y) ] < MIN_GROUP )		// 區塊快被吃掉就跳過
			continue;

		remove_observation( image.gray_channel( x, y ) , label_map(x,y) , counts, sum , sumsq );	
		update_estimates( MAX_LABEL, probs, mean, sigma, counts, sum, sumsq );
	
		label_map(x,y)	=	sample_group( MAX_LABEL, image.gray_channel(x,y), probs, mean, sigma );

		add_observation( image.gray_channel( x, y), label_map(x,y) , counts, sum, sumsq );

		// 一段時間印出資訊
		if( n % 1000000 == 0 )
		{
			for( i = 0; i < MAX_LABEL; i++ )
				cout << setw(7) << setprecision(5) << mean[i] << " ";
			cout << endl;
			for( i = 0; i < MAX_LABEL; i++ )
				cout << setw(7) << setprecision(5) << sigma[i] << " ";
			cout << endl << endl;

			for( i = 0; i < width; i++ )
				for( int j = 0; j < height; j++ )
				{
					label	=	label_map(i,j);
					img.setPixel( i, j, (label*761838257287)%193707721 );
					//img.gray_channel( ii, jj, label_map(ii,jj) * 30 );
				}
			img.save("../TEMP/output.bmp");

		}
	}

	// 刪除資料
	delete [] counts;	counts	=	0;
	delete [] sum;		sum		=	0;
	delete [] sumsq;	sumsq	=	0;
	delete [] probs;	probs	=	0;
	delete [] mean;		mean	=	0;
	delete [] sigma;	sigma	=	0;
}