: visual studio½sÄ¶¨Ï¥ÎªºbatÀÉ
if not exist ..\debug\ (
	mkdir ..\debug\
)
: if not exist ..\debug\*.dll (
:	copy ..\lib\win32\libcurl\lib\*.dll 		..\debug
: )
qmake ../qt_test.pro -r -spec win32-msvc2019
nmake debug
