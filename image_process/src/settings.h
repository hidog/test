#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QColor>
#include <QSize>
#include <QList>
#include <QPoint>
#include <QRect>

#include "progress.h"

namespace OpenPIV
{

	// 陰影
	const int	up			=	0;
	const int	down		=	1;
	const int	left		=	2;
	const int	right		=	3;
	const int	left_up		=	4;
	const int	left_down	=	5;
	const int	right_up	=	6;
	const int	right_down	=	7;

	// Processors
	const int SubPixBlob	=	0;
	const int Watershed		=	1;
	const int FFT			=	2;
	const int Steer			=	3;
	const int Edge			=	4;
	const int Corner		=	5;
	const int Channel		=	6;
	const int ColorCode		=	7;

    // Detectors
    const int GaussianSubPixel = 0;

    // Output
    const int Text = 0;
    const int HDF5 = 1;

	// watershed
	const int i4_8	=	0;
	const int i4_1	=	1;
	const int MSER	=	2;
};

class Settings : public QObject
{
#ifndef _GTEST_
    Q_OBJECT
#endif
    protected:
        void changeDeltaX();
        void changeDeltaY();

    private:
        // Display
        QSize _imageSize;

        // Masking
        bool _isMask;
        QRect _roi;
        bool roiSet;

        // Processing
        int _deltaX, _deltaY;
        int xSpin, ySpin;
        int _intLengthX, _intLengthY;
        int _processor;
        int _detector;

        // Batch processing
        bool _batchFilter;
        bool _batchMask;
        bool _batchShowImage;
        bool _batchShowVectors;
        bool _batchThreading;

        // Vectors
        QColor _vectorColourFiltered;
        QColor _vectorColourUnfiltered;
        double _vectorScale;
        double _vectorSub;


        // Output
        QString _outputFolder;
        QString _expName;
        int _outputFormat;

		// Blob detector功能
		int		scale_w,	scale_l;		// 尺度設定

		// measure 使用的
		int		measure_x,	measure_y;		// x, y 座標
		QRgb	rgb;						// 顏色資訊

		// 負責回報進度用的
		Progress	*progress;


    public:
        Settings( Progress *_progress );
        virtual ~Settings();

        // Processing
        int deltaX();       
        int deltaY();
        int intLengthX();
        int intLengthY();
        int processor();
        int detector();
        QSize imageSize();
        void setImageSize(QSize imageSizePass);
        bool isMask();
        QRect roi();

        // Batch processing
        bool batchFilter();
        bool batchMask();
        bool batchShowImage();
        bool batchShowVectors();
        bool batchThreading();

        // Vectors
        QColor vectorColourFiltered();
        QColor vectorColourUnfiltered();
        double vectorScale();
        double vectorSub();


        // Output
        QString expName();
        QString outputFolder();
        int outputFormat();

		// sub pixel blob detector用的
		int			get_scale_w();
		int			get_scale_l();

		// measure用的
		int			get_measure_X();
		int			get_measure_Y();

		// 設定progress
		Progress*	getProgress();

		// Watershed用的
		bool		isHighlight;			// 是否要反白
		int			watershed;				// 決定要用watershed的哪一個方法

		// 其他
		void		setRGB( QRgb _rgb );	// 設定顏色
		QRgb		getRGB();				// 回傳顏色

		// 陰影檢測用
		int			shadow_dir;				// 陰影方向
		int			square_size;			// 正方形大小
		bool		set_square;				// 是否進入設定正方形的階段
		int			shadow_depth_threshold;	// 深度的threshold
		bool		inhence_mode;			// 判斷是否為inhencemode  決定要抓單點陰影 還是整段式陰影 應付noise很強的狀況
		int			file_size;				// 檔案個數
		bool		save_check;				// 是否列印結果(image)到檔案
		bool		batch_check;			// 是否批次作業
		int			dark_light;				// 抓亮影或者陰影
		int			id;						// 檔案 id

#ifndef _GTEST_
    public slots:
#endif
        
        // Processing
        void setDeltaX(int deltaXPass);
        void setDeltaY(int deltaYPass);
        void setIntLengthX(int intLengthXPass);
        void setIntLengthY(int intLengthYPass);
        void setProcessor(int processPass);
        void setDetector(int detectPass);
        void setIsMask(bool isMaskPass);
        void setRoi(QRect roiPass);

        // Batch processing
        void setBatchFilter(bool batchFilterPass);
        void setBatchMask(bool batchMaskPass);
        void setBatchShowImage(bool batchShowImagePass);
        void setBatchShowVectors(bool batchShowVectorsPass);
        void setBatchThreading(bool batchThreadingPass);

        // Vectors
        void setVectorColourFiltered(QColor vectorColourPass);
        void setVectorColourUnfiltered(QColor vectorColourPass);
        void setVectorScale(double vectorScalePass);
        void setVectorSub(double vectorSubPass);


        // Output
        void setExpName(QString expNamePass);
        void setOutputFolder(QString outputFolderPass);
        void setOutputFormat(int outputFormatPass);

		// blob detector
		void	set_scale( int w, int l );
		void	set_scale_w( int w );
		void	set_scale_l( int l );

		// measure使用的
		void	set_measure_X( int x );
		void	set_measure_Y( int y );

		// 陰影使用
		void	setInhence( int state );		// 設定inhence mode
		void	setSaved( int state );			// 設定是否要輸出結果到檔案 (image部分)
		void	setBatch( int state );			// 設定是否為批次作業



#ifndef _GTEST_
    signals:
        void imageSizeChanged();				// Display
        void processSettingsChanged();			// Processing
        void vectorSettingChanged();			// Vectors
#endif


};

#endif // SETTINGS_H
