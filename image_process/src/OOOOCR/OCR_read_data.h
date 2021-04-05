//#pragma once
#ifndef _OCR_READ_DATA_H
#define _OCR_READ_DATA_H


/*
	檔案切成 head model
	head格式
	nr_class
	nr_feature
	O_label (陣列形式)

	例如

	33
	129
	1 2 3 4 5 6 7 8 ...

	model是Omega的資料
*/



#include <stdio.h>
#include "OCR_image_process.h"
#include "OCR_shared.h"


#define		_SVM_FILE_CNY_20_HEAD_		"..\\DATA\\CNY20_head.bin"		// CNY20 head
#define		_SVM_FILE_CNY_20_MODEL_		"..\\DATA\\CNY20_model.bin"		// CNY20 model位置

#define		_SVM_FILE_CNY_100_HEAD_		"..\\DATA\\CNY100_head.bin"		// CNY100 head
#define		_SVM_FILE_CNY_100_MODEL_	"..\\DATA\\CNY100_model.bin"	// CNY100 model位置



void		OCR_read_SVM_model( OCR_INIT_TYPE init_type );					// 讀取 svm 資料

void		OCR_pre_read_SVM_model( OCR_INIT_TYPE init_type );				// 預先讀取 svm 資料

void		OCR_read_SCALE_TABLE();									        // 預先讀取 scale table 資料						

//void		OCR_model_to_binary( OCR_INIT_TYPE init_type );					// 將 model檔案轉成 binary
//void		OCR_test();					

#endif