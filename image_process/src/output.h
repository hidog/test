/**************************************************************************************************************************

	跟output有關的放在這邊

/***************************************************************************************************************************/

#ifndef OUTPUT_H
#define OUTPUT_H

#include <QImage>
#include <QPainter>
#include <QSvgGenerator>
#include <QSvgRenderer>


class	Output
{
	private:


	public:
		Output();
		~Output();

		void	outputSVG( QImage imagePass, QString filename );				// 產生輸出到SVG檔案

};




#endif // OUTPUT_H
