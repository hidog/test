#include "LWOCR.h"



#ifdef _OCR_USE_FLOAT_
double		add_value	=	-1;		// 補值資料
#else
short		add_value	=	-1;
#endif



/******************************************************************************
	回傳圖片的最大值
*******************************************************************************/
short	LWO_max( BYTE *img )
{
	short	i;
	short	max;

	max		=	-1;
	for( i = 0; i < LWO_IMAGE_SIZE; i++ )
	{
		if( max < LWO_IMAGE_PIXEL( img, i ) )
			max	=	LWO_IMAGE_PIXEL( img, i );
	}
	return	max;
}



/******************************************************************************
	回傳圖片的做小值
*******************************************************************************/
short	LWO_min( BYTE *img )
{
	short	i;
	short	min;

	min		=	9999;
	for( i = 0; i < LWO_IMAGE_SIZE; i++ )
	{
		if( min > LWO_IMAGE_PIXEL( img, i ) )
			min	=	LWO_IMAGE_PIXEL( img, i );
	}

	return	min;
}







/******************************************************************************
	對圖片做binarized
	img   = 原始圖片
	bin   = 存放二值圖的位置
	value = 二值化後 非零的值該放多少 (方便看圖)

	用 cetner去砍一刀後計算平均 標準差 
*******************************************************************************/
void		LWO_binary( BYTE *img, BYTE *bin )
{
	short	i;

#ifdef _OCR_USE_FLOAT_
	double	sum,	mean,	devia;
	double	sum2,	mean2,	devia2;		// 算高值的mean 用來補值
#else
	int		sum;
	short	mean,	devia;
	int		sum2;
	short	mean2,	devia2;
#endif

	short	itemp;
#ifdef _OCR_USE_FLOAT_
	double	dtemp;
#else
	int		dtemp;
#endif

	short	max,	min,	count;
	short	count2;		//算高值的count
#ifdef _OCR_USE_FLOAT_
	double	center;
#else
	short	center;
#endif

#ifdef _OCR_USE_FLOAT_
	double	THR;
#else
	short	THR;
#endif

	add_value	=	-1;		// 用來檢查用 判斷補值資料是否有被修改掉

	max		=	LWO_max( img );
	min		=	LWO_min( img );

	// 用最大最小的中間去先砍一刀 再算剩下的平均
#ifdef _OCR_USE_FLOAT_
	center	=	0.5 * (max + min);
#else
	center	=	(max+min)/2;
#endif

	// 計算平均
	sum		=	0;
	sum2	=	0;
	count	=	0;
	count2	=	0;
	for( i = 0; i < LWO_IMAGE_SIZE; i++ )
	{
		itemp	=	LWO_IMAGE_PIXEL( img, i );
#ifdef _OCR_USE_FLOAT_
		if( itemp < center )
#else
		if( itemp <= center )		// 因為改short 要多等號
#endif
		{
			sum	+=	itemp;
			count++;
		}
		else	// 算高值區的mean
		{
			sum2	+=	itemp;
			count2++;
		}
	}
	mean	=	sum / count;
	mean2	=	sum2 / count2;

	// 標準差
	sum		=	0;
	sum2	=	0;
	count	=	0;
	count2	=	0;
	for( i = 0; i < LWO_IMAGE_SIZE; i++ )
	{
		itemp	=	LWO_IMAGE_PIXEL( img, i );
#ifdef _OCR_USE_FLOAT_
		if( itemp < center )
#else
		if( itemp <= center )
#endif
		{
			dtemp	=	( itemp - mean ) * ( itemp - mean );
			sum		+=	dtemp;
			count++;
		}
		else
		{
			dtemp	=	( itemp - mean2 ) * ( itemp - mean2 );
			sum2		+=	dtemp;
			count2++;
		}
	}
	devia	=	sqrt( 1.0 * sum / count );
	devia2	=	sqrt( 1.0 * sum2 / count2 );

#ifdef _OCR_USE_FLOAT_
	set_bg_data( mean2 + 0.5*devia2 );		// 利用高值的平均去補值
#else
	set_bg_data( mean2 + devia2/2 );
#endif

#ifdef _OCR_USE_FLOAT_
	THR		=	mean + 0.5*devia;		// 二值化的 threshold
#else
	THR		=	mean + devia / 2;
#endif
	// 二值化
	for( i = 0; i < LWO_IMAGE_SIZE; i++ )
	{
		itemp	=	LWO_IMAGE_PIXEL( img, i );
#ifdef _OCR_USE_FLOAT_
		if( itemp < THR )
#else
		if( itemp <= THR )	// 改整數要加等號
#endif
			LWO_IMAGE_PIXEL( bin, i )	=	LWO_BIN_VALUE;
		else
			LWO_IMAGE_PIXEL( bin, i )	=	0;
	}
}





/******************************************************************************
	反轉
*******************************************************************************/
void		LWO_bin_rev( BYTE *bin )
{
	short	i,	j;

	for( i = 0; i < LWO_IMAGE_SIZE; i++ )
	{
		if( bin[i] == 0 )
			bin[i]	=	LWO_BIN_VALUE;
		else
			bin[i]	=	0;
	}
}




/******************************************************************************
	搜尋出一個大的框 將字框住 
	這邊是直接搜尋最 上 右 下 左 的點
	會連雜訊一起框住
*******************************************************************************/
void	LWO_crop_big_boxing( CROP_DATA *crop_data )
{
	short	top		=	-1,					// 用 -1 表示沒搜尋到
			bottom	=	-1,	
			left	=	-1,	
			right	=	-1;

	short	i,	j;

	BYTE	*bin	=	crop_data->bin;		// 二值化圖片資料

	// top
	for( j = 0; j < LWO_IMAGE_HEIGHT; j++ )
	{
		for( i = 0; i < LWO_IMAGE_WIDTH; i++ )
		{
			if( LWO_IMAGE_XY( bin, i, j ) == LWO_BIN_VALUE )
			{
				top		=	j;
				break;
			}
		}
		if( top >= 0 )		// 已經搜尋到資料 提早跳出
			break;
	}
	// bottom
	for( j = LWO_IMAGE_HEIGHT - 1; j >= 0; j-- )
	{
		for( i = 0; i < LWO_IMAGE_WIDTH; i++ )
		{
			if( LWO_IMAGE_XY( bin, i, j ) == LWO_BIN_VALUE )
			{
				bottom	=	j;
				break;
			}
		}
		if( bottom >= 0 )
			break;
	}
	// left
	for( i = 0; i < LWO_IMAGE_WIDTH; i++ )
	{
		for( j = 0; j < LWO_IMAGE_HEIGHT; j++ )
		{
			if( LWO_IMAGE_XY( bin, i, j ) == LWO_BIN_VALUE )
			{
				left	=	i;
				break;
			}
		}
		if( left >= 0 )
			break;
	}
	// right
	for( i = LWO_IMAGE_WIDTH - 1; i >= 0; i-- )
	{
		for( j = 0; j < LWO_IMAGE_HEIGHT; j++ )
		{
			if( LWO_IMAGE_XY( bin, i, j ) == LWO_BIN_VALUE )
			{
				right	=	i;
				break;
			}
		}
		if( right >= 0 )
			break;
	}

	if( top == -1 )		top		=	0;
	if( bottom == -1 )	bottom	=	LWO_IMAGE_HEIGHT - 1;
	if( left == -1 )	left	=	0;
	if( right == -1 )	right	=	LWO_IMAGE_WIDTH - 1;		// 處理沒字的情況

	// 回寫資料
	crop_data->top		=	top;
	crop_data->left		=	left;
	crop_data->bottom	=	bottom;
	crop_data->right	=	right;

}


/******************************************************************************
	設定補值資料
*******************************************************************************/
#ifdef _OCR_USE_FLOAT_
static void		set_bg_data( double value )
#else
static void		set_bg_data( short value )
#endif
{
	add_value	=	value;
}




/******************************************************************************
	將框往內縮 (碰到某一行都沒有點 就往內縮)
	都沒有點這點可以調整
	同時會搜尋出一條都沒有雜訊的 行/列 做為補值資料 
	(搜尋不到就不補值 滿版情況)
	如果搜尋過程碰到某一 行/列 點數很多 就提早結束 
	(防止字斷掉 加速 等等  對 L 這種細邊字無效果)
*******************************************************************************/
void	LWO_crop_shrink_boxing( CROP_DATA *crop_data )
{
	short	top			=	crop_data->top,			// 取出大框的資料
			bottom		=	crop_data->bottom,	
			left		=	crop_data->left,	
			right		=	crop_data->right;

	BYTE	*img		=	crop_data->img;			// 圖片資料
	BYTE	*bin		=	crop_data->bin;			// 二值化圖片資料

	short	i,	j;
	short	itemp;
	short	count;

	// 因為想多檢查自己  所以拿掉 left + 1   以下以此類推
	// left
	for( i = left; i < LWO_IMAGE_WIDTH; i++ )
	{
		count	=	0;
		for( j = 0; j < LWO_IMAGE_HEIGHT; j++ )
		{
			itemp	=	LWO_IMAGE_XY( bin, i, j ) > 0 ? 1 : 0;		// 這邊把字設計成白色  背景黑色   如果顛倒會錯
			count	+=	itemp;
		}
		if( count == 0 )
			left	=	i + 1;			// 注意這個 + 1  !!

		if( count > LWO_IS_WORD )
			break;
	}
	// right
	for( i = right; i >= 0; i-- )
	{
		count	=	0;
		for( j = 0; j < LWO_IMAGE_HEIGHT; j++ )
		{
			itemp	=	LWO_IMAGE_XY( bin, i, j ) > 0 ? 1 : 0;
			count	+=	itemp;
		}
		if( count == 0 )
			right	=	i - 1;

		if( count > LWO_IS_WORD )
			break;
	}
	// top
	for( j = top; j < LWO_IMAGE_HEIGHT; j++ )		// 這邊假設一定有字  所以搜尋範圍從頭到尾  沒有字的話會出問題
	{
		count	=	0;
		for( i = 0; i < LWO_IMAGE_WIDTH; i++ )
		{
			itemp	=	LWO_IMAGE_XY( bin, i, j ) > 0 ? 1 : 0;		// 轉成 0/1
			count	+=	itemp;
		}
		if( count == 0 )				// 內縮
			top	=	j + 1;				// 第 j 行 全為0, 所以內縮一排

		if( count > LWO_IS_WORD )		// 認為有字  是因為這個code 所以搜尋範圍才可以到底  要不然會有bug   (基本上就是防斷掉機制)
			break;
	}
	// bottom
	for( j = bottom; j >= 0; j-- )
	{
		count	=	0;
		for( i = 0; i < LWO_IMAGE_WIDTH; i++ )
		{
			itemp	=	LWO_IMAGE_XY( bin, i, j ) > 0 ? 1 : 0;
			count	+=	itemp;
		}
		if( count == 0 )
			bottom	=	j - 1;

		if( count > LWO_IS_WORD )
			break;
	}

	// 檢查是否有問題
	if( top >= bottom )		
	{
		top		=	0;
		bottom	=	LWO_IMAGE_HEIGHT - 1;
	}
	if( left >= right )
	{
		left	=	0;
		right	=	LWO_IMAGE_WIDTH - 1;
	}

	// 回寫資料
	crop_data->top		=	top;
	crop_data->bottom	=	bottom;
	crop_data->left		=	left;
	crop_data->right	=	right;
}



/******************************************************************************
	取得補值資料
	這邊採用循環方式 (區分方向)
*******************************************************************************/
#ifdef _OCR_USE_FLOAT_
static double		get_bg_data()
#else
static short		get_bg_data()
#endif
{
	if( add_value > 0 )
		return	add_value;
	else
	{
		printf("error...\n");
		return	0;
	}
}






/******************************************************************************
	將圖片做 resize 動作 
	寬高 由 (w,h) -> (nw, nh)

	這邊是bilinear的版本 

	clip_img 是剪裁的資料 寬高 = (w,h)
	希望轉成 (nw, nh)
	並且將資料存在 img

	http://tech-algorithm.com/articles/bilinear-image-scaling/  參考這邊的code去改寫
    int[] temp = new int[w2*h2] ;
    int A, B, C, D, x, y, index, gray ;

    float x_ratio = ((float)(w-1))/w2 ;
    float y_ratio = ((float)(h-1))/h2 ;
    float x_diff, y_diff, ya, yb ;

    int offset = 0 ;
    for (int i=0;i<h2;i++) {
        for (int j=0;j<w2;j++) {
            x = (int)(x_ratio * j) ;
            y = (int)(y_ratio * i) ;
            x_diff = (x_ratio * j) - x ;
            y_diff = (y_ratio * i) - y ;
            index = y*w+x ;

            // range is 0 to 255 thus bitwise AND with 0xff
            A = pixels[index] & 0xff ;
            B = pixels[index+1] & 0xff ;
            C = pixels[index+w] & 0xff ;
            D = pixels[index+w+1] & 0xff ;
            
            // Y = A(1-w)(1-h) + B(w)(1-h) + C(h)(1-w) + Dwh
            gray = (int)(
                    A*(1-x_diff)*(1-y_diff) +  B*(x_diff)*(1-y_diff) +
                    C*(y_diff)*(1-x_diff)   +  D*(x_diff*y_diff)
                    ) ;

            temp[offset++] = gray ;                                   
        }
    }
    return temp ;

*******************************************************************************/
void		LWO_scaled_bilinear( BYTE *img, BYTE *clip_img, short w, short h, short nw, short nh )
{
	// note: bilinear沒拿掉float  需要再改

	// src :  pixels 換成 clip_img
	// temp 換成 img (最後存在這邊)
	int		i,	j;

	int		A,	B,	C,	D,	x,	y,	index,	gray;
	double	x_ratio	=	((double)(w-1))/nw;
	double	y_ratio	=	((double)(h-1))/nh;
	double	x_diff,	y_diff,	ya,	yb;

	int		offset	=	0;

	for( i = 0; i < nh; i++ )
	{
		for( j = 0; j < nw; j++ )	// j = 寬  i = 高  跟平常習慣不同 要注意
		{
			x		=	(int)( x_ratio * j );
			y		=	(int)( y_ratio * i );
			x_diff	=	( x_ratio * j ) - x;
			y_diff	=	( y_ratio * i ) - y;
			index	=	y * w + x;

			// 將資料轉成 0~255
			A		=	*( clip_img + index )         & 0xff ;
			B		=	*( clip_img + index + 1 )     & 0xff ;
			C		=	*( clip_img + index + w )     & 0xff ;
			D		=	*( clip_img + index + w + 1 ) & 0xff ;

			// Y = A(1-w)(1-h) + B(w)(1-h) + C(h)(1-w) + Dwh
			gray	=	(int)( A*(1-x_diff)*(1-y_diff) + B*(x_diff)*(1-y_diff) + C*(y_diff)*(1-x_diff) + D*(x_diff*y_diff) ) ;	

			*( img + offset++ )	=	gray;	// 寫回去
		}

	}
}


/******************************************************************************
	crop 後 w/h 過小
	這時候選擇擴張 置中 水平不resize 垂直resize
	大部分是 I + 髒污
	少部分是 L T 斷掉
*******************************************************************************/
void	LWO_wh_small( CROP_DATA *crop_data )
{
	LWO_crop_big_boxing( crop_data );	// 框大一點
}



/******************************************************************************
	計算 histogram (用binary data)
*******************************************************************************/
static void		get_histogram( CROP_DATA *crop_data )
{
	BYTE	*bin	=	crop_data->bin;

	short	i,	j;
	short	count;

	// 水平
	for( i = 0; i < LWO_IMAGE_WIDTH; i++ )
	{
		count	=	0;
		for( j = 0; j < LWO_IMAGE_HEIGHT; j++ )
		{
			if( LWO_IMAGE_XY( bin, i, j ) == LWO_BIN_VALUE )
				count++;
		}
		crop_data->histogram_horizon[i]		=	count;
	}
	// 垂直
	for( j = 0; j < LWO_IMAGE_HEIGHT; j++ )
	{
		count	=	0;
		for( i = 0; i < LWO_IMAGE_WIDTH; i++ )
		{
			if( LWO_IMAGE_XY( bin, i, j ) == LWO_BIN_VALUE )
				count++;
		}
		crop_data->histogram_vertical[j]	=	count;
	}
}


/******************************************************************************
	強制內縮
	要注意不能內縮過頭 (又有雜訊又有斷掉)
	先搜尋histogram, 挑最大的當中心隔開
	                 次大跟最大差不多 則當雙分水嶺 (例如 U )
					 並且不能出現在邊緣
	先假設雜點不容易出現在上下 以左右為主
	沒有中心隔開 會發生收縮過度的問題
*******************************************************************************/
void		LWO_force_pinch( CROP_DATA *crop_data )
{
	short	top		=	crop_data->top,
			left	=	crop_data->left,
			bottom	=	crop_data->bottom,
			right	=	crop_data->right;

	short	histogram_horizon[LWO_IMAGE_WIDTH];
	short	histogram_vertical[LWO_IMAGE_HEIGHT];

	short	i;
	short	tmp;
	short	max,		max_id;
	short	center_1,	center_2;	// 當中心 1, 2的座標
	short	max_1,		max_2;		// 最大 次大

	memcpy( histogram_horizon, crop_data->histogram_horizon, sizeof(short)*LWO_IMAGE_WIDTH );			// 為了方便搜尋 複製資料
	memcpy( histogram_vertical, crop_data->histogram_vertical, sizeof(short)*LWO_IMAGE_HEIGHT );		// 為了方便搜尋 複製資料

	// 搜尋histogram max
	max		=	0;
	for( i = 2; i < LWO_IMAGE_WIDTH - 2; i++ )		// 因為在邊緣的可能是雜點
	{
		if( histogram_horizon[i] > max )
		{
			max		=	histogram_horizon[i];
			max_id	=	i;
		}
	}
	center_1					=	max_id;
	max_1						=	max;
	histogram_horizon[center_1]	=	-1;				// 避免再度搜尋到
	
	// 搜尋histogram max 次大
	max		=	0;
	for( i = 2; i < LWO_IMAGE_WIDTH - 2; i++ )		// 因為在邊緣的可能是雜點
	{
		if( histogram_horizon[i] > max )
		{
			max		=	histogram_horizon[i];
			max_id	=	i;
		}
	}
	center_2					=	max_id;
	max_2						=	max;
	histogram_horizon[center_2]	=	-1;	// 避免再度搜尋到

	if( max_1 - max_2 <= 1 )		// 雙峰字 例如 U
	{
		// max不需要交換 因為沒有用在空間上.
		if( center_1 > center_2 )
		{
			tmp			=	center_1;
			center_1	=	center_2;
			center_2	=	tmp;
		}

		// 開始進行強制內縮  水平向
		// 左邊的內縮
		for( i = left < 0 ? 0 : left; i < center_1; i++ )	// left在初始化後就會改掉值
		{
			if( crop_data->histogram_horizon[i] == 0 )		// 因為histogram_horizon被改過 必須用原本的來搜尋
				left	=	i + 1;		// 注意這個 + 1 !!!!
		}
		// 右邊的內縮
		for( i = right >= LWO_IMAGE_WIDTH ? LWO_IMAGE_WIDTH-1 : right; i > center_2; i-- )
		{
			if( crop_data->histogram_horizon[i] == 0 )		// 因為histogram_horizon被改過 必須用原本的來搜尋
				right	=	i - 1;
		}
	}

	else		// 單峰字 例如 A
	{
		// 開始進行強制內縮  水平向
		// 左邊的內縮
		for( i = left; i < center_1; i++ )
		{
			if( crop_data->histogram_horizon[i] == 0 )		// 因為histogram_horizon被改過 必須用原本的來搜尋
				left	=	i + 1;
		}
		// 右邊的內縮
		for( i = right; i > center_1; i-- )
		{
			if( crop_data->histogram_horizon[i] == 0 )		// 因為histogram_horizon被改過 必須用原本的來搜尋
				right	=	i - 1;
		}
	}

	// 寫回資料
	crop_data->top		=	top;
	crop_data->left		=	left;
	crop_data->right	=	right;
	crop_data->bottom	=	bottom;

}


/******************************************************************************
	往外補一層框 
	(調整邊界 沒真正補值 回傳判斷是否為 I )
*******************************************************************************/
short	LWO_add_box( short *_top, short *_bottom, short *_left, short *_right )
{
	short	top		=	*_top,
			bottom	=	*_bottom,
			left	=	*_left,
			right	=	*_right;

	short	is_I	=	-1;

	// 找完框 開始做裁剪
	// 這邊是滿版 不需要特別動作
	if( top == 0 && bottom == LWO_IMAGE_HEIGHT - 1 && left == 0 && right == LWO_IMAGE_WIDTH - 1 )
	{
		is_I	=	0;	// 不是 I
	}
	// 非滿版 需要多挖一圈 (缺少部分進行補值)
	else
	{
		// 多挖一圈的動作
		top		-=	ADD_VALUE_SIZE;		// 允許 -1
		bottom	+=	ADD_VALUE_SIZE;		// 允許超過邊框

		if( right - left >= LWO_IS_I )
		{
			left	-=	ADD_VALUE_SIZE;
			right	+=	ADD_VALUE_SIZE;
			is_I	=	0;		// 不是 I
		}
		else
		{
			is_I	=	1;		// 代表是 I
		}
	}

	// 回寫資料
	*_top		=	top;
	*_bottom	=	bottom;
	*_left		=	left;
	*_right		=	right;

	return	is_I;
}





/******************************************************************************
	針對 I  crop有問題的  recrop拿掉雜點
*******************************************************************************/
void		LWO_I_denoise( CROP_DATA *crop_data )
{
	short	top		=	crop_data->top,
			bottom	=	crop_data->bottom,
			left	=	crop_data->left,
			right	=	crop_data->right;

	BYTE	*bin	=	crop_data->bin;
	BYTE	*img	=	crop_data->img;

	short	i,	j;

	for( i = 0; i < LWO_IMAGE_WIDTH; i++ )
		for( j = 0; j < LWO_IMAGE_HEIGHT; j++ )
		{
			if( i >= left && i <= right && j >= top && j <= bottom )
				continue;		// 無動作

			if( LWO_IMAGE_XY( bin, i, j ) == LWO_BIN_VALUE )
				LWO_IMAGE_XY( img, i, j )	=	get_bg_data();
		}
}







/******************************************************************************
	對圖片去做 crop 的動作
	裡面會包含兩個動作
	1. boxing  框出圖形
	2. resize  將圖形調整到 22*32

	以下參考crop_data定義

	img : 原始影像
	bin : 二值化影像

	_top, _bottom, _left, _right  框的上下左右界 

	bg_value      : 隨便抓一條沒有汙點的 行/列  用來補值
	bg_value_size : bg_value 的 size

	回傳值用來判斷是否為 I

*******************************************************************************/
short	LWO_crop( CROP_DATA *crop_data, OCR_INIT_TYPE init_type )
{
	BYTE	buffer_clip_img[ (LWO_IMAGE_WIDTH + ADD_VALUE_SIZE*2) * (LWO_IMAGE_HEIGHT + ADD_VALUE_SIZE*2) ];		// 最大上限不超過這個範圍 所以buffer取這麼大

	BYTE	*img		=	crop_data->img;
	BYTE	*bin		=	crop_data->bin;
	BYTE	*clip_img	=	&(buffer_clip_img[0]);		// 用指標來操作
//	short	*bg_data	=	crop_data->bg_data;			// 都是寫入同一個區塊 所以不需要重新指向

	short	is_I	=	0;		// 這個字是否為 I
	short	i,	j;
	short	top		=	-1,
			bottom	=	-1,
			left	=	-1,
			right	=	-1;
	short	new_width,	new_height;			// 新的寬 高

	double	wh_ratio;				// 只用一次的 保持浮點數
#ifdef _OCR_USE_FLOAT_
	double	cx,		cy;
#else
	short	cx,		cy;				// 不除2 就可以當整數計算了
#endif

	LWO_CROP_TYPE	crop_type	=	LWO_NO_TYPE;		// crop  error錯誤的處理

	memset( clip_img, 0, sizeof(BYTE) * ((LWO_IMAGE_WIDTH + ADD_VALUE_SIZE*2) * (LWO_IMAGE_HEIGHT + ADD_VALUE_SIZE*2)) );

	// 搜尋出一個大的框 會連同雜訊一起框住
	LWO_crop_big_boxing( crop_data );

	// 將框往內縮 (並且找出一條沒有雜點的 行/列 )
	LWO_crop_shrink_boxing( crop_data );

	// 取得框的資料
	top		=	crop_data->top;
	bottom	=	crop_data->bottom;
	left	=	crop_data->left;
	right	=	crop_data->right;


	// 調整邊界 往外補一層框 (這邊是訂出邊界範圍 沒有真正補值)
	is_I	=	LWO_add_box( &top, &bottom, &left, &right );

	// 將框的資料寫回去crop_data
	crop_data->top		=	top;
	crop_data->bottom	=	bottom;
	crop_data->left		=	left;
	crop_data->right	=	right;

	// 重新設定寬 高
	if( is_I == 0 )
	{
		new_width	=	right - left + 1;		// 注意 + 1
		new_height	=	bottom - top + 1;
	}
	else		//  I 的 寬 都是原本的寬 因為寬度上不縮放
	{
		new_width	=	LWO_IMAGE_WIDTH;		// 因為 I 不做橫向縮放
		new_height	=	bottom - top + 1;
	}


	/*
		crop 異常 必須做處理
		w/h <= 0.4  擴張 置中 水平不處理 垂直resize
		w/h >= 0.8  強制內縮  走原本判斷 (再次判斷是否為 I)
		中心偏離    強制內縮  走原本判斷 (再次判斷是否為 I)
		強制內縮還是異常者 不處理
		不是 I 的才處理
		如果 U 中間斷掉 又碰到大雜點 會出問題
		(目前搜尋histogram作為搜尋範圍判斷)
		(histogram剛好在邊緣兩pixel內 跳過)
	*/
	wh_ratio	=	1.0 * new_width / new_height;
#ifdef _OCR_USE_FLOAT_
	cx			=	0.5 * ( left + right );
	cy			=	0.5 * ( top + bottom );
#else
	cx			=	left + right;		// 不除2 !!
	cy			=	top + bottom;
#endif

	crop_type	=	LWO_NO_TYPE;		// 一開始是預設值

	if( is_I == 0 )   // 是 I 的跳過
	{
		if( wh_ratio <= 0.4 && init_type == OCR_CNY_20 )		// 小額才做 因為大額會有 1 造成錯誤
		{
			// 計算 histogram
			get_histogram( crop_data );

			// 擴張 置中 水平不處理 垂直resize
			LWO_wh_small( crop_data );

			// 往外補一圈  (猶豫是否要這樣做)
			LWO_add_box( &crop_data->top, &crop_data->bottom, &crop_data->left, &crop_data->right );

			// 重新寫入上下左右的資料  (因為會寫在crop_data裡面)
			top			=	crop_data->top;
			bottom		=	crop_data->bottom;
			left		=	crop_data->left;
			right		=	crop_data->right;

			// 重新設定 new_width等等
			new_width	=	LWO_IMAGE_WIDTH;		// 必須跟 I 一樣
			new_height	=	bottom - top + 1;
			// 假如是 I 已經被踢掉 所以不管
			crop_type	=	LWO_WH_SMALL;
		}
		else if( wh_ratio >= 0.8 )
		{
			// 計算 histogram
			get_histogram( crop_data );

			// 強制內縮  走原本判斷 (再次判斷是否為 I)
			LWO_force_pinch( crop_data );

			// 往外擴張 並且判斷是否為 I
			is_I	=	LWO_add_box( &crop_data->top, &crop_data->bottom, &crop_data->left, &crop_data->right );

			if( is_I == 1 )
				LWO_I_denoise( crop_data );		// 針對 I 去做 消除雜點

			// 重新寫入上下左右的資料
			top			=	crop_data->top;		// 因為底下有提早結束 所以必須在這邊就先寫入資料.  (應該可以讓code更簡潔)
			bottom		=	crop_data->bottom;
			left		=	crop_data->left;
			right		=	crop_data->right;
			// 重新設定 new_width等等
			if( is_I == 0 )
			{
				new_width	=	right - left + 1;		// 注意 + 1
				new_height	=	bottom - top + 1;
			}
			else
			{		
				new_width	=	LWO_IMAGE_WIDTH;		// 因為 I 不做橫向縮放
				new_height	=	bottom - top + 1;
			}

			// 如果還是有問題 就不做resize了 
			wh_ratio	=	1.0 * new_width / new_height;
#ifdef _OCR_USE_FLOAT_
			cx			=	0.5 * ( left + right );		// 目前無判斷 (考慮擴充性 保留code 可以加mark)
			cy			=	0.5 * ( top + bottom );	
#else
			cx			=	left + right;
			cy			=	top + bottom;
#endif

			crop_type	=	LWO_WH_BIG;

			if( wh_ratio >= 0.8 && is_I == 0 )		// I還是得做動作  
			{
				crop_data->crop_type	=	crop_type;
				return	-1;
			}

		}
#ifdef _OCR_USE_FLOAT_
		else if( cx > 10.5 + LWO_CENTER_OUT || cx < 10.5 - LWO_CENTER_OUT || cy > 15.5 + LWO_CENTER_OUT || cy < 15.5 - LWO_CENTER_OUT )
#else
		else if( cx > 21 + LWO_CENTER_OUT || cx < 21 - LWO_CENTER_OUT || cy > 31 + LWO_CENTER_OUT || cy < 31 - LWO_CENTER_OUT )
#endif
		{
			// 計算 histogram
			get_histogram( crop_data );

			// 強制內縮  走原本判斷 (再次判斷是否為 I)
			LWO_force_pinch( crop_data );

			// 往外擴張 並且判斷是否為 I
			is_I	=	LWO_add_box( &crop_data->top, &crop_data->bottom, &crop_data->left, &crop_data->right );

			if( is_I == 1 )
				LWO_I_denoise( crop_data );		// 針對 I 去做 消除雜點

			// 重新寫入上下左右的資料
			top			=	crop_data->top;
			bottom		=	crop_data->bottom;
			left		=	crop_data->left;
			right		=	crop_data->right;
			// 重新設定 new_width等等
			if( is_I == 0 )
			{
				new_width	=	right - left + 1;		// 注意 + 1
				new_height	=	bottom - top + 1;
			}
			else
			{		
				new_width	=	LWO_IMAGE_WIDTH;		// 因為 I 不做橫向縮放
				new_height	=	bottom - top + 1;
			}

			// 如果還是有問題 就不做resize了 
			wh_ratio	=	1.0 * new_width / new_height;
			cx			=	0.5 * ( left + right );
			cy			=	0.5 * ( top + bottom );	

			crop_type	=	LWO_CENTER_ERROR;

			if( wh_ratio >= 0.8 && is_I == 0 )		// I還是得做動作
			{
				crop_data->crop_type	=	crop_type;
				return	-1;
			}
		}
		else
		{
			// 正常的
			crop_type	=	LWO_NO_TYPE;		// 沒有recrop
		}
	}
	

	crop_data->crop_type	=	crop_type;		// 回寫資料


	// 開始進行剪裁 並且必須進行補值
	if( crop_type == LWO_WH_SMALL )
	{
		LWO_place_center_for_wh_small( crop_data, clip_img );		// 過小的時候 做置中等處理
	}
	else if( is_I == 0 )
	{
		for( i = 0; i < new_width; i++ )
			for( j = 0; j < new_height; j++ )
			{
				// 範圍內 不需要補值
				if( left+i >= 0 && left+i < LWO_IMAGE_WIDTH && top+j >= 0 && top+j < LWO_IMAGE_HEIGHT )
					*( clip_img + j*new_width + i )		=	LWO_IMAGE_XY( img, left+i, top+j );

				else
					*( clip_img + j*new_width + i )		=	get_bg_data();
			}
	}
	else if( is_I == 1 )	// 因為 I 必須多做置中
	{
		LWO_place_center_for_I( crop_data, clip_img );
	}
	else
	{
		// error...
	}

	//LWO_scaled_bilinear( img, clip_img, new_width, new_height, LWO_IMAGE_WIDTH, LWO_IMAGA_HEIGHT );		// 對圖片做縮放  bilinear版本
	LWO_scaled_nearest( img, clip_img, new_width, new_height, LWO_IMAGE_WIDTH, LWO_IMAGE_HEIGHT );			// nearest版本

	return	is_I;
}


/******************************************************************************
	將圖片置中剪裁 (主要是為了 I )
*******************************************************************************/
void	LWO_place_center_for_wh_small( CROP_DATA *crop_data, BYTE *clip_img )
{
	BYTE	*img		=	crop_data->img;				// 原始圖片資料
//	short	*bg_data	=	crop_data->bg_data;			// 補值資料

	short	top			=	crop_data->top;
	short	bottom		=	crop_data->bottom;
	short	left		=	crop_data->left;
	short	right		=	crop_data->right;

	short	new_width	=	LWO_IMAGE_WIDTH;			// I 的寬不變
	short	new_height	=	bottom - top + 1;

	short	center_x2	=	(left + right) / 2;			// 挖出的框的中心  要把這個點對到 (center_x, center_y)
	short	center_y2	=	(top + bottom) / 2;

	short	center_x	=	(new_width-1) / 2;			// 剪裁後圖片的中心
	short	center_y	=	(new_height-1) / 2;

	short	off_x		=	center_x2 - center_x;		// 位移資料
	short	off_y		=	center_y2 - center_y;	

	short	i,	j;

	for( i = 0; i < new_width; i++ )
		for( j = 0; j < new_height; j++ )
		{
			if( i+off_x >= 0 && i+off_x < LWO_IMAGE_WIDTH && j+off_y >= 0 && j+off_y < LWO_IMAGE_HEIGHT )
				*( clip_img + j*new_width + i )	=	LWO_IMAGE_XY( img, i+off_x, j+off_y );
			else
				*( clip_img + j*new_width + i )	=	get_bg_data();
		}
}





/******************************************************************************
	將圖片置中剪裁 (主要是為了 I )
*******************************************************************************/
void	LWO_place_center_for_I( CROP_DATA *crop_data, BYTE *clip_img )
{
	BYTE	*img		=	crop_data->img;				// 原始圖片資料
//	short	*bg_data	=	crop_data->bg_data;			// 補值資料

	short	top			=	crop_data->top;
	short	bottom		=	crop_data->bottom;
	short	left		=	crop_data->left;
	short	right		=	crop_data->right;

	short	new_width	=	LWO_IMAGE_WIDTH;			// I 的寬不變
	short	new_height	=	bottom - top + 1;

	short	center_x2	=	(left + right) / 2;			// 挖出的框的中心  要把這個點對到 (center_x, center_y)
	short	center_y2	=	(top + bottom) / 2;

	short	center_x	=	(new_width-1) / 2;			// 剪裁後圖片的中心
	short	center_y	=	(new_height-1) / 2;

	short	off_x		=	center_x2 - center_x;		// 位移資料
	short	off_y		=	center_y2 - center_y;	

	short	i,	j;

	for( i = 0; i < new_width; i++ )
		for( j = 0; j < new_height; j++ )
		{
			if( i+off_x >= 0 && i+off_x < LWO_IMAGE_WIDTH && j+off_y >= 0 && j+off_y < LWO_IMAGE_HEIGHT )
				*( clip_img + j*new_width + i )	=	LWO_IMAGE_XY( img, i+off_x, j+off_y );
			else
				*( clip_img + j*new_width + i )	=	get_bg_data();
		}
}




/******************************************************************************
	將圖片做 resize nearest版本

	寬高由 (w,h) -> (nw,nh)
	原始資料 = clip_img  轉好的放在 img

	http://tech-algorithm.com/articles/nearest-neighbor-image-scaling/  參考這邊的程式碼

    int[] temp = new int[w2*h2] ;
    double x_ratio = w1/(double)w2 ;
    double y_ratio = h1/(double)h2 ;
    double px, py ; 
    for (int i=0;i<h2;i++) {
        for (int j=0;j<w2;j++) {
            px = Math.floor(j*x_ratio) ;
            py = Math.floor(i*y_ratio) ;
            temp[(i*w2)+j] = pixels[(int)((py*w1)+px)] ;
        }
    }
    return temp ;

*******************************************************************************/
void		LWO_scaled_nearest( BYTE *img, BYTE *clip_img, short w, short h, short nw, short nh )
{
	short	i,	j;
#ifdef _OCR_USE_FLOAT_
	double	x_ratio	=	(double)w / nw;
	double	y_ratio	=	(double)h / nh;
#endif
	short	px,	py;

    for( i = 0; i < nh; i++ )  
	{
        for( j = 0; j < nw; j++ )	// j = 寬  i = 高  跟平常使用不同
		{
#ifdef _OCR_USE_FLOAT_
            px	=	(int)( j*x_ratio );  
            py	=	(int)( i*y_ratio );  
#else
			px	=	j * w / nw;
			py	=	i * h / nh;
#endif

			*( img + i*nw + j )	=	*( clip_img + py*w + px );
        }
    }
}






