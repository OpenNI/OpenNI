@echo off

echo.
echo #########################
echo # Setting up DDK env... #
echo #########################

REM Save the current directory
pushd .

REM Get the root of the DDK from the user
if "%1"=="" goto usage
if "%1"=="-?" goto usage
if "%1"=="/?" goto usage
if "%1"=="-h" goto usage
if "%1"=="/h" goto usage

set BASEDIR=%1
IF "%BASEDIR:~-1%"=="\" SET BASEDIR=%BASEDIR:~0,-1%
shift

REM reasonable defaults
set _FreeBuild=true
set _IA64bit=false
set _AMD64bit=false
set _BscMake=FALSE
set _RunOacr=TRUE
set _VersionedHeaders=TRUE
set _LatestOsTarget=WIN7
set RAZZLETOOLPATH=%BASEDIR%\Bin
set separate_object_root=FALSE
set DDK_TARGET_OS=Win7
set _title=Windows Win7

:NextArg
REM Ways to get script usage
if /I "%1"=="/?"     goto usage
if /I "%1"=="-?"     goto usage
if /I "%1"=="\?"     goto usage
if /I "%1"=="-help"  goto usage
if /I "%1"=="/help?" goto usage

REM Valid target architectures (undecoracted '64' implies IA-64)
if /I "%1"=="64"     goto ArgOK
if /I "%1"=="x64"    goto ArgOK
if /I "%1"=="I386"   goto ArgOK
if /I "%1"=="IA64"   goto ArgOK
if /I "%1"=="IA32"   goto ArgOK
if /I "%1"=="x86"    goto ArgOK
if /I "%1"=="x32-64" goto ArgOK

REM Checked or Free builds
if /I "%1"=="f"       goto ArgOK
if /I "%1"=="fre"     goto ArgOK
if /I "%1"=="free"    goto ArgOK
if /I "%1"=="c"       goto ArgOK
if /I "%1"=="chk"     goto ArgOK
if /I "%1"=="CHK"     goto ArgOK
if /I "%1"=="checked" goto ArgOK

REM Valid target operating system versions
if /I "%1"=="Wlh"  goto ArgOK
if /I "%1"=="WNet" goto ArgOK
if /I "%1"=="WXP"  goto ArgOK
if /I "%1"=="WIN7" goto ArgOK

REM Valid subkit build environments
if /I "%1"=="hal" goto ArgOK

REM Final checks
if /I "%1"=="bscmake" goto ArgOK
if /I "%1"=="hdr_ver" goto ArgOK
if /I "%1"=="no_oacr" goto ArgOK
if /I "%1"=="oacr"    goto ArgOK
if /I "%1"=="separate_object_root" goto ArgOK
if /I "%1"=="objs" goto ArgOK
if NOT "%1"=="" goto usage
if "%1" == ""   goto :GetStarted

:ArgOK
if /I "%1"=="64"      set _IA64bit=true
if /I "%1"=="x64"     set _AMD64bit=true
if /I "%1"=="I386"    set _IA32bit=true
if /I "%1"=="IA64"    set _IA64bit=true
if /I "%1"=="IA32"    set _IA32bit=true
if /I "%1"=="x86"     set _IA32bit=true
if /I "%1"=="x32-64"  set _AMD64bit=true

if /I "%1"=="f"       set _FreeBuild=true
if /I "%1"=="fre"     set _FreeBuild=true
if /I "%1"=="free"    set _FreeBuild=true
if /I "%1"=="c"       set _FreeBuild=false
if /I "%1"=="chk"     set _FreeBuild=false
if /I "%1"=="CHK"     set _FreeBuild=false
if /I "%1"=="checked" set _FreeBuild=false

if /I "%1"=="Wlh"     set DDK_TARGET_OS=WinLH
if /I "%1"=="WNet"    set DDK_TARGET_OS=WinNET
if /I "%1"=="WXP"     set DDK_TARGET_OS=WinXP
if /I "%1"=="win7"    set DDK_TARGET_OS=Win7

if /I "%1"=="bscmake" set _BscMake=TRUE
if /I "%1"=="no_oacr" set _RunOacr=False
if /I "%1"=="oacr"    set _RunOacr=TRUE
if /I "%1"=="hdr_ver" set _VersionedHeaders=TRUE
if /I "%1"=="separate_object_root" set separate_object_root=TRUE
if /I "%1"=="objs" set separate_object_root=TRUE

shift
goto :NextArg

@REM OK, do the real stuff now

:GetStarted
set COFFBASE_TXT_FILE=%BASEDIR%\bin\coffbase.txt

REM Make sure DDK_TARGET_OS is set to the default if not passed at the CMD line
if "%DDK_TARGET_OS%"=="" set DDK_TARGET_OS=WinLH

if /I "%DDK_TARGET_OS%"=="WinLH"    goto WLH
if /I "%DDK_TARGET_OS%"=="WinNET"   goto WNET
if /I "%DDK_TARGET_OS%"=="WinXP"    goto WXP
if /I "%DDK_TARGET_OS%"=="Win7"     goto WIN7
REM Fall thru to WIN7 by default

REM Target OS specific settings
:WIN7
set _ddkspec=win7
set _NT_TARGET_VERSION=0x601
goto END_OF_TARGETS

:WLH
set _title=Windows Vista and Windows Server 2008
set _ddkspec=wlh
set _NT_TARGET_VERSION=0x600
goto END_OF_TARGETS

:WNET
set _title=Windows Server 2003
set _ddkspec=wnet
set _NT_TARGET_VERSION=0x502
goto END_OF_TARGETS

:WXP
set _title=Windows XP
set _ddkspec=wxp
set _NT_TARGET_VERSION=0x501
if "%_AMD64bit%"=="true" @echo Cannot build AMD64 bit binaries for Windows XP.  Defaulting to X86.
set _AMD64bit=
set _IA32bit=true
goto END_OF_TARGETS

:END_OF_TARGETS

REM Target architectures section
if "%PROCESSOR_ARCHITECTURE%"==""      set PROCESSOR_ARCHITECTURE=x86
if "%PROCESSOR_ARCHITECTURE%"=="x86"   set CPU=x86
if "%PROCESSOR_ARCHITECTURE%"=="x86"   set _IA32bit=true
if "%PROCESSOR_ARCHITECTURE%"=="IA64"  set CPU=IA64
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" set CPU=AMD64
if "%CPU%"=="" goto cpuerror

if "%_AMD64bit%" == "true" goto setAMD64
if "%_IA64bit%"  == "true" goto setIA64

set 386=1
set PATH=%BASEDIR%\bin\x86;%BASEDIR%\bin\x86\x86;%BASEDIR%\bin\SelfSign;%PATH%
set BUILD_DEFAULT_TARGETS=-386
set _BUILDARCH=x86
set PATH=%BASEDIR%\tools\pfd\bin\bin\x86;%BASEDIR%\tools\tracing\i386;%PATH%
set _title=%_title% x86
goto envtest

:setAMD64
set _title=%_title% x64
set BUILD_DEFAULT_TARGETS=-amd64
set _BUILDARCH=AMD64
set AMD64=1
set PATH=%BASEDIR%\tools\pfd\bin\bin\AMD64;%BASEDIR%\tools\tracing\amd64;%BASEDIR%\bin\SelfSign;%PATH%
set BUILD_OPTIONS=%BUILD_OPTIONS% ~imca ~toastpkg

if not "%PROCESSOR_ARCHITECTURE%"=="AMD64" goto amd64crosscompile
REM AMD64 Native builds
@ECHO WARNING: x64 Native compiling isn't supported. Using cross compilers.

:amd64crosscompile
REM X86 to AMD64 cross compile
set PATH=%BASEDIR%\bin\x86\amd64;%BASEDIR%\bin\x86;%BASEDIR%\bin\SelfSign;%PATH%
set PATH=%BASEDIR%\tools\pfd\bin\bin\x86_AMD64;%BASEDIR%\bin\SelfSign;%PATH%
goto envtest

:setIA64
set _title=%_title% IA-64
set BUILD_DEFAULT_TARGETS=-ia64
set _BUILDARCH=IA64
set IA64=1

:ia64crosscompile
rem X86 to IA-64 cross compile
set PATH=%BASEDIR%\bin\x86\ia64;%BASEDIR%\bin\x86;%BASEDIR%\tools\tracing\ia64;%BASEDIR%\bin\SelfSign;%PATH%

REM Checked v. Free section
:envtest
if NOT "%DDKBUILDENV%"=="" goto UseExistingDDKBuildSetting
if "%_FreeBuild%" == "true" goto free
goto checked

:UseExistingDDKBuildSetting
if /I "%DDKBUILDENV%"=="FRE"     goto free
if /I "%DDKBUILDENV%"=="FREE"    goto free
if /I "%DDKBUILDENV%"=="CHK"     goto checked
if /I "%DDKBUILDENV%"=="CHECKED" goto checked
REM Clear this to avoid possibly tainting the env. with a bad value
set DDKBUILDENV=
goto usage

:free
REM set up an NT free build environment
set _title=%_title% Free Build Environment
set BUILD_ALT_DIR=fre_%_ddkspec%_%_BUILDARCH%
set NTDBGFILES=1
set NTDEBUG=ntsdnodbg
set NTDEBUGTYPE=windbg
set MSC_OPTIMIZATION=
set DDKBUILDENV=fre
goto done

:checked
REM set up an NT checked build environment
set _title=%_title% Checked Build Environment
set BUILD_ALT_DIR=chk_%_ddkspec%_%_BUILDARCH%
set NTDBGFILES=1
set NTDEBUG=ntsd
set NTDEBUGTYPE=both
set MSC_OPTIMIZATION=/Od /Oi 
set DDKBUILDENV=chk
set DEPRECATE_DDK_FUNCTIONS=1
set BUILD_ALLOW_COMPILER_WARNINGS=1

:done
REM buffer_overflow_checks only on for x86 Server 2003 and all LH BE
if "%DDK_TARGET_OS%" =="WinXP" goto no_bo_checks
set BUFFER_OVERFLOW_CHECKS=1
:no_bo_checks


REM Begin common section
if /I "%separate_object_root%" == "TRUE" set _title=%_title% objs

set LANGUAGE_NEUTRAL=0

set PROJECT_ROOT=%BASEDIR%\src
set PUBLIC_ROOT=%BASEDIR%

set Lib=%BASEDIR%\lib
set Include=%BASEDIR%\inc\api
set WPP_CONFIG_PATH=%BASEDIR%\bin\wppconfig

set NTMAKEENV=%BASEDIR%\bin
set BUILD_MAKE_PROGRAM=nmake.exe
set BUILD_DEFAULT=-ei -nmake -i -nosqm
set BUILD_MULTIPROCESSOR=1
set NO_BINPLACE=TRUE
if /I "%_BscMake%" NEQ "TRUE" set NO_BROWSER_FILE=TRUE
set PUBLISH_CMD=@echo Publish not available...

if "%tmp%"=="" set tmp=%TEMP%

set NEW_CRTS=1
set USE_OBJECT_ROOT=1

set CRT_INC_PATH=%BASEDIR%\inc\crt
set ATL_INC_PATH=%BASEDIR%\inc
set ATL_INC_ROOT=%BASEDIR%\inc
set MFC_INC_PATH=%BASEDIR%\inc\mfc42
set SDK_INC_PATH=%BASEDIR%\inc\api
set DDK_INC_PATH=%BASEDIR%\inc\ddk
set WDM_INC_PATH=%BASEDIR%\inc\ddk
set OAK_INC_PATH=%BASEDIR%\inc\api
set IFSKIT_INC_PATH=%BASEDIR%\inc\ddk
set HALKIT_INC_PATH=%BASEDIR%\inc\ddk
set DRIVER_INC_PATH=%BASEDIR%\inc\ddk
set SDK_LIB_DEST=%BASEDIR%\lib\%_ddkspec%
set DDK_LIB_DEST=%SDK_LIB_DEST%
set IFSKIT_LIB_DEST=%SDK_LIB_DEST%

set SDK_LIB_PATH=%BASEDIR%\lib\%_ddkspec%\*
set DDK_LIB_PATH=%SDK_LIB_PATH%
set HALKIT_LIB_PATH=%SDK_LIB_PATH%
set IFSKIT_LIB_PATH=%SDK_LIB_PATH%

set CRT_LIB_PATH=%BASEDIR%\lib\crt\*
set ATL_LIB_PATH=%BASEDIR%\lib\atl\*
set MFC_LIB_PATH=%BASEDIR%\lib\mfc\*

REM Add OACR settings to x86 and x64 builds
if /I "%_RunOacr%" NEQ "true" goto OACRInitEnd

REM Do not run OACR for IA64 build environments
if /I "%_IA64bit%"=="true" echo OACR is not supported for IA64
if /I "%_IA64bit%"=="true" goto OACRInitEnd

:OACRInit32bit
if /I "%_IA32bit%" NEQ "true" goto OACRInit64bit
set SUBSTITUTE_386_CC=%BASEDIR%\Bin\x86\oacr\oacrcl
set SUBSTITUTE_AMD64_CC=%BASEDIR%\Bin\x86\oacr\oacrcl
set SUBSTITUTE_LINK=%BASEDIR%\Bin\x86\oacr\oacrlink
set SUBSTITUTE_CS_COMPILER_NAME=%BASEDIR%\BIN\x86\oacr\oacrcsc /nologo
set PATH=%BASEDIR%\bin\x86\oacr;%PATH%
set BUILD_OACR=1

If Not Exist %BASEDIR%\OACR\%BUILD_ALT_DIR% MD %BASEDIR%\OACR\%BUILD_ALT_DIR%

REM Turn on checking to workaround incorrect definition of __in
set PREFAST_VALID_IMPLIES_NOTNULL=1

if Exist %BASEDIR%\bin\x86\oacr\oacr.bat call %BASEDIR%\bin\x86\oacr\oacr.bat monitor
goto OACRInitEnd

:OACRInit64bit
if "%_AMD64bit%" NEQ "true" goto OACRInitEnd
set SUBSTITUTE_AMD64_CC=%BASEDIR%\Bin\amd64\oacr\oacrcl
set SUBSTITUTE_CS_COMPILER_NAME=%BASEDIR%\Bin\amd64\oacr\oacrcsc /nologo
set SUBSTITUTE_LINK=%BASEDIR%\Bin\amd64\oacr\oacrlink

set PATH=%BASEDIR%\bin\amd64\oacr;%PATH%
set BUILD_OACR=1

If Not Exist %BASEDIR%\OACR\%BUILD_ALT_DIR% MD %BASEDIR%\OACR\%BUILD_ALT_DIR%

REM Turn on checking to workaround incorrect definition of __in
set PREFAST_VALID_IMPLIES_NOTNULL=1

if Exist %BASEDIR%\bin\amd64\oacr\oacr.bat call %BASEDIR%\bin\amd64\oacr\oacr.bat monitor

:OACRInitEnd

REM Call WDF environment batch file.
if exist %BASEDIR%\bin\setwdf.bat call %BASEDIR%\bin\setwdf.bat

REM Static Driver Verifier (SDV)
if defined SDV goto :SDV_DEFINED
set SDV=%BASEDIR%\tools\sdv
set PATH=%SDV%\bin;%PATH%
goto :SDV_CONTINUE
:SDV_DEFINED
echo WARNING: Using external version of Static Driver Verifier.
:SDV_CONTINUE

REM Don't require coffbase.txt entries
set LINK_LIB_IGNORE=4198

REM Don't do font mapping by default.
set RCNOFONTMAP=1

if "%_IA64bit%"  == "true" Goto SkipToolSettings
if "%_AMD64bit%" == "true" Goto SkipToolSettings

:SkipToolSettings
set USERNAME=WinDDK

title %_title%
if "%OS%" == "Windows_NT" goto WinNT
if not "%OS%" == "Windows_NT" goto Win9x
goto end

:WinNT
cd /d %BASEDIR%

doskey /macrofile=%BASEDIR%\bin\generic.mac

REM Create dirs if needed...
if Not Exist %BASEDIR%\dirs %BASEDIR%\Bin\x86\Makedirs.exe %BASEDIR%

goto end

:AlreadyDefinedError
echo.
echo ERROR: This window already has the DDK build environment set.  Please open 
echo        a new window if you want to change or reset your build environment.
echo.

goto end

:cpuerror
echo.
echo Error: PROCESSOR_ARCHITECTURE environment variable not recognized.
echo.
echo.

goto end

:Win9x
@echo ERROR: The Windows DDK no longer supports Windows 95/98/Me as a development
@echo        platform.
goto end

:usage
echo.
echo Usage: "ddkcompile <directory> [fre|chk] [64|x64] [WIN7|WLH|WXP|WNET] [bscmake] [no_oacr] [separate_object_root]"
echo.
echo   By default, ddkcompile.bat will set the environment variable NO_BROWSER_FILE.
echo   Using the "bscmake" option will cause ddkcompile.bat to not define this variable.
echo.
echo   Example:  ddkcompile d:\ddk chk        set checked environment
echo   Example:  ddkcompile d:\ddk            set free environment for Windows Win7 (default)
echo   Example:  ddkcompile d:\ddk fre WIN7   set free environment for Windows Win7 (default)
echo   Example:  ddkcompile d:\ddk fre 64     sets IA-64 bit free environment
echo   Example:  ddkcompile d:\ddk fre x64    sets x64 bit free environment
echo   Example:  ddkcompile d:\ddk fre x32-64 sets x64 bit free environment
echo   Example:  ddkcompile d:\ddk fre WXP    sets free build environment for Windows XP
echo.
echo.

:end
if "%CPU%"=="AMD64" set PATH=%BASEDIR%\bin\amd64;%PATH%
set _FreeBuild=
set _IA64bit=
set _ddkspec=
set _title=
set _BscMake=
set _VersionedHeaders=
set _LatestOsTarget=
set _NTDRIVE=%~d0
set _NTROOT=%~p0
set _NTROOT=%_NTROOT:~0,-1%
set _BuildType=%DDKBUILDENV%

REM Create separate object root
if /I "%separate_object_root%" == "FALSE" goto exit

set _NTTREE=%BASEDIR%.binaries\%DDK_TARGET_OS%\%_BUILDARCH%%DDKBUILDENV%
set OBJECT_ROOT=%BASEDIR%.obj

set BINPLACE_EXCLUDE_FILE=%BASEDIR%\bin\symbad.txt
set BINPLACE_LOG=%_NTTREE%\build_logs\binplace.log
set BINPLACE_PDB_DLL=mspdb80.dll

set NO_BINPLACE=

set NTDBGFILES=1
set NTDBGFILES_PRIVATE=1

if Not Exist %_NTTREE% md %_NTTREE%
if Not Exist %_NTTREE%\build_logs md %_NTTREE%\build_logs
if Not Exist %OBJECT_ROOT% md %OBJECT_ROOT%

:exit

REM Go back to the previoulsy saved directory
popd

REM Compile the code...
echo.
echo ################
echo # Compiling... #
echo ################
echo.
echo Target is: %_BUILDARCH%!
echo.

build /ceZ

REM The end...
exit /b 0
