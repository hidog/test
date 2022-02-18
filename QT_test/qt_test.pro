win32{ message("this is win32 test...") }
win64{ message("this is win64 test...") }
macos{ message("this is macos test...") }
unix:!macos{ message("this is unix test...") }

# message( "PWD = $$PWD" )


QT  	+= core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



# QTCreator, Unix環境下, 沒特別設置,IDE的Debug, Release build, assert均有作用. Profile build, assert無作用. 看起來要最佳優化要選Profile
# UNIX, 終端機模式下正常.
CONFIG += c++11 warn_on
TARGET	= qt_run
# 測試改名, 所以執行檔用qt_run



# use for command line version.
# unix的時候沒成功,正確講法是行為很怪,偶而成功,偶而跳終端機畫面但會當機
# macos也失敗
win32:Debug {
    QT -= gui
    CONFIG += console
    CONFIG -= app_bundle
}
TEMPLATE = app



win32:Debug:DESTDIR		=	../debug/
win32:Release:DESTDIR	=	../release/
unix:!macos:DESTDIR     =   ./bin/
macos:DESTDIR           =   ./bin/


win32 {
	DEFINES	+=	_CRT_SECURE_NO_WARNINGS \
				_SCL_SECURE_NO_WARNINGS \
				_CONSOLE \
				WIN32
}

Debug:DEFINES += _DEBUG \
	             DEBUG 

Release:DEFINES += NDEBUG 

unix:!macos {
	DEFINES 	+=	UNIX 
}

macos {
	DEFINES 	+=	MACOS
}

DEFINES		+=	QT_TEST



INCLUDEPATH +=	./src \
                ./qt \
				./3rd/QRencode/include
				
win32 {
	# INCLUDEPATH	+= ..\src
}

#INCLUDEPATH   +=  /Users/hidog/code/boost



# 看文件過去應該支援這個方式搜尋 .so, 但目前測試失敗.
# unix:!macos:DEPENDPATH += -L/home/hidog/code/test/QT_test/3rd/QRencode/lib/linux/



# 這邊必須使用 .. 
win32:Debug:LIBS    +=  -L../3rd/QRencode/lib/debug -lqrencoded
win32:Release:LIBS  +=  -L../3rd/QRencode/lib/release -lqrencode
# 用cmd環境下可以用相對路徑, 用QTCreator的時候必須設定絕對路徑.
#unix:!macos:LIBS    +=  -L./3rd/QRencode/lib/linux/ -lqrencode
unix:!macos:LIBS    +=  -L/home/hidog/code/test/QT_test/3rd/QRencode/lib/linux/ -lqrencode
macos:LIBS			+=	-L/Users/hidog/code/test/QT_test/3rd/QRencode/lib/xcode/ -lqrencode
# LIBS +=



# 設定runtime, so的路徑.
unix:!macos:QMAKE_RPATHDIR += /home/hidog/code/test/QT_test/3rd/QRencode/lib/linux/
# macos底下有效, 也可以手動建立 Frameworks 資料夾並且把dylib複製過去
# QT_run.app/Contents/Frameworks
macos:QMAKE_RPATHDIR += /Users/hidog/code/test/QT_test/3rd/QRencode/lib/xcode/



# 這兩個作用不確定,有空研究
# QMAKE_LFLAGS += 
# QMAKE_LFLAGS_RPATH = 
 



# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



SOURCES += ./src/main.cpp \
           ./qt/mainwindow.cpp

HEADERS += ./qt/mainwindow.h

FORMS += ./qt/mainwindow.ui



TRANSLATIONS += qt_test_zh_TW.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target



# RESOURCES +=	resource/title_bar.qrc \
# 				resource/mainwindow.qrc

# OTHER_FILES += icon.rc
# RC_FILE += icon.rc		
# program icon.  mac不知道是否通用. 抽空研究.  目前聽說要用不同的方式   see http://qt-project.org/doc/qt-4.8/appicon.html   http://213style.blogspot.tw/2013/03/qt-icon-change.html


