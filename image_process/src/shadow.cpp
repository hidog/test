#include "shadow.h"

//using namespace std;


//test
namespace	emboss{



/************************************************************************************
	建構
/************************************************************************************/
Shadow::Shadow()
{
	size		=	0;
	sub_inx		=	0;
	link_inx	=	0;
}


/************************************************************************************
	解構
/************************************************************************************/
Shadow::~Shadow()
{
	size	=	0;
	delete [] sub_inx;
	delete [] link_inx;
}


/************************************************************************************
	設定size
	這邊的_size不能從setting讀取 
	因為是補值後的size
/************************************************************************************/
void	Shadow::set( int _size, const  ShadowSettings &setting )
{
	size		=	_size;				// 注意這邊的size是補值後的size,不是原本的size

	assert( size > 0 );

	sub_inx		=	new int[size];
	link_inx	=	new int[size];

	threshold	=	setting.threshold;
	enhance		=	setting.enhance;
	bump		=	setting.bump;

	dark_light	=	setting.dark_light;
}



/************************************************************************************
	尋找單調子序列
	回傳一組vector, value = 1 表示該元素在sub seq內
	                value = 0 表示不在sub seq內.
/************************************************************************************/
void	Shadow::sub_mono_seq( andrea::vector<unsigned char> &seq, andrea::vector<unsigned char> &is_choose )
{
	// 因為是找完才補值 所以可以加上這個判斷.
	assert( seq.size() == size && is_choose.size() == size );

	int		i,	j,	max;
	int		sub_find;
	int		find_inx;
	int		start_inx;		

	// 初始化資料  這邊曾經因為size錯誤造成bug
	memset( sub_inx  , 0, (size)*sizeof(int) );
	memset( link_inx , 0, (size)*sizeof(int) );

	// 尋找最長mono sub seq 
	// 速度可以進一步進步到 n*log(n) (目前 = n^2)
	//max			=	0;
	//start_inx	=	0;
	for( i = size-1; i >= 0; i-- )
	{
		sub_find	=	0;
		find_inx	=	0; 
		
		// 順著方向找 速度會比較快 (底下>=的判斷會變少)
		for( j = i+1;  j < size  ; j++ )
		{					
			/* 這邊的>=會決定遞增遞減*/  
			if( sub_inx[j] >= sub_find  &&  seq[i] <= seq[j] )  
			{
				if( sub_inx[j] == sub_find  && seq[j] > seq[find_inx] )
				{
					sub_find	=	sub_inx[j];
					find_inx	=	j;					
				}
				else if( sub_inx[j] > sub_find   )
				{
					sub_find	=	sub_inx[j];
					find_inx	=	j;
				}
			}
			else if( sub_find > size-j )		// 表示已經不可能找到更長的seq,可以跳出.  小心這邊產生的bug
				break;
		}

		sub_inx[i]	=	sub_find + 1;
		link_inx[i]	=	find_inx;			// 將資料串起來 建立sub seq使用

		// 尋找最大值  這邊可進一步加速 (因為max跟後面剩餘項有關)
		// 主要目的是找出 mono sub seq的起點
		/*if( sub_inx[i] > max )
		{
			max			=	sub_inx[i];
			start_inx	=	i;
		}*/
	}

	// 尋找最大值  這邊可進一步加速 (因為max跟後面剩餘項有關)
	// 主要目的是找出 mono sub seq的起點
	max			=	0;
	start_inx	=	0;
	for( i = 0; i < size; i++ )
	{
		if( sub_inx[i] > max )
		{
			max			=	sub_inx[i];
			start_inx	=	i;
		}	
		else if( max > size-i )		// 表示已經不可能找到更長的seq,可以跳出.  小心這邊產生的bug
			break;
	}


	// 建立list  建立mono sub seq的表
	is_choose.clear();
	i	=	start_inx;
	do{
		assert( i >= 0 && i < size );
		is_choose(i)	=	1;
		i				=	link_inx[i];		// next step
	}while( i > 0 );	//  0 表示沒有下一個

}



/************************************************************************************
	尋找單調子序列
	回傳一組vector, value = 1 表示該元素在sub seq內
	                value = 0 表示不在sub seq內.

	加速用 避免重複使用記憶體

	實際上是小於等於 跟名字不同
/************************************************************************************/
void	Shadow::sub_mono_seq_less( bVectorByte &seq, bVectorByte &is_choose )
{
	assert( is_choose.size() == size && seq.size() == size);

	int				i,	j,	max;
	int				sub_find;
	int				find_inx;
	int				start_inx;	

	memset( sub_inx  , 0, (size)*sizeof(int) );
	memset( link_inx , 0, (size)*sizeof(int) );

	// 尋找最長mono sub seq 
	// 速度可以進一步進步到 n*log(n)
	max			=	0;
	start_inx	=	0;
	for( i = size-1; i >= 0; i-- )
	{
		sub_find	=	0;
		find_inx	=	0; 
		//for( j = size-1; j > i; j-- )
		for( j = i+1;  j < size  ; j++ )
		{					
			// 尋找子序列  串接的動作在底下
			if( sub_inx[j] >= sub_find && seq[i] <= seq[j] )
			{
				if( sub_inx[j] == sub_find && seq(j) > seq(find_inx) )
				{
					sub_find	=	sub_inx[j];
					find_inx	=	j;					
				}
				else if( sub_inx[j] > sub_find )
				{
					sub_find	=	sub_inx[j];
					find_inx	=	j;
				}
			}
			else if( sub_find > size-j )		// 表示已經不可能找到更長的seq,可以跳出.  小心這邊產生的bug
				break;
		}

		sub_inx[i]	=	sub_find + 1;
		link_inx[i]	=	find_inx;			// 將資料串起來 建立sub seq使用

		// 尋找最大值  這邊可進一步加速 (因為max跟後面剩餘項有關)
		// 主要目的是找出 mono sub seq的起點
		if( sub_inx[i] > max )
		{
			max			=	sub_inx[i];
			start_inx	=	i;
		}
	}

	// 建立list  建立mono sub seq的表
	is_choose.clear();
	i	=	start_inx;
	do{
		assert( i < size );
		is_choose(i)	=	SHADOW_VALUE;
		i				=	link_inx[i];		// next step
	}while( i > 0 );	//  0 表示沒有下一個
}





/************************************************************************************
	尋找單調子序列
	回傳一組vector, value = 1 表示該元素在sub seq內
	                value = 0 表示不在sub seq內.

	加速用 避免重複使用記憶體

	實際上是大於等於 跟名字不同
/************************************************************************************/
void	Shadow::sub_mono_seq_greater( bVectorByte &seq, bVectorByte &is_choose )
{
	assert( is_choose.size() == size && seq.size() == size );

	int				i,	j,	max;
	int				sub_find;
	int				find_inx;
	int				start_inx;	

	memset( sub_inx  , 0, (size)*sizeof(int) );
	memset( link_inx , 0, (size)*sizeof(int) );

	// 尋找最長mono sub seq 
	// 速度可以進一步進步到 n*log(n)
	max			=	0;
	start_inx	=	0;
	for( i = size-1; i >= 0; i-- )
	{
		sub_find	=	0;
		find_inx	=	0; 
		//for( j = size-1; j > i; j-- )
		for( j = i+1;  j < size  ; j++ )
		{					
			// 尋找子序列  串接的動作在底下  跟less只差在這邊
			if( sub_inx[j] >= sub_find && seq[i] >= seq[j] ) 
			{
				if( sub_inx[j] == sub_find && seq(j) > seq(find_inx) )
				{
					sub_find	=	sub_inx[j];
					find_inx	=	j;					
				}
				else if( sub_inx[j] > sub_find )
				{
					sub_find	=	sub_inx[j];
					find_inx	=	j;
				}
			}
			else if( sub_find > size-j )		// 表示已經不可能找到更長的seq,可以跳出.  小心這邊產生的bug
				break;
		}

		sub_inx[i]	=	sub_find + 1;
		link_inx[i]	=	find_inx;			// 將資料串起來 建立sub seq使用

		// 尋找最大值  這邊可進一步加速 (因為max跟後面剩餘項有關)
		// 主要目的是找出 mono sub seq的起點
		if( sub_inx[i] > max )
		{
			max			=	sub_inx[i];
			start_inx	=	i;
		}
	}

	// 建立list  建立mono sub seq的表
	is_choose.clear();
	i	=	start_inx;
	do{
		is_choose(i)	=	SHADOW_VALUE;
		i				=	link_inx[i];		// next step
	}while( i > 0 );	//  0 表示沒有下一個
}




/************************************************************************************
	絲網檢測
	seq:        資料
	settings:   各種參數設定
	這邊利用同時做遞增 遞減子序列 再取交集

	將狀況區分為四種

	1. 亮影 凹    取交集  遞減
	2. 暗影 凸    取交集  遞增
	3. 亮影 凹    取聯集  遞減
	4. 暗影 凹    取聯集  遞減

	亮暗決定遞增或遞減
	這邊只取單方向 由上一層去判斷以上四點

	補值也交給上一層判斷

/************************************************************************************/
void	Shadow::screen_detector( bVectorByte &seq,  ShadowReturnData &return_data, int th_id )
{

	int					size		=	seq.size();
	int					i,	j,	i1,	i2;
	double				mag;

	bVectorByte			&is_choose		=	return_data.mono_sub_seq;
	bVectorByte			&shadow_list	=	return_data.shadow_seq;

	assert( is_choose.size() == size && shadow_list.size() == size );

	if( bump == PROTRUDING )		// 凸
		sub_mono_seq_less( seq, is_choose );
		//sub_mono_seq<unsigned char>( seq, less_than_or_equal<unsigned char>, is_choose, sub_inx, link_inx );
	else if( bump == CONCAVE )		// 凹
		sub_mono_seq_greater( seq, is_choose );
		//sub_mono_seq<unsigned char>( seq, greater_than_or_equal<unsigned char>, is_choose, sub_inx, link_inx );
	else
		assert(false);

	// 假如最後一個資料沒被選中,則需要補值  
	// 本來想省略 最後發現不行...orz
	if( is_choose(size-1) == 0 )
	{
		// 搜尋最後一個被選到的資料
		for( i = size-1; i >= 0; i-- )
		{
			if( is_choose(i) != 0 )
				break;
		}
		seq.add_value( seq(i) );
		is_choose.add_value(1);
		size	=	seq.size();	// size = size + 1
	}	


	// 開始搜尋亮暗影
	shadow_list.clear();
	i1	=	1;
	i2	=	2;
	while(1)
	{
		// 新增的中止條件
		if( i1 >= size )
			break;

		find_non_sub_seq_interval( i1, i2, is_choose );		// 搜尋出一段不在  sub mono seq 的區段 

		//if( i2 + 1 >= size )  // 終止條件
		if( i2 >= size )		// 這邊有修改過,小心bug
			break;

		for( j = i1; j < i2 && j < size;  j++  )
		{
			if( dark_light == EMBOSS_DARK )					// 抓陰影
				mag		=	seq(i1-1) - seq(j);
			else if( dark_light == EMBOSS_LIGHT )			// 抓亮影
				mag		=	seq(j) - seq(i1-1);
			else
				assert(false);

			// 超過threshold都算是陰影
			// inhence = false, 表示抓整段的陰影
			if( mag > threshold )
				shadow_list(j)	=	SHADOW_VALUE;		
		}
		i1	=	i2 + 1;
	}	// end while

}




/************************************************************************************
	陰影偵測
	seqo :      原始的資料
	seq:        加上gauss blur的資料
	settings:   各種參數設定
/************************************************************************************/
void	Shadow::shadow_detector( bVectorByte &seq,  ShadowReturnData &return_data )
{
	int		size	=	seq.size();

	bVectorByte	 &is_choose		=	return_data.mono_sub_seq;		// 遞增 sub seq 的 list.   1 表示有在 sub seq 內  0 表示沒有
	bVectorByte	 &shadow_list	=	return_data.shadow_seq;			// 找出來的陰影區間資料
	bVectorByte	 &depth			=	return_data.shadow_depth;

	assert( is_choose.size() == size && shadow_list.size() == size && depth.size() == size );

	// 進去找遞增子序列
	sub_mono_seq( seq, is_choose );

	int			i,		j;
	int			i1,		i2;
	int			max,	max_inx;
	double		mag;

	// 假如最後一個資料沒被選中,則需要補值
	if( is_choose(size-1) == 0 )
	{
		// 搜尋最後一個被選到的資料
		for( i = size-1; i >= 0; i-- )
		{
			if( is_choose(i) != 0 )
				break;
		}

		seq.add_value( seq(i) );
		is_choose.add_value( 1 );
		size	=	seq.size();	
	}	

	// 比threshold低的都算是陰影
	// 搜尋出一個interval,  並且都是沒被選到的
	i1		=	1;		// 假設陰影不會出現在第一個
	i2		=	2;
	max		=	0;
	max_inx	=	0;

	shadow_list.clear();							// 假設depth沒補值卻發生index存取錯誤 表示有演算法上的問題  要注意   (因為有補值的話,最後一個點一定不會拿進去算)
	depth.clear();

	while(1)
	{
		// 新增的中止條件
		if( i1 >= size )
			break;

		find_non_sub_seq_interval( i1, i2, is_choose );		// 搜尋出一段不在  sub mono seq 的區段 

		//if( i2 + 1 >= size )  // 終止條件
		if( i2 >= size )		// 這邊有修改過,小心bug
			break;

		for( j = i1; j < i2 && j < size;  j++  )
		{
			//if( !settings.dark_light )			// 抓陰影
			if( dark_light == EMBOSS_DARK )			// 抓陰影
				mag		=	seq(i1-1) - seq(j);
			//else									// 抓亮影
			else if( dark_light == EMBOSS_LIGHT )	// 抓亮影
				mag		=	seq(j) - seq(i1-1);
			else
				assert(false);

			depth(j)	=	(int)mag;					// 不管怎樣 都記錄該段跟 sub mono seq 的距離
	
			// 超過threshold都算是陰影
			// inhence = false, 表示抓整段的陰影
			if( mag > threshold && !enhance )
				shadow_list(j)	=	SHADOW_VALUE;		

			// 紀錄最深的位置
			if( mag > max )
			{
				max		=	(int)mag;
				max_inx	=	j;
			}
		}

		i1	=	i2 + 1;
	}

	// inhence = true, 抓最深的當陰影. 主要用在noise比陰影還強的狀況.
	if( enhance )
		shadow_list( max_inx )	=	SHADOW_VALUE;		
}






/************************************************************************************
	求出不再sub seq的區間
/************************************************************************************/
void	Shadow::find_non_sub_seq_interval( int &i1, int &i2, bVectorByte &is_choose )
{
	const int	size	=	is_choose.size();		// 因為會補值 不能放到外面.

	// 不考慮最後一個
	while(  i1 < size-1  &&  is_choose(i1) == 1  )	i1++;
	
	i2	=	i1;
	while(  i2 < size && is_choose(i2) == 0  )		i2++;

	// 表示沒搜尋到
	if( is_choose(i1) == 1 )
		i2	=	size-1;

	assert( i1 < size && i2 < size );
}






} // end namespace emboss