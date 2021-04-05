#ifndef _EMBOSS_H
#define _EMBOSS_H
   
//#define EMBOSS_SN	"$Id: 7fc304dc580783af82bcd4281e67a8d22e921c92 $"
   
#ifdef _DEBUG
#pragma comment(lib,"embossd.lib")
#elif NDEBUG
#pragma comment(lib,"emboss.lib")
#else
#error no _DEBUG or NDBUG
#endif

//test    
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	抽取色彩通道
	處理色彩等等的功能
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <cmath>
#include <cassert>

#include "shadow.h"
#include "andreaimage.h"

#include <vector>


namespace	emboss{


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// PIXEL物件  這邊只有x, y座標
struct	PIXEL
{
	int		x;
	int		y;

	PIXEL() : x(0), y(0) {}
	PIXEL( int _x, int _y ) : x(_x), y(_y) {}
};

// 負責處理結果 (以region為單位  小的叫做block)
struct	RREmboss
{
	int		size;			// 記錄這個Region有幾個block
	int		devia;			// 陰影質心偏差
	int		shadow_number;	// 陰影位置的偏差數量
	int		number;			// 整個區塊廢品區數量
	bool	center_out;		// 判斷是否要參考質心偏差

	int		anchor_x;		// region的左上角
	int		anchor_y;
	int		region_w;		// region的寬高
	int		region_h;
};

// 同上 絲網部分
struct	RRScreen
{
	int			size;
	EMBOSS_DIRECTION	direction;	// 整個區塊正常陰影方向
	int			ng_count;			// 整個區塊廢品區數量
	//int		shadow_count;		// 廢品區塊陰影數量

	int			anchor_x;			// region的左上角
	int			anchor_y;
	int			region_w;			// region的寬高
	int			region_h;
};


// 凹凸用的回傳結果資料
struct	EmbossData
{
	// 斜向 正向 共通資料
	double				centroid_x;						// 陰影的質心位置 (一整塊下去算)
	double				centroid_y;
	double				centroid_x2;					// 斜向使用
	double				centroid_y2;
	int					shadow_size;					// 陰影個數
	int					out_shadow;						// 不在允許區的陰影個數
	int					up_line_1,		up_line_2;		// 畫檢測區用 上面的線 起點終點   (左右共用這個資料)   這邊使用的是pixel座標位置
	int					down_line_1,	down_line_2;	// 畫檢測區用 下面的線 起點終點
	bool				is_center_out;					// 質心是否落在允許區內

	bMatrixByte			allow_area;						// 容許區 ( = 1  才可以允許陰影出現     = 0 不允許陰影出現
	bMatrixByte			allow_area2;					// 容許區2 ( = 1  才可以允許陰影出現     = 0 不允許陰影出現   斜向使用

	// 運算 = 
	void	operator = ( const EmbossData &em )
	{
		centroid_x		=	em.centroid_x;	
		centroid_y		=	em.centroid_y;
		centroid_x2		=	em.centroid_x2;	
		centroid_y2		=	em.centroid_y2;
		shadow_size		=	em.shadow_size;
		out_shadow		=	em.out_shadow;
		up_line_1		=	em.up_line_1;
		up_line_2		=	em.up_line_2;
		down_line_1		=	em.down_line_1;
		down_line_2		=	em.down_line_2;
		allow_area		=	em.allow_area;
		allow_area2		=	em.allow_area2;
		is_center_out	=	em.is_center_out;
	}
	// 預設建構函數
	EmbossData()
	{
		centroid_x		=	0;	
		centroid_y		=	0;
		centroid_x2		=	0;
		centroid_y2		=	0;
		shadow_size		=	0;
		out_shadow		=	0;
		up_line_1		=	0;
		up_line_2		=	0;
		down_line_1		=	1;
		down_line_2		=	1;
		is_center_out	=	false;

		allow_area. resize(0,0);
		allow_area2.resize(0,0);
	}
	// 丟入setting的建構子
	EmbossData( const EmbossSettings settings )
	{
		shadow_size		=	0;
		out_shadow		=	0;
		up_line_1		=	settings.up_line_1;
		up_line_2		=	settings.up_line_2;
		down_line_1		=	settings.down_line_1;
		down_line_2		=	settings.down_line_2;

		allow_area		=	settings.allow_area;
		allow_area2		=	settings.allow_area2;
	}
	// 複製建構函數
	EmbossData( const EmbossData &em )
	{
		*this	=	em;
	}
	// 解構函數
	~EmbossData(){}	
};	// end EmbossData


// 絲網檢測回傳結果資料
struct	ScreenData
{
	/*
		中線點的起點跟終點
		這邊的切法是  上下 上 下  都切左右塊    (方向一致)
		              左右 左 右  都切上下塊
					  斜向還在思考中
	*/
	int					area1,	area2;				// 將檢測區切成兩塊 分別計算各自的陰影數
	int					th1,	th2;				// 用來判斷方向的 threshold
	EMBOSS_DIRECTION			direct;						// 檢測結果 (方向)
	int					value;						// 檢測結果 (值)
	bMatrixByte			area_map1;					// 標中線後  分成兩塊區塊  給值 當成mask
	bMatrixByte			area_map2;					// 標中線後  分成兩塊區塊  給值 當成mask  斜向使用

	// 運算子 = 
	void 	operator = ( const ScreenData &sc )
	{
		area1			=	sc.area1;
		area2			=	sc.area2;
		th1				=	sc.th1;
		th2				=	sc.th2;
		direct			=	sc.direct;
		value			=	sc.value;
		area_map1		=	sc.area_map1;
		area_map2		=	sc.area_map2;
	}
	// 用setting來設定的運算子
	ScreenData( const ScreenSettings settings )
	{
		th1				=	settings.th1;
		th2				=	settings.th2;
		direct			=	EMBOSS_DIRECTION_MAX;
		value			=	0;
		area_map1		=	settings.area_map1;
		area_map2		=	settings.area_map2;
	}
	// 複製建構子
	ScreenData( const ScreenData &sc )
	{
		*this	=	sc;
	}
	// 預設建構函數
	ScreenData()
	{
		area1			=	0;
		area2			=	0;
		th1				=	0;
		th2				=	0;
		direct			=	EMBOSS_DIRECTION_MAX;
		value			=	0;
		area_map1.resize(0,0);
		area_map2.resize(0,0);
	}
	// 解構函數
	~ScreenData(){}
};	// end ScreenData




// square物件   主要用在紀錄紀錄每一個正方形的檢測區
struct	SquareData
{
	CHANNEL				channel;						// 決定要讀哪個channel的資料

	int					anchor_x,	anchor_y;			// 定位座標 (依據方向而定)
	int					anchor_x2,	anchor_y2;			// 斜向需要兩組資料
	int					img_width,	img_height;			// 圖片長 寬  用來判斷相機 2k 4k 8k

	// 斜向 正向 共通資料
	int					size;							// 紀錄資料大小 (正方形sizse)
	EMBOSS_DIRECTION	direction;						// 方向
	EMBOSS_DARK_LIGHT	dark_light;						// 亮影或陰影
	BUMP				bump;							// 圖案是凹或凸
	bool				enhance;						// 單陰影或多陰影
	int					x,	y;							// 左上角的座標
	int					threshold;						// 陰影深度的thr

	// 凹凸檢測的資料
	EmbossData			em_data;

	// 絲網檢測使用的資料
	ScreenData			sc_data;

	// operator = (複製)
	void	operator = ( const SquareData &sd )
	{
		channel		=	sd.channel;
		size	=	sd.size;
		anchor_x	=	sd.anchor_x;
		anchor_y	=	sd.anchor_y;
		anchor_x2	=	sd.anchor_x2;
		anchor_y2	=	sd.anchor_y2;
		img_width	=	sd.img_width;
		img_height	=	sd.img_height;
		direction	=	sd.direction;
		dark_light	=	sd.dark_light;
		bump		=	sd.bump;
		enhance		=	sd.enhance;
		x			=	sd.x;
		y			=	sd.y;
		threshold	=	sd.threshold;
		em_data		=	sd.em_data;
		sc_data		=	sd.sc_data;

	}

	// 建構函數
	SquareData()
	{
		channel		=	CHANNEL_MAX;

		anchor_x	=	0;
		anchor_y	=	0;
		anchor_x2	=	0;
		anchor_y2	=	0;
		img_width	=	0;
		img_height	=	0;

		size		=	0;
		direction	=	EMBOSS_DIRECTION_MAX;
		dark_light	=	EMBOSS_DARK_LIGHT_MAX;
		bump		=	BUMP_MAX;
		enhance		=	false;
		x			=	0;
		y			=	0;
		threshold	=	0;
		em_data		=	EmbossData();
		sc_data		=	ScreenData();
	}
	// 建構函數 by settings  (進入前設定)
	SquareData( const ShadowSettings &settings )
	{
		channel		=	settings.channel;
		img_width	=	settings.img_width;
		img_height	=	settings.img_height;

		x			=	settings.x;
		y			=	settings.y;
		size		=	settings.size;
		threshold	=	settings.threshold;
		enhance		=	settings.enhance;
		direction	=	settings.direction;
		dark_light	=	settings.dark_light;
		bump		=	settings.bump;

		em_data	=	EmbossData( settings.em_settings );
		sc_data	=	ScreenData( settings.sc_settings );

	}
	// 建構函數 (複製)
	SquareData( const SquareData &sd )
	{
		*this		=	sd;
	}
	// 解構
	~SquareData()
	{
		size		=	0;
		x			=	0;
		y			=	0;
	}

};			// end   struct  SquareData






//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ typedef ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef		std::vector<SquareData>		EmbossVector;




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// KernelDensity 物件  這邊用來過濾雜訊
class	KernelDensity
{
	private:
		bMatrixDouble	kernel;														// smooth 的 kernel

	public:
		KernelDensity();															// 建構函數

		void			dosomething( bMatrixDouble &p_data, bMatrixByte &data );	// 執行
		void			kernel_square( int n );										// 產生基本的方塊狀 kernel
		int				element_count(bMatrixByte &data);							// 元素個數計算

};





// Emboss物件
class	Emboss	
{
	public:
		AndreaImage::AndreaChannel		channel_func;								// channel function,指向AndreaImage image底下的function.   雙向Screen用這個來讀取資料

		EMBOSS_GOOD_BAD					good_bad;									// 判斷該block是正品還是廢品
		Shadow							shadow_obj;									// 陰影檢測物件

		EMBOSS_DIRECTION				direction;									// 整個emboss物件指定方向
		int								size;										// 指定大小
		int								x_left,	x_right,							// 檢測區的定位點
										y_up,	y_down;		
		int								roi_x,	roi_y;								// 定位點
		int								anchor_x,									// 錨點   會在 set_square_bound 內做設定
										anchor_y,
										anchor_x2,
										anchor_y2;
		PIXEL							start,		end;							// 起終點座標 (屬於暫存用的)

		AndreaImage						image;
		SquareData						roi;										// 檢測區資料 預先要好資料
		KernelDensity					kd;											// 用來過濾雜訊
		double							kd_th;										// kernel density的threshold.  決定怎樣的情況要過濾掉.

		// 底下原本是function內的區域變數 20130304 搬到class member (為了加速)
		ShadowReturnData				*line_data;									// 本來放在ROI底下 一次一條的資料
		ShadowReturnData				*line_data2;
		bVectorByte						*seq_line;									// 掃一條1D的資料
		bVectorByte						*seq_line2;									// 斜向使用
		bVectorByte						seq_extend;									// 會做補值的動作
		const static int				extend_len	=	30;							// 往前延伸pixel寬度
		int								original_len;								// 原始陣列長度
		ShadowReturnData				shadow_data_tmp;							// 陰影資料  (一個是原始的 一個是刪除延伸資料拿來回傳用的的)
		ShadowSettings					shadow_settings;							// 陰影設定 屬於唯獨資料
		bMatrixDouble					bm_tmp;										// kernel density使用

		// 全方向使用
		bVectorByte						*seq_line_up;								// 全方向使用的seq_line
		bVectorByte						*seq_line_down;
		bVectorByte						*seq_line_left;
		bVectorByte						*seq_line_right;
		ShadowReturnData				*line_data_up;								// 全方向使用的line_data
		ShadowReturnData				*line_data_down;
		ShadowReturnData				*line_data_left;
		ShadowReturnData				*line_data_right;
		bMatrixByte						up_map;										// 暫存 最後用來merge
		bMatrixByte						down_map;				
		bMatrixByte						left_map;
		bMatrixByte						right_map;

		// 最後結果的部分 原本放在 SquareData roi底下.
		bVectorByte						edge;										// 存放edge位置
		bVectorByte						edge2;										// 斜向
		bMatrixByte						shadow;										// 最後陰影的結果 用byte存
		bMatrixByte						shadow2;									// 斜向
		bMatrixByte						deep;										// 深度 (求質心使用)
		bMatrixByte						deep2;										// 斜向
		bMatrixByte						allow_area;									// 容許區資料 (全方向沒這東西)
		bMatrixByte						allow_area2;

	public:
		Emboss();																								// 建構
		virtual					~Emboss();																		// 解構

		//~~~~~~~~~~~~~~~~~ 預先設定 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void					set( SquareData _roi );															// 設定 roi 資料 (預先產生好記憶體空間避免re-allocate)
		void					generate_allow_area();															// 產生mask資料
		void					set_image( AndreaImage imagePass );												// 設定圖片

		//~~~~~~~~~~~~~~~~~ 批次使用 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~						
		void					batch_shadow_line( int th_id );													// 一次處理一條線
		void					batch_shadow_line_2( int th_id );												// 一次處理一條線 斜向使用
		void					batch_shadow_ALLD( int th_id );													// 一次處理一條線 全方向使用
		void					batch_loadimage();																// 批次執行讀圖
		void					batch_linedata2matrix();														// 批次轉換資料
		void					batch_kernel_density();															// 批次濾除雜訊
		void					batch_roi_centroid();															// 批次求質心
		void					batch_result_output_emboss_defect( AndreaImage img, const int value );			// 批次標記defect map

		//~~~~~~~~~~~~~~~~~ 外部介面 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~														
		EMBOSS_DIRECTION		get_direction();																// 取得方向
		void					exec();																			// 相當於原本的execution  multi-thread時使用

		//~~~~~~~~~~~~~~~~~ 內部細節 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void					loadimage( bVectorByte &seq );													// 讀取圖片資料  (加速用)
		bool					is_vertical_horizontal( const EMBOSS_DIRECTION direction );						// 判斷是否為垂直水平方向(上下左右) 
		bool					is_single_direction( const EMBOSS_DIRECTION direction );						// 判斷是否為 單 方向
		bool					is_bin_direction( const EMBOSS_DIRECTION direction );							// 判斷是否為 雙 方向
		void					set_square_bound();																// 設定檢測區邊界範圍  定位點
		void					get_edge();																		// 計算邊

		//~~~~~~~~~~~~~~~~~ 結果輸出 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void					result_output_emboss           ( AndreaImage img );								// 凹凸結果輸出  (用shadow資料)
		void					result_output_emboss_allow_area( AndreaImage img );								// 畫出容許區
		void					result_output_shadow           ( AndreaImage img );								// 畫出陰影
		void					result_output_emboss_centroid  ( AndreaImage img );								// 畫出質心
		void					result_output_edge( AndreaImage img );											// 畫出edge
		void					emboss_result( SquareData &square );											// 計算陰影的結果 (正或廢)
		int						emboss_result_out_area();														// 回傳落在允許區外的陰影個數
		bool					emboss_result_out_center();														// 判斷質心是否落在允許區外 
		int						all_dir_result();																// 陰影容許禁區 判斷
		//~~~~~~~~~~~~~~~~~ 其他 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
};


// Screen物件  絲網 雙向專用.
class	Screen	
{
	public:
		AndreaImage::AndreaChannel		channel_func;					// channel function,指向AndreaImage image底下的function.   雙向Screen用這個來讀取資料

		EMBOSS_GOOD_BAD					good_bad;						// 判斷該block是正品還是廢品
		Shadow							shadow_obj;						// 陰影檢測物件

		AndreaImage						image;
		SquareData						roi;							// 檢測區資料 

		EMBOSS_DIRECTION				direction;						// 整個emboss物件指定方向
		EMBOSS_DARK_LIGHT				dark_light;						// 亮暗影
		BUMP							bump;							// 圖騰凹或凸
		int								size;							// 指定大小
		int								x_left,	x_right,				// 檢測區的邊界點
										y_up,	y_down;		
		int								roi_x,	roi_y;					// 定位點
		int								anchor_x,						// 錨點   會在 set_square_bound 內做設定
										anchor_y,
										anchor_x2,
										anchor_y2;
		PIXEL							start,		end;				// 起終點座標 (屬於暫存用的)


		// 原本應該是暫存變數 為了加速 改成member value
		const static int				extend_len	=	30;				// 往前延伸pixel寬度
		int								original_len;					// 原始陣列長度
		ShadowSettings					shadow_settings;				// 絲網設定資料
		ShadowReturnData				*line_data;						// 合併後的資料 (將來回方向合併)
		ShadowReturnData				*line_data2;					// 斜向使用
		ShadowReturnData				line_front;						// 過去方向 
		ShadowReturnData				line_back;						// 回來方向
		bVectorByte						*seq_line_front;				// 一條1D的資料  (過去)
		bVectorByte						*seq_line_front2;				// 斜向
		bVectorByte						*seq_line_back;					// 回來
		bVectorByte						*seq_line_back2;				// 斜向
		bVectorByte						seq_extend;						// 將原始資料補值後丟入下一層
		ShadowReturnData				shadow_data_tmp;				// 包含補值資料 由下一層回傳的資料.

		// 全方向使用
		bVectorByte						*seq_line_up;
		bVectorByte						*seq_line_down;
		bVectorByte						*seq_line_left;
		bVectorByte						*seq_line_right;
		ShadowReturnData				*line_data_up;
		ShadowReturnData				*line_data_down;
		ShadowReturnData				*line_data_left;
		ShadowReturnData				*line_data_right;
		bMatrixByte						up_map;		
		bMatrixByte						down_map;
		bMatrixByte						left_map;
		bMatrixByte						right_map;

		// 結果使用  原本放在 SquareData roi底下
		bMatrixByte						shadow;							// 陰影資料
		bMatrixByte						shadow2;						// 斜向使用
		bMatrixByte						area_map1;						// 區分成兩個區塊 
		bMatrixByte						area_map2;		

	public:

		Screen();																								// 建構
		virtual					~Screen();																		// 解構

		//~~~~~~~~~~~~~~~~~ 預先設定 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void					set( SquareData _roi );															// 設定 roi 資料 (預先產生好記憶體空間避免re-allocate)
		void					generate_center_line_map();														// 產生預設的center map (絲網使用)
		bool					determine_bump( AndreaImage imagePass );										// 決定該區塊的圖騰是凹還是凸 Page2會直接使用
		int						determine_screen_value();														// 回傳 絲網檢測 的 thr
		void					set_image( AndreaImage imagePass );												// 設定圖片

		//~~~~~~~~~~~~~~~~~ 外部介面 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void					batch_shadow_line( int th_id );													// 一次處理一條線
		void					batch_shadow_line_2( int th_id );												// 一次處理一條線 斜向使用
		void					batch_loadimage();																// 批次執行讀圖
		void					batch_linedata2matrix();														// 批次轉換資料
		EMBOSS_DIRECTION		get_direction();																// 取得方向
		void					exec();																			// 相當於原本的execution  multi-thread時使用  絲網雙向使用
		void					batch_shadow_ALLD( int th_id );							
		void					batch_result_output_screen_defect( AndreaImage img, const int value );			// 批次畫defect map

		//~~~~~~~~~~~~~~~~~ 內部細節 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		int						all_dir_result();
		bVectorInt				loadimage( PIXEL s, PIXEL e, const EMBOSS_DIRECTION direction );											// 讀取圖片資料
		void					loadimage( bVectorByte &seq_front, bVectorByte &seq_back );													// 讀取圖片資料 (雙向加速用)
		void					loadimage( bVectorByte &seq );
		ShadowReturnData		shadow_line( PIXEL start, PIXEL end, EMBOSS_DIRECTION direct, ShadowSettings shadow_data );					// 一次檢查一條線
		bool					is_vertical_horizontal( const EMBOSS_DIRECTION direction );													// 判斷是否為垂直水平方向(上下左右)   或者斜向(左上左下右上右下)
		bool					is_single_direction( const EMBOSS_DIRECTION direction );													// 判斷是否為 單 方向
		bool					is_bin_direction( const EMBOSS_DIRECTION direction );														// 判斷是否為 雙 方向
		void					set_square_bound();																							// 設定檢測區邊界範圍  定位點

		//~~~~~~~~~~~~~~~~~ 結果輸出 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void					result_output_shadow            ( AndreaImage img );														// 畫出陰影
		void					result_output_screen		    ( AndreaImage img );														// 絲網印出結果
		void					result_output_screen_area		( AndreaImage img );														// 將兩個區塊標記在圖片上
		void					screen_result_v_d( SquareData &square );																	// 絲網檢測 結果  判斷value & direction  目前沒有拆成兩個function
		void					screen_result( SquareData &square );																		// 絲網檢測 結果  

		//~~~~~~~~~~~~~~~~~ 其他 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		bMatrixByte				screen_center_line_map( PIXEL anchor, int size, EMBOSS_DIRECTION direct, PIXEL start, PIXEL end );			// 絲網 將座標 轉成 map
		bMatrixByte				screen_center_line_map2( PIXEL anchor, int size, EMBOSS_DIRECTION direct, PIXEL start, PIXEL end );			// 絲網 將座標 轉成 map 斜向使用
};





// 負責用openMP控制multi-thread的物件
class	Emboss_MT
{
	// note: 為了方便  就不做private了
	public:
		const static int	MAX_THR		=	7;					// 最大thread個數

		int			size;
		Emboss		*em;

		Emboss_MT();											// 建構
		~Emboss_MT();											// 解構

		void	init( int _size );								// 初始化
		void	destroy();										// 類似解構 將資料刪除.
		void	exec( AndreaImage image );						// 執行 (在這邊進行openMP)
		void	exec_page2( AndreaImage image );
		void	defect( AndreaImage img, int value );			// 標記defect map, 使用openMP
};



// 負責用openMP控制multi-thread的物件
class	Screen_MT
{
	// note: 為了方便  就不做private了
	public:
		const static int	MAX_THR		=	7;					// 最大thread個數

		int			size;
		Screen		*sc;

		Screen_MT();											// 建構
		~Screen_MT();											// 解構

		void	init( int _size );								// 初始化
		void	destroy();										// 類似解構 將資料刪除
		void	exec( AndreaImage image );						// 執行 (在這邊進行openMP)
		void	exec_page2( AndreaImage image );				// page2 使用 
		void	defect( AndreaImage img, int value );			// 標記defect map 使用openMP
};






}		// end namespace emboss




#endif