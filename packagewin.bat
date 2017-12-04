@echo off

echo Packaging...

IF "%1"=="" GOTO HAVE_0

rd /q /s windows
del /q bs2mgr-win.zip
md windows
%1\windeployqt.exe build-bs2mgr-Desktop_Qt_5_9_3_MinGW_32bit-Release\release\bs2mgr.exe --dir windows --force
copy build-bs2mgr-Desktop_Qt_5_9_3_MinGW_32bit-Release\release\bs2mgr.exe windows
copy "%1\libstdc++-6.dll" windows
copy %1\libwinpthread-1.dll windows
copy %1\libgcc_s_dw2-1.dll windows


cd windows
zip -r ..\bs2mgr-win.zip * -x *.git*
cd..

echo Packaging done

exit /b

:HAVE_0
echo Specify the path to qt. Example: %0 C:\Qt\5.8\mingw53_32\bin
