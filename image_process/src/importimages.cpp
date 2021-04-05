/*
====================================================================================

File: importimages.cpp
Description: This class inherits QDialog and the layout is designed in Qt Designer.
    It is the dialog interface used to read images from the selected directory.  Native
    file browsers perform quite slowly with a significant quantity of images in a
    directory.  With TR-PIV systems this can be upwards of 10,000 image pairs.
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

#include "importimages.h"
#include "datacontainer.h"
#include "ui_importimages.h"

#include <QDialog>
#include <QDir>

ImportImages::ImportImages(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    setupConnections();
}

ImportImages::~ImportImages()
{}

void	ImportImages::setWindow(QString directoryPass)
{
    dirString	=	directoryPass;
    if (!dirString.isEmpty())
    {
        dirEdit->setText(dirString);
        QDir qDir	=	QDir(dirString);
        fileList	=	qDir.entryList();
        filter();
        imageListWidget->addItems(fileList);
    }
}

void ImportImages::setData(DataContainer *filedataPass)
{
    filedata = filedataPass;
}

QStringList ImportImages::listA()
{
    return frameA;
}

QStringList ImportImages::listB()
{
    return frameB;
}


/*===========================================================
	跳出來的import image 的視窗
	由這邊連結 import 跟 cancel按鈕
/*===========================================================*/
void ImportImages::setupConnections()
{
    connect( importButton,    SIGNAL(clicked()),                  this, SLOT(importPressed()) );
    connect( cancelButton,    SIGNAL(clicked()),                  this, SLOT(cancelPressed()) );
    connect( imageListWidget, SIGNAL(removeSelected(QList<int>)), this, SLOT(removeFiles(QList<int>)) );
}

void ImportImages::removeFiles (QList<int> selected)
{
    QStringList tempList;
    int intIterator = 0;
    for (int i = 0; i < fileList.count(); i++)
    {
        if (!(selected.at(intIterator) == i)) tempList.append( fileList.value(i) );
        else intIterator++;
    }
    fileList = tempList;
    imageListWidget->clear();
    imageListWidget->addItems( fileList );
}


/*===========================================================================
	設定progress
/*===========================================================================*/
void	ImportImages::setProgress( Progress *pass )
{
	progress	=	pass;
}









void ImportImages::cancelPressed()
{
    this->hide();
}


/*======================================================================
	按下import按鈕後
	這邊會只列出想要看到的副檔名
/*======================================================================*/
void ImportImages::filter()
{
    int			iterations	=	fileList.length();
    QStringList tempList( fileList );

    QString		str;
    fileList.clear();

    for (int i = 0; i < iterations; i++)
    {
        if ( //tempList.value(i).endsWith(".tif",Qt::CaseInsensitive) ||
             //tempList.value(i).endsWith(".png",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".bmp",Qt::CaseInsensitive) 
            //tempList.value(i).endsWith(".jpg",Qt::CaseInsensitive) ||
            //tempList.value(i).endsWith(".jpeg",Qt::CaseInsensitive) ||
            //tempList.value(i).endsWith(".gif",Qt::CaseInsensitive) ||
            //tempList.value(i).endsWith(".pbm",Qt::CaseInsensitive) ||
            //tempList.value(i).endsWith(".pgm",Qt::CaseInsensitive) ||
            //tempList.value(i).endsWith(".ppm",Qt::CaseInsensitive) ||
            //tempList.value(i).endsWith(".xbm",Qt::CaseInsensitive) ||
            //tempList.value(i).endsWith(".xpm",Qt::CaseInsensitive) 
			)
        {
            str		=	dirString;
            str.append("/");
            fileList.append(str.append(tempList.value(i)));
        }
    }
    fileList.sort();
}


/* This is an algorithm that attempts to sort image list into 'A' images and
   'B' images. */
/*======================================================================
	import image 小視窗
	讀取image list後
	透過這邊來處理
/*======================================================================*/
void ImportImages::autoConfig ( QStringList importList )
{
    importList.sort();

    int			i,	rows;
    QString		str;

    rows	= 	importList.size();
	for( i=0; i<rows; i++ )
	{
		str		=	importList.at(i);
		frameA.insert( i, str );
		frameB.insert( i, str );
	}
}



bool ImportImages::onlyNum(QString str)
{
    int i, count;
    count = 0;
    for (i = 0; i < str.size(); i++) if (str.at(i).isLetter()) count++;
    if (count > 0) return false;
    else return true;
}
