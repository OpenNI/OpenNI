;/****************************************************************************
;*                                                                           *
;*  OpenNI 1.0 Alpha                                                         *
;*  Copyright (C) 2010 PrimeSense Ltd.                                       *
;*                                                                           *
;*  This file is part of OpenNI.                                             *
;*                                                                           *
;*  OpenNI is free software: you can redistribute it and/or modify           *
;*  it under the terms of the GNU Lesser General Public License as published *
;*  by the Free Software Foundation, either version 3 of the License, or     *
;*  (at your option) any later version.                                      *
;*                                                                           *
;*  OpenNI is distributed in the hope that it will be useful,                *
;*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
;*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
;*  GNU Lesser General Public License for more details.                      *
;*                                                                           *
;*  You should have received a copy of the GNU Lesser General Public License *
;*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
;*                                                                           *
;****************************************************************************/
;


;SetCompress off
SetCompress auto
SetCompressor ZLIB

!include "MUI.nsh"
!include "x64.nsh"
!include "winmessages.nsh"
!include "EnvVarUpdate.nsh"

;***************
;*** General ***
;***************
!define OPENNI_VER "1.0.0"
!define OPENNI_NAME "OpenNI ${OPENNI_VER} for Windows"
!define env_hklm 'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'

Name "${OPENNI_NAME}"
OutFile "Output\OPENNI-Win32-${OPENNI_VER}.exe"
InstallDir "$PROGRAMFILES\OpenNI\"

;*******************************
;*** face Configuration ***
;*******************************
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "Header.bmp"
!define MUI_ABORTWARNING

;*************
;*** Pages ***
;*************
;!insertmacro MUI_PAGE_LICENSE "License.txt" --- Not needed for now
;!insertmacro MUI_PAGE_COMPONENTS --- Not needed for now since we only have one component!
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;****************
;*** Languages***
;****************
!insertmacro MUI_LANGUAGE "English"

;***************
;*** Install ***
;***************
Section "${OPENNI_NAME}" SecPSSDK

  ; Use the default outout directory
  SetOutPath "$INSTDIR"
  
  ; Copy all the files from the redist dir
  File /r ..\redist\*.*

  ; Create an uninstaller
  WriteUninstaller "$INSTDIR\Uninstall OpenNI ${OPENNI_VER}.exe"

  ; Add the uninstall option into the control panel add/remove
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${OPENNI_NAME}" "DisplayName"\
  "${OPENNI_NAME} (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${OPENNI_NAME}" "UninstallString" \
  "$INSTDIR\Uninstall OpenNI ${OPENNI_VER}.exe"

  ; Create menu shortcuts
  CreateDirectory "$SMPROGRAMS\OpenNI"
  SetOutPath "$INSTDIR\Samples\Bin\Release"
  CreateDirectory "$SMPROGRAMS\OpenNI\Samples"
  CreateShortCut "$SMPROGRAMS\OpenNI\Samples\Samples Directory.lnk" "$INSTDIR\Samples\"
  CreateShortCut "$SMPROGRAMS\OpenNI\Samples\NiSimpleRead.lnk" "$INSTDIR\Samples\Bin\Release\NiSimpleRead.exe"
  CreateShortCut "$SMPROGRAMS\OpenNI\Samples\NiViewer.lnk" "$INSTDIR\Samples\Bin\Release\NiViewer.exe"  
  

  SetOutPath "$INSTDIR"

  CreateDirectory "$SMPROGRAMS\OpenNI\Documentation"
  CreateShortCut "$SMPROGRAMS\OpenNI\Documentation\API Reference ${OPENNI_VER}.lnk" "$INSTDIR\Documentation\OpenNI.chm"
  
  WriteRegStr HKLM "Software\OpenNI" "InstallDir" "$INSTDIR"
  WriteRegStr HKLM "Software\OpenNI" "Version" "${OPENNI_VER}"

  ; Install the visual studio 2008 DLLs
  ExecWait '"$INSTDIR\Tools\vcredist_x86.exe" /q'

  ; Install the USB Driver
  ${If} ${RunningX64} 
    ExecWait '"$INSTDIR\Driver\dpinst-amd64.exe" /sw /el'
  ${Else}
    ExecWait '"$INSTDIR\Driver\dpinst-x86.exe" /sw /el'
  ${EndIf} 

  ; Register the .oni file type
  WriteRegStr HKCR ".oni" "" "NIRecording"
  WriteRegStr HKCR "NIRecording" "" "OpenNI Recording File"
  WriteRegStr HKCR "NIRecording\DefaultIcon" "" "$INSTDIR\Samples\Bin\Release\NiViewer.exe,0"  
  WriteRegStr HKCR "NIRecording\shell\open\command" "" '"$INSTDIR\Samples\Bin\Release\NiViewer.exe" "%1"'  
  
  ; Add Env Var
  WriteRegExpandStr ${env_hklm} OPEN_NI_INSTALL_PATH  $INSTDIR
  WriteRegExpandStr ${env_hklm} OPEN_NI_BIN  $INSTDIR\Bin
  WriteRegExpandStr ${env_hklm} OPEN_NI_INCLUDE  $INSTDIR\Include
  WriteRegExpandStr ${env_hklm} OPEN_NI_LIB  $INSTDIR\Lib
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000 

  ; Also add OPEN_NI_INSTALL_PATH to current environment (current environment is not updated by writing to registry)
  System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("OPEN_NI_INSTALL_PATH", "$INSTDIR").r2'

    ; Add To PATH
  ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\Bin"

  ; Register recorder
  ExecWait '"$INSTDIR\bin\niReg.exe" "$INSTDIR\bin\nimRecorder.dll"'
  ; Register MockNodes
  ExecWait '"$INSTDIR\bin\niReg.exe" "$INSTDIR\bin\nimMockNodes.dll"'
  ; Register Codecs
  ExecWait '"$INSTDIR\bin\niReg.exe" "$INSTDIR\bin\nimCodecs.dll"'
  
  SectionEnd

;********************
;*** Descriptions ***
;********************
;LangString DESC_SecPSSDK ${LANG_ENGLISH} "${OPENNI_NAME}"
;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
;!insertmacro MUI_DESCRIPTION_TEXT ${SecPSSDK} $(DESC_SecPSSDK)
;!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;*****************
;*** Uninstall ***
;*****************
Section "Uninstall"
  ; Unregister recorder
  ExecWait '"$INSTDIR\bin\niReg.exe" -u "$INSTDIR\bin\nimRecorder.dll"'
  ; Unregister MockNodes
  ExecWait '"$INSTDIR\bin\niReg.exe" -u "$INSTDIR\bin\nimMockNodes.dll"'
  
  ; Delete the files
  RMDir /r /REBOOTOK $INSTDIR
  RMDir $INSTDIR

  ; Delete the uninstall option from the control panel add/remove
  DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\${OPENNI_NAME}"

  ; Unregister the .oni file type
  DeleteRegKey HKCR ".oni"
  DeleteRegKey HKCR "NIRecording"  
  
  DeleteRegKey HKLM "Software\OpenNI"

  ; Delete the shortcuts
  RMDir /r "$SMPROGRAMS\OPENNI"
  
  ; Remove the Env Var
  DeleteRegValue ${env_hklm} OPEN_NI_INSTALL_PATH
  DeleteRegValue ${env_hklm} OPEN_NI_BIN
  DeleteRegValue ${env_hklm} OPEN_NI_INCLUDE
  DeleteRegValue ${env_hklm} OPEN_NI_LIB
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000 
  
  ;Remote From Path
  ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR\Bin"
   
SectionEnd