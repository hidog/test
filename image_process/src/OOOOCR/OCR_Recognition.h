#ifndef _OCR_RECOGNITION_H
#define _OCR_RECOGNITION_H

#include "./OCR_image_process.h"
#include "./OCR_read_data.h"
#include "./OCR_shared.h"
#include "./LWOCR.h"




//#define	_TEMPLATE_PATH	"D:\\TEST\\Template"				// 樣板路徑
#define	_TEMPLATE_PATH_CNY_20	"..\\DATA\\CNY20_Template"		// 樣板路徑 小額
#define	_TEMPLATE_PATH_CNY_100	"..\\DATA\\CNY100_Template"		// 樣板路徑 小額 


#define V_Sample_Num	2 //若不能整除 會有邊角像素遺失的問題
#define H_Sample_Num	2


#define BZ_V_Sample_Num 5 //若不能整除 會有邊角像素遺失的問題(2,3,5,9,15)
#define BZ_H_Sample_Num 4 //(2,3,4,7)

#define OCR_2_30		1073741823		// 2^30 - 1    用來處理 overflow 問題







void	OCR_init( OCR_INIT_TYPE init_type );							// OCR 初始化
void	OCR_init_template(  );					// OCR 初始化樣板


int		OCR_exec( BYTE *img, BYTE *bin );				// 主程序進入點


int		OCR_Recognize( BYTE *Segmental_image_Binary, BYTE *Segmental_image );		// 文字辨識 實體部分



void	OCR_bin_feature( BYTE *Segmental_image_Binary, short *Sample, int *BZ_Feature );		// 計算 binary圖的feature

void	OCR_feature( BYTE *Segmental_image, short *Sample, short *source_matching, int *Feature, int *BZ_Feature );	// 一般的特徵

void	OCR_act_out( int *Feature, int *reorder_label, short *reorder_output );	// 處理act_out  (需要處理overflow 比大小)

void	OCR_template( int *reorder_label, short *reorder_output , short *source_matching );	// 樣板比對 



void	OCR_free();									// 釋放記憶體


#endif