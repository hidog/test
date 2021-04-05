#ifndef _OCR_SHARED_H
#define _OCR_SHARED_H



//切割大小
#define	Sub_width		22
#define Sub_height		32

//#define	_OCR_USE_FLOAT_					// 是否使用float的定義


typedef enum	OCR_INIT_TYPE_DEFINE
{
	OCR_CNY_20		=	0,				// 小額
	OCR_CNY_100		=	1,			// 大額

} OCR_INIT_TYPE ;


typedef		unsigned char	BYTE;


short	template_original[ 37 * Sub_width * Sub_height ];



short	nr_class;												// class 個數
short	nr_feature;												// feature 個數

#ifdef _OCR_USE_FLOAT_
float	*Omega;													// 資料 必須在init的時候被初始化
#else
short	*Omega;
#endif

short	*O_label;												// 跟Omega一組的 原本取名 label (改成global,所以名字也改掉)

int		Scale_Table[10001];										// STORE (1/MAX-MIN) TABLE

#ifdef _OCR_USE_FLOAT_
float		Shift_Template_Value[ 37 * 4 * 12 * 16 * 16 ];
float	 	Shift_Template_ASST [ 37 * 4 * 12 * 2 ];
#else
int		Shift_Template_Value[ 37 * 4 * 12 * 16 * 16 ];
int	 	Shift_Template_ASST [ 37 * 4 * 12 * 2 ];
#endif

void	Pre_Load_Shift_Template(int _index, int _block, int _v_shift, int _h_shift);

#endif