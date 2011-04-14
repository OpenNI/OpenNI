@echo off

@echo ********************************
@echo *** SDK Windows Driver Maker ***
@echo ********************************
@echo.

@echo *******************************************
@echo *** Recreating the old Bin directory... ***
@echo *******************************************
del /F /Q ..\Bin\*.*
del /F /Q ..\Bin\x86\*.*
del /F /Q ..\Bin\amd64\*.*

@echo.
@echo ***********************************
@echo *** Building the x86 version... ***
@echo ***********************************
call ddkcompile.bat %DDKPATH% fre x86 Win7
copy sys\objfre_win7_x86\i386\psdrv3.sys  ..\Bin\x86
if "%ERRORLEVEL%" == "1" goto ErrorBuild

@echo.
@echo ***********************************
@echo *** Signing the x86 version... ***
@echo ***********************************
IF EXIST ..\sign\myprivatekey.pvk signtool sign /v /ac ..\Sign\MSCV-VSClass3.cer /s My /n "Prime Sense Ltd."  /t http://timestamp.verisign.com/scripts/timstamp.dll /d "PrimeSense Sensor Device Driver v3.0 - %DATE% %TIME% on %COMPUTERNAME%" /du http://www.primesense.com/ ..\Bin\x86\psdrv3.sys
IF EXIST ..\sign\myprivatekey.pvk signtool verify /v /kp ..\Bin\x86\psdrv3.sys
if "%ERRORLEVEL%" == "1" goto ErrorSign

@echo.
@echo *************************************
@echo *** Building the amd64 version... ***
@echo *************************************
call ddkcompile.bat %DDKPATH% fre x64 WIN7
copy sys\objfre_win7_amd64\amd64\psdrv3.sys  ..\Bin\amd64
if "%ERRORLEVEL%" == "1" goto ErrorBuild

@echo.
@echo ************************************
@echo *** Signing the amd64 version... ***
@echo ************************************
IF EXIST ..\sign\myprivatekey.pvk signtool sign /v /ac ..\Sign\MSCV-VSClass3.cer /s My /n "Prime Sense Ltd."  /t http://timestamp.verisign.com/scripts/timstamp.dll /d "PrimeSense Sensor Device Driver v3.0 - %DATE% %TIME% on %COMPUTERNAME%" /du http://www.primesense.com/ ..\Bin\amd64\psdrv3.sys
IF EXIST ..\sign\myprivatekey.pvk signtool verify /v /kp ..\Bin\amd64\psdrv3.sys
if "%ERRORLEVEL%" == "1" goto ErrorSign

@echo.
@echo **************************
@echo *** Coying INF file... ***
@echo **************************
copy inf\psdrv3.inf ..\Bin
if "%ERRORLEVEL%" == "1" goto ErrorCopy

@echo.
@echo *****************************
@echo *** Coying WDF redists... ***
@echo *****************************
copy wdf\x86\*.* ..\Bin\x86
if "%ERRORLEVEL%" == "1" goto ErrorCopy

copy wdf\amd64\*.* ..\Bin\amd64
if "%ERRORLEVEL%" == "1" goto ErrorCopy

@echo.
@echo *****************************
@echo *** Making the cat file.. ***
@echo *****************************
inf2cat /driver:..\Bin /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64,2000,Server2003_X86,Server2003_X64,Server2008_X86,Server2008_X64,Server2008R2_X64
if "%ERRORLEVEL%" == "1" goto ErrorCat

@echo.
@echo ******************************
@echo *** Signing the cat file.. ***
@echo ******************************
IF EXIST ..\sign\myprivatekey.pvk signtool sign /v /ac ..\Sign\MSCV-VSClass3.cer /s My /n "Prime Sense Ltd."  /t http://timestamp.verisign.com/scripts/timstamp.dll /d "PrimeSense Sensor Device Driver v3.0 - %DATE% %TIME% on %COMPUTERNAME%" /du http://www.primesense.com/ ..\Bin\psdrv3.cat
IF EXIST ..\sign\myprivatekey.pvk signtool verify /v /kp ..\Bin\psdrv3.cat
if "%ERRORLEVEL%" == "1" goto ErrorSign

@echo.
@echo ************************
@echo *** Coying DPInst... ***
@echo ************************
copy dpinst\*.* ..\Bin
if "%ERRORLEVEL%" == "1" goto ErrorCopy

@echo.
@echo **********************
@echo *** Coying PDBs... ***
@echo **********************
attrib /S -r ..\PDBs\*.pdb
xcopy /E /I /Y sys\ps*.pdb ..\PDBs

@echo.
@echo ********************************************
@echo *** Finished creating the Bin directory! ***
@echo ********************************************

goto End

:ErrorBuild
@echo.
@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
@echo !!! An error occured while building the driver !!!
@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
pause
exit /b 1
goto END

:ErrorCat
@echo.
@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
@echo !!! An error occured while building the cat file !!!
@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
pause
exit /b 1
goto END

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

