@echo off

@echo ********************************
@echo *** SDK Windows Driver Maker ***
@echo ********************************
@echo.

@echo *******************************************
@echo *** Recreating the old Bin directory... ***
@echo *******************************************
del /F /Q ..\BinVSDbg\*.*
del /F /Q ..\BinVSDbg\x86\*.*
del /F /Q ..\BinVSDbg\amd64\*.*

@echo.
@echo ***************************
@echo *** Coying SYS files... ***
@echo ***************************
copy sys\x86\debug\psdrv3.sys ..\BinVSDbg\x86
if "%ERRORLEVEL%" == "1" goto ErrorCopy

copy sys\amd64\debug\psdrv3.sys ..\BinVSDbg\amd64
if "%ERRORLEVEL%" == "1" goto ErrorCopy

@echo.
@echo **************************
@echo *** Coying INF file... ***
@echo **************************
copy inf\psdrv3.inf ..\BinVSDbg
if "%ERRORLEVEL%" == "1" goto ErrorCopy

@echo.
@echo *****************************
@echo *** Coying WDF redists... ***
@echo *****************************
copy wdf\x86\*.* ..\BinVSDbg\x86
if "%ERRORLEVEL%" == "1" goto ErrorCopy

copy wdf\amd64\*.* ..\BinVSDbg\amd64
if "%ERRORLEVEL%" == "1" goto ErrorCopy

@echo.
@echo **********************
@echo *** Coying PDBs... ***
@echo **********************
attrib /S -r ..\PDBs\*.pdb
xcopy /E /I /Y sys\ps*.pdb ..\PDBs

@echo.
@echo ************************************
@echo *** Signing the amd64 version... ***
@echo ************************************
set PATH=%DDKPATH%\bin\x86;%PATH%
IF EXIST ..\sign\myprivatekey.pvk signtool sign /v /ac ..\Sign\MSCV-VSClass3.cer /s My /n "Prime Sense Ltd."  /t http://timestamp.verisign.com/scripts/timstamp.dll /d "PrimeSense Sensor Device Driver v3.0 - %DATE% %TIME% on %COMPUTERNAME%" /du http://www.primesense.com/ ..\BinVSDbg\amd64\psdrv3.sys
IF EXIST ..\sign\myprivatekey.pvk signtool verify /v /kp ..\BinVSDbg\amd64\psdrv3.sys
if "%ERRORLEVEL%" == "1" goto ErrorSign

@echo.
@echo ********************************************
@echo *** Finished creating the Bin directory! ***
@echo ********************************************

goto End

:ErrorCopy
@echo.
@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
@echo !!! An error occured while copying a file !!!
@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
pause
exit /b 1
goto END

:ErrorSign
@echo.
@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
@echo !!! An error occured while verifying the digital signature !!!
@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
pause
exit /b 1
goto END

:End

