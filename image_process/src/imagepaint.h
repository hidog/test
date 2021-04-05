/*
====================================================================================

File: imagepaint.h
Description: This function inherits QGraphicsItem and uses QPainter to draw the
    provided QImage.
Copyright (C) 2010  OpenPIV (http://www.openpiv.net)

Contributors to this code:
Zachary Taylor

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

====================================================================================
*/

#ifndef IMAGEPAINT_H
#define IMAGEPAINT_H

#include <QGraphicsItem>
#include <QGraphicsSvgItem>
//#include <QObject>

//class ImagePaint : public QObject, public QGraphicsItem
class ImagePaint : public QGraphicsItem , public QGraphicsSvgItem
{
    //Q_OBJECT

    private:
        QRect	total;
        QImage	image;
        QRectF	viewRect;
        //QVector<QRgb> colors;

    public:
		ImagePaint();
        ImagePaint(QImage imagePass);
		ImagePaint(QString name);
        virtual ~ImagePaint();

		//void	setGraphicsEffect(QGraphicsEffect *effect);

        void	setImage(QImage imagePass);

        void	paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		void	paint( QPainter *painter );																			// 不帶參數 直接畫圖
        QRectF	boundingRect() const;

		void	hide();

};

#endif // IMAGEPAINT_H
