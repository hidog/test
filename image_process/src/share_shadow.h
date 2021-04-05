#ifndef _SHARE_SHADOW_H
#define _SHARE_SHADOW_H


/*=========================================================================================================
	將AOI程式會用到的enum, define放在這邊.
/*=========================================================================================================*/

#define		EMBOSS_CENTER_WEIGHT	20000			// 質心偏移 AOI畫面上會出現的權重值  定20000是因為window最大85*85 考慮斜向 *2


// 支援浮點數的point
struct	FPOINT
{
	double	x;
	double	y;
};



// 方向
enum	EMBOSS_DIRECTION
{
	EMBOSS_UP				=	0,
	EMBOSS_DOWN				=	1,
	EMBOSS_LEFT				=	2,
	EMBOSS_RIGHT			=	3,
	EMBOSS_LEFT_UP			=	4,
	EMBOSS_LEFT_DOWN		=	5,
	EMBOSS_RIGHT_UP			=	6,
	EMBOSS_RIGHT_DOWN		=	7,
	EMBOSS_ALL_DIRECTION	=	8,			// 全方向
	EMBOSS_UP_DOWN			=	9,			// 同時做上下 取交集
	EMBOSS_LEFT_RIGHT		=	10,			// 同時做左右 取交集
	EMBOSS_LU_RD			=	11,			// 左上 到 右下
	EMBOSS_LD_RU			=	12,			// 左下 到 右上
	EMBOSS_MIDDLE			=	13,			// 中間
	EMBOSS_DIRECTION_MAX	=	14
};

// 陰影或亮影
enum	EMBOSS_DARK_LIGHT
{
	EMBOSS_DARK				=	0,
	EMBOSS_LIGHT			=	1,
	EMBOSS_DARK_LIGHT_MAX	=	2
};

// 該block是正品還是廢品
enum	EMBOSS_GOOD_BAD
{
	EMBOSS_BAD			=	0,		// 廢品
	EMBOSS_GOOD			=	1,		// 該block是正品
};


#endif