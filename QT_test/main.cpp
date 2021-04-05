#include "ui/mainwindow.h"

//http://doc.qt.io/qt-4.8/qsplashscreen.html#QSplashScreen
//QSplashScreen 

#include <QApplication>
#include <QMetaType>
#include <QFileInfo>

#include "src/def.h"

#include <QDir>
#include <QDebug>
#include <QtGui> 
#include <QObject>   
#include <QString>
#include <QTextCodec>

// 621

// 當初寫來做轉換檔名 還需要測試跟修改


void f( QFileInfo &info )
{
	QDir dir(info.absoluteFilePath() );
	
	QFileInfoList list = dir.entryInfoList();
	for( int i = 0; i < list.size(); i++ )
	{
		int index = list[i].fileName().indexOf(" - ");
		//qDebug() << i << ", " << index << ", " << list[i].absoluteFilePath().toStdString().c_str();

		QString str1 = list[i].fileName().left( index );
		QString str2 = list[i].fileName().mid( index + 3 );
		qDebug() << str1 << " , " << str2;

		QDir new_folder( "D:\\anime_collection\\tmp\\" );
		new_folder.mkdir(str1);
		QFile file( list[i].absoluteFilePath() );
		QString new_path = QString( "D:\\anime_collection\\tmp\\%1\\%2").arg(str1).arg(str2);
		qDebug() << new_path;
		file.rename( new_path );
	}

}


int		main(int argc, char *argv[])
{
	QString path = QObject::tr("D:\\anime_collection\\tmp\\");
	QDir dir( path );
	QFileInfoList list = dir.entryInfoList();

	for( int i = 0; i < list.size(); i++ )
	{
		qDebug() << i << ", " << list[i].absoluteFilePath().toStdString().c_str();
	}

	QDir sub_dir( list[2].absoluteDir() );
	qDebug() << sub_dir;
	sub_dir.removeRecursively();

	//f( list[7] );

	system("PAUSE");
	return 0;
	/*qRegisterMetaType<DbProj>("DbProj");
	qRegisterMetaType<DbProj>("DbProj&");

    qRegisterMetaType<QVector<int> >("QVector<int>");
	qRegisterMetaType<QList<QFileInfo> >("QList<QFileInfo>");
	qRegisterMetaType<QList<FileStatus> >("QList<FileStatus>");

    QApplication	a(argc, argv);
    MainWindow		w;
    w.show();

    return a.exec();*/
}
