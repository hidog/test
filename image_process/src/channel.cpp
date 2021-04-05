
#include "channel.h"

using namespace std;

#if 0


/***************************************************************************************
	建構
/****************************************************************************************/
#ifdef _OPENPIV_
Channel::Channel( Settings *settingsPass, DataContainer *filedataPass )
	:	AndreaEngine()
{
	progress		=	settingsPass->getProgress();	
	settings		=	settingsPass;
	filedata		=	filedataPass;

	// 陰影檢測
	square_data		=	NULL;
	square_data2	=	NULL;

    mutex.lock();
    mutex.unlock();
}
#else
Channel::Channel()
{
	// 陰影檢測
	square_data		=	NULL;
	square_data2	=	NULL;
}
#endif


/***************************************************************************************
	解構
/****************************************************************************************/
Channel::~Channel()
{
	delete	[]	square_data;
	delete	[]	square_data2;
}








/***************************************************************************************
	陰影檢測用的
	一次檢查一條線
/****************************************************************************************/
ShadowReturnData	Channel::shadow_line( PIXEL start, PIXEL end )
{
	int		i;
	int		len	=	30;		// 往前延伸pixel寬度

	bVectorInt			seq_line	=	loadimage( start, end );		// 原始圖檔資料 (line)

	int		ct	=	seq_line.size();

	bVectorInt			seq(ct + len);									// 實際上丟進去的資料 會做補值的動作
	ShadowReturnData	shadow_data_tmp,	shadow_data;				// 陰影資料
	ShadowSettings		shadow_settings;								// 陰影設定值

	// 初始化vector資料
	shadow_data				=	ShadowReturnData( ct );

	// 將原始資料塞進去
	shadow_data.raw_data	=	seq_line;
	
	// 往前補值
	for( i = 0; i < len; i++ )
		seq(i)	=	seq_line(0);             //(int)(seqttt(0)+0.5);   

	// 接回原本的資料
	for( i = len; i < len+ct; i++ )
		seq(i)	=	seq_line(i-len);		//(int)(seqttt(i-len)+0.5);		// 四捨五入

	shadow_settings.inhence_mode	=	settings->inhence_mode;			// 設定陰影資料
	shadow_settings.dark_light		=	settings->dark_light;			// 設定抓亮影或陰影
	shadow_data_tmp					=	shadow_detector( seq, threshold, seq, shadow_settings );		// 陰影資料 包含補值得部分  兩個參數  一個是原始資料  一個是可以加上gauss blur的資料

	// 刪除補值的部分
	for( i = len; i < len+ct; i++ )
	{
		shadow_data.mono_sub_seq( i-len )	=	shadow_data_tmp.mono_sub_seq(i);
		shadow_data.shadow_seq( i-len )		=	shadow_data_tmp.shadow_seq(i);
		shadow_data.shadow_depth( i-len )	=	shadow_data_tmp.shadow_depth(i);
		shadow_data.edge					=	shadow_data_tmp.edge - len;
		shadow_data.deepest					=	shadow_data_tmp.deepest - len ;
	}

	return	shadow_data;
}




/***************************************************************************************
	陰影檢測用的
	一次檢查一個區塊
/****************************************************************************************/
SquareData		Channel::shadow_square( PIXEL square_roi )
{
	int		x_left,	x_right,	y_up,	y_down;

	// 上下左右四方向
	if( direction == OpenPIV::up || direction == OpenPIV::down ||
		direction == OpenPIV::left || direction == OpenPIV::right )
	{
		x_left	=	square_roi.x;								// x的左邊起始點
		x_right	=	square_roi.x + square_roi.square_size-1;	// x的右邊終點
		y_up	=	square_roi.y;								// y的上方起始點
		y_down	=	square_roi.y + square_roi.square_size-1;	// y的下方終點
	}
	// 左上左下右上右下四個方向  這邊會稍微有點歪 暫時先不處理
	else
	{
		x_left	=	square_roi.x - square_roi.square_size/2;							// x的左邊起始點
		x_right	=	square_roi.x + square_roi.square_size/2 + square_roi.square_size;	// x的右邊終點
		y_up	=	square_roi.y - square_roi.square_size/2;							// y的上方起始點
		y_down	=	square_roi.y + square_roi.square_size/2 + square_roi.square_size;							// y的下方終點
	}

	PIXEL				p1,	p2;
	int					i;

	SquareData		return_data;			// 回傳資料

	// 初始化 squeare data	共通部分. ( x, y 在switch的地方才初始化 )
	return_data.direction	=	direction;
	return_data.size		=	square_roi.square_size;
	return_data.x			=	0;
	return_data.y			=	0;
	return_data.line_data	=	new		ShadowReturnData[ return_data.size ];

	switch( direction )
	{
		case OpenPIV::up :
			return_data.x	=	x_left;
			return_data.y	=	y_up;
			for( i = 0; i < square_roi.square_size; i++ )
			{
				p1.x	=	x_left + i;
				p2.x	=	x_left + i;
				p1.y	=	y_up;
				p2.y	=	y_down;
				return_data.line_data[i]	=	shadow_line( p1, p2 );
			}
			break;

		case OpenPIV::down :
			return_data.x	=	x_left;
			return_data.y	=	y_down;
			for( i = 0; i < square_roi.square_size; i++ )
			{
				p1.x	=	x_left + i;
				p2.x	=	x_left + i;
				p1.y	=	y_down;
				p2.y	=	y_up;
				return_data.line_data[i]	=	shadow_line( p1, p2 );
			}
			break;

		case OpenPIV::left :
			return_data.x	=	x_left;
			return_data.y	=	y_up;
			for( i = 0; i < square_roi.square_size; i++ )
			{
				p1.x	=	x_left;
				p2.x	=	x_right;
				p1.y	=	y_up + i;
				p2.y	=	y_up + i;
				return_data.line_data[i]	=	shadow_line( p1, p2 );
			}
			break;

		case OpenPIV::right :
			return_data.x	=	x_right;
			return_data.y	=	y_up;
			for( i = 0; i < square_roi.square_size; i++ )
			{
				p1.x	=	x_right;
				p2.x	=	x_left;
				p1.y	=	y_up + i;
				p2.y	=	y_up + i;
				return_data.line_data[i]	=	shadow_line( p1, p2 );
			}
			break;

		case OpenPIV::left_up :
			// 這邊會超微偏出檢測區  要小心 
			// 判斷方式會跟上下左右的部分有些不同
			return_data.x	=	x_left;
			return_data.y	=	(y_up + y_down)/2;
			for( i = 0; i < square_roi.square_size; i++ )
			{
				p1.x	=	return_data.x + i;
				p2.x	=	return_data.x + i + square_roi.square_size - 1;
				p1.y	=	return_data.y - i;
				p2.y	=	return_data.y - i + square_roi.square_size - 1;
				return_data.line_data[i]	=	shadow_line( p1, p2 );
			}
			break;
			
		case OpenPIV::left_down :
			return_data.x	=	x_left;
			return_data.y	=	(y_up + y_down)/2;
			for( i = 0; i < square_roi.square_size; i++ )
			{
				p1.x	=	return_data.x + i;
				p2.x	=	return_data.x + i + square_roi.square_size - 1;
				p1.y	=	return_data.y + i;
				p2.y	=	return_data.y + i + square_roi.square_size - 1;
				return_data.line_data[i]	=	shadow_line( p1, p2 );
			}
			break;

		case OpenPIV::right_up :
			return_data.x	=	x_right;
			return_data.y	=	(y_up + y_down)/2;
			for( i = 0; i < square_roi.square_size; i++ )
			{
				p1.x	=	return_data.x - i;
				p2.x	=	return_data.x - i - square_roi.square_size - 1;
				p1.y	=	return_data.y - i;
				p2.y	=	return_data.y - i - square_roi.square_size - 1;
				return_data.line_data[i]	=	shadow_line( p1, p2 );
			}
			break;

		case OpenPIV::right_down :
			return_data.x	=	x_right;
			return_data.y	=	(y_up + y_down)/2;
			for( i = 0; i < square_roi.square_size; i++ )
			{
				p1.x	=	return_data.x - i;
				p2.x	=	return_data.x - i - square_roi.square_size - 1;
				p1.y	=	return_data.y + i;
				p2.y	=	return_data.y + i - square_roi.square_size - 1;
				return_data.line_data[i]	=	shadow_line( p1, p2 );
			}
			break;

		default:
			progress->debuger( "error!!", QColor( 255, 0, 0 ) );
			break;
	}

	return		return_data;
}



/***************************************************************************************
	陰影檢測用的
	讀取圖檔資料
/****************************************************************************************/
bVectorInt	Channel::loadimage( PIXEL start, PIXEL end )
{
	bVectorInt	seq;
	int			delta;
	int			i;

	// 上方陰影
	if( direction == OpenPIV::up )
	{
		seq.resize( end.y - start.y + 1 );
		seq.clear();

		for( i = 0; i < seq.size(); i++ )
			seq( i )	=	image.gray_channel( start.x, start.y + i );
	}

	// 下方陰影
	else if( direction == OpenPIV::down )
	{
		seq.resize( start.y - end.y + 1 );
		seq.clear();

		for( i = 0; i < seq.size(); i++ )
			seq( i )	=	image.gray_channel( start.x, start.y - i );
	}

	// 左邊陰影
	else if( direction == OpenPIV::left )
	{
		seq.resize( end.x - start.x + 1 );
		seq.clear();

		for( i = 0; i < seq.size(); i++ )
			seq( i )	=	image.gray_channel( start.x + i, start.y );
	}

	// 右邊陰影
	else if( direction == OpenPIV::right )
	{
		seq.resize( start.x - end.x + 1 );
		seq.clear();

		for( i = 0; i < seq.size(); i++ )
			seq( i )	=	image.gray_channel( start.x - i, start.y );
	}

	// 左上 陰影   (左上->右下)
	else if( direction == OpenPIV::left_up )
	{
		seq.resize( end.x - start.x + 1 );
		seq.clear();

		for( i = 0; i < seq.size(); i++ )
			seq( i )	=	image.gray_channel( start.x + i, start.y + i );
	}

	// 左下陰影   (左下->右上)
	else if( direction == OpenPIV::left_down )
	{
		seq.resize( end.x - start.x + 1 );
		seq.clear();

		for( i = 0; i < seq.size(); i++ )
			seq( i )	=	image.gray_channel( start.x + i, start.y - i );
	}

	// 右上陰影
	else if( direction == OpenPIV::right_up )
	{
		seq.resize( start.x - end.x + 1 );
		seq.clear();

		for( i = 0; i < seq.size(); i++ )
			seq(i)	=	image.gray_channel( start.x - i, start.y + i );
	}

	// 右下陰影
	else if( direction == OpenPIV::right_down )
	{
		seq.resize( start.x - end.x + 1 );
		seq.clear();

		for( i = 0; i < seq.size(); i++ )
			seq(i)	=	image.gray_channel( start.x - i, start.y - i );
	}
	
	else
		progress->debuger( QString("error!!"), QColor(255,0,0) );

	return	seq;
}




/***************************************************************************************
	將結果標記在圖片上 顯示
/****************************************************************************************/
void	Channel::mark_on_image( SquareData square )
{
	int		i,	j;
	int		x,	y;
	int		deepest;
	int		edge;

	// 上方陰影
	if( direction == OpenPIV::up )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			x	=	square.x + i;
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					y	=	square.y + j;
					final.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				y	=	square.y + deepest;
				final.pixel_channel( x, y, 0, 255, 255 );
			}

			// edge 標記紅色
			edge	=	square.line_data[i].edge;
			y		=	square.y + edge;
			final.pixel_channel( x, y, 255, 0, 0 );
		}
	}

	// 下方陰影
	else if( direction == OpenPIV::down )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			x	=	square.x + i;
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					y	=	square.y - j;
					final.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				y	=	square.y - deepest;
				final.pixel_channel( x, y, 0, 255, 255 );
			}
			edge	=	square.line_data[i].edge;
			y		=	square.y - edge;
			final.pixel_channel( x, y, 255, 0, 0 );
		}
	}

	// 左邊陰影
	else if( direction == OpenPIV::left )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			y	=	square.y + i;
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x + j;
					final.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				x	=	square.x + deepest;
				final.pixel_channel( x, y, 0, 255, 255 );
			}
		}
	}

	// 右邊陰影
	else if( direction == OpenPIV::right )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			y	=	square.y + i;
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x - j;
					final.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				x	=	square.x - deepest;
				final.pixel_channel( x, y, 0, 255, 255 );
			}
		}
	}

	// 左上陰影
	else if( direction == OpenPIV::left_up )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x + i + j;
					y	=	square.y - i + j;
					final.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				x	=	square.x + i + deepest;
				y	=	square.y - i + deepest;
				final.pixel_channel( x, y, 0, 255, 255 );
			}
		}
	}

	// 左下陰影
	else if( direction == OpenPIV::left_down )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x + i + j;
					y	=	square.y + i - j;
					final.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				x	=	square.x + i + deepest;
				y	=	square.y + i - deepest;
				final.pixel_channel( x, y, 0, 255, 255 );
			}
		}
	}

	// 右上陰影
	else if( direction == OpenPIV::right_up )
	{
		for( i = 0; i < square.size; i++ )
		{
			for( j = 0; j < square.size; j++ )
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x - i - j;
					y	=	square.y - i + j;
					final.pixel_channel( x, y, 0, 0, 255 );
				}

				deepest		=	square.line_data[i].deepest;
				if( square.line_data[i].shadow_seq[ deepest ] == 1 )
				{
					x	=	square.x - i - deepest;
					y	=	square.y - i + deepest;
					final.pixel_channel( x, y, 0, 255, 255 );
				}
			}
		}
	}

	// 右下陰影
	else if( direction == OpenPIV::right_down )
	{
		for( i = 0; i < square.size; i++ )
		{
			for( j = 0; j < square.size; j++ )
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x - i - j;
					y	=	square.y + i - j;
					final.pixel_channel( x, y, 0, 0, 255 );
				}

				deepest		=	square.line_data[i].deepest;
				if( square.line_data[i].shadow_seq[ deepest ] == 1 )
				{
					x	=	square.x - i - deepest;
					y	=	square.y + i - deepest;
					final.pixel_channel( x, y, 0, 255, 255 );
				}
			}
		}
	}

	else
		progress->debuger( QString("error!!"), QColor( 255, 0, 0 ) );
}





/***************************************************************************************
	將結果標記在圖片上 顯示
/****************************************************************************************/
void	Channel::mark_on_image( SquareData square, QImage &img )
{
	int		i,	j;
	int		x,	y;
	int		deepest;
	int		edge;

	// 上方陰影
	if( direction == OpenPIV::up )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			x	=	square.x + i;
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					y	=	square.y + j;
					img.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				y	=	square.y + deepest;
				img.pixel_channel( x, y, 0, 255, 255 );
			}

			// edge 標記紅色
			edge	=	square.line_data[i].edge;
			y		=	square.y + edge;
			img.pixel_channel( x, y, 255, 0, 0 );
		}
	}

	// 下方陰影
	else if( direction == OpenPIV::down )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			x	=	square.x + i;
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					y	=	square.y - j;
					img.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				y	=	square.y - deepest;
				img.pixel_channel( x, y, 0, 255, 255 );
			}

			// edge 標記紅色
			edge	=	square.line_data[i].edge;
			y		=	square.y - edge;
			img.pixel_channel( x, y, 255, 0, 0 );
		}
	}

	// 左邊陰影
	else if( direction == OpenPIV::left )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			y	=	square.y + i;
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x + j;
					img.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				x	=	square.x + deepest;
				img.pixel_channel( x, y, 0, 255, 255 );
			}

			// edge 標記紅色
			edge	=	square.line_data[i].edge;
			x		=	square.x + edge;
			img.pixel_channel( x, y, 255, 0, 0 );
		}
	}

	// 右邊陰影
	else if( direction == OpenPIV::right )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			y	=	square.y + i;
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x - j;
					img.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				x	=	square.x - deepest;
				img.pixel_channel( x, y, 0, 255, 255 );
			}

			// edge 標記紅色
			edge	=	square.line_data[i].edge;
			x		=	square.x - edge;
			img.pixel_channel( x, y, 255, 0, 0 );
		}
	}

	// 左上陰影
	else if( direction == OpenPIV::left_up )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x + i + j;
					y	=	square.y - i + j;
					img.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				x	=	square.x + i + deepest;
				y	=	square.y - i + deepest;
				img.pixel_channel( x, y, 0, 255, 255 );
			}

			// edge 標記紅色
			edge	=	square.line_data[i].edge;
			x		=	square.x + i + edge;
			y		=	square.y - i + edge;
			img.pixel_channel( x, y, 255, 0, 0 );
		}
	}

	// 左下陰影
	else if( direction == OpenPIV::left_down )
	{
		for( i = 0; i < square.size; i++ )		// 隨便一個都可以  所以挑0  最安全
		{
			for( j = 0; j < square.size; j++ )	// 因為是正方形 所以可以這樣做 長方形要修改
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x + i + j;
					y	=	square.y + i - j;
					img.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			// 最深的標記另一個顏色
			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				x	=	square.x + i + deepest;
				y	=	square.y + i - deepest;
				img.pixel_channel( x, y, 0, 255, 255 );
			}
			// edge 標記紅色
			edge	=	square.line_data[i].edge;
			x		=	square.x + i + edge;
			y		=	square.y + i - edge;
			img.pixel_channel( x, y, 255, 0, 0 );
		}
	}

	// 右上陰影
	else if( direction == OpenPIV::right_up )
	{
		for( i = 0; i < square.size; i++ )
		{
			for( j = 0; j < square.size; j++ )
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x - i - j;
					y	=	square.y - i + j;
					img.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				x	=	square.x - i - deepest;
				y	=	square.y - i + deepest;
				img.pixel_channel( x, y, 0, 255, 255 );
			}

			// edge 標記紅色
			edge	=	square.line_data[i].edge;
			x		=	square.x - i - edge;
			y		=	square.y - i + edge;
			img.pixel_channel( x, y, 255, 0, 0 );
	
		}
	}

	// 右下陰影
	else if( direction == OpenPIV::right_down )
	{
		for( i = 0; i < square.size; i++ )
		{
			for( j = 0; j < square.size; j++ )
			{
				if( square.line_data[i].shadow_seq[j] == 1 )
				{
					x	=	square.x - i - j;
					y	=	square.y + i - j;
					img.pixel_channel( x, y, 0, 0, 255 );
				}
			}

			deepest		=	square.line_data[i].deepest;
			if( square.line_data[i].shadow_seq[ deepest ] == 1 )
			{
				x	=	square.x - i - deepest;
				y	=	square.y + i - deepest;
				img.pixel_channel( x, y, 0, 255, 255 );
			}

			// edge 標記紅色
			edge	=	square.line_data[i].edge;
			x		=	square.x - i - edge;
			y		=	square.y + i - edge;
			img.pixel_channel( x, y, 255, 0, 0 );
			
		}
	}

	else
		progress->debuger( QString("error!!"), QColor( 255, 0, 0 ) );
}



/***************************************************************************************
	計算質心位置
	連續函數 = 積分後除上平均
	離散	 = 加起來除上平均
/****************************************************************************************/
double	Channel::centroid( bVectorInt deep, int start, int end )
{
	int		i;
	double	xsum	=	0;
	double	sum		=	0;
	double	mag;

	for( i = start; i < end; i++ )
	{
		xsum	+=	i * deep(i);
		sum		+=	deep(i);
	}
	mag	=	xsum / sum;

	return	mag;
}







#ifdef _OPENPIV_
/***************************************************************************************
	實際執行程式碼的位置
/****************************************************************************************/
void	Channel::dosomething()
{
//Timer	t;
//t.Start();

	progress->debuger("shadow detector...");

	//QFile::remove( QString("./output/test.txt") ) ;    刪除檔案範例
	//QProcess	prc;
	//prc.start("DEL ./output/*.txt");
	//system("DEL ./output/1.txt");

	bVectorInt	raw;		// 原始資料

	int			i,	j,	k;
	int			x,	y;
	int			istart,	iend;
	double		dtmp;

	char	str[100];

	// 初始化資料
	image			=	filedata->get_image();
//	image			=	QImage( str );
	final			=	image;
	QRgb	rgb		=	settings->getRGB();

	// 讀取檢測區
	square			=	filedata->getPixelList();
	threshold		=	settings->shadow_depth_threshold;		// 深度的threshold
	direction		=	settings->shadow_dir;					// 方向

	// 初始化 square data
	square_data		=	new		SquareData[square.size()];

	// 斜巷才需要初始化 square_dat2
	if( direction == OpenPIV::left_down || direction == OpenPIV::left_up ||
		direction == OpenPIV::right_down || direction == OpenPIV::right_up )
			square_data2	=	new		SquareData[square.size()];

	// 針對每個檢測區去偵測陰影
	for( i = 0; i < square.size(); i++ )
	{
		// 一次檢測一個區塊
		square_data[i]	=	shadow_square( square.at(i) );

		// 斜向必須多做一次  (目前偏上下)
		// 左上 & 右上
		if( direction == OpenPIV::left_up || direction == OpenPIV::right_up )
		{
			PIXEL	square_tmp	=	square.at(i);		// 名字叫做 PIXEL 只是因為方便懶得改結構名稱 XD   之後記得修改掉
			square_tmp.y--;
			square_data2[i]	=	shadow_square( square_tmp );
		}
		// 左下 & 右下
		else if( direction == OpenPIV::left_down || direction == OpenPIV::right_down )
		{
			PIXEL	square_tmp	=	square.at(i);
			square_tmp.y++;
			square_data2[i]	=	shadow_square( square_tmp );
		}

	}


	// 將資料標記在圖片上
	for( i = 0; i < square.size(); i++ )
		mark_on_image( square_data[i] );

	if( direction == OpenPIV::left_down || direction == OpenPIV::left_up ||
		direction == OpenPIV::right_down || direction == OpenPIV::right_up )
	{
		for( i = 0; i < square.size(); i++ )
			mark_on_image( square_data2[i] );
	}
	final.save("output_1.bmp");


	// 利用DensityKernel 濾掉雜訊
	KernelDensity	kd,		kd2;
	bMatrixDouble	bm_tmp,	bm_tmp2;
	QImage			final2	=	image;

	for( i = 0; i < square.size(); i++ )
	{
		kd.set_data( square_data[i] );
		bm_tmp	=	kd.dosomething();

		if( direction == OpenPIV::left_down || direction == OpenPIV::left_up ||
			direction == OpenPIV::right_down || direction == OpenPIV::right_up )
		{
			kd2.set_data( square_data2[i] );
			bm_tmp2	=	kd2.dosomething();
		}

		// 將濾掉陰影的資料逆向設定回去square_data
		for( j = 0; j < square_data[i].size; j++ )
			for( k = 0; k < square_data[i].size; k++ )
			{
				// 這邊採用取交集的作法 (讓點變少 濾掉noise造成的)
				if( bm_tmp( j, k ) < 1.5/9.0 )	// 只有一個點的過濾掉
					square_data[i].line_data[j].shadow_seq(k)	=	0;

				/*if( bm_tmp( j, k ) > 1.5/9.0 )	// 只有一個點的過濾掉
					square_data[i].line_data[j].shadow_seq(k)	=	1;
				else
					square_data[i].line_data[j].shadow_seq(k)	=	0;*/

				if( direction == OpenPIV::left_down || direction == OpenPIV::left_up ||
					direction == OpenPIV::right_down || direction == OpenPIV::right_up )
				{
					if( bm_tmp2( j, k ) < 1.5/9.0 )	// 只有一個點的過濾掉
						square_data2[i].line_data[j].shadow_seq(k)	=	0;
				}

			}

		// 將結果輸出
		//mark_on_image( square_data[i], final2 );
	}
	




	// 偵測edge
	for( i = 0; i < square.size(); i++ )
	{
		for( j = 0; j < square_data[i].size; j++ )
		{
			raw									=	fillup_shadow( square_data[i].line_data[j].shadow_seq, square_data[i].line_data[j].raw_data );		// 取得補值後的原始資料
			square_data[i].line_data[j].edge	=	find_edge( raw );																					// 取得 edge 位置

			//square_data[i].line_data[j].edge	=	find_edge( square_data[i].line_data[j].shadow_seq, square_data[i].line_data[j].raw_data );

			if( direction == OpenPIV::left_down || direction == OpenPIV::left_up ||
				direction == OpenPIV::right_down || direction == OpenPIV::right_up )
			{
				raw									=	fillup_shadow( square_data2[i].line_data[j].shadow_seq,  square_data2[i].line_data[j].raw_data );
				square_data2[i].line_data[j].edge	=	find_edge( raw );
				//square_data2[i].line_data[j].edge	=	find_edge( square_data[i].line_data[j].shadow_seq, square_data[i].line_data[j].raw_data );
			}
		}
		mark_on_image( square_data[i], final2 );
		if( direction == OpenPIV::left_down || direction == OpenPIV::left_up ||
			direction == OpenPIV::right_down || direction == OpenPIV::right_up )
				mark_on_image( square_data2[i], final2 );
	}


	// 開始計算陰影位置
	for( i = 0; i < square.size(); i++ )
	{
		for( j = 0; j < square_data[i].size; j++ )
		{
			square_data[i].line_data[j].edge_distance.resize(0);

			// 搜尋一段陰影區
			istart	=	0;
			iend	=	1;
			while(1)
			{
				//if( istart >= square_data[i].line_data[j].shadow_seq.size() )		break;

				// 搜尋出一段陰影區間
				while( istart < square_data[i].line_data[j].shadow_seq.size() && square_data[i].line_data[j].shadow_seq( istart ) == 0 )		istart++;
				iend	=	istart+1;
				while( iend <  square_data[i].line_data[j].shadow_seq.size()  && square_data[i].line_data[j].shadow_seq( iend ) == 1  )			iend++;

				if( istart >= square_data[i].line_data[j].shadow_seq.size() )		break;

				// 找到陰影段 去計算質心位置
				dtmp	=	centroid( square_data[i].line_data[j].shadow_depth, istart, iend ) - square_data[i].line_data[j].edge;
				square_data[i].line_data[j].edge_distance.push_back( dtmp );

				istart	=	iend + 1;
			}
		}
	}


	// 輸出陰影
	boost::container::vector<double>	arr;
	for( i = 0; i < square.size(); i++ )
	{
		if( settings->batch_check )
			sprintf( str, "./output/%06d.txt", i );
		else
			sprintf( str, "./output/output.txt");
		ofstream	out( str, ios::app );

		if( settings->id == 0 )
		{
			out << "square " << i << "\n\nsize = " << square_data[0].size 
				<< "\nx = " << square[i].x << ", " << square[i].y 
				<< "\nthreshold = " << settings->shadow_depth_threshold << "\n\n\n";
		}

		arr.clear();

		for( j = 0; j < square_data[i].size; j++ )
		{

			for( k = 0; k < square_data[i].line_data[j].edge_distance.size(); k++ )
			{
				dtmp	=	square_data[i].line_data[j].edge_distance.at(k);
				arr.push_back( dtmp );
			}
				//out2 << square_data[i].line_data[j].edge_distance.at(k) << " ";
			//out2 << endl;

		}

		// 計算平均 標準差
		double	mean;
		double	devia;
		dtmp	=	0;
		for( k = 0; k < arr.size(); k++ )
			dtmp	+=	arr.at(k);
		mean	=	dtmp / arr.size();
		dtmp	=	0;
		for( k = 0; k < arr.size(); k++ )
			dtmp	+=	(arr.at(k) - mean)*(arr.at(k)-mean);
		devia	=	sqrt( dtmp/(arr.size()-1) );

		out << setw(5) << settings->id << setw(12) << mean << setw(12) << devia << setw(5) << arr.size() << endl;
		out.close();
	}

	

//	ofstream	out2("s_e_distance.txt", ios::app );
//	out2 << setw(5) << id << setw(12) << mean << setw(12) << devia << setw(5) << arr.size() << endl;
//	out2.close();

	progress->outputImage( 0, final2 );		// 將DensityKernel的結果輸出

	if( settings->batch_check )
		sprintf( str, "./output/%06d.bmp", settings->id );
	else
		sprintf( str, "./output/final.bmp");
	final2.save(str);



//t.Stop();
//progress->debuger( QString("time = %1 ms").arg( t.Elapsed() ), QColor( 0, 0, 255 ) );

	//progress->outputImage( 0, final );

	progress->debuger("finish extract channel ...", QColor( 255, 0, 0 ) );
	
}
#endif





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ KernelDensity 部分 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/***************************************************************************************
	建構函數
/****************************************************************************************/
KernelDensity::KernelDensity()
{}







/***************************************************************************************
	設定資料
	會將格式做轉換
/****************************************************************************************/
bMatrixInt		KernelDensity::set_data( SquareData _data )
{
	int		i,	j;

	const int	size	=	_data.size;

	data.resize( size, size );

	// 將資料轉成一個正方形的matrix
	for( i = 0; i < size; i++ )
		for( j = 0; j < size; j++ )
			data( i, j )	=	_data.line_data[i].shadow_seq(j);

	return	data;
}


/***************************************************************************************
	產生 Gauss Kernel
/****************************************************************************************/
bMatrixDouble	KernelDensity::kernel_gauss( int n )
{
	kernel.resize( n*2+1, n*2+1 );
	int			i,	j;
	double		deviation	=	n / sqrt( 3.0 );

	boost::math::normal		g( 0, deviation );

	for( i = -n; i <= n; i++ )
		for( j = -n; j <= n; j++ )
			kernel( i+n, j+n )	=	pdf( g, i ) * pdf( g, j );

	return	kernel;
}



/***************************************************************************************
	產生基本的方塊狀 kernel
/****************************************************************************************/
bMatrixDouble	KernelDensity::kernel_square( int n )
{
	int		m	=	2*n + 1;

	kernel.resize( m, m );

	int		i,	j;

	for( i = 0; i < m; i++ )
		for( j = 0; j < m; j++ )
			kernel( i, j )	=	1.0;

	return	kernel;
}



/***************************************************************************************
	元素個數計算
/****************************************************************************************/
int		KernelDensity::element_count()
{
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
bMatrixDouble	KernelDensity::dosomething()
{
	int		i,	j,	x,	y;
	int		N;				// 元素個數
	int		N_size;			// 檢測區大小
	double	dcount;

	kernel_square( 1 );		// 1 會產生 3*3 的 matrix

	p_data.resize( data.size1(), data.size2() );		// 初始化資料

	// 計算元素個數
	N		=	element_count();

	N_size	=	data.size1() * data.size2();

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
			dcount	/=	(9.0);							// 9.0 = h^D
			p_data( i, j )	=	dcount ;			// 避免windows大小干擾結果
		}

	ofstream	out("kd.txt");
	for( j = 0; j < p_data.size2(); j++ )
	{
		for( i = 0; i < p_data.size1(); i++ )
			out << p_data(i,j) << " ";
		out << endl;
	}

	double	count	=	0;
	for( i = 0; i < p_data.size1(); i++ )
		for( j = 0; j < p_data.size2(); j++ )
			count	+=	p_data( i, j );
	out << "\n\n\n" << count;
	out.close();

	return	p_data;
}
#endif