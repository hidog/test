: visual studio½sÄ¶¨Ï¥ÎªºbatÀÉ
if not exist ..\release\ (
	mkdir ..\release\
)
:if not exist ..\release\*.dll (
:	copy ..\lib\win32\libcurl\lib\*.dll 			..\release
:)
qmake ../qt_test.pro -r -spec win32-msvc2019
nmake release
