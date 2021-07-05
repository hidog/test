win32{ message("this is win32 test...") }
win64{ message("this is win64 test...") }
macos{ message("this is macos test...") }
unix:!macos{ message("this is unix test...") }



QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



CONFIG += c++11
TARGET	= qt_test



# use for command line version.
win32:Debug {
	QT -= gui
	CONFIG += console
	CONFIG -= app_bundle
}
TEMPLATE = app



win32:Debug:DESTDIR		=	../debug/
win32:Release:DESTDIR	=	../release/



win32 {
	DEFINES	+=	_CRT_SECURE_NO_WARNINGS \
				_SCL_SECURE_NO_WARNINGS \
				_CONSOLE \
				WIN32
	Debug:DEFINES += _DEBUG \
	                 DEBUG 
}

win32:Release:DEFINES += NDEBUG 

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



# 這邊必須使用 .. 
# 其他範例 win32:Debug:LIBS +=	-L../3rd/QRencode/lib/debug -lqrencoded
# win32:Release:LIBS += -L../3rd/QRencode/lib/release -lqrencoded
win32:Debug:LIBS +=	-L../3rd/QRencode/lib/debug

win32:Release:LIBS += -L../3rd/QRencode/lib/release -lqrencode

# LIBS += -L"3rdparty/CatWhisperer/lib" -lCatWhisperer



# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



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


