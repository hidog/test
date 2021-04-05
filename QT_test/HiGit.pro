#-------------------------------------------------
#
# Project created by QtCreator 2015-10-31T22:03:11
#
#-------------------------------------------------

message("HiGit...")

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32 {
CONFIG += console
CONFIG -= app_bundle
}

TARGET		=	HiGit
TEMPLATE	=	app

INCLUDEPATH 	+=	. ui src

win32 {
	message("win32")
	Release:DESTDIR	=	../release
	Debug:DESTDIR	=	../debug
}



SOURCES		+=	main.cpp 
    


