: visual studio½sÄ¶¨Ï¥ÎªºbatÀÉ
if not exist ..\release\ (
	mkdir ..\release\
)
if not exist ..\release\*.dll (
copy ..\3rd\QRencode\lib\release\*.dll ..\release
)
qmake ../qt_test.pro -r -spec win32-msvc
nmake release
