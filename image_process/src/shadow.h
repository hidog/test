/*--------------------------------------------------------------------------------------------------------------------------------------------------------
	shadow.h
	底層演算法部分
/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef _SHADOW_H
#define _SHADOW_H

#include "share_shadow.h"
#include "andreamatrix.hpp"
#include "andreavector.hpp"
#include <cstring>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    typedef   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef	andrea::matrix<double>			bMatrixDouble;
typedef andrea::vector<double>			bVectorDouble;
typedef	andrea::matrix<int>				bMatrixInt;
typedef andrea::vector<int>				bVectorInt;
typedef andrea::vector<unsigned char>	bVectorByte;
typedef andrea::matrix<unsigned char>	bMatrixByte;

typedef bool (*compareInt)(int, int );												// function
typedef bool (*compareByte)( unsigned char, unsigned char );						// function

// namespace emboss
namespace	emboss{



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    define & global value   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define		SHADOW_VALUE		1		// 陰影值
#define		UN_SHADOW_VALUE		0		// 非陰影值



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    enum   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 凹或凸
enum	BUMP
{
	CONCAVE		=	0,			// 凹
	PROTRUDING,					// 凸
	BUMP_MAX
};

// channel type
enum	CHANNEL
{
	RED		=	0,		// 紅色
	GREEN,
	BLUE,
	GRAY,
	CHANNEL_MAX
};




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    struct   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 凹凸檢測專用的資料
struct	EmbossSettings
{	
	// 也可以利用給座標的方式來畫直線  (依照需求來做調整)
	int				up_line_1;			// 凹凸檢測 開陰影出現區間 上面線 起點     (實際上左右也是用上下來存  就不另外取名叫做左右了)
	int				up_line_2;			//                         上面線 終點
	int				down_line_1;		//                         下面線 起點
	int				down_line_2;		//                         下面線 終點

	bMatrixByte		allow_area;
	bMatrixByte		allow_area2;

	// 預設建構
	EmbossSettings()
	{
		up_line_1		=	0;
		up_line_2		=	0;
		down_line_1		=	0;
		down_line_2		=	0;

		allow_area. resize(0,0);
		allow_area2.resize(0,0);
	}
	// 複製建構
	EmbossSettings( const EmbossSettings &em_setting )
	{
		*this	=	em_setting;
	}
	// operator = 
	void	operator = ( const EmbossSettings &em_setting )
	{
		up_line_1		=	em_setting.up_line_1;
		up_line_2		=	em_setting.up_line_2;
		down_line_1		=	em_setting.down_line_1;
		down_line_2		=	em_setting.down_line_2;
		allow_area		=	em_setting.allow_area;
		allow_area2		=	em_setting.allow_area2;
	}
	// 解構
	~EmbossSettings()
	{}

};

// 絲網檢測專用的資料
struct	ScreenSettings
{
	// 絲網檢測用
	int				th1;				// 兩塊區塊比較的threshold (用相減去跟threshold比較)
	int				th2;	
	bMatrixByte		area_map1;			// 絲網用 將區塊切成兩分
	bMatrixByte		area_map2;			// 斜向用

	ScreenSettings()
	{
		th1				=	0;			
		th2				=	0;	
		area_map1.resize(0,0);		
		area_map2.resize(0,0);			
	}
	ScreenSettings( const ScreenSettings& ss )
	{
		*this	=	ss;
	}
	void	operator = ( const ScreenSettings& ss )
	{
		th1				=	ss.th1;
		th2				=	ss.th2;
		area_map1		=	ss.area_map1;
		area_map2		=	ss.area_map2;
	}

	~ScreenSettings() {}
};


// 陰影設定資料
struct	ShadowSettings
{	
	// 共通設定
	int					x,		y;			// 左上角座標
	int					img_width;			// 圖片大小 用來判斷是2k 4k 8k
	int					img_height;			
	int					size;				// 正方形大小
	int					threshold;			// 陰影深度的thr
	bool				enhance;			// true: 找單陰影	
	EMBOSS_DIRECTION	direction;			// 方向
	EMBOSS_DARK_LIGHT	dark_light;			// 決定亮或暗影	
	BUMP				bump;				// 凹或凸
	CHANNEL				channel;			// 圖片要讀哪個channel  (估計以後emboss也有機會增加)

	// 陰影檢測設定
	EmbossSettings	em_settings;

	// 絲網檢測用
	ScreenSettings	sc_settings;

};


// 陰影回傳資料
class	ShadowReturnData
{
public:
    bVectorByte		mono_sub_seq;			// 單調子序列
	bVectorByte		shadow_seq;				// 陰影位置
	bVectorByte		shadow_depth;			// 深度資訊  (原始資料 - 陰影)	

	// 預設建構
	ShadowReturnData()
	{
		mono_sub_seq. resize(0);
		shadow_seq.   resize(0);
		shadow_depth. resize(0);
	}
	// 指定大小建構
	ShadowReturnData( int n )
	{
		mono_sub_seq. resize(n);
		shadow_seq.   resize(n);
		shadow_depth. resize(n);
	}
	// 解構
	~ShadowReturnData()
	{
	}
	// 複製建構
	ShadowReturnData( const ShadowReturnData &srd )
	{
		*this	=	srd;
	}
	// 運算 =
	void	operator = ( const ShadowReturnData &srd )
	{
		//raw_data		=	srd.raw_data;
		mono_sub_seq	=	srd.mono_sub_seq;
		shadow_seq		=	srd.shadow_seq;
		shadow_depth	=	srd.shadow_depth;
	}


	void	init( int n )
	{
		mono_sub_seq. resize(n);
		shadow_seq.   resize(n);
		shadow_depth. resize(n);
	}
};








//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Shadow  class   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class	Shadow
{
	// note: 為了方便寫成public
	public:
		int		*sub_inx;					// 原本是暫存用記憶體 為了避免memory allocate 改成member value
		int		*link_inx;

		int		size;						// 從setting來的資料
		int		threshold;					// 陰影深度
		bool	enhance;					// 決定要抓單點 還是整段式陰影
		BUMP	bump;						// 決定圖騰是凹或凸

		EMBOSS_DARK_LIGHT	dark_light;		// 決定要抓亮或陰影

		Shadow();		// 建構
		~Shadow();		// 解構

		void			set( int _size, const  ShadowSettings &setting );						// 設定

		// 尋找單調子數列	加速用
		void			sub_mono_seq( bVectorByte &seq, bVectorByte &is_choose );				// 用在Emboss  (還有絲網全方向)

		void			sub_mono_seq_less( bVectorByte &seq, bVectorByte &is_choose );			// 實際上是小於等於 (遞增)  用在Screen
		void			sub_mono_seq_greater( bVectorByte &seq, bVectorByte &is_choose );		// 實際上是大於等於	(遞減)	

		// 陰影偵測
		void			shadow_detector( bVectorByte &seq, ShadowReturnData &return_data );

		// 絲網檢測
		void			screen_detector( bVectorByte &seq, ShadowReturnData &return_data, int th_id );

		// 求出不再sub seq的區間
		void			find_non_sub_seq_interval( int &i1, int &i2, bVectorByte &is_choose );


};




} // end namespace emboss


#endif