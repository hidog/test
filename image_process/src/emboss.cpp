#include "emboss.h"


namespace emboss{



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Screen  class  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/***************************************************************************************
	建構
/****************************************************************************************/
Screen::Screen()
{
	channel_func	=	NULL;
	good_bad		=	EMBOSS_GOOD;
	// shadow_obj
	// image
	// roi
	direction		=	EMBOSS_DIRECTION_MAX;
	dark_light		=	EMBOSS_DARK_LIGHT_MAX;
	bump			=	BUMP_MAX;
	size			=	-1;

	x_left			=	-1;
	x_right			=	-1;
	y_up			=	-1;
	y_down			=	-1;
	roi_x			=	-1;
	roi_y			=	-1;
	anchor_x		=	-1;
	anchor_y		=	-1;
	anchor_x2		=	-1;
	anchor_y2		=	-1;

	start			=	PIXEL(-1,-1);
	end				=	PIXEL(-1,-1);

	original_len	=	-1;
	// shadow_settings
	line_data		=	NULL;
	line_data2		=	NULL;
	//line_fron;
	//line_back;
	seq_line_front	=	NULL;
	seq_line_front2	=	NULL;
	seq_line_back	=	NULL;
	seq_line_back2	=	NULL;

	seq_extend.resize(0);
	//shadow_data_tmp

	seq_line_up		=	NULL;
	seq_line_down	=	NULL;
	seq_line_left	=	NULL;
	seq_line_right	=	NULL;

	line_data_up	=	NULL;
	line_data_down	=	NULL;
	line_data_left	=	NULL;
	line_data_right	=	NULL;

	//up_map.resize(0,0);
	//down_map.resize(0,0);
	//left_map.resize(0,0);
	//right_map.resize(0,0);

	//shadow;	
	//shadow2;

}


/***************************************************************************************
	解構
/****************************************************************************************/
Screen::~Screen()
{
	channel_func	=	NULL;
	good_bad		=	EMBOSS_GOOD;

	delete	[] line_data;				line_data		=	NULL;
	delete	[] line_data2;				line_data2		=	NULL;
	delete	[] seq_line_front;			seq_line_front	=	NULL;
	delete	[] seq_line_front2;			seq_line_front2	=	NULL;
	delete	[] seq_line_back;			seq_line_back	=	NULL;
	delete	[] seq_line_back2;			seq_line_back2	=	NULL;

	delete	[] seq_line_up;				seq_line_up		=	NULL;
	delete	[] seq_line_down;			seq_line_down	=	NULL;
	delete	[] seq_line_left;			seq_line_left	=	NULL;
	delete	[] seq_line_right;			seq_line_right	=	NULL;

	delete	[] line_data_up;			line_data_up	=	NULL;
	delete	[] line_data_down;			line_data_down	=	NULL;
	delete	[] line_data_left;			line_data_left	=	NULL;
	delete	[] line_data_right;			line_data_right	=	NULL;
}





/***************************************************************************************
	陰影容許禁區 
	判斷有幾個陰影 
	超過數量就叫做廢品 (嚴格的話 出現一個就是廢品)
/****************************************************************************************/
int		Screen::all_dir_result()
{
	int		i,	j;
	int		count;

	assert( direction == EMBOSS_ALL_DIRECTION );
	assert( shadow.size1() == size && shadow.size2() == size );

	count	=	0;
	for( i = 0; i < size; i++ )
		for( j = 0; j < size; j++ )
		{
			if( shadow( i, j ) == SHADOW_VALUE )
				count++;
		}
	
	return	count;
}


/***************************************************************************************
	陰影檢測用的
	讀取圖檔資料
	加速用 (避免記憶體重複宣告)
	必須假設 seq 之前就已經完成resize動作
/****************************************************************************************/
void	Screen::loadimage( bVectorByte &seq )
{
	assert( seq.size() == size );

	int			i;

	// 檢查點是否正確  檢查相對位置
	switch(direction)
	{
		case EMBOSS_UP:
			assert(start.x == end.x && start.y < end.y);
			break;
		case EMBOSS_DOWN:
			assert(start.x == end.x && start.y > end.y);
			break;
		case EMBOSS_LEFT:
			assert(start.x < end.x && start.y == end.y);
			break;
		case EMBOSS_RIGHT:
			assert(start.x > end.x && start.y == end.y);
			break;
		case EMBOSS_LEFT_UP:
			assert(end.x-start.x == end.y-start.y);
			break;
		case EMBOSS_LEFT_DOWN:
			assert(end.x-start.x == start.y-end.y);
			break;
		case EMBOSS_RIGHT_UP:
			assert(start.x-end.x == end.y-start.y);
			break;
		case EMBOSS_RIGHT_DOWN:
			assert(start.x-end.x == start.y-end.y);
			break;
		default:
			assert(false);
	}

#ifdef _DEBUG
	// 先決定size  (因為考慮長方形 這邊會有所不同 預先考慮進去) 
	// 檢查size  debug模式下才使用
	int		tmp_size;
	switch(direction)
	{
		case EMBOSS_UP :
			tmp_size	=	end.y - start.y + 1;
			break;
		case EMBOSS_DOWN :
			tmp_size	=	start.y - end.y + 1;
			break;
		case EMBOSS_LEFT :
			tmp_size	=	end.x - start.x + 1;
			break;
		case EMBOSS_RIGHT :
			tmp_size	=	start.x - end.x + 1;
			break;
		case EMBOSS_LEFT_UP :
			tmp_size	=	end.x - start.x + 1;
			break;
		case EMBOSS_LEFT_DOWN :
			tmp_size	=	end.x - start.x + 1;
			break;
		case EMBOSS_RIGHT_UP :
			tmp_size	=	start.x - end.x + 1;
			break;
		case EMBOSS_RIGHT_DOWN :
			tmp_size	=	start.x - end.x + 1;
			break;
		case EMBOSS_ALL_DIRECTION:
			tmp_size	=	size;	// 暫時先這樣
			break;
		default:
			assert(false);
	}
	assert( seq.size() == tmp_size );		// 檢查seq的size
	assert( size == tmp_size );				// 檢查跟setting的size
#endif

	assert( channel_func != NULL );

	// 開始讀取圖片資料
	switch( direction )
	{
		case EMBOSS_UP :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( start.x, start.y + i );
			break;

		case EMBOSS_DOWN :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( start.x, start.y - i );
			break;

		case EMBOSS_LEFT:
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( start.x + i, start.y );
			break;

		case EMBOSS_RIGHT :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( start.x - i, start.y );
			break;

		case EMBOSS_LEFT_UP :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( start.x + i, start.y + i );
			break;

		case EMBOSS_LEFT_DOWN :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( start.x + i, start.y - i );
			break;

		case EMBOSS_RIGHT_UP :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( start.x - i, start.y + i );
			break;

		case EMBOSS_RIGHT_DOWN :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( start.x - i, start.y - i );
			break;

		default :
			assert(false);
	}
}


/***************************************************************************************
	陰影檢測用的
	一次檢查一條線
	全方向使用
/****************************************************************************************/
void	Screen::batch_shadow_ALLD( int th_id )
{
	// EMBOSS_UP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ShadowReturnData	&shadow_data	=	line_data_up[th_id];

	seq_extend.set_value( 0, extend_len, seq_line_up[th_id](0) );		// 產生往前延伸資料
	seq_extend.add_vector( extend_len, seq_line_up[th_id] );			// 接回原本的資料

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.shadow_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp );

	// 刪除補值的部分
	shadow_data.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	shadow_data.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();

	// EMBOSS_DOWN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ShadowReturnData	&shadow_data2	=	line_data_down[th_id];

	seq_extend.set_value( 0, extend_len, seq_line_down[th_id](0) );		// 產生往前延伸資料
	seq_extend.add_vector( extend_len, seq_line_down[th_id] );			// 接回原本的資料

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.shadow_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp );

	// 刪除補值的部分
	shadow_data2.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	shadow_data2.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();

	// EMBOSS_LEFT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ShadowReturnData	&shadow_data3	=	line_data_left[th_id];

	// 產生往前延伸資料
	seq_extend.set_value( 0, extend_len, seq_line_left[th_id](0) );		// 往前補值
	seq_extend.add_vector( extend_len, seq_line_left[th_id] );			// 接回原本的資料

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.shadow_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp );

	// 刪除補值的部分
	shadow_data3.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	shadow_data3.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();

	// EMBOSS_RIGHT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ShadowReturnData	&shadow_data4	=	line_data_right[th_id];

	// 產生往前延伸資料
	seq_extend.set_value( 0, extend_len, seq_line_right[th_id](0) );		// 往前補值
	seq_extend.add_vector( extend_len, seq_line_right[th_id] );				// 接回原本的資料

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.shadow_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp );

	// 刪除補值的部分
	shadow_data4.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	shadow_data4.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();
}







/***************************************************************************************
	批次執行讀圖
/****************************************************************************************/
void	Screen::batch_loadimage()
{
	int					i;
	int					x,		y,		x2,		y2;

	assert( direction == EMBOSS_UP_DOWN || direction == EMBOSS_LEFT_RIGHT || direction == EMBOSS_LU_RD || direction == EMBOSS_LD_RU || direction == EMBOSS_ALL_DIRECTION );
#ifdef _DEBUG
	for( i = 0; i < size; i++ )
		assert( seq_line_front[i].size() == seq_line_back[i].size() && seq_line_front[i].size() == size );
#endif


	// 開始對檢測區取資料做判斷
	switch( direction )
	{
		case EMBOSS_UP_DOWN:
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x_left + i;
				end.x	=	x_left + i;
				start.y	=	y_up;
				end.y	=	y_down;
				loadimage( seq_line_front[i], seq_line_back[i] );
			}
			break;

		case EMBOSS_LEFT_RIGHT :
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x_left;
				end.x	=	x_right;
				start.y	=	y_up + i;
				end.y	=	y_up + i;
				loadimage( seq_line_front[i], seq_line_back[i] );
			}
			break;

	
		case EMBOSS_LU_RD:
			x	=	x_left;
			y	=	(y_up + y_down)/2;
			x2	=	x;
			y2	=	y + 1;
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x + i;							
				end.x	=	x + i + size - 1;				
				start.y	=	y - i;							
				end.y	=	y - i + size - 1;				
				loadimage( seq_line_front[i], seq_line_back[i] );

				start.x	=	x2 + i;
				end.x	=	x2 + i + size - 1;
				start.y	=	y2 - i;
				end.y	=	y2 - i + size - 1;
				loadimage( seq_line_front2[i], seq_line_back2[i] );
			}
			break;

		case EMBOSS_LD_RU:
			x	=	x_left;
			y	=	(y_up + y_down)/2;
			x2	=	x;
			y2	=	y + 1;
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x + i;
				end.x	=	x + i + size - 1;
				start.y	=	y + i ;
				end.y	=	y + i - size + 1;
				loadimage( seq_line_front[i], seq_line_back[i] );

				start.x	=	x2 + i;
				end.x	=	x2 + i + size - 1;
				start.y	=	y2 + i ;
				end.y	=	y2 + i - size + 1;
				loadimage( seq_line_front2[i], seq_line_back2[i] );
			}
			break;

		case EMBOSS_ALL_DIRECTION:
			for(  i = 0; i < size; i++ )
			{
				direction	=	EMBOSS_UP_DOWN;
				start.x		=	x_left + i;
				end.x		=	x_left + i;
				start.y		=	y_up;
				end.y		=	y_down;
				loadimage( seq_line_up[i], seq_line_down[i] );

				direction	=	EMBOSS_LEFT_RIGHT;
				start.x		=	x_left;
				end.x		=	x_right;
				start.y		=	y_up + i;
				end.y		=	y_up + i;
				loadimage( seq_line_left[i], seq_line_right[i] );

				direction	=	EMBOSS_ALL_DIRECTION;	// 因為可以在loadimage做檢查 所以每次都要把direction重設一次
			}
			assert( direction == EMBOSS_ALL_DIRECTION );
			break;

		default:
			assert(false);
	}
}





/***************************************************************************************
	回傳 絲網檢測 的 thr
	設定樣張的時候使用
	必須在excution後才能呼叫此function
	不然會沒有陰影資料
/****************************************************************************************/
int		Screen::determine_screen_value()
{
	assert( direction == EMBOSS_UP_DOWN || direction == EMBOSS_LEFT_RIGHT || direction == EMBOSS_LU_RD || 
		    direction == EMBOSS_LD_RU   || direction == EMBOSS_LU_RD      || direction == EMBOSS_LD_RU );

	assert( shadow.size1() == shadow.size2() && area_map1.size1() == area_map1.size2() && shadow.size1() == area_map1.size1() );
	assert( shadow2.size1() == shadow2.size2() && area_map2.size1() == area_map2.size2() && shadow2.size1() == area_map2.size1() );
	assert( shadow.size1() == shadow2.size1() && area_map1.size1() == area_map2.size1() );

	int		i,	j;
	int		area1;		
	int		area2;
	
	/* 
		中線點的起點跟終點
		這邊的切法是  上下 上 下  都切左右塊    (方向一致)   area1 = 上  area2 = 下
		              左右 左 右  都切上下塊				 area1 = 左  area2 = 下
					  斜向還在思考中
	*/
	area1	=	0;		
	area2	=	0;

	// 開始計算陰影分布
	for( i = 0; i < size; i++ )
		for( j = 0; j < size; j++ )
		{
			if( shadow( i, j ) == SHADOW_VALUE )
			{
				if( direction == EMBOSS_UP_DOWN || direction == EMBOSS_LEFT_RIGHT || direction == EMBOSS_LU_RD || direction == EMBOSS_LD_RU )
				{
					if( area_map1(i,j) == 1 )	area1++;
					else						area2++;		// 因為沒有中立區 所以非一及二
				}
				else
					assert(false);
			}
			if( direction == EMBOSS_LU_RD || direction == EMBOSS_LD_RU )
			{
				if( shadow2( i, j ) == SHADOW_VALUE )
				{
					if( area_map2(i,j) == 1 )	area1++;
					else						area2++;		// 因為沒有中立區 所以非一及二
				}
			}
		}

	return	abs( area1 - area2 );		// 回傳值直接放兩個區塊的值相減
}




/***************************************************************************************
	絲網檢測 結果

	請注意 陰影容許禁區 這邊需要多加判斷
	(因為兩邊判斷正廢方式不同 只好這樣寫)
/****************************************************************************************/
void	Screen::screen_result( SquareData &square )
{
	// 雙方向
	if( is_bin_direction(direction) )
		screen_result_v_d( square );

	else if( direction == EMBOSS_ALL_DIRECTION )
	{	
		square.sc_data.value	=	all_dir_result();
		square.sc_data.direct	=	EMBOSS_DIRECTION_MAX;		// 改成用值來判斷 方向永遠給一個會錯的值 (搭配AOI程式寫法)
	}
	else
		assert(false);
}



/***************************************************************************************
	絲網檢測 結果
	計算value & direction 
	(目前沒有拆成兩個function)
/****************************************************************************************/
void	Screen::screen_result_v_d( SquareData &square )
{
	const int		THR1			=	roi.sc_data.th1;
	const int		THR2			=	roi.sc_data.th2;

	assert( shadow.size1() == shadow.size2() && shadow2.size1() == shadow2.size2() && shadow.size1() == shadow2.size1() );
	assert( area_map1.size1() == area_map1.size2() && area_map2.size1() == area_map2.size2() && area_map1.size1() == area_map2.size1() );
	assert( shadow.size1() == size );

	int			i,	j;
	
	/* 
		中線點的起點跟終點
		這邊的切法是  上下 上 下  都切左右塊    (方向一致)   area1 = 上  area2 = 下
		              左右 左 右  都切上下塊				 area1 = 左  area2 = 下
					  斜向還在思考中
	*/
	int		area1	=	0;		
	int		area2	=	0;

	// 將陰影結果印出

	// 開始計算陰影分布
	for( i = 0; i < size; i++ )
		for( j = 0; j < size; j++ )
		{
			if( shadow( i, j ) == SHADOW_VALUE )
			{
				if( direction == EMBOSS_UP_DOWN || direction == EMBOSS_LEFT_RIGHT || direction == EMBOSS_LU_RD || direction == EMBOSS_LD_RU )
				{
					if( area_map1(i,j) == 1 )			area1++;
					else if( area_map1(i,j) == 2 )		area2++;		// 因為沒有中立區 所以非一及二
					else								assert(false);
				}
				else
					assert(false);
			}
			if( direction == EMBOSS_LU_RD || direction == EMBOSS_LD_RU )
			{
				if( shadow2( i, j ) == SHADOW_VALUE )
				{
					if( area_map2(i,j) == 1 )			area1++;
					else if( area_map2(i,j) == 2 )		area2++;		// 因為沒有中立區 所以非一及二
					else								assert(false);
				}
			}
		}

	// 回寫資料
	square.sc_data.area1	=	area1;
	square.sc_data.area2	=	area2;

	// 產生結果 值
	if( area1 - area2 >= THR1 )
		square.sc_data.value	=	area1;

	else if( area2 - area1 >= THR2 )
		square.sc_data.value	=	area2;

	else
		square.sc_data.value	=	(area1 + area2)/2;


	// 產生結果  方向 
	switch(direction)
	{
		case EMBOSS_UP:
		case EMBOSS_DOWN:
		case EMBOSS_UP_DOWN:
			if( area1 - area2 > THR1 )
				square.sc_data.direct	=	EMBOSS_UP;
			else if( area2 - area1 > THR2 )
				square.sc_data.direct	=	EMBOSS_DOWN;
			else
				square.sc_data.direct	=	EMBOSS_MIDDLE;
			break;
		case EMBOSS_LEFT:
		case EMBOSS_RIGHT:
		case EMBOSS_LEFT_RIGHT:
			if( area1 - area2 > THR1 )
				square.sc_data.direct	=	EMBOSS_LEFT;
			else if( area2 - area1 > THR2 )
				square.sc_data.direct	=	EMBOSS_RIGHT;
			else
				square.sc_data.direct	=	EMBOSS_MIDDLE;
			break;
		case EMBOSS_LU_RD:
			if( area1 - area2 > THR1 )
				square.sc_data.direct	=	EMBOSS_LEFT_UP;
			else if( area2 - area1 > THR2 )
				square.sc_data.direct	=	EMBOSS_RIGHT_DOWN;
			else
				square.sc_data.direct	=	EMBOSS_MIDDLE;
			break;
		case EMBOSS_LD_RU:
			if( area1 - area2 > THR1 )
				square.sc_data.direct	=	EMBOSS_LEFT_DOWN;
			else if( area2 - area1 > THR2 )
				square.sc_data.direct	=	EMBOSS_RIGHT_UP;
			else
				square.sc_data.direct	=	EMBOSS_MIDDLE;
			break;
		default:
			assert(false);
	}

}





/***************************************************************************************
	產生預設的絲網map
/****************************************************************************************/
void	Screen::generate_center_line_map()
{
	area_map1.resize( size, size );
	area_map2.resize( size, size );

	int		i,	j;

	for( i = 0; i < size; i++ )
		for( j = 0; j < size; j++ )
		{
			if( i < size/2 )
			{
				area_map1( i, j )	=	1;
				area_map2( i, j )	=	1;
			}
			else
			{
				area_map1( i, j )	=	2;
				area_map2( i, j )	=	2;
			}
		}
}


/***************************************************************************************
	絲網 
	絲網 將座標 轉成 map
	給定的值為之後要畫上去的顏色.
/****************************************************************************************/
bMatrixByte		Screen::screen_center_line_map( PIXEL anchor, int size, EMBOSS_DIRECTION direct, PIXEL start, PIXEL end )
{
	int				x1,			y1;					// 存放起點邊 的 一個點
	int				x2,			y2;					// 起點邊的另一個點
	int				i,			j;
	int				start_x,	start_y;
	int				x,			y;
	double			value;
	bMatrixByte		r_data( size, size );			// 回傳值

	if( direct == EMBOSS_LU_RD || direct == EMBOSS_LD_RU )
	{
		anchor.x	-=	size/2;
		anchor.y	+=	size/2;		// 因為定位點是左上邊中點 所以要平移
	}

	/*
		給定點 (x1,y1)   (x2,y2)
		得到 line     (x-x1)(y-y2) - (x-x2)(y-y1) = 0
	*/

	/*
		這邊給2  給1 旋轉90度
		是為了搭配AOI程式
		修改的話要小心bug
	*/

	switch(direct)
	{
		case EMBOSS_UP_DOWN:
			x1			=	anchor.x;	
			y1			=	anchor.y;
			x2			=	anchor.x;
			y2			=	anchor.y + size - 1;	// 考慮頭尾問題
			assert( start.x == x1 && start.x == x2 && start.y >= y1 && start.y <= y2 );		// 檢查點是否落在邊框上

			x1			=	anchor.x + size - 1;	// 考慮頭尾問題
			y1			=	anchor.y;
			x2			=	anchor.x + size - 1;
			y2			=	anchor.y + size - 1;
			assert( end.x == x1 && end.x == x2 && end.y >= y1 && end.y <= y2 );

			for( i = 0; i < size; i++ )
			{
				for( j = 0; j < size; j++ )
				{
					x		=	anchor.x + i;
					y		=	anchor.y + j;
					value	=	(x-start.x)*(y-end.y) - (x-end.x)*(y-start.y) ;
					if( value >= 0 )	r_data(i,j)		=	2;
					else				r_data(i,j)		=	1;		// 沒有不處理區
				}
			}
			break;

		case EMBOSS_LEFT_RIGHT:
			x1			=	anchor.x;	
			y1			=	anchor.y;
			x2			=	anchor.x + size - 1;	// 考慮頭尾問題
			y2			=	anchor.y;
			assert( start.x >= x1 && start.x <= x2 && start.y == y1 && start.y == y2 );

			x1			=	anchor.x;	
			y1			=	anchor.y + size - 1;	// 考慮頭尾問題
			x2			=	anchor.x + size - 1;
			y2			=	anchor.y + size - 1;

			for( i = 0; i < size; i++ )
			{
				for( j = 0; j < size; j++ )
				{
					x		=	anchor.x + i;
					y		=	anchor.y + j;
					value	=	(x-start.x)*(y-end.y) - (x-end.x)*(y-start.y) ;
					if( value >= 0 )	r_data(i,j)		=	1;
					else				r_data(i,j)		=	2;		// 沒有不處理區
				}
			}
			break;

		case EMBOSS_LU_RD:
			x1			=	anchor.x;	
			y1			=	anchor.y;
			x2			=	anchor.x + size - 1;	// 考慮頭尾問題
			y2			=	anchor.y + size - 1;
			assert( start.x - x1 == start.y - y1 && x2 - start.x == y2 - start.y );

			x1			=	anchor.x + size - 1;	// 考慮頭尾問題
			y1			=	anchor.y - size + 1;	// 實際上是 + (size-1)    - (size-1)
			x2			=	x1 + size - 1;
			y2			=	y1 + size - 1;
			assert( end.x - x1 == end.y - y1 && x2 - end.x == y2 - end.y );

			for( i = 0; i < size; i++ )
			{
				start_x		=	anchor.x + i;
				start_y		=	anchor.y - i;

				for( j = 0; j < size; j++ )
				{
					x		=	start_x + j;
					y		=	start_y + j;	// x,y 是 對應到map的座標
					value	=	(x-start.x)*(y-end.y) - (x-end.x)*(y-start.y) ;

					if( value >= 0 )	r_data(j,i)		=	2;
					else				r_data(j,i)		=	1;		// 沒有不處理區
									       /* 故意旋轉90度*/
				}
			}
			break;

		case EMBOSS_LD_RU:
			x1			=	anchor.x;	
			y1			=	anchor.y;
			x2			=	anchor.x + size - 1;	// 考慮頭尾問題
			y2			=	anchor.y - size + 1;
			assert( start.x - x1 == y1 - start.y && x2 - start.x == start.y - y2 );

			x1			=	anchor.x + size - 1;	// 考慮頭尾問題
			y1			=	anchor.y + size - 1;
			x2			=	x1 + size - 1;
			y2			=	y1 - size + 1;
			assert( end.x - x1 == y1 - end.y && x2 - end.x == end.y - y2 );

			for( i = 0; i < size; i++ )
			{
				start_x		=	anchor.x + i;
				start_y		=	anchor.y + i;

				for( j = 0; j < size; j++ )
				{
					x		=	start_x + j;
					y		=	start_y - j;	// x,y 是 對應到map的座標
					value	=	(x-start.x)*(y-end.y) - (x-end.x)*(y-start.y) ;

					if( value >= 0 )	r_data(i,j)		=	1;
					else				r_data(i,j)		=	2;		// 沒有不處理區
				}
			}
			break;

		case EMBOSS_ALL_DIRECTION:
			r_data.clear();
			break;
		default:
			assert(false);
	}

	return	r_data;
}






/***************************************************************************************
	絲網 
	絲網 將座標 轉成 map
	給定的值為之後要畫上去的顏色.
	2號給斜向使用
/****************************************************************************************/
bMatrixByte		Screen::screen_center_line_map2( PIXEL anchor, int size, EMBOSS_DIRECTION direct, PIXEL start, PIXEL end )
{
	int				x1,			y1;					// 存放起點邊 的 一個點
	int				x2,			y2;					// 起點邊的另一個點
	int				i,			j;
	int				start_x,	start_y;
	int				x,			y;
	double			value;
	bMatrixByte		r_data( size, size );			// 回傳值

	/*
		這邊給2  給1 旋轉90度
		是為了搭配AOI程式
		修改的話要小心bug
	*/


	if( direct == EMBOSS_LU_RD || direct == EMBOSS_LD_RU )
	{
		anchor.x	-=	size/2;
		anchor.y	+=	size/2;		// 因為定位點是左上邊中點 所以要平移
	}

	switch(direct)
	{
		case EMBOSS_UP_DOWN:
		case EMBOSS_LEFT_RIGHT:
			r_data.clear();
			break;

		case EMBOSS_LU_RD:
			x1			=	anchor.x;	
			y1			=	anchor.y;
			x2			=	anchor.x + size - 1;	// 考慮頭尾問題
			y2			=	anchor.y + size - 1;
			assert( start.x - x1 == start.y - y1 && x2 - start.x == y2 - start.y );

			x1			=	anchor.x + size - 1;	// 考慮頭尾問題
			y1			=	anchor.y - size + 1;	// 其實是 + (size-1) 跟 - (size-1)
			x2			=	x1 + size - 1;
			y2			=	y1 + size - 1;
			assert( end.x - x1 == end.y - y1 && x2 - end.x == y2 - end.y );

			for( i = 0; i < size; i++ )
			{
				start_x		=	anchor.x + i;
				start_y		=	anchor.y - i + 1;
									// 留意這個offset
				for( j = 0; j < size; j++ )
				{
					x		=	start_x + j;
					y		=	start_y + j;	// x,y 是 對應到map的座標
					value	=	(x-start.x)*(y-end.y) - (x-end.x)*(y-start.y) ;

					if( value >= 0 )	r_data(j,i)		=	2;
					else				r_data(j,i)		=	1;		// 沒有不處理區
					                     /*故意旋轉90度*/
				}
			}
			break;

		case EMBOSS_LD_RU:
			x1			=	anchor.x;	
			y1			=	anchor.y;
			x2			=	anchor.x + size - 1;	// 考慮頭尾問題
			y2			=	anchor.y - size + 1;
			assert( start.x - x1 == y1 - start.y && x2 - start.x == start.y - y2 );

			x1			=	anchor.x + size - 1;	// 考慮頭尾問題
			y1			=	anchor.y + size - 1;
			x2			=	x1 + size - 1;
			y2			=	y1 - size + 1;
			assert( end.x - x1 == y1 - end.y && x2 - end.x == end.y - y2 );

			for( i = 0; i < size; i++ )
			{
				start_x		=	anchor.x + i;
				start_y		=	anchor.y + i + 1;
									// 留意這個offset
				for( j = 0; j < size; j++ )
				{
					x		=	start_x + j;
					y		=	start_y - j;	// x,y 是 對應到map的座標
					value	=	(x-start.x)*(y-end.y) - (x-end.x)*(y-start.y) ;

					if( value >= 0 )	r_data(i,j)		=	1;
					else				r_data(i,j)		=	2;		// 沒有不處理區
				}
			}
			break;

		case EMBOSS_ALL_DIRECTION:
			r_data.clear();
			break;
		default:
			assert(false);
	}

	return	r_data;
}



/***************************************************************************************
	設定圖片
/****************************************************************************************/
void	Screen::set_image( AndreaImage imagePass )
{
	image	=	imagePass;
}







/***************************************************************************************
	批次轉換資料
/****************************************************************************************/
void	Screen::batch_linedata2matrix()
{
	assert( shadow.size1() == shadow.size2() && shadow.size1() == size );
	assert( shadow2.size1() == shadow2.size2() && shadow2.size1() == size );

	int		i,	j;

	switch(direction)
	{
		case EMBOSS_UP_DOWN:
			/*
				資料排列        轉成 
				1   6           1  2  3  4  5
				2   7           6  7  8  9 10    
				3   8
				4   9
				5  10
				( i, j ) 相等  因為資料存放會在旋轉90度
			*/
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
					shadow( i, j )	=	line_data[i].shadow_seq(j);
			break;

		case EMBOSS_LEFT_RIGHT:
			/*
				資料排列           轉成 
				1  2  3  4  5      1  2  3  4  5
				6  7  8  9 10      6  7  8  9 10          (要注意旋轉 90 度)
			*/
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
					shadow( i, j )	=	line_data[j].shadow_seq(i);
			break;


		case EMBOSS_LU_RD:			// left up <-> right down 暫時跟 EMBOSS_LEFT_UP 相同
			/*
				資料排列           轉成 
				  6                1  2  3  4  5
				1   7              6  7  8  9 10         這邊希望轉成跟左下一樣 (方便印出)
				  2  8
					3  9
					  4  10
						5
			*/
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					shadow( i, j )	=	line_data[j].shadow_seq(i);
					// 第二筆資料要轉
					shadow2( i, j )	=	line_data2[j].shadow_seq(i);
				}
			break;


		case EMBOSS_LD_RU:			// left down <-> right up 暫時跟left down相同
			/*
				資料排列           轉成 
						5		   1  2  3  4  5            (斜向都希望以這個為準)
					  4  10        6  7  8  9 10
					3   9
				  2   8  
				1   7    
				  6  8
				   11
			*/
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					shadow( i, j )	=	line_data[i].shadow_seq(j);
					// 第二筆資料
					shadow2( i, j )	=	line_data2[i].shadow_seq(j);
				}
			break;
		case EMBOSS_ALL_DIRECTION:

			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					up_map( i, j )		=	line_data_up[i].shadow_seq(j);	
					down_map( i, j )	=	line_data_down[i].shadow_seq(size-j-1);
					left_map( i, j )	=	line_data_left[j].shadow_seq(i);
					right_map( i, j )	=	line_data_right[j].shadow_seq(size-i-1);

					// 轉換完 開始計算結果 (併成一個迴圈為了加速)
					if( up_map(i,j) == 1 || down_map(i,j) == 1 || left_map(i,j) == 1 || right_map(i,j) == 1 )
						shadow( i, j )	=	1;
					else
						shadow( i, j )	=	0;

				}
			break;

		default:
			assert(false);

	}
}





/***************************************************************************************
	一次檢查一條線
	絲網雙向用
	批次用
/****************************************************************************************/
void	Screen::batch_shadow_line( int th_id )
{
	int		i;

	// front 方向
	
	// 產生往前延伸資料
	seq_extend.set_value( 0, extend_len, seq_line_front[th_id](0) );
	// 接回原本的資料
	seq_extend.add_vector( extend_len, seq_line_front[th_id] );

	// 陰影資料 丟入下一層的function做運算
	shadow_obj.screen_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp, th_id );

	// 刪除補值的部分
	line_front.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	line_front.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();


	// 完成一個方向了....

	// back 方向

	// 往前補值
	seq_extend.set_value( 0, extend_len, seq_line_back[th_id](0) );
	// 接回原本的資料
	seq_extend.add_vector( extend_len, seq_line_back[th_id] );

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.screen_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp, th_id );

	// 刪除補值的部分
	line_back.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	line_back.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();


	// 進行merge 合併兩個資料  front back
	for( i = 0; i < original_len; i++ )
	{
		// 交集
		if( (bump == CONCAVE && dark_light == EMBOSS_LIGHT) || (bump == PROTRUDING && dark_light == EMBOSS_DARK) )
		{
			                                                                         /* 反轉  */
			if( line_front.shadow_seq(i) == SHADOW_VALUE && line_back.shadow_seq( original_len-i-1) == SHADOW_VALUE )
				line_data[th_id].shadow_seq(i)		=	SHADOW_VALUE;
			else
				line_data[th_id].shadow_seq(i)		=	UN_SHADOW_VALUE;
		}
		// 聯集
		else 
		{
			                                                                        /* 反轉 */
			if( line_front.shadow_seq(i) == SHADOW_VALUE || line_back.shadow_seq(original_len-i-1) == SHADOW_VALUE )
				line_data[th_id].shadow_seq(i)		=	SHADOW_VALUE;
			else
				line_data[th_id].shadow_seq(i)		=	UN_SHADOW_VALUE;
		}
	}
}


/***************************************************************************************
	一次檢查一條線
	絲網雙向用
	批次用
	2號 使用在斜向
/****************************************************************************************/
void	Screen::batch_shadow_line_2( int th_id )
{
	int		i;

	// front 方向
	
	// 產生往前延伸資料
	seq_extend.set_value( 0, extend_len, seq_line_front2[th_id](0) );
	// 接回原本的資料
	seq_extend.add_vector( extend_len, seq_line_front2[th_id] );

	// 陰影資料 丟入下一層的function做運算
	shadow_obj.screen_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp, th_id );

	// 刪除補值的部分
	line_front.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	line_front.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();

	// 完成一個方向了....

	// back 方向

	// 往前補值
	seq_extend.set_value( 0, extend_len, seq_line_back2[th_id](0) );
	// 接回原本的資料
	seq_extend.add_vector( extend_len, seq_line_back2[th_id] );

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.screen_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp, th_id );

	// 刪除補值的部分
	line_back.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	line_back.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();


	// 進行merge 合併兩個資料  front back
	for( i = 0; i < original_len; i++ )
	{
		// 交集
		if( (bump == CONCAVE && dark_light == EMBOSS_LIGHT) || (bump == PROTRUDING && dark_light == EMBOSS_DARK) )
		{
			if( line_front.shadow_seq(i) == SHADOW_VALUE && line_back.shadow_seq( original_len-i-1 ) == SHADOW_VALUE )
				line_data2[th_id].shadow_seq(i)		=	SHADOW_VALUE;
			else
				line_data2[th_id].shadow_seq(i)		=	UN_SHADOW_VALUE;
		}
		// 聯集
		else 
		{
			if( line_front.shadow_seq(i) == SHADOW_VALUE || line_back.shadow_seq( original_len-i-1 ) == SHADOW_VALUE )
				line_data2[th_id].shadow_seq(i)		=	SHADOW_VALUE;
			else
				line_data2[th_id].shadow_seq(i)		=	UN_SHADOW_VALUE;
		}
	}


}



/***************************************************************************************
	判斷是否為垂直水平方向(上下左右)   
/****************************************************************************************/
bool	Screen::is_vertical_horizontal( const EMBOSS_DIRECTION direction )
{
	if( direction == EMBOSS_UP || direction == EMBOSS_DOWN ||
		direction == EMBOSS_LEFT || direction == EMBOSS_RIGHT ||
		direction == EMBOSS_UP_DOWN || direction == EMBOSS_LEFT_RIGHT )
			return	true;
	else
			return	false;
}






/***************************************************************************************
	設定檢測區邊界範圍  定位點
/****************************************************************************************/
void	Screen::set_square_bound()
{
	assert( roi_x >= 0 && roi_y >= 0 );
	assert( size > 0 );
	assert( direction < EMBOSS_DIRECTION_MAX );

	// 上下左右四方向
	if( is_vertical_horizontal(direction) || direction == EMBOSS_ALL_DIRECTION )
	{
		x_left	=	roi_x;						// x的左邊起始點
		x_right	=	roi_x + size - 1;			// x的右邊終點
		y_up	=	roi_y;						// y的上方起始點
		y_down	=	roi_y + size - 1;   		// y的下方終點
	}	
	// 左上左下右上右下四個方向  這邊會稍微有點歪 暫時先不處理
	else
	{
		x_left	=	roi_x - size/2;				// x的左邊起始點
		x_right	=	roi_x + size/2 + size;  	// x的右邊終點
		y_up	=	roi_y - size/2;         	// y的上方起始點
		y_down	=	roi_y + size/2 + size;  	// y的下方終點
	}

	switch( direction )
	{
		case EMBOSS_UP :
			// 設定定位點 (依據方向)
			anchor_x	=	x_left;
			anchor_y	=	y_up;
			break;

		case EMBOSS_DOWN :
			anchor_x	=	x_left;
			anchor_y	=	y_down;
			break;

		case EMBOSS_LEFT :
			anchor_x	=	x_left;
			anchor_y	=	y_up;
			break;

		case EMBOSS_RIGHT :
			anchor_x	=	x_right;
			anchor_y	=	y_up;
			break;

		case EMBOSS_LEFT_UP :
			// 這邊會超微偏出檢測區  要小心 
			// 判斷方式會跟上下左右的部分有些不同
			anchor_x	=	x_left;								// 斜向分兩組資料
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;							// 資料二 (往上或下平移一格)
			anchor_y2	=	anchor_y + 1;
			break;
			
		case EMBOSS_LEFT_DOWN :
			anchor_x	=	x_left;
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;
			anchor_y2	=	anchor_y + 1;
			break;

		case EMBOSS_RIGHT_UP :
			anchor_x	=	x_right;
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;
			anchor_y2	=	anchor_y + 1;
			break;

		case EMBOSS_RIGHT_DOWN :
			anchor_x	=	x_right;
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;
			anchor_y2	=	anchor_y + 1; 
			break;

		case EMBOSS_LU_RD :			// 左上 到 右下
			anchor_x	=	x_left;							// 斜向分兩組資料
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;					// 資料二 (往上或下平移一格)
			anchor_y2	=	anchor_y + 1;
			break;

		case EMBOSS_LD_RU :			// 左下 到 右上
			anchor_x	=	x_left;
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;
			anchor_y2	=	anchor_y + 1;
			break;

		case EMBOSS_ALL_DIRECTION:
		case EMBOSS_UP_DOWN:
		case EMBOSS_LEFT_RIGHT:
			// 不需要設定邊界  因為會拆開成不同方向來做
			break;

		default:
			assert(false);
	}

}




/***************************************************************************************
	絲網檢測用的
	讀取圖檔資料
	雙向加速用 (避免記憶體重複宣告)
	必須假設 seq 之前就已經完成resize動作
/****************************************************************************************/
void	Screen::loadimage( bVectorByte &seq_front, bVectorByte &seq_back )
{
	assert( seq_front.size() == seq_back.size() && seq_front.size() == size );

	int			i;

	// 檢查點是否正確  檢查相對位置
	switch(direction)
	{
		case EMBOSS_UP_DOWN:
			assert(start.x == end.x && start.y < end.y);
			break;
		case EMBOSS_LEFT_RIGHT:
			assert(start.x < end.x && start.y == end.y);
			break;
		case EMBOSS_LU_RD:
			assert(end.x-start.x == end.y-start.y);
			break;
		case EMBOSS_LD_RU:
			assert(end.x-start.x == start.y-end.y);
			break;
		default:
			assert(false);
	}

#ifdef _DEBUG  
	// 檢查size  debug模式下才使用
	int		tmp_size;
	switch(direction)
	{
		case EMBOSS_UP_DOWN:
			tmp_size	=	end.y - start.y + 1;
			break;
		case EMBOSS_LEFT_RIGHT:
			tmp_size	=	end.x - start.x + 1;
			break;
		case EMBOSS_LU_RD:
			tmp_size	=	end.x - start.x + 1;
			break;
		case EMBOSS_LD_RU:
			tmp_size	=	end.x - start.x + 1;
			break;
		default:
			assert(false);
	}
	assert( seq_front.size() == tmp_size && seq_back.size() == tmp_size && size == tmp_size );		// 檢查seq的size
#endif

	assert( channel_func != NULL );

	// 開始讀取圖片資料
	switch( direction )
	{
		case EMBOSS_UP_DOWN:
			for( i = 0; i < size; i++ )
			{
				seq_front(i)		=	(image.* channel_func)( start.x, start.y + i );
				// 直接將index反轉
				seq_back(size-i-1)	=	(image.* channel_func)( start.x, start.y + i );
			}
			break;

		case EMBOSS_LEFT_RIGHT:
			for( i = 0; i < size; i++ )
			{
				seq_front(i)		=	(image.* channel_func)( start.x + i, start.y );
				seq_back(size-i-1)	=	(image.* channel_func)( start.x + i, start.y );
			}
			break;

		case EMBOSS_LU_RD:
			for( i = 0; i < size; i++ )
			{
				seq_front(i)		=	(image.* channel_func)( start.x + i, start.y + i );
				seq_back(size-i-1)	=	(image.* channel_func)( start.x + i, start.y + i );
			}
			break;

		case EMBOSS_LD_RU:
			for( i = 0; i < size; i++ )
			{
				seq_front(i)		=	(image.* channel_func)( start.x + i, start.y - i );
				seq_back(size-i-1)	=	(image.* channel_func)( start.x + i, start.y - i );
			}
			break;

		default :
			assert(false);
	}
}


/***************************************************************************************
	陰影檢測用的
	讀取圖檔資料
	目前會用在決定bump這種不需要效率的地方.
/****************************************************************************************/
bVectorInt		Screen::loadimage( PIXEL s, PIXEL e, const EMBOSS_DIRECTION direction )
{
	bVectorInt		seq;
	int				i;

	// 檢查點是否正確  檢查相對位置
	switch(direction)
	{
		case EMBOSS_UP:
			assert( s.x == e.x && s.y < e.y);
			break;
		case EMBOSS_DOWN:
			assert( s.x == e.x && s.y > e.y );
			break;
		case EMBOSS_LEFT:
			assert( s.x < e.x && s.y == e.y );
			break;
		case EMBOSS_RIGHT:
			assert( s.x > e.x && s.y == e.y );
			break;
		case EMBOSS_LEFT_UP:
			assert( e.x - s.x == e.y - s.y );
			break;
		case EMBOSS_LEFT_DOWN:
			assert( e.x - s.x == s.y - e.y );
			break;
		case EMBOSS_RIGHT_UP:
			assert( s.x - e.x == e.y - s.y );
			break;
		case EMBOSS_RIGHT_DOWN:
			assert( s.x - e.x == s.y - e.y );
			break;
		default:
			assert(false);
	}

#ifdef _DEBUG
	int		_size	=	0;

	// 先決定size  (因為考慮長方形 這邊會有所不同 預先考慮進去)
	switch(direction)
	{
		case EMBOSS_UP :
			_size	=	e.y - s.y + 1;
			break;
		case EMBOSS_DOWN :
			_size	=	s.y - e.y + 1;
			break;
		case EMBOSS_LEFT :
			_size	=	e.x - s.x + 1;
			break;
		case EMBOSS_RIGHT :
			_size	=	s.x - e.x + 1;
			break;
		case EMBOSS_LEFT_UP :
			_size	=	e.x - s.x + 1;
			break;
		case EMBOSS_LEFT_DOWN :
			_size	=	e.x - s.x + 1;
			break;
		case EMBOSS_RIGHT_UP :
			_size	=	s.x - e.x + 1;
			break;
		case EMBOSS_RIGHT_DOWN :
			_size	=	s.x - e.x + 1;
			break;
		default:
			assert(false);
	}
	assert( _size == size );
#endif

	// 初始化seq
	seq.resize(size);

	assert( channel_func != NULL );

	// 開始讀取圖片資料
	switch( direction )
	{
		case EMBOSS_UP :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( s.x, s.y + i );
			break;

		case EMBOSS_DOWN :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( s.x, s.y - i );
			break;

		case EMBOSS_LEFT:
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( s.x + i, s.y );
			break;

		case EMBOSS_RIGHT :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( s.x - i, s.y );
			break;

		case EMBOSS_LEFT_UP :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( s.x + i, s.y + i );
			break;

		case EMBOSS_LEFT_DOWN :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( s.x + i, s.y - i );
			break;

		case EMBOSS_RIGHT_UP :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( s.x - i, s.y + i );
			break;

		case EMBOSS_RIGHT_DOWN :
			for( i = 0; i < size; i++ )
				seq(i)	=	(image.* channel_func)( s.x - i, s.y - i );
			break;

		default :
			assert(false);
	}

	return	seq;
}


/***************************************************************************************
	批次畫defect map
/****************************************************************************************/
void	Screen::batch_result_output_screen_defect( AndreaImage img, const int value )
{
	assert( shadow.size1() == shadow.size2()  );
	assert( shadow2.size1() == shadow2.size2()  );
	assert( shadow.size1() == shadow2.size1() && shadow.size1() == size );

	const int	x	=	roi_x;
	const int	y	=	roi_y;

	int		i,	j;
	int		mask		=	0x80;	// 用最高位 = 1來當mask  區分兩個區塊
	int		tmp_value	=	0;		

	switch(direction)
	{
		case EMBOSS_UP_DOWN:
		case EMBOSS_LEFT_RIGHT:
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					tmp_value	=	0;
					if( area_map1(i,j) == 1 )
						tmp_value	|=	mask;						// 兩區塊 一個區塊加上mask 最高位bit
					if( shadow(i,j) == SHADOW_VALUE )
						tmp_value	|=	value;						// 寫入原本的資料

					img.gray_channel( i + x, j + y, tmp_value );	// 寫入資料
				}
			break;

		case EMBOSS_LU_RD:
		case EMBOSS_LD_RU:
		// 這兩個都用左下當基準  來做旋轉
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 第一方向
					tmp_value	=	0;
					if( area_map1(i,j) == 1 )
						tmp_value	|=	mask;			// 兩區塊 一個區塊加上mask 最高位bit
					if( shadow(i,j) == SHADOW_VALUE )
						tmp_value	|=	value;			// 寫入原本的資料

					img.gray_channel( x_left + i + j, (y_up+y_down)/2 + i - j, tmp_value );

					// 第二方向
					tmp_value	=	0;
					if( area_map2(i,j) == 1 )
						tmp_value	|=	mask;			// 兩區塊 一個區塊加上mask 最高位bit
					if( shadow2(i,j) == SHADOW_VALUE )
						tmp_value	|=	value;			// 寫入原本的資料

					img.gray_channel( x_left + i + j, (y_up+y_down)/2 + i - j + 1, tmp_value );		// (固定都用 y+1 當 offset)*/
				}
			break;
		
		case EMBOSS_ALL_DIRECTION:
			// 只要出現陰影都算 沒有mask的觀念
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					if( shadow( i, j ) == SHADOW_VALUE )
						img.gray_channel( x + i, y + j , value );
				}

			break;


		default:
			assert(false);
	}
}




/***************************************************************************************
	畫出兩個區塊
/****************************************************************************************/
void	Screen::result_output_screen_area( AndreaImage img )
{
	int					x			=	roi_x;
	int					y			=	roi_y;
	int					i,			j;
	int					start_x,	start_y;

	assert( area_map1.size1() == size && area_map1.size2() == size );
	assert( area_map2.size1() == size && area_map2.size2() == size );

	if( direction == EMBOSS_LU_RD || direction == EMBOSS_LD_RU )
	{
		x	-=	size/2;
		y	+=	size/2;		// 定位點轉回左邊點
	}

	switch(direction)
	{
		case EMBOSS_UP_DOWN:
		case EMBOSS_LEFT_RIGHT:
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					if( area_map1(i,j) == 1 )			img.pixel_channel( x+i, y+j, 123, 255, 0 );
					else if ( area_map1(i,j) == 2 )		img.pixel_channel( x+i, y+j, 255, 0, 123 );
					else								img.pixel_channel( x+i, y+j, 0, 123, 255 );
				}
			break;
		case EMBOSS_LU_RD:
			for( i = 0; i < size; i++ )
			{
				start_x	=	x + i;
				start_y	=	y - i;
				for( j = 0; j < size; j++ )
				{
					      /*故意的旋轉*/
					if( area_map1(j,i) == 1 )			img.pixel_channel( start_x+j, start_y+j, 123, 255, 0 );
					else if ( area_map1(j,i) == 2 )		img.pixel_channel( start_x+j, start_y+j, 255, 0, 123 );
					else								img.pixel_channel( start_x+j, start_y+j, 0, 123, 255 );

					if( area_map2(j,i) == 1 )			img.pixel_channel( start_x+j, start_y+j +1, 123, 255, 0 );
					else if ( area_map2(j,i) == 2 )		img.pixel_channel( start_x+j, start_y+j +1, 255, 0, 123 );
					else								img.pixel_channel( start_x+j, start_y+j +1, 0, 123, 255 );
				}
			}
			break;
		case EMBOSS_LD_RU:
			for( i = 0; i < size; i++ )
			{
				start_x	=	x + i;		
				start_y	=	y + i;
				for( j = 0; j < size; j++ )
				{
					if( area_map1(i,j) == 1 )			img.pixel_channel( start_x+j, start_y-j, 123, 255, 0 );
					else if ( area_map1(i,j) == 2 )		img.pixel_channel( start_x+j, start_y-j, 255, 0, 123 );
					else								img.pixel_channel( start_x+j, start_y-j, 0, 123, 255 );

					if( area_map2(i,j) == 1 )			img.pixel_channel( start_x+j, start_y-j +1, 123, 255, 0 );
					else if ( area_map2(i,j) == 2 )		img.pixel_channel( start_x+j, start_y-j +1, 255, 0, 123 );
					else								img.pixel_channel( start_x+j, start_y-j +1, 0, 123, 255 );
				}
			}
			break;
		case EMBOSS_ALL_DIRECTION:
			break;
		default:
			assert(false);
	}
}




/***************************************************************************************
	取得方向
/****************************************************************************************/
EMBOSS_DIRECTION	Screen::get_direction()
{
	//return	roi.direction;		// 好像沒辦法檢查是否有塞進資料@~@  未來改用指標(?)
	return	direction;
}




/***************************************************************************************
	畫出陰影
/****************************************************************************************/
void	Screen::result_output_shadow( AndreaImage img )
{
	assert( shadow.size1() == shadow.size2() && shadow2.size1() == shadow2.size2() && shadow.size1() == shadow2.size1() && shadow.size1() == size );

	const int			x			=	roi_x;
	const int			y			=	roi_y;

	int		i,	j;

	switch(direction)
	{
		case EMBOSS_UP :
		case EMBOSS_DOWN:
		case EMBOSS_LEFT:
		case EMBOSS_RIGHT:
		case EMBOSS_UP_DOWN:
		case EMBOSS_LEFT_RIGHT:
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 畫出陰影
					if( shadow(i,j) == SHADOW_VALUE )
						img.gray_channel( i + x, j + y, 227 );
						//img.pixel_channel( i + x, j + y, 0, 255, 255 );
				}
			break;

		case EMBOSS_LEFT_UP:
		case EMBOSS_LEFT_DOWN:
		case EMBOSS_LU_RD:
		case EMBOSS_LD_RU:
		// 這兩個都用左下當基準  來做旋轉
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 畫出陰影
					if( shadow(i,j) == SHADOW_VALUE )
						img.gray_channel( x_left + i + j, (y_up+y_down)/2 + i - j, 227 );
						//img.pixel_channel( x_left + i + j, (y_up+y_down)/2 + i - j, 0, 255, 255 );

					if( shadow2(i,j) == SHADOW_VALUE )
						img.gray_channel( x_left + i + j, (y_up+y_down)/2 + i - j + 1, 227 );
						//img.pixel_channel( x_left + i + j, (y_up+y_down)/2 + i - j + 1, 0, 255, 255 );		// (固定都用 y+1 當 offset)

				}
			break;

		case EMBOSS_RIGHT_UP:
		case EMBOSS_RIGHT_DOWN:
		// 這兩個要多一個offset
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 劃出陰影
					if( shadow(i,j) == SHADOW_VALUE )
						img.gray_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j, 227 );
						//img.pixel_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j, 0, 255, 255 );
					                                           
					if( shadow2(i,j) == SHADOW_VALUE )
											           /*留意這邊的計算*/
						img.gray_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j + 1, 227 );	
						//img.pixel_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j + 1, 0, 255, 255 );		// (固定都用 y+1 當 offset)

				}
			break;
		
		case EMBOSS_ALL_DIRECTION:
			// 只要出現陰影都算 沒有mask的觀念
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					if( shadow( i, j ) == SHADOW_VALUE )
						img.gray_channel( x + i, y + j , 227 );
						//img.pixel_channel( x + i, y + j , 0, 255, 255 );
				}

			break;


		default:
			assert(false);
	}
}







/***************************************************************************************
	設定 roi 資料 
	(預先產生好記憶體空間避免re-allocate)
/****************************************************************************************/
void	Screen::set( SquareData _roi )
{
	int		i;

	roi				=	_roi;
	size			=	roi.size;
	direction		=	roi.direction;
	dark_light		=	roi.dark_light;
	bump			=	roi.bump;

	// 設定要讀取的channel  沒設定會跑出錯誤
	switch( roi.channel )
	{
		case RED:
			channel_func	=	&AndreaImage::red_channel;
			break;

		case GREEN:
			channel_func	=	&AndreaImage::green_channel;
			break;

		case BLUE:
			channel_func	=	&AndreaImage::blue_channel;
			break;

		case GRAY:
			channel_func	=	&AndreaImage::gray_channel;
			break;

		default:
			assert(false);
	}

	original_len	=	size;

	roi_x			=	roi.x;
	roi_y			=	roi.y;

	good_bad		=	EMBOSS_GOOD;

	// 開始設定變數
	shadow_settings.dark_light	=	roi.dark_light;
	shadow_settings.threshold	=	roi.threshold;
	shadow_settings.enhance		=	false;

	shadow_obj.set( original_len + extend_len, shadow_settings );	// 注意 這邊的size不是原本的size 是加上補值的size!!


	line_data	=	new	ShadowReturnData[size];
	for( i = 0; i < size; i++ )
		line_data[i]	=	ShadowReturnData(size);
	line_data2	=	new ShadowReturnData[size];
	for( i = 0; i < size; i++ )
		line_data2[i]	=	ShadowReturnData(size);

	line_front	=	ShadowReturnData(size);
	line_back	=	ShadowReturnData(size);

	seq_line_front	=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
		seq_line_front[i].resize(size);

	seq_line_front2	=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
		seq_line_front2[i].resize(size);

	seq_line_back	=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
		seq_line_back[i].resize(size);

	seq_line_back2	=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
		seq_line_back2[i].resize(size);

	area_map1	=	roi.sc_data.area_map1;
	area_map2	=	roi.sc_data.area_map2;


	seq_extend.resize( original_len + extend_len );
	
	//shadow_data_tmp		=	ShadowReturnData( original_len + extend_len );
	shadow_data_tmp.init( original_len + extend_len );

	// 全方向
	seq_line_up		=	new bVectorByte[size];
	seq_line_down	=	new bVectorByte[size];
	seq_line_left	=	new bVectorByte[size];
	seq_line_right	=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
	{
		seq_line_up[i].resize(size);
		seq_line_down[i].resize(size);
		seq_line_left[i].resize(size);
		seq_line_right[i].resize(size);
	}

	line_data_up		=	new ShadowReturnData[size];
	line_data_down		=	new ShadowReturnData[size];
	line_data_left		=	new ShadowReturnData[size];
	line_data_right		=	new ShadowReturnData[size];
	for( i = 0; i < size; i++ )
	{
		line_data_up[i]		=	ShadowReturnData(size);
		line_data_down[i]	=	ShadowReturnData(size);
		line_data_left[i]	=	ShadowReturnData(size);
		line_data_right[i]	=	ShadowReturnData(size);
		
	}

	up_map.resize(size,size);
	down_map.resize(size,size);
	left_map.resize(size,size);
	right_map.resize(size,size);

	shadow.resize(size,size);
	shadow2.resize(size,size);


	// 這個設定必須在設定好roi之後.
	set_square_bound();

}



/***************************************************************************************
	決定該區塊的圖騰是凹還是凸 
	Page2會直接使用
/****************************************************************************************/
bool	Screen::determine_bump( AndreaImage imagePass )
{
	image		=	imagePass;		// 設定圖片

	assert( direction == EMBOSS_UP_DOWN || direction == EMBOSS_LEFT_RIGHT || direction == EMBOSS_LU_RD || direction == EMBOSS_LD_RU || direction == EMBOSS_ALL_DIRECTION );
	
	double				value1,	value2;
	int					sum,	count;
	int					*mask;
	int					i,		j;
	int					x,		y;

	PIXEL				p1,		p2;
	bMatrixByte			map( size, size );
	bVectorInt			line( size );

	if( direction == EMBOSS_ALL_DIRECTION )
	{
		roi.bump	=	PROTRUDING;		// 預設值
		return	true;
	}

	// 讀取資料
	switch(direction)
	{
		case EMBOSS_UP_DOWN:
			for( i = 0; i < size; i++ )
			{
				p1.x	=	x_left + i;
				p2.x	=	x_left + i;
				p1.y	=	y_up;
				p2.y	=	y_down;
				line	=	loadimage( p1, p2, EMBOSS_UP );

				for( j = 0; j < size; j++ )
					map( i, j )		=	line(j);
			}
			break;

		case EMBOSS_LEFT_RIGHT:
			for( i = 0; i < size; i++ )
			{
				p1.x	=	x_left;
				p2.x	=	x_right;
				p1.y	=	y_up + i;
				p2.y	=	y_up + i;
				line	=	loadimage( p1, p2, EMBOSS_LEFT );

				for( j = 0; j < size; j++ )
					map( i, j )		=	line(j);
			}
			break;

		case EMBOSS_LU_RD:
			x	=	x_left;
			y	=	(y_up + y_down)/2;
			for( i = 0; i < size; i++ )
			{
				p1.x	=	x + i;							
				p2.x	=	x + i + size - 1;				
				p1.y	=	y - i;							
				p2.y	=	y - i + size - 1;				
				line	=	loadimage( p1, p2, EMBOSS_LEFT_UP );							// 第一筆資料 這邊應該只需要一筆資料就夠了

				for( j = 0; j < size; j++ )
					map( i, j )		=	line(j);
			}
			break;

		case EMBOSS_LD_RU:
			x	=	x_left;
			y	=	(y_up + y_down)/2;
			for( i = 0; i < size; i++ )
			{
				p1.x	=	x + i;
				p2.x	=	x + i + size - 1;
				p1.y	=	y + i ;
				p2.y	=	y + i - size + 1;
				line	=	loadimage( p1, p2, EMBOSS_LEFT_DOWN );

				for( j = 0; j < size; j++ )
					map( i, j )		=	line(j);
			}
			break;

		default:
			assert(false);
	}

	// 加總
	for( i = 0; i < size; i++ )
	{
		sum		=	0;
		for( j = 0; j < size; j++ )
			sum		+=	map( j, i );
		line(i)		=	sum;
	}

	// 產生mask
	mask	=	new int[size];

	for( i = 0; i < (size+1)/2; i++ )
		mask[i]	=	i+1;
	for( i = (size+1)/2; i < size; i++ )
		mask[i]	=	size-i;
	
	// 計算值
	count	=	0;
	for( i = 0; i < size; i++ )
		count	+=	mask[i];

	// 用mask加總
	sum		=	0;
	for( i = 0; i < size; i++ )
		sum		+=	mask[i] * line(i);
	value1	=	1.0*sum / count;

	// 算平均
	sum		=	0;
	for( i = 0; i < size; i++ )
		sum		+=	line(i);
	value2	=	1.0*sum / size;

	delete [] mask;		// 記得刪除資料
	mask	=	0;

	if( value1 > value2 )		// 凸
	{
		shadow_obj.bump			=	PROTRUDING;		// 直接修改了  為了方便
		roi.bump				=	PROTRUDING;		// 這邊底下不能刪掉 會有bug
		bump					=	PROTRUDING;
		shadow_settings.bump	=	PROTRUDING;
	}
	else if( value1 < value2 )	// 凹
	{
		shadow_obj.bump			=	CONCAVE;		// 直接修改了  為了方便
		roi.bump				=	CONCAVE;
		bump					=	CONCAVE;
		shadow_settings.bump	=	CONCAVE;
	}
	else
	{
		shadow_obj.bump			=	BUMP_MAX;		// 直接修改了  為了方便
		roi.bump				=	BUMP_MAX;		// default setting
		bump					=	BUMP_MAX;
		shadow_settings.bump	=	BUMP_MAX;
		return	false;
	}

	return	true;
}









/***************************************************************************************
	判斷是否為 雙 方向
/****************************************************************************************/
bool	Screen::is_bin_direction( const EMBOSS_DIRECTION direction )
{
	if( direction == EMBOSS_UP_DOWN || direction == EMBOSS_LEFT_RIGHT ||
		direction == EMBOSS_LU_RD || direction == EMBOSS_LD_RU )
			return	true;
	else
		return	false;
}




/***************************************************************************************
	是否為單方向
/****************************************************************************************/
bool	Screen::is_single_direction( const EMBOSS_DIRECTION direction )
{
	if( direction == EMBOSS_UP || direction == EMBOSS_DOWN ||
		direction == EMBOSS_LEFT || direction == EMBOSS_RIGHT ||
		direction == EMBOSS_LEFT_UP || direction == EMBOSS_LEFT_DOWN ||
		direction == EMBOSS_RIGHT_UP || direction == EMBOSS_RIGHT_DOWN )
			return	true;
	else
		return	false;
}






/***************************************************************************************
	絲網印出結果
/****************************************************************************************/
void	Screen::result_output_screen( AndreaImage img )
{
	// 暫時關掉
	//if( direction == EMBOSS_UP_DOWN || direction == EMBOSS_LEFT_RIGHT ||
	//	direction == EMBOSS_LU_RD   || direction == EMBOSS_LD_RU )
	//		result_output_screen_area( img );				// 標記兩個區塊

	result_output_shadow( img );					// 標記陰影
}







/***************************************************************************************
	相當於原本的execution  
	multi-thread時使用
	絲網雙向使用
/****************************************************************************************/
void	Screen::exec()
{
	int		i;

	if( direction == EMBOSS_ALL_DIRECTION )
	{
		for( i = 0; i < size; i++ )
		{
			batch_shadow_ALLD(i);
		}
		return;
	}

	for( i = 0; i < size; i++ )
		batch_shadow_line(i);

	if( direction >= EMBOSS_LU_RD && direction <= EMBOSS_LD_RU )		// 為了加速 小心enum value
	{
		for( i = 0; i < size; i++ )
			batch_shadow_line_2(i);
	}
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Emboss  class  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/***************************************************************************************
	建構
/****************************************************************************************/
Emboss::Emboss()
{
	channel_func	=	NULL;
	good_bad		=	EMBOSS_GOOD;
	//Shadow					shadow_obj;											// 陰影檢測物件
	direction		=	EMBOSS_DIRECTION_MAX;					
	size			=	-1;											
	x_left			=	-1;
	x_right			=	-1;
	y_up			=	-1;
	y_down			=	-1;		
	roi_x			=	-1;
	roi_y			=	-1;
	anchor_x		=	-1;
	anchor_y		=	-1;
	anchor_x2		=	-1;
	anchor_y2		=	-1;
	start			=	PIXEL(-1,-1);
	end				=	PIXEL(-1,-1);;
	//AndreaImage				image;
	//SquareData				roi;												// 檢測區資料 預先要好資料
	//KernelDensity			kd;													// 用來過濾雜訊
	line_data		=	NULL;											
	line_data2		=	NULL;
	seq_line		=	NULL;	
	seq_line2		=	NULL;
	//bVectorByte				seq_extend;											// 會做補值的動作
	original_len	=	-1;
	//ShadowReturnData		shadow_data_tmp;									// 陰影資料  (一個是原始的 一個是刪除延伸資料拿來回傳用的的)
	//ShadowSettings			shadow_settings;									// 陰影設定 屬於唯獨資料
	//bMatrixDouble			bm_tmp;												// kernel density使用
	seq_line_up		=	NULL;
	seq_line_down	=	NULL;
	seq_line_left	=	NULL;
	seq_line_right	=	NULL;
	line_data_up	=	NULL;
	line_data_down	=	NULL;
	line_data_left	=	NULL;
	line_data_right	=	NULL;
	//bMatrixByte				up_map;												// 暫存 最後用來merge
	//bMatrixByte				down_map;
	//bMatrixByte				left_map;
	//bMatrixByte				right_map;

	//shadow.resize(0,0);	
	//shadow2.resize(0,0);
	//deep;
	//deep2;


}


/***************************************************************************************
	解構
/****************************************************************************************/
Emboss::~Emboss()
{
	good_bad		=	EMBOSS_GOOD;
	//Shadow					shadow_obj;											// 陰影檢測物件
	direction		=	EMBOSS_DIRECTION_MAX;					
	size			=	-1;											
	x_left			=	-1;
	x_right			=	-1;
	y_up			=	-1;
	y_down			=	-1;		
	roi_x			=	-1;
	roi_y			=	-1;
	anchor_x		=	-1;
	anchor_y		=	-1;
	anchor_x2		=	-1;
	anchor_y2		=	-1;
	start			=	PIXEL(-1,-1);
	end				=	PIXEL(-1,-1);;
	//AndreaImage				image;
	//SquareData				roi;												// 檢測區資料 預先要好資料
	//KernelDensity			kd;													// 用來過濾雜訊
	delete [] line_data;	line_data		=	NULL;											
	delete [] line_data2;	line_data2		=	NULL;
	delete [] seq_line;		seq_line		=	NULL;	
	delete [] seq_line2;	seq_line2		=	NULL;
	//bVectorByte				seq_extend;											// 會做補值的動作
	original_len	=	-1;
	//ShadowReturnData		shadow_data_tmp;									// 陰影資料  (一個是原始的 一個是刪除延伸資料拿來回傳用的的)
	//ShadowSettings			shadow_settings;									// 陰影設定 屬於唯獨資料
	//bMatrixDouble			bm_tmp;												// kernel density使用
	delete [] seq_line_up;		seq_line_up		=	NULL;
	delete [] seq_line_down;	seq_line_down	=	NULL;
	delete [] seq_line_left;	seq_line_left	=	NULL;
	delete [] seq_line_right;	seq_line_right	=	NULL;
	delete [] line_data_up;		line_data_up	=	NULL;
	delete [] line_data_down;	line_data_down	=	NULL;
	delete [] line_data_left;	line_data_left	=	NULL;
	delete [] line_data_right;	line_data_right	=	NULL;
	//bMatrixByte				up_map;												// 暫存 最後用來merge
	//bMatrixByte				down_map;
	//bMatrixByte				left_map;
	//bMatrixByte				right_map;


}


/***************************************************************************************
	批次執行讀圖
/****************************************************************************************/
void	Emboss::batch_loadimage()
{
	int					i;
	int					x,		y,		x2,		y2;

	// 開始對檢測區取資料做判斷
	switch( direction )
	{
		case EMBOSS_UP :
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x_left + i;
				end.x	=	x_left + i;
				start.y	=	y_up;
				end.y	=	y_down;
				loadimage( seq_line[i] );
			}
			break;

		case EMBOSS_DOWN :
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x_left + i;
				end.x	=	x_left + i;
				start.y	=	y_down;
				end.y	=	y_up;
				loadimage( seq_line[i] );
			}
			break;

		case EMBOSS_LEFT :
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x_left;
				end.x	=	x_right;
				start.y	=	y_up + i;
				end.y	=	y_up + i;
				loadimage( seq_line[i] );
			}
			break;

		case EMBOSS_RIGHT :
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x_right;
				end.x	=	x_left;
				start.y	=	y_up + i;
				end.y	=	y_up + i;
				loadimage( seq_line[i] );
			}
			break;

		case EMBOSS_LEFT_UP :
			x	=	x_left;
			y	=	(y_up + y_down)/2;
			x2	=	x;
			y2	=	y + 1;
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x + i;							
				end.x	=	x + i + size - 1;				
				start.y	=	y - i;							
				end.y	=	y - i + size - 1;				
				loadimage( seq_line[i] );

				start.x	=	x2 + i;
				end.x	=	x2 + i + size - 1;
				start.y	=	y2 - i;
				end.y	=	y2 - i + size - 1;
				loadimage( seq_line2[i] );
			}
			break;
			
		case EMBOSS_LEFT_DOWN :
			x	=	x_left;
			y	=	(y_up + y_down)/2;
			x2	=	x;
			y2	=	y + 1;
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x + i;
				end.x	=	x + i + size - 1;
				start.y	=	y + i ;
				end.y	=	y + i - size + 1;
				loadimage( seq_line[i] );

				start.x	=	x2 + i;
				end.x	=	x2 + i + size - 1;
				start.y	=	y2 + i ;
				end.y	=	y2 + i - size + 1;
				loadimage( seq_line2[i] );
			}
			break;

		case EMBOSS_RIGHT_UP :
			x	=	x_right;
			y	=	(y_up + y_down)/2;
			x2	=	x;
			y2	=	y + 1;
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x - i;
				end.x	=	x - i - size + 1;
				start.y	=	y - i;
				end.y	=	y - i + size - 1;
				loadimage( seq_line[i] );

				start.x	=	x2 - i;
				end.x	=	x2 - i - size + 1;
				start.y	=	y2 - i;
				end.y	=	y2 - i + size - 1;
				loadimage( seq_line2[i] );
			}
			break;

		case EMBOSS_RIGHT_DOWN :
			x	=	x_right;
			y	=	(y_up + y_down)/2;
			x2	=	x;
			y2	=	y + 1;
			for(  i = 0; i < size; i++ )
			{
				start.x	=	x - i;
				end.x	=	x - i - size + 1;
				start.y	=	y + i ;
				end.y	=	y + i - size + 1;
				loadimage( seq_line[i] );

				start.x	=	x2 - i;
				end.x	=	x2 - i - size + 1;
				start.y	=	y2 + i ;
				end.y	=	y2 + i - size + 1;
				loadimage( seq_line2[i] );
			}
			break;

		case EMBOSS_ALL_DIRECTION:
			for(  i = 0; i < size; i++ )
			{
				// EMBOSS_UP
				direction	=	EMBOSS_UP;

				start.x	=	x_left + i;
				end.x	=	x_left + i;
				start.y	=	y_up;
				end.y	=	y_down;
				loadimage( seq_line_up[i] );

				// EMBOSS_DOWN
				direction	=	EMBOSS_DOWN;

				start.x	=	x_left + i;
				end.x	=	x_left + i;
				start.y	=	y_down;
				end.y	=	y_up;
				loadimage( seq_line_down[i] );

				// EMBOSS_LEFT
				direction	=	EMBOSS_LEFT;

				start.x	=	x_left;
				end.x	=	x_right;
				start.y	=	y_up + i;
				end.y	=	y_up + i;
				loadimage( seq_line_left[i] );

				// EMBOSS_RIGHT
				direction	=	EMBOSS_RIGHT;

				start.x	=	x_right;
				end.x	=	x_left;
				start.y	=	y_up + i;
				end.y	=	y_up + i;
				loadimage( seq_line_right[i] );

				// 將方向改回來
				direction	=	EMBOSS_ALL_DIRECTION;		//放在這邊是為了可以偵測一些錯誤
			}
			assert( direction == EMBOSS_ALL_DIRECTION );
			break;

		default:
			assert(false);
	}
}





/***************************************************************************************
	設定圖片
/****************************************************************************************/
void	Emboss::set_image( AndreaImage imagePass )
{
	image	=	imagePass;
}




/***************************************************************************************
	批次濾除雜訊
/****************************************************************************************/
void	Emboss::batch_kernel_density()
{
	assert( kd_th >= 0 && kd_th <= 1 );

	if( direction == EMBOSS_ALL_DIRECTION )
		return;

	int					i,	j;			

	kd.dosomething( bm_tmp, shadow );

	// 將濾掉陰影的資料逆向設定回去 line data
	for( i = 0; i < size; i++ )
		for( j = 0; j < size; j++ )
		{
			// 這邊採用取交集的作法 (讓點變少 濾掉noise造成的)
			if( bm_tmp( i, j ) < kd_th )					// 只有一個點的過濾掉
				shadow( i, j )	=	UN_SHADOW_VALUE;
		}

	// 斜向要做第二次
	if( !is_vertical_horizontal(roi.direction) )
	{
		kd.dosomething( bm_tmp, shadow2 );

		for( i = 0; i < size; i++ )
			for( j = 0; j < size; j++ )
			{
				// 這邊採用取交集的作法 (讓點變少 濾掉noise造成的)
				if( bm_tmp( i, j ) < kd_th )					// 只有一個點的過濾掉
					shadow2( i, j )		=	UN_SHADOW_VALUE;
			}
	}
}



/***************************************************************************************
	批次轉換資料
/****************************************************************************************/
void	Emboss::batch_linedata2matrix()
{
	assert( shadow.size1() == shadow.size2() && shadow.size1() == deep.size1() && deep.size1() == deep.size2() );
	assert( shadow2.size1() == shadow2.size2() && shadow2.size1() == deep2.size1() && deep2.size1() == deep2.size2() );
	assert( shadow.size1() == shadow2.size1() && deep.size1() == deep2.size1() );

	int					i,	j;

	switch(direction)
	{
		case EMBOSS_UP :
			/*
				資料排列        轉成 
				1   6           1  2  3  4  5
				2   7           6  7  8  9 10    
				3   8
				4   9
				5  10
				( i, j ) 相等  因為資料存放會在旋轉90度
			*/
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					shadow( i, j )	=	line_data[i].shadow_seq(j);
					deep( i, j )	=	line_data[i].shadow_depth(j);
				}
			break;

		case EMBOSS_DOWN :
			/*
				資料排列        轉成 
				5  10           1  2  3  4  5
				4   9           6  7  8  9 10    
				3   8
				2   7
				1   6
			*/
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					shadow( i, j )	=	line_data[i].shadow_seq(size-j-1);
					deep( i, j )	=	line_data[i].shadow_depth(size-j-1);
				}
			break;

		case EMBOSS_LEFT :
			/*
				資料排列           轉成 
				1  2  3  4  5      1  2  3  4  5
				6  7  8  9 10      6  7  8  9 10          (要注意旋轉 90 度)
			*/
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					shadow( i, j )	=	line_data[j].shadow_seq(i);
					deep( i, j )	=	line_data[j].shadow_depth(i);
				}
			break;

		case EMBOSS_RIGHT :
			/*
				資料排列           轉成 
				5  4  3  2  1      1  2  3  4  5
			   10  9  8  7  6      6  7  8  9 10          (要注意旋轉 90 度)
			*/
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					shadow( i, j )	=	line_data[j].shadow_seq(size-i-1);
					deep( i, j )	=	line_data[j].shadow_depth(size-i-1);
				}
			break;

		case EMBOSS_LEFT_UP:
			/*
				資料排列           轉成 
				  6                1  2  3  4  5
				1   7              6  7  8  9 10         這邊希望轉成跟左下一樣 (方便印出)
				  2  8
					3  9
					  4  10
						5
			*/
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					shadow( i, j )	=	line_data[j].shadow_seq(i);
					deep( i, j )	=	line_data[j].shadow_depth(i);
					// 第二筆資料要轉
					shadow2( i, j )	=	line_data2[j].shadow_seq(i);
					deep2( i, j )	=	line_data2[j].shadow_depth(i);
				}
			break;

		case EMBOSS_LEFT_DOWN:
			/*
				資料排列           轉成 
						5		   1  2  3  4  5            (斜向都希望以這個為準)
					  4  10        6  7  8  9 10
					3   9
				  2   8  
				1   7    
				  6  8
				   11
			*/
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					shadow( i, j )	=	line_data[i].shadow_seq(j);
					deep( i, j )	=	line_data[i].shadow_depth(j);
					// 第二筆資料
					shadow2( i, j )	=	line_data2[i].shadow_seq(j);
					deep2( i, j )	=	line_data2[i].shadow_depth(j);
				}
			break;

		case EMBOSS_RIGHT_UP:
			/*
				資料排列           轉成 
				   11              1  2  3  4  5
					  6            6  7  8  9 10
					7   1    
				  8   2
				9   3
			 10   4
				5
			*/			
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					shadow( i, j )	=	line_data[size-i-1].shadow_seq(size-j-1);
					deep( i, j )	=	line_data[size-i-1].shadow_depth(size-j-1);
					// 第二筆資料
					shadow2( i, j )	=	line_data2[size-i-1].shadow_seq(size-j-1);
					deep2( i, j )	=	line_data2[size-i-1].shadow_depth(size-j-1);
				}
			break;


		case EMBOSS_RIGHT_DOWN:
			/*
				資料排列           轉成 
					 5	           1  2  3  4  5
				  10  4            6  7  8  9 10
					9  3  
					 8  2
					  7  1
					   6
			*/	
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					shadow( i, j )	=	line_data[size-j-1].shadow_seq(size-i-1);
					deep( i, j )	=	line_data[size-j-1].shadow_depth(size-i-1);
					// 第二筆資料
					shadow2( i, j )	=	line_data2[size-j-1].shadow_seq(size-i-1);
					deep2( i, j )	=	line_data2[size-j-1].shadow_depth(size-i-1);
				}
			break;

		case EMBOSS_ALL_DIRECTION:
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					up_map( i, j )		=	line_data_up[i].shadow_seq(j);
					down_map( i, j )	=	line_data_down[i].shadow_seq(size-j-1);
					left_map( i, j )	=	line_data_left[j].shadow_seq(i);
					right_map( i, j )	=	line_data_right[j].shadow_seq(size-i-1);

					// 併成一個迴圈  為了加速 
					if( up_map(i,j) == SHADOW_VALUE || down_map(i,j) == SHADOW_VALUE || left_map(i,j) == SHADOW_VALUE || right_map(i,j) == SHADOW_VALUE )
						shadow( i, j )	=	1;
					else
						shadow( i, j )	=	0;
				}
			break;

		default:
			assert(false);

	}
}





/***************************************************************************************
	陰影檢測用的
	一次檢查一條線
/****************************************************************************************/
void	Emboss::batch_shadow_line_2( int th_id )
{
	ShadowReturnData	&shadow_data	=	line_data2[th_id];

	// 產生往前延伸資料
	seq_extend.set_value( 0, extend_len, seq_line2[th_id](0) );
	// 接回原本的資料
	seq_extend.add_vector( extend_len, seq_line2[th_id] );

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.shadow_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp );

	// 刪除補值的部分
	shadow_data.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	shadow_data.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );
	shadow_data.shadow_depth.add_vector( shadow_data_tmp.shadow_depth,    extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();
}






/***************************************************************************************
	陰影檢測用的
	一次檢查一條線
	全方向使用
/****************************************************************************************/
void	Emboss::batch_shadow_ALLD( int th_id )
{
	// EMBOSS_UP
	ShadowReturnData	&shadow_data	=	line_data_up[th_id];

	// 產生往前延伸資料
	// 往前補值
	seq_extend.set_value( 0, extend_len, seq_line_up[th_id](0) );
	// 接回原本的資料
	seq_extend.add_vector( extend_len, seq_line_up[th_id] );

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.shadow_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp );

	// 刪除補值的部分
	shadow_data.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	shadow_data.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );
	shadow_data.shadow_depth.add_vector( shadow_data_tmp.shadow_depth,    extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();

	// EMBOSS_DOWN
	ShadowReturnData	&shadow_data2	=	line_data_down[th_id];

	// 產生往前延伸資料
	// 往前補值
	seq_extend.set_value( 0, extend_len, seq_line_down[th_id](0) );
	// 接回原本的資料
	seq_extend.add_vector( extend_len, seq_line_down[th_id] );

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.shadow_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp );

	// 刪除補值的部分
	shadow_data2.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	shadow_data2.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );
	shadow_data2.shadow_depth.add_vector( shadow_data_tmp.shadow_depth,    extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();

	// EMBOSS_LEFT
	ShadowReturnData	&shadow_data3	=	line_data_left[th_id];

	// 產生往前延伸資料
	// 往前補值
	seq_extend.set_value( 0, extend_len, seq_line_left[th_id](0) );
	// 接回原本的資料
	seq_extend.add_vector( extend_len, seq_line_left[th_id] );

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.shadow_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp );

	// 刪除補值的部分
	shadow_data3.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	shadow_data3.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );
	shadow_data3.shadow_depth.add_vector( shadow_data_tmp.shadow_depth,    extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();

	// EMBOSS_RIGHT
	ShadowReturnData	&shadow_data4	=	line_data_right[th_id];

	// 產生往前延伸資料
	// 往前補值
	seq_extend.set_value( 0, extend_len, seq_line_right[th_id](0) );
	// 接回原本的資料
	seq_extend.add_vector( extend_len, seq_line_right[th_id] );

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.shadow_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp );

	// 刪除補值的部分
	shadow_data4.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	shadow_data4.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );
	shadow_data4.shadow_depth.add_vector( shadow_data_tmp.shadow_depth,    extend_len, extend_len+original_len );

	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();
}



/***************************************************************************************
	陰影檢測用的
	一次檢查一條線
/****************************************************************************************/
void	Emboss::batch_shadow_line( int th_id )
{
	ShadowReturnData	&shadow_data	=	line_data[th_id];

	// 產生往前延伸資料
	seq_extend.set_value( 0, extend_len, seq_line[th_id](0) );
	// 接回原本的資料
	seq_extend.add_vector( extend_len, seq_line[th_id] );

	// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料
	shadow_obj.shadow_detector( seq_extend, /*shadow_settings,*/ shadow_data_tmp );

	// 刪除補值的部分
	shadow_data.mono_sub_seq.add_vector( shadow_data_tmp.mono_sub_seq,    extend_len, extend_len+original_len );
	shadow_data.shadow_seq.add_vector  ( shadow_data_tmp.shadow_seq,      extend_len, extend_len+original_len );
	shadow_data.shadow_depth.add_vector( shadow_data_tmp.shadow_depth,    extend_len, extend_len+original_len );


	shadow_data_tmp.mono_sub_seq.clear_add_value();		// 清除補值
	seq_extend.clear_add_value();
}





/***************************************************************************************
	判斷是否為垂直水平方向(上下左右)   
/****************************************************************************************/
bool	Emboss::is_vertical_horizontal( const EMBOSS_DIRECTION direction )
{
	if( direction == EMBOSS_UP || direction == EMBOSS_DOWN ||
		direction == EMBOSS_LEFT || direction == EMBOSS_RIGHT ||
		direction == EMBOSS_UP_DOWN || direction == EMBOSS_LEFT_RIGHT )
			return	true;
	else
			return	false;
}







/***************************************************************************************
	設定檢測區邊界範圍  定位點
/****************************************************************************************/
void	Emboss::set_square_bound()
{
	assert( roi_x >= 0 && roi_y >= 0 );
	assert( size > 0 );
	assert( direction < EMBOSS_DIRECTION_MAX );

	// 上下左右四方向
	if( is_vertical_horizontal(direction) || direction == EMBOSS_ALL_DIRECTION )
	{
		x_left	=	roi_x;					// x的左邊起始點
		x_right	=	roi_x + size - 1;		// x的右邊終點
		y_up	=	roi_y;					// y的上方起始點
		y_down	=	roi_y + size - 1;       // y的下方終點
	}	
	// 左上左下右上右下四個方向  這邊會稍微有點歪 暫時先不處理
	else
	{
		x_left	=	roi_x - size/2;			// x的左邊起始點
		x_right	=	roi_x + size/2 + size;  // x的右邊終點
		y_up	=	roi_y - size/2;         // y的上方起始點
		y_down	=	roi_y + size/2 + size;  // y的下方終點
	}

	switch( direction )
	{
		case EMBOSS_UP :
			// 設定定位點 (依據方向)
			anchor_x	=	x_left;
			anchor_y	=	y_up;
			break;

		case EMBOSS_DOWN :
			anchor_x	=	x_left;
			anchor_y	=	y_down;
			break;

		case EMBOSS_LEFT :
			anchor_x	=	x_left;
			anchor_y	=	y_up;
			break;

		case EMBOSS_RIGHT :
			anchor_x	=	x_right;
			anchor_y	=	y_up;
			break;

		case EMBOSS_LEFT_UP :
			// 這邊會超微偏出檢測區  要小心 
			// 判斷方式會跟上下左右的部分有些不同
			anchor_x	=	x_left;								// 斜向分兩組資料
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;							// 資料二 (往上或下平移一格)
			anchor_y2	=	anchor_y + 1;
			break;
			
		case EMBOSS_LEFT_DOWN :
			anchor_x	=	x_left;
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;
			anchor_y2	=	anchor_y + 1;
			break;

		case EMBOSS_RIGHT_UP :
			anchor_x	=	x_right;
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;
			anchor_y2	=	anchor_y + 1;
			break;

		case EMBOSS_RIGHT_DOWN :
			anchor_x	=	x_right;
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;
			anchor_y2	=	anchor_y + 1; 
			break;

		case EMBOSS_LU_RD :			// 左上 到 右下
			anchor_x	=	x_left;							// 斜向分兩組資料
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;					// 資料二 (往上或下平移一格)
			anchor_y2	=	anchor_y + 1;
			break;

		case EMBOSS_LD_RU :			// 左下 到 右上
			anchor_x	=	x_left;
			anchor_y	=	(y_up + y_down)/2;
			anchor_x2	=	anchor_x;
			anchor_y2	=	anchor_y + 1;
			break;

		case EMBOSS_ALL_DIRECTION:
		case EMBOSS_UP_DOWN:
		case EMBOSS_LEFT_RIGHT:
			// 不需要設定邊界  因為會拆開成不同方向來做
			break;

		default:
			assert(false);
	}

}





/***************************************************************************************
	陰影檢測用的
	讀取圖檔資料
	加速用 (避免記憶體重複宣告)
	必須假設 seq 之前就已經完成resize動作
/****************************************************************************************/
void	Emboss::loadimage( bVectorByte &seq )
{
	int			i;

	// 檢查點是否正確  檢查相對位置
	switch(direction)
	{
		case EMBOSS_UP:
			assert(start.x == end.x && start.y < end.y);
			break;
		case EMBOSS_DOWN:
			assert(start.x == end.x && start.y > end.y);
			break;
		case EMBOSS_LEFT:
			assert(start.x < end.x && start.y == end.y);
			break;
		case EMBOSS_RIGHT:
			assert(start.x > end.x && start.y == end.y);
			break;
		case EMBOSS_LEFT_UP:
			assert(end.x-start.x == end.y-start.y);
			break;
		case EMBOSS_LEFT_DOWN:
			assert(end.x-start.x == start.y-end.y);
			break;
		case EMBOSS_RIGHT_UP:
			assert(start.x-end.x == end.y-start.y);
			break;
		case EMBOSS_RIGHT_DOWN:
			assert(start.x-end.x == start.y-end.y);
			break;
		default:
			assert(false);
	}

#ifdef _DEBUG
	// 先決定size  (因為考慮長方形 這邊會有所不同 預先考慮進去) 
	// 檢查size  debug模式下才使用
	int		tmp_size;
	switch(direction)
	{
		case EMBOSS_UP :
			tmp_size	=	end.y - start.y + 1;
			break;
		case EMBOSS_DOWN :
			tmp_size	=	start.y - end.y + 1;
			break;
		case EMBOSS_LEFT :
			tmp_size	=	end.x - start.x + 1;
			break;
		case EMBOSS_RIGHT :
			tmp_size	=	start.x - end.x + 1;
			break;
		case EMBOSS_LEFT_UP :
			tmp_size	=	end.x - start.x + 1;
			break;
		case EMBOSS_LEFT_DOWN :
			tmp_size	=	end.x - start.x + 1;
			break;
		case EMBOSS_RIGHT_UP :
			tmp_size	=	start.x - end.x + 1;
			break;
		case EMBOSS_RIGHT_DOWN :
			tmp_size	=	start.x - end.x + 1;
			break;
		default:
			assert(false);
	}
	assert( seq.size() == tmp_size );		// 檢查seq的size
	assert( size == tmp_size );				// 檢查跟setting的size
#endif

	// 開始讀取圖片資料
	switch( direction )
	{
		case EMBOSS_UP :
			for( i = 0; i < size; i++ )
				//seq( i )	=	image.gray_channel( start.x, start.y + i );
				seq(i)		=	(image.* channel_func)( start.x, start.y + i );
			break;

		case EMBOSS_DOWN :
			for( i = 0; i < size; i++ )
				//seq( i )	=	image.gray_channel( start.x, start.y - i );
				seq( i )	=	(image.* channel_func)( start.x, start.y - i );
			break;

		case EMBOSS_LEFT:
			for( i = 0; i < size; i++ )
				//seq( i )	=	image.gray_channel( start.x + i, start.y );
				seq( i )	=	(image.* channel_func)( start.x + i, start.y );
			break;

		case EMBOSS_RIGHT :
			for( i = 0; i < size; i++ )
				//seq( i )	=	image.gray_channel( start.x - i, start.y );
				seq( i )	=	(image.* channel_func)( start.x - i, start.y );
				break;

		case EMBOSS_LEFT_UP :
			for( i = 0; i < size; i++ )
				//seq( i )	=	image.gray_channel( start.x + i, start.y + i );
				seq( i )	=	(image.* channel_func)( start.x + i, start.y + i );
			break;

		case EMBOSS_LEFT_DOWN :
			for( i = 0; i < size; i++ )
				//seq( i )	=	image.gray_channel( start.x + i, start.y - i );
				seq( i )	=	(image.* channel_func)( start.x + i, start.y - i );
			break;

		case EMBOSS_RIGHT_UP :
			for( i = 0; i < size; i++ )
				//seq(i)	=	image.gray_channel( start.x - i, start.y + i );
				seq( i )	=	(image.* channel_func)( start.x - i , start.y + i );
			break;

		case EMBOSS_RIGHT_DOWN :
			for( i = 0; i < size; i++ )
				//seq(i)	=	image.gray_channel( start.x - i, start.y - i );
				seq( i )	=	(image.* channel_func)( start.x - i, start.y - i );
			break;

		default :
			assert(false);
	}
}





/***************************************************************************************
	產生mask資料
	(預設 第二頁使用 避免沒資料)
/****************************************************************************************/
void	Emboss::generate_allow_area()
{
	const int			up_1		=	roi.em_data.up_line_1;
	const int			up_2		=	roi.em_data.up_line_2;
	const int			down_1		=	roi.em_data.down_line_1;
	const int			down_2		=	roi.em_data.down_line_2;

	int			i,		j;

	// 初始化資料
	allow_area.resize( size, size );
	allow_area.clear();
	allow_area2.resize( size, size );
	allow_area.clear();

	switch(direction)
	{
		case EMBOSS_UP :
		case EMBOSS_DOWN :
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
					allow_area( i, j )	=	0;
			break;
		case EMBOSS_LEFT :
		case EMBOSS_RIGHT :
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
					allow_area( i, j )	=	0;
			break;
		case EMBOSS_LEFT_UP :
		case EMBOSS_RIGHT_DOWN :
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					allow_area( i, j )	=	0;
					allow_area2( i, j )	=	0;
				}
			break;
		case EMBOSS_LEFT_DOWN :
		case EMBOSS_RIGHT_UP :
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					allow_area( i, j )	=	0;
					allow_area2( i, j )	=	0;
				}
			break;

		case EMBOSS_ALL_DIRECTION:
		case EMBOSS_UP_DOWN:
		case EMBOSS_LEFT_RIGHT:
		case EMBOSS_LU_RD:
		case EMBOSS_LD_RU:
			// 不需要有允許區
			break;

		default:
			assert(false);
	}
}



/***************************************************************************************
	批次標記defect map
	容許區會加上第一個位元作為mask
/****************************************************************************************/
void	Emboss::batch_result_output_emboss_defect( AndreaImage img, const int value )
{
	assert( shadow.size1() == shadow.size2() );
	assert( shadow2.size1() == shadow2.size2() );

	const int	x	=	roi_x;
	const int	y	=	roi_y;

	int		i,	j;
	int		tmp_value	=	0;			// 暫存用 為了加上mask
	int		mask		=	0x80;		// 1byte最高位為1

	switch(direction)
	{
		case EMBOSS_UP :
		case EMBOSS_DOWN:
		case EMBOSS_LEFT:
		case EMBOSS_RIGHT:
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					tmp_value	=	0;	
					if( allow_area(i,j) == 1 )
						tmp_value	|=	mask;					// 加上最高位bit的mask  AOI程式會看到透明色
					if( shadow(i,j) == SHADOW_VALUE )
						tmp_value	|=	value;					// 是陰影就加上陰影色

					img.gray_channel( i+x, j+y, tmp_value );	// 寫入資訊 修改成不管怎樣都會寫入 (沒資料就是0)
				}
			break;

		case EMBOSS_LEFT_UP:
		case EMBOSS_LEFT_DOWN:
		//case EMBOSS_LU_RD:
		//case EMBOSS_LD_RU:
		// 這兩個都用左下當基準  來做旋轉
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 第一方向
					tmp_value	=	0;	
					if( allow_area(i,j) == 1 )
						tmp_value	|=	mask;				// 加上最高位bit的mask  AOI程式會看到透明色
					if( shadow(i,j) == SHADOW_VALUE )
						tmp_value	|=	value;				// 是陰影就加上陰影色

					img.gray_channel( x_left + i + j, (y_up+y_down)/2 + i - j, tmp_value );		
					
					// 第二方向
					tmp_value	=	0;	
					if( allow_area2(i,j) == 1 )
						tmp_value	|=	mask;				// 加上最高位bit的mask  AOI程式會看到透明色
					if( shadow2(i,j) == SHADOW_VALUE )
						tmp_value	|=	value;				// 是陰影就加上陰影色
					
					img.gray_channel( x_left + i + j, (y_up+y_down)/2 + i - j + 1, tmp_value );		// (固定都用 y+1 當 offset)
				}
			break;

		case EMBOSS_RIGHT_UP:
		case EMBOSS_RIGHT_DOWN:
		// 這兩個要多一個offset
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 第一方向
					tmp_value	=	0;	
					if( allow_area(i,j) == 1 )
						tmp_value	|=	mask;				// 加上最高位bit的mask  AOI程式會看到透明色
					if( shadow(i,j) == SHADOW_VALUE )
						tmp_value	|=	value;				// 是陰影就加上陰影色	

					img.gray_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j, tmp_value );

					// 第二方向
					tmp_value	=	0;	
					if( allow_area2(i,j) == 1 )
						tmp_value	|=	mask;				// 加上最高位bit的mask  AOI程式會看到透明色
					if( shadow2(i,j) == SHADOW_VALUE )
						tmp_value	|=	value;				// 是陰影就加上陰影色

					img.gray_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j + 1, tmp_value );		// (固定都用 y+1 當 offset)
											           /*留意這邊的計算*/
				}
			break;
		
		case EMBOSS_ALL_DIRECTION:
			// 只要出現陰影都算 沒有mask的觀念
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
					if( shadow( i, j ) == SHADOW_VALUE )
						img.gray_channel( x + i, y + j , value );
			break;


		default:
			assert(false);
	}
}






/***************************************************************************************
	取得方向
/****************************************************************************************/
EMBOSS_DIRECTION	Emboss::get_direction()
{
	return	roi.direction;		// 好像沒辦法檢查是否有塞進資料@~@  未來改用指標(?)
}



/***************************************************************************************
	畫出陰影
/****************************************************************************************/
void	Emboss::result_output_shadow( AndreaImage img )
{
	assert( shadow.size1() == shadow.size2()  );
	assert( shadow2.size1() == shadow2.size2()  );

	const int	x	=	roi_x;
	const int	y	=	roi_y;

	int		i,	j;

	switch(direction)
	{
		case EMBOSS_UP :
		case EMBOSS_DOWN:
		case EMBOSS_LEFT:
		case EMBOSS_RIGHT:
		case EMBOSS_UP_DOWN:
		case EMBOSS_LEFT_RIGHT:
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 畫出陰影
					if( shadow(i,j) == SHADOW_VALUE )
						//img.gray_channel( i + x, j + y, 31 );
						img.pixel_channel( i + x, j + y, 0, 255, 255 );
					//else
					//	img.gray_channel( i+x, j+y, 0 );
				}
			break;

		case EMBOSS_LEFT_UP:
		case EMBOSS_LEFT_DOWN:
		case EMBOSS_LU_RD:
		case EMBOSS_LD_RU:
		// 這兩個都用左下當基準  來做旋轉
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 畫出陰影
					if( shadow(i,j) == SHADOW_VALUE )
						img.gray_channel( x_left + i + j, (y_up+y_down)/2 + i - j, 31 );
						//img.pixel_channel( x_left + i + j, (y_up+y_down)/2 + i - j, 0, 255, 255 );

					if( shadow2(i,j) == SHADOW_VALUE )
						img.gray_channel( x_left + i + j, (y_up+y_down)/2 + i - j + 1, 31 );
						//img.pixel_channel( x_left + i + j, (y_up+y_down)/2 + i - j + 1, 0, 255, 255 );		// (固定都用 y+1 當 offset)

				}
			break;

		case EMBOSS_RIGHT_UP:
		case EMBOSS_RIGHT_DOWN:
		// 這兩個要多一個offset
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 劃出陰影
					if( shadow(i,j) == SHADOW_VALUE )
						img.gray_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j, 31 );
						//img.pixel_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j, 0, 255, 255 );
					                                           
					if( shadow2(i,j) == SHADOW_VALUE )
											           /*留意這邊的計算*/
						img.gray_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j + 1, 31 );
						//img.pixel_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j + 1, 0, 255, 255 );		// (固定都用 y+1 當 offset)

				}
			break;
		
		case EMBOSS_ALL_DIRECTION:
			// 只要出現陰影都算 沒有mask的觀念
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					if( shadow( i, j ) == SHADOW_VALUE )
						img.gray_channel( x + i, y + j , 31 );
						//img.pixel_channel( x + i, y + j , 0, 255, 255 );
				}

			break;


		default:
			assert(false);
	}
}






/***************************************************************************************
	畫出容許區
/****************************************************************************************/
void	Emboss::result_output_emboss_allow_area( AndreaImage img )
{
	const int			x				=	roi_x;
	const int			y				=	roi_y;

	int		i,	j;
	int		count	=	0;

	switch(direction)
	{
		case EMBOSS_UP :
		case EMBOSS_DOWN:
		case EMBOSS_LEFT:
		case EMBOSS_RIGHT:
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 畫出容許區
					if( allow_area(i,j) == SHADOW_VALUE )
						img.pixel_channel( i + x, j + y, 255, 255, 0 );
				}
			break;

		case EMBOSS_LEFT_UP:
		case EMBOSS_LEFT_DOWN:
		// 這兩個都用左下當基準  來做旋轉
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 畫出容許區
					if( allow_area(i,j) == SHADOW_VALUE )
						img.pixel_channel( x_left + i + j, (y_up+y_down)/2 + i - j, 255, 255, 0 );
					if( allow_area2(i,j) == SHADOW_VALUE )
						img.pixel_channel( x_left + i + j, (y_up+y_down)/2 + i - j + 1, 255, 255, 0 );		// (固定都用 y+1 當 offset)

				}
			break;

		case EMBOSS_RIGHT_UP:
		case EMBOSS_RIGHT_DOWN:
		// 這兩個要多一個offset
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 畫出容許區
					if( allow_area(i,j) == SHADOW_VALUE )
						img.pixel_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j, 255, 255, 0 );
					if( allow_area2(i,j) == SHADOW_VALUE )
						img.pixel_channel(x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j + 1, 255, 255, 0 );		// (固定都用 y+1 當 offset)


				}
			break;

		case EMBOSS_ALL_DIRECTION:			// 全方位的沒有容許區
			break;
	
		default:
			assert(false);
	}

}





/***************************************************************************************
	畫出質心
/****************************************************************************************/
void	Emboss::result_output_emboss_centroid( AndreaImage img )
{
	const int			cx			=	(int)roi.em_data.centroid_x;
	const int			cy			=	(int)roi.em_data.centroid_y;
	const int			cx2			=	(int)roi.em_data.centroid_x2;
	const int			cy2			=	(int)roi.em_data.centroid_y2;
	const int			x			=	roi_x;
	const int			y			=	roi_y;

	int					x1,	x2,	y1,	y2;

	assert( cx < size && cy < size && cx2 < size && cy2 < size );

	switch(direction)
	{
		case EMBOSS_UP:
		case EMBOSS_DOWN:
		case EMBOSS_LEFT:
		case EMBOSS_RIGHT:
			if( cx >= 0 && cy >= 0 )
			{
				x1	=	x + cx;
				y1	=	y + cy;
				img.gray_channel( x1, y1, 224 );
				//img.pixel_channel( x1, y1, 255, 0, 0 );
			}
			break;

		case EMBOSS_LEFT_UP:
		case EMBOSS_LEFT_DOWN:
			if( cx >= 0 && cy >= 0 && cx2 >= 0 && cy2 >= 0 )
			{
				x1	=	x_left + cx + cy;
				y1	=	(y_up+y_down)/2 + cx - cy;
				x2	=	x_left + cx2 + cy2;
				y2	=	(y_up+y_down)/2 + cx2 - cy2 + 1;
				img.gray_channel( (x1+x2)/2, (y1+y2)/2, 224 );
				//img.pixel_channel( (x1+x2)/2, (y1+y2)/2, 255, 0, 0 );
			}
			break;

		case EMBOSS_RIGHT_UP:
		case EMBOSS_RIGHT_DOWN:
			if( cx >= 0 && cy >= 0 && cx2 >= 0 && cy2 >= 0 )
			{
				x1	=	x_left + cx + cy + 1 + (1+size)%2;
				y1	=	(y_up+y_down)/2 + cx - cy;
				x2	=	x_left + cx2 + cy2 + 1 + (1+size)%2;
				y2	=	(y_up+y_down)/2 + cx2 - cy2 + 1;
				img.gray_channel( (x1+x2)/2, (y1+y2)/2, 224 );
				//img.pixel_channel( (x1+x2)/2, (y1+y2)/2, 255, 0, 0 );
			}
			break;

		case EMBOSS_ALL_DIRECTION:	// 無質心
			break;

		default:
			assert(false);
	}
}




/***************************************************************************************
	凹凸結果輸出  (用shadow資料)
	顯示 輸出
/****************************************************************************************/
void	Emboss::result_output_emboss( AndreaImage img )
{
	//result_output_emboss_allow_area(  img );			// 畫出容許區
	result_output_shadow(  img );						// 畫出陰影
	result_output_emboss_centroid(  img );				// 畫出質心
}




/***************************************************************************************
	設定 roi 資料 
	(預先產生好記憶體空間避免re-allocate)
/****************************************************************************************/
void	Emboss::set( SquareData _roi )
{
	int		i;

	roi				=	_roi;
	size			=	roi.size;
	direction		=	roi.direction;

	original_len	=	size;

	roi_x			=	roi.x;
	roi_y			=	roi.y;

	good_bad		=	EMBOSS_GOOD;

	// 設定要讀取的channel  沒設定會跑出錯誤
	switch( roi.channel )
	{
		case RED:
			channel_func	=	&AndreaImage::red_channel;
			break;

		case GREEN:
			channel_func	=	&AndreaImage::green_channel;
			break;

		case BLUE:
			channel_func	=	&AndreaImage::blue_channel;
			break;

		case GRAY:
			channel_func	=	&AndreaImage::gray_channel;
			break;

		default:
			assert(false);
	}

	
	// 暫時先用圖片大小來決定 kernel density (假如速度影響過大 才改成預先設定的方式,以後要從介面來做修改)
	if( roi.img_width >= 8000 )
		kd_th	=	1.5 / 9.0 ;
	else if( roi.img_width >= 4000 )
		kd_th	=	1.5 / 9.0 ;
	else if( roi.img_width < 3000 )
		kd_th	=	1.5 / 9.0 ;
	else
		assert(false);

	// 同時初始化資料 要記憶體

	// 單向部分
	seq_line	=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
		seq_line[i].resize(size);
	seq_line2	=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
		seq_line2[i].resize(size);

	seq_extend.resize( original_len + extend_len );
	shadow_data_tmp	=	ShadowReturnData( original_len + extend_len );

	
	// 存放edge資料
	edge.resize(size);
	edge2.resize(size);


	line_data	=	new ShadowReturnData[size];
	for( i = 0; i < size; i++ )
		line_data[i]	=	ShadowReturnData( size );
	line_data2		=	new ShadowReturnData[size];
	for( i = 0; i < size; i++ )
		line_data2[i]	=	ShadowReturnData( size );

	bm_tmp.resize( size, size );

	allow_area		=	roi.em_data.allow_area;		// 容許區資料
	allow_area2		=	roi.em_data.allow_area2;

	// 陰影設定
	shadow_settings.enhance		=	roi.enhance;
	shadow_settings.dark_light	=	roi.dark_light;
	shadow_settings.threshold	=	roi.threshold;
	shadow_settings.enhance		=	false;

	shadow_obj.set( original_len + extend_len, shadow_settings );		// 注意這邊的設定是加上延伸部分 不然會炸掉

	// 全方向
	seq_line_up		=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
		seq_line_up[i].resize(size);
	seq_line_down	=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
		seq_line_down[i].resize(size);
	seq_line_left	=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
		seq_line_left[i].resize(size);
	seq_line_right	=	new bVectorByte[size];
	for( i = 0; i < size; i++ )
		seq_line_right[i].resize(size);

	line_data_up		=	new ShadowReturnData[size];
	for( i = 0; i < size; i++ )
		line_data_up[i]		=	ShadowReturnData( size );
	line_data_down		=	new ShadowReturnData[size];
	for( i = 0; i < size; i++ )
		line_data_down[i]	=	ShadowReturnData( size );
	line_data_left		=	new ShadowReturnData[size];
	for( i = 0; i < size; i++ )
		line_data_left[i]	=	ShadowReturnData( size );
	line_data_right		=	new ShadowReturnData[size];
	for( i = 0; i < size; i++ )
		line_data_right[i]	=	ShadowReturnData( size );

	up_map.resize(size,size);																															
	down_map.resize(size,size);
	left_map.resize(size,size);
	right_map.resize(size,size);

	shadow.resize(size,size);
	shadow2.resize(size,size);
	deep.resize(size,size);
	deep2.resize(size,size);

	// 這個設定必須在設定好roi之後.
	set_square_bound();

}





/***************************************************************************************
	判斷是否為 雙 方向
/****************************************************************************************/
bool	Emboss::is_bin_direction( const EMBOSS_DIRECTION direction )
{
	if( direction == EMBOSS_UP_DOWN || direction == EMBOSS_LEFT_RIGHT ||
		direction == EMBOSS_LU_RD || direction == EMBOSS_LD_RU )
			return	true;
	else
		return	false;
}




/***************************************************************************************
	是否為單方向
/****************************************************************************************/
bool	Emboss::is_single_direction( const EMBOSS_DIRECTION direction )
{
	if( direction == EMBOSS_UP || direction == EMBOSS_DOWN ||
		direction == EMBOSS_LEFT || direction == EMBOSS_RIGHT ||
		direction == EMBOSS_LEFT_UP || direction == EMBOSS_LEFT_DOWN ||
		direction == EMBOSS_RIGHT_UP || direction == EMBOSS_RIGHT_DOWN )
			return	true;
	else
		return	false;
}






/***************************************************************************************
	求整塊陰影的質心
/****************************************************************************************/
void	Emboss::batch_roi_centroid()
{
	assert( shadow.size1() == shadow.size2() && shadow2.size1() == shadow2.size2() && shadow.size1() == shadow2.size1() && shadow.size1() == size );

	int			i,		j;
	double		sum,	msum;
	double		x,		y;
	double		x2,		y2;

	// 求質心
	// x 方向
	sum		=	0;
	msum	=	0;
	for( i = 0; i < size; i++ )
		for( j = 0; j < size; j++ )
		{
			if( shadow(i,j) == 1 )
			{
				sum		+=	deep(i,j);
				msum	+=	(/*0.5 +*/ i) * deep(i,j);
			}
		}
	if( sum == 0 )			x	=	-1;						// -1 表示沒質心
	else					x	=	msum / sum;	

	// y 方向
	sum		=	0;
	msum	=	0;
	for( i = 0; i < size; i++ )
		for( j = 0; j < size; j++ )
		{
			if( shadow(i,j) == 1 )
			{
				sum		+=	deep(i,j);
				msum	+=	(/*0.5 +*/ j) * deep(i,j);
			}
		}
	if( sum == 0 )			y	=	-1;						// -1 表示沒質心
	else					y	=	msum / sum;	

	roi.em_data.centroid_x	=	x;							// 將資料寫入roi
	roi.em_data.centroid_y	=	y;
	roi.em_data.centroid_x2	=	-1;
	roi.em_data.centroid_y2 =	-1;

	// 計算標準差
	//roi_centroid_deviation( roi );

	// 斜向需要再做一次
	if( !is_vertical_horizontal( roi.direction) ) 
	{
		// 求質心
		// x 方向
		sum		=	0;
		msum	=	0;
		for( i = 0; i < size; i++ )
			for( j = 0; j < size; j++ )
			{
				if( shadow2(i,j) == 1 )
				{
					sum		+=	deep2(i,j);
					msum	+=	(/*0.5 +*/ i) * deep2(i,j);
				}
			}
		if( sum == 0 )			x2	=	-1;						// -1 表示沒質心
		else					x2	=	msum / sum;	

		// y 方向
		sum		=	0;
		msum	=	0;
		for( i = 0; i < size; i++ )
			for( j = 0; j < size; j++ )
			{
				if( shadow2(i,j) == 1 )
				{
					sum		+=	deep2(i,j);
					msum	+=	(/*0.5 +*/ j) * deep2(i,j);
				}
			}
		if( sum == 0 )			y2	=	-1;						// -1 表示沒質心
		else					y2	=	msum / sum;	

		roi.em_data.centroid_x2	=	x2;							// 用這樣的作法來處理斜向
		roi.em_data.centroid_y2 =	y2;
	}

}





/***************************************************************************************
	回傳落在允許區外的陰影個數
/****************************************************************************************/
int		Emboss::emboss_result_out_area()
{	
	const int			x				=	roi_x;
	const int			y				=	roi_y;

	int			i,	j;
	int			count;

	assert( shadow.size1() == shadow.size2() && shadow.size1() == allow_area.size1() && allow_area.size1() == allow_area.size2() );
	assert( shadow2.size1() == shadow2.size2() && shadow2.size1() == allow_area2.size1() && allow_area2.size1() == allow_area2.size2() );
	assert( shadow.size1() == shadow2.size1() );

	// 計數器歸零
	count	=	0;

	// 開始計算落出允許區的陰影個數
	switch(direction)
	{
		case EMBOSS_UP :
		case EMBOSS_DOWN:
		case EMBOSS_LEFT:
		case EMBOSS_RIGHT:
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					if( shadow(i,j) == SHADOW_VALUE && allow_area(i,j) == 0 )
						count ++;
				}
			break;

		case EMBOSS_LEFT_UP:
		case EMBOSS_LEFT_DOWN:
		// 這兩個都用左下當基準  來做旋轉
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					if( shadow(i,j) == SHADOW_VALUE && allow_area(i,j) == 0 )
						count ++;

					if( shadow2(i,j) == SHADOW_VALUE && allow_area2(i,j) == 0 )
						count ++;
				}
			break;

		case EMBOSS_RIGHT_UP:
		case EMBOSS_RIGHT_DOWN:
		// 這兩個要多一個offset
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					if( shadow(i,j) == SHADOW_VALUE && allow_area(i,j) == 0 )
						count ++;
					if( shadow2(i,j) == SHADOW_VALUE && allow_area2(i,j) == 0 )
						count ++;
				}
			break;
	
		default:
			assert(false);
	}
	
	return	count;
}




/***************************************************************************************
	判斷質心是否落在允許區外
/****************************************************************************************/
bool	Emboss::emboss_result_out_center()
{
	const double		cx		=	roi.em_data.centroid_x;
	const double		cy		=	roi.em_data.centroid_y;
	const double		cx2		=	roi.em_data.centroid_x2;
	const double		cy2		=	roi.em_data.centroid_y2;

	int		x,	y;

	assert( shadow.size1() == shadow.size2() && shadow.size1() == allow_area.size1() && allow_area.size1() == allow_area.size2() );
	assert( shadow2.size1() == shadow2.size2() && shadow2.size1() == allow_area2.size1() && allow_area2.size1() == allow_area2.size2() );
	assert( shadow.size1() == shadow2.size1() );

	// 判斷質心
	switch(direction)
	{
		case EMBOSS_UP:
		case EMBOSS_DOWN:
		case EMBOSS_LEFT:
		case EMBOSS_RIGHT:
			if( cx >= 0 && cy >= 0 )
			{
				x		=	(int) cx;
				y		=	(int) cy;

				if( allow_area( x, y ) == 1 )
					return	true;
				else
					return	false;
			}
			else	// -1, -1這樣的判斷代表沒抓到陰影 永遠都是true
				return	true;
			break;

		case EMBOSS_LEFT_UP:
		case EMBOSS_LEFT_DOWN:
			if( cx >= 0 && cy >= 0 && cx2 >= 0 && cy2 >= 0 )
			{
				x		=	(int) cx;
				y		=	(int) cy;

				if( allow_area( x, y ) == 1 )
					return	true;
				else
					return	false;

				x		=	(int) cx2;
				y		=	(int) cy2;

				if( allow_area2( x, y ) == 1 )
					return	true;
				else
					return	false;

			}
			else
				return	true;
			break;

		case EMBOSS_RIGHT_UP:
		case EMBOSS_RIGHT_DOWN:
			if( cx >= 0 && cy >= 0 && cx2 >= 0 && cy2 >= 0 )
			{
				x		=	(int) cx;
				y		=	(int) cy;

				if( allow_area( x, y ) == 1 )
					return	true;
				else
					return	false;

				x		=	(int) cx2;
				y		=	(int) cy2;

				if( allow_area2( x, y ) == 1 )
					return	true;
				else
					return	false;
			}
			return	true;
			break;

		default:
			assert(false);
	}

	return	false;
}


/***************************************************************************************
	陰影容許禁區 
	判斷有幾個陰影 (有一個就叫做廢品)
/****************************************************************************************/
int		Emboss::all_dir_result()
{
	assert( shadow.size1() == size && shadow.size2() == size );

	int		i,	j;
	int		count;

	assert( direction == EMBOSS_ALL_DIRECTION );

	count	=	0;
	for( i = 0; i < size; i++ )
		for( j = 0; j < size; j++ )
		{
			if( shadow( i, j ) == SHADOW_VALUE )
				count++;
		}
	
	return	count;
}


/***************************************************************************************
	計算陰影的結果 (正或廢)
/****************************************************************************************/
void	Emboss::emboss_result( SquareData &square )
{
	// 單向
	if( is_single_direction(direction) )
	{
		square.em_data.is_center_out	=	emboss_result_out_center();		// 質心是否落在允許區內   true 表示 正品  false 表示廢品
		square.em_data.out_shadow		=	emboss_result_out_area();		// 計算落在允許區外的陰影個數
	}
	// 陰影容許禁區 這邊的作法是讓他有陰影出現就會判斷為廢品
	// 尚未將容許禁區的判斷獨立出來 !!
	else if( direction == EMBOSS_ALL_DIRECTION )
	{
		square.em_data.out_shadow		=	all_dir_result();
		square.em_data.is_center_out	=	true;		// 修改掉這邊 改成用點數判斷. 這個永遠都是true
	}
	else
		assert(false);
}




/***************************************************************************************
	計算edge
/****************************************************************************************/
void	Emboss::get_edge()
{
	int		i;

	for( i = 0; i < size; i++ )
	{
		edge(i)		=	1;
		edge2(i)	=	1;
	}
}



/***************************************************************************************
	畫出edge
/****************************************************************************************/
void	Emboss::result_output_edge( AndreaImage img )
{
	assert( shadow.size1() == shadow.size2()  );
	assert( shadow2.size1() == shadow2.size2()  );

	const int	x	=	roi_x;
	const int	y	=	roi_y;

	int		i,	j;

	switch(direction)
	{
		case EMBOSS_UP :
		case EMBOSS_DOWN:
		case EMBOSS_LEFT:
		case EMBOSS_RIGHT:
		case EMBOSS_UP_DOWN:
		case EMBOSS_LEFT_RIGHT:
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 畫出陰影
					if( shadow(i,j) == SHADOW_VALUE )
						img.gray_channel( i + x, j + y, 31 );
						//img.pixel_channel( i + x, j + y, 0, 255, 255 );
				}
			break;

		case EMBOSS_LEFT_UP:
		case EMBOSS_LEFT_DOWN:
		case EMBOSS_LU_RD:
		case EMBOSS_LD_RU:
		// 這兩個都用左下當基準  來做旋轉
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 畫出陰影
					if( shadow(i,j) == SHADOW_VALUE )
						img.gray_channel( x_left + i + j, (y_up+y_down)/2 + i - j, 31 );
						//img.pixel_channel( x_left + i + j, (y_up+y_down)/2 + i - j, 0, 255, 255 );

					if( shadow2(i,j) == SHADOW_VALUE )
						img.gray_channel( x_left + i + j, (y_up+y_down)/2 + i - j + 1, 31 );
						//img.pixel_channel( x_left + i + j, (y_up+y_down)/2 + i - j + 1, 0, 255, 255 );		// (固定都用 y+1 當 offset)

				}
			break;

		case EMBOSS_RIGHT_UP:
		case EMBOSS_RIGHT_DOWN:
		// 這兩個要多一個offset
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					// 劃出陰影
					if( shadow(i,j) == SHADOW_VALUE )
						img.gray_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j, 31 );
						//img.pixel_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j, 0, 255, 255 );
					                                           
					if( shadow2(i,j) == SHADOW_VALUE )
											           /*留意這邊的計算*/
						img.gray_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j + 1, 31 );
						//img.pixel_channel( x_left + i + j + 1 + (1+size)%2, (y_up+y_down)/2 + i - j + 1, 0, 255, 255 );		// (固定都用 y+1 當 offset)

				}
			break;
		
		case EMBOSS_ALL_DIRECTION:
			// 只要出現陰影都算 沒有mask的觀念
			for( i = 0; i < size; i++ )
				for( j = 0; j < size; j++ )
				{
					if( shadow( i, j ) == SHADOW_VALUE )
						img.gray_channel( x + i, y + j , 31 );
						//img.pixel_channel( x + i, y + j , 0, 255, 255 );
				}

			break;


		default:
			assert(false);
	}
}



/***************************************************************************************
	相當於原本的execution  
	multi-thread時使用
/****************************************************************************************/
void	Emboss::exec()
{
	int		i;

	if( direction == EMBOSS_ALL_DIRECTION )
	{
		for( i = 0; i < size; i++ )
		{
			batch_shadow_ALLD(i);
		}
		return;
	}

	// 單向
	for( i = 0; i < size; i++ )
		batch_shadow_line(i);

	// 斜向 多一倍空間
	if( direction >= EMBOSS_LEFT_UP && direction <= EMBOSS_RIGHT_DOWN )	// 為了加速 小心enum value
	{
		for( i = 0; i < size; i++ )
			batch_shadow_line_2(i);
	}

	// 計算edge
	get_edge();

}













//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ KernelDensity 部分 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/***************************************************************************************
	建構函數
/****************************************************************************************/
KernelDensity::KernelDensity()
{
	kernel_square(1);
}




/***************************************************************************************
	產生基本的方塊狀 kernel
/****************************************************************************************/
void	KernelDensity::kernel_square( int n )
{
	int		m	=	2*n + 1;

	kernel.resize( m, m );

	int		i,	j;

	for( i = 0; i < m; i++ )
		for( j = 0; j < m; j++ )
			kernel( i, j )	=	1.0;
}



/***************************************************************************************
	元素個數計算
/****************************************************************************************/
int		KernelDensity::element_count( bMatrixByte &data )
{
	assert( data.size1() == data.size2() && data.size1() > 0 );

	int		i,	j;
	int		N	=	0;

	for( i = 0; i < data.size1(); i++ )
		for( j = 0; j < data.size2(); j++ )
		{
			if( data( i, j ) == 1 )
				N++;
		}

	return	N;
}



/***************************************************************************************
	執行資料計算的地方
/****************************************************************************************/
void	KernelDensity::dosomething( bMatrixDouble &p_data,  bMatrixByte &data )
{
	assert( data.size1() == data.size2() && data.size1() == p_data.size1() && p_data.size1() == p_data.size2() && data.size1() > 0 );
	assert( data.size1() >= 3 );

	int		i,	j,	x,	y;
	double	dcount;
		
	p_data.clear();		// 一定要清除資料.

	// 開始掃描元素
	for( i = 0; i < data.size1(); i++ )
		for( j = 0; j < data.size2(); j++ )
		{
			dcount	=	0;
			for( x = -1; x <= 1; x++ )
				for( y = -1; y <= 1; y++ )
				{
					if( i+x >= 0 && j+y >= 0 && i+x < data.size1() && j+y < data.size2() )
						dcount	+=	data( i+x, j+y ) * kernel( x+1, y+1 );
				}
			dcount	/=	(9.0);						// 9.0 = h^D
			p_data( i, j )	=	dcount ;			// 避免windows大小干擾結果
		}
}






//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Emboss Multi-thread 部分 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/***************************************************************************************
	建構
/****************************************************************************************/
Emboss_MT::Emboss_MT()
{
	em		=	NULL;
	size	=	0;
}


/***************************************************************************************
	解構
/****************************************************************************************/
Emboss_MT::~Emboss_MT()
{
	delete	[] em;
	size	=	0;
}


/***************************************************************************************
	初始化
/****************************************************************************************/
void	Emboss_MT::init( int _size )
{
	assert( _size >= 0 );

	delete	[] em;	// 先將舊資料刪除

	size	=	_size;
	em		=	new Emboss[size];
}

/***************************************************************************************
	類似解構
	將資料刪除
/****************************************************************************************/
void	Emboss_MT::destroy()
{
	size	=	0;
	delete	[] em;
	em	=	NULL;
}



/***************************************************************************************
	標記defect map, 
	使用openMP
/****************************************************************************************/
void	Emboss_MT::defect( AndreaImage img, int value )
{

#pragma omp parallel for
	for( int i = 0; i < size; i++ )
	{
		if( em[i].good_bad == EMBOSS_BAD )
			em[i].batch_result_output_emboss_defect( img, value );
	}

}



/***************************************************************************************
	執行 
	(在這邊進行openMP)
/****************************************************************************************/
void	Emboss_MT::exec( AndreaImage image )
{
	//omp_set_num_threads(MAX_THR);
#pragma omp parallel for
	for( int i = 0; i < size; i++ )
	{
		em[i].set_image( image );			// 設定圖片
		em[i].batch_loadimage();			// 讀取圖片
		// 上面兩個  72 * 84*84  需要時間 = 0.63ms  

		em[i].exec();						// 執行
		// 這邊是吃最多時間的部分

		em[i].batch_linedata2matrix();		// 轉換座標
		em[i].batch_kernel_density();		// 過濾雜訊
		// 這邊的時間略高 (3ms)

		em[i].batch_roi_centroid();			// 計算質心
	}
}


/***************************************************************************************
	執行 
	(在這邊進行openMP)
/****************************************************************************************/
void	Emboss_MT::exec_page2( AndreaImage image )
{
	// 設定圖片
	for( int i = 0; i < size; i++ )
	{
		em[i].set_image( image );
		em[i].batch_loadimage();		// 讀取圖片
		// 上面兩個  72 * 84*84  需要時間 = 0.63ms  

		em[i].exec();
		// 這邊是吃最多時間的部分

		em[i].batch_linedata2matrix();
		em[i].batch_kernel_density();

		em[i].batch_roi_centroid();			// 計算質心
	}
	// 這邊的時間略高 (3ms)
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Screen Multi-thread 部分 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/***************************************************************************************
	建構
/****************************************************************************************/
Screen_MT::Screen_MT()
{
	sc		=	NULL;
	size	=	0;
}


/***************************************************************************************
	解構
/****************************************************************************************/
Screen_MT::~Screen_MT()
{
	delete	[] sc;
	size	=	0;
}


/***************************************************************************************
	初始化
/****************************************************************************************/
void	Screen_MT::init( int _size )
{
	assert( _size >= 0 );

	delete	[] sc;		// 先將舊的資料清除

	size	=	_size;
	sc		=	new Screen[size];
}


/***************************************************************************************
	類似解構
	將資料刪除
/****************************************************************************************/
void	Screen_MT::destroy()
{
	size	=	0;
	delete	[] sc;
	sc		=	NULL;
}


/***************************************************************************************
	執行 
	(在這邊進行openMP)
/****************************************************************************************/
void	Screen_MT::exec( AndreaImage image )
{
//	omp_set_num_threads(MAX_THR);


#pragma omp parallel for
	for( int i = 0; i < size; i++ )
	{
		sc[i].set_image( image );			// 設定圖片
		sc[i].batch_loadimage();			// 讀取圖片

		sc[i].exec();						// 執行
		// 這邊是吃最多時間的部分

		sc[i].batch_linedata2matrix();		// 座標轉換
	}
}


/***************************************************************************************
	標記defect map, 
	使用openMP
/****************************************************************************************/
void	Screen_MT::defect( AndreaImage img, int value )
{

#pragma omp parallel for
	for( int i = 0; i < size; i++ )
	{
		if( sc[i].good_bad == EMBOSS_BAD )
			sc[i].batch_result_output_screen_defect( img, value );
	}

}


/***************************************************************************************
	Page2使用的執行
/****************************************************************************************/
void	Screen_MT::exec_page2( AndreaImage image )
{
	for( int i = 0; i < size; i++ )
	{
		// 設定圖片
		sc[i].set_image( image );

		// 讀取圖片
		sc[i].batch_loadimage();

		sc[i].exec();
		// 這邊是吃最多時間的部分

		sc[i].batch_linedata2matrix();
	}

}






} // end namespace emboss