: visual studio�sĶ�ϥΪ�bat��
if not exist ..\debug\ (
	mkdir ..\debug\
)
if not exist ..\debug\*.dll (
copy ..\3rd\QRencode\lib\debug\*.dll ..\debug
)
qmake ../qt_test.pro -r -spec win32-msvc
nmake debug
