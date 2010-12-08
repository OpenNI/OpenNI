@echo off
IF NOT EXIST "C:\Program Files\NSIS\makensisw.exe" GOTO TRY64

ECHO "Building on x86 (32bit)..."
"C:\Program Files\NSIS\makensisw.exe" "OpenNI.nsi"
GOTO END

:TRY64
ECHO "Building on x64 (64bit)..."
"C:\Program Files (x86)\NSIS\makensisw.exe" "OpenNI.nsi"

:END