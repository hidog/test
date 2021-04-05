#include "output.h"



/********************************************************************************************************************
	建構函數
*********************************************************************************************************************/
Output::Output()
{}


/********************************************************************************************************************
	解構函數
*********************************************************************************************************************/
Output::~Output()
{}


/********************************************************************************************************************
	產生輸出到SVG檔案
	給定檔名
*********************************************************************************************************************/
void	Output::outputSVG( QImage imagePass, QString filename = "test.svg" )
{
	//QString path("test.svg");
	//QString		path	=	filedata->filename;
	//path.replace( ".bmp", ".svg" );


	// 將bmp轉svg 範例

//![configure SVG generator]
 
    QSvgGenerator	generator;

    //generator.setFileName(path);
	generator.setFileName( filename );

	generator.setSize( QSize(imagePass.width(), imagePass.height()) );

	generator.setViewBox( QRect(0, 0, imagePass.width(), imagePass.height()) );
    generator.setTitle("SVG Generator Example Drawing");
    generator.setDescription("An SVG drawing created by the SVG Generator "
                                "Example provided with Qt.");
//![configure SVG generator]
//![begin painting]
	//QImage	img	=	imagePass;


    QPainter painter(&generator);

	painter.drawImage( QPoint(0,0), imagePass  );
	//painter.drawEllipse( 100, 100, 20, 50 );


    painter.begin(&generator);
	//displayWidget->paint(&img);
	//displayWidget->paint(painter);
    painter.end();
//![end painting]
}


