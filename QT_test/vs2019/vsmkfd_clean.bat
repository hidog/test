: visual studio½sÄ¶¨Ï¥ÎªºbatÀÉ

if exist Makefile.Debug (
nmake debug-clean 
)

if exist .qmake.stash (
del .qmake.stash
del lrelease_wrapper.bat
del Makefile
del Makefile.Debug
del Makefile.Release
del uic_wrapper.bat
rmdir /S /Q .qm
rmdir /S /Q debug
rmdir /S /Q release
)