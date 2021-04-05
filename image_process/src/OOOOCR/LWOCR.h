#ifndef _LWOCR_H
#define _LWOCR_H

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "OCR_shared.h"


/************************************************************************************************************************************

	note: 圖片用 array 來存 這邊假設是灰階圖 先寬後高的存放方式
		  _OCR_USE_FLOAT_ 定義在 OCR_shared.h  用來決定是否使用float

************************************************************************************************************************************/


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  define  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define		LWO_BIN_VALUE		1															// 二值化後 非零的值  (為了show圖方便)
#define		LWO_IMAGE_WIDTH		22															// 圖片寬
#define		LWO_IMAGE_HEIGHT	32															// 圖片高
#define		LWO_IMAGE_SIZE		( LWO_IMAGE_WIDTH * LWO_IMAGE_HEIGHT )						// pixel 個數 704 = 22 * 32

#define		LWO_IMAGE_PIXEL( PT, INDEX )	(*( (PT) + (INDEX) ))							// 讀取pixel位置
#define		LWO_IMAGE_XY( PT, X, Y )		(*( (PT) + ((Y)*(LWO_IMAGE_WIDTH)) + (X) ))		// 讀取 (X,Y) 位置   括號是為了避免出錯  (假如傳 X = left+i之類的進來)

#define		LWO_IS_WORD			2															// 大於這個 THR 就認為是有字
#define		ADD_VALUE_SIZE		1															// 往外延伸這個距離  (缺少的部分補值)
#define		LWO_IS_I			5															// 是否為 I 的 thr

#ifdef _OCR_USE_FLOAT_
	#define		LWO_CENTER_OUT		3														// 中間偏移的允許範圍
#else
	#define		LWO_CENTER_OUT		6														// 因為沒有除2 所以變成 6
#endif





/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  enum  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef enum	LWO_CROP_TYPE_DEFINE
{
	LWO_WH_SMALL		=	0,		// w/h <= 0.4
	LWO_WH_BIG			=	1,		// w/h >= 0.8
	LWO_CENTER_ERROR	=	2,		// 中心偏離
	LWO_NO_TYPE			=	3,		// 無定義

} LWO_CROP_TYPE ;



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  struct  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// crop 圖片所需的參數
typedef struct	CROP_DATA_DEFINE
{
	BYTE				*img;									// 原始圖片
	BYTE				*bin;									// 二值化圖片
	short				top;									// 框的上下左右
	short				bottom;
	short				left;
	short				right;
	//short				*bg_data;								// 補值資料
	short				id;
	short				histogram_horizon[LWO_IMAGE_WIDTH];		// 水平的 histogram
	short				histogram_vertical[LWO_IMAGE_HEIGHT];	// 垂直的 histotram
	LWO_CROP_TYPE		crop_type;								// 紀錄crop發生的問題
} CROP_DATA ;



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  function  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
short		LWO_max( BYTE *img );																		// 回傳圖片的最大值
short		LWO_min( BYTE *img );																		// 回傳圖片的做小值

void		LWO_binary( BYTE *img, BYTE *bin );															// 對圖片做binarized

short		LWO_crop( CROP_DATA *crop_data, OCR_INIT_TYPE init_type );									// 對圖片做crop 參數細節看 struct 定義   回傳值可以判斷是否為 I
void		LWO_crop_big_boxing( CROP_DATA *crop_data );												// 搜尋出一個大的框 將字框住 (連noise一起框住)
void		LWO_crop_shrink_boxing( CROP_DATA *crop_data );												// 將框往內縮 (碰到某一行都沒有點 就往內縮)

void		LWO_scaled_bilinear( BYTE *img, BYTE *clip_img, short w, short h, short nw, short nh );		// 將圖片做 resize 動作 寬高 由 (w,h) -> (nw, nh)  這邊是bilinear的版本
void		LWO_scaled_nearest( BYTE *img, BYTE *clip_img, short w, short h, short nw, short nh );		// 將圖片做 resize nearest版本
void		LWO_place_center_for_I( CROP_DATA *crop_data, BYTE *clip_img );								// 將圖片置中剪裁 給 I 用版本
void		LWO_place_center_for_wh_small( CROP_DATA *crop_data, BYTE *clip_img );						// 將圖片置中剪裁 給 wh <= 0.4 版本
void		LWO_force_pinch( CROP_DATA *crop_data );													// 強制內縮

short		LWO_add_box( short *_top, short *_bottom, short *_left, short *_right );					// 往外補一層框 (調整邊界 沒真正補值 回傳判斷是否為 I )
void		LWO_I_denoise( CROP_DATA *crop_data );														// 針對 I  crop有問題的  recrop拿掉雜點

#ifdef _OCR_USE_FLOAT_
static void		set_bg_data( double value );															// 設定補值資料
static double	get_bg_data();																			// 取得補值資料
#else
static void		set_bg_data( short value );																// 設定補值資料
static short	get_bg_data();																			// 取得補值資料
#endif

static void		get_histogram( CROP_DATA *crop_data );													// 計算 histogram (用binary data)


void		LWO_bin_rev( BYTE *bin );																	// 將資料反轉
void		LWO_wh_small( CROP_DATA *crop_data );														// 處理 wh <= 0.4



#endif