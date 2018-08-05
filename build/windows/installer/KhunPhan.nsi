;--------------------------------
; KhunPhan.nsi
;--------------------------------

!include LogicLib.nsh
!include x64.nsh
!include StrStr.nsh
!include GetParameters.nsh
!include GetParameterValue.nsh
!include FileFunc.nsh
!include SplitFirstStrPart.nsh
!include "MUI2.nsh"
  
!define APPNAME    "KhunPhan"
!define APPVERSION "0.60"
; Refreshing Windows Defines
!define SHCNE_ASSOCCHANGED 0x8000000
!define SHCNF_IDLIST 0
!define ERROR_ALREADY_EXISTS 183
!define ARP "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
!define BASEDIR    "..\..\.."

CRCCheck on
SetDateSave on
SetDatablockOptimize on
BGGradient A52A2A 000000 000000
SetCompressor /SOLID lzma
LicenseBkColor /windows

; file info
VIAddVersionKey ProductName     "${APPNAME}"
VIAddVersionKey LegalCopyright  "(C) 2002-2018 W. Schwotzer"
VIAddVersionKey Comment         "A strategic 3D puzzle game"
VIAddVersionKey ProductVersion  "${APPVERSION}"
VIAddVersionKey FileDescription "A strategic 3D puzzle game"
VIAddVersionKey FileVersion     "${APPVERSION}.0.0"
VIProductVersion "${APPVERSION}.0.0"

; The name of the installer
Name "${APPNAME} ${APPVERSION}"

; The installation file to create
OutFile "..\${APPNAME}-Setup-${APPVERSION}.exe"

; Have Windows Vista, 2008, 7, etc. trust us to not be a "legacy" installer
;RequestExecutionLevel admin

!macro VerifyUserIsAdmin
  UserInfo::GetAccountType
  Pop $0
  ${If} $0 != "admin" ;Require admin rights on NT4+
    MessageBox MB_ICONSTOP "Administrator rights required!"
    SetErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
    Quit
  ${EndIf}
!macroend

Function .onInit

  ; Check if installer is already running. If open a user dialog and abort installation.
  System::Call 'kernel32::CreateMutex(i 0, i 0, t "${APPNAME}_Mutex") ?e'
  Pop $R0
  StrCmp $R0 ${ERROR_ALREADY_EXISTS} 0 init.verifyadmin
  MessageBox MB_OK "${APPNAME} installer is already running."
  Abort
init.verifyadmin:
  ; Depending on admin status install application for all or current user.
  UserInfo::GetAccountType
  Pop $0
  ${If} $0 == "admin"
    SetShellVarContext all
  ${Else}
    SetShellVarContext current
  ${EndIf}
  ; Estimate default installation directory for 32-/64-bit installation
  ; On 64-bit OS a command line parameter can be used to force a
  ; 32-bit installation: 
  ; KhunPhan-Setup-x.yy.exe /ARCH=x86
  ; Result:
  ;   32-bit OS:
  ;      Always: $ARCH == "x86"
  ;   64-bit OS:
  ;      If command line parameter is set: $ARCH == "x86"
  ;      If no command line parameter set: $ARCH == "x64"
  Var /GLOBAL Arch
  StrCpy $Arch "x86"
  ${If} ${RunningX64}
    Push "ARCH"  ; push the parameter name onto the stack
    Push "x64"   ; push the default value for this parameter
    Call GetParameterValue
    Pop $2
	StrCpy $Arch "$2"
    ${If} $Arch == "x86"
      StrCpy $INSTDIR $PROGRAMFILES32\${APPNAME}
    ${Else}
      StrCpy $INSTDIR $PROGRAMFILES64\${APPNAME}
    ${EndIf}
  ${Else}
    StrCpy $INSTDIR $PROGRAMFILES\${APPNAME}
  ${EndIf}

  ; Estimate by Registry access if Application is already installed
  ; If so open a user dialog and evtl. uninstall it or abort.
  ${If} ${RunningX64}
    SetRegView 64
    ReadRegStr $R0 HKLM "${ARP}" "UninstallString"
    IfFileExists "$R0" init.askuninst init.try32bit
init.try32bit:  
    SetRegView 32
  ${Else}
    SetRegView Default
  ${EndIf}
  ReadRegStr $R0 HKLM "${ARP}" "UninstallString"
  IfFileExists "$R0" init.askuninst init.done
init.askuninst:
  MessageBox MB_YESNO|MB_ICONQUESTION "${APPNAME} is already installed. Uninstall the existing version?" IDYES init.uninstall IDNO init.quit
init.quit:
  Quit
init.uninstall:
  ReadRegStr $R1 HKLM "${ARP}" "InstallLocation"
  ClearErrors
  ExecWait '"$R0" /S _?=$R1'
  IfErrors +1 init.done  
  MessageBox MB_OK "Error during uninstallation"
  Quit  
init.done:
; Set registry mode according to architecture to be installed
  ${If} $Arch == "x64"
    SetRegView 64
  ${Else}
    SetRegView Default
  ${EndIf}

FunctionEnd

;-------------------------------
; Pages
!define MUI_COMPONENTSPAGE_SMALLDESC  
!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_LICENSE "${BASEDIR}\COPYING"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
  
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; The install sections
Section "${APPNAME}" BinaryFiles

  SectionIn RO  
  SetOutPath $INSTDIR ; Set output path to the installation directory.
  ; Add files to be extracted to the current $OUTDIR path
${If} $Arch == "x64"
  File /a "${BASEDIR}\x64\Release\khunphan.exe"
  File /a "${BASEDIR}\x64\Release\freeglut.dll"
  File /a "${BASEDIR}\x64\Release\glew32.dll"
  File /a "${BASEDIR}\x64\Release\libogg-0.dll"
  File /a "${BASEDIR}\x64\Release\libpng16.dll"
  File /a "${BASEDIR}\x64\Release\libvorbis-0.dll"
  File /a "${BASEDIR}\x64\Release\libvorbisfile-3.dll"
  File /a "${BASEDIR}\x64\Release\libxml2.dll"
  File /a "${BASEDIR}\x64\Release\SDL2.dll"
  File /a "${BASEDIR}\x64\Release\SDL2_mixer.dll"
${Else}
  File /a "${BASEDIR}\Win32\Release\khunphan.exe"
  File /a "${BASEDIR}\Win32\Release\freeglut.dll"
  File /a "${BASEDIR}\Win32\Release\glew32.dll"
  File /a "${BASEDIR}\Win32\Release\libogg-0.dll"
  File /a "${BASEDIR}\Win32\Release\libpng16.dll"
  File /a "${BASEDIR}\Win32\Release\libvorbis-0.dll"
  File /a "${BASEDIR}\Win32\Release\libvorbisfile-3.dll"
  File /a "${BASEDIR}\Win32\Release\libxml2.dll"
  File /a "${BASEDIR}\Win32\Release\SDL2.dll"
  File /a "${BASEDIR}\Win32\Release\SDL2_mixer.dll"
${EndIf}
  File /a /oname=Changes.txt "${BASEDIR}\ChangeLog"
  File /a /oname=Copying.txt "${BASEDIR}\COPYING"
  File /a /oname=Readme.txt "${BASEDIR}\README"
SectionEnd

Section "Texture, music, sound and locale files" DataFiles

  SectionIn RO
  ; Install Textures
  SetOutPath $INSTDIR\Textures
  File /a "${BASEDIR}\src\Textures\characters.png"
  File /a "${BASEDIR}\src\Textures\logo.png"
  File /a "${BASEDIR}\src\Textures\music_on.png"
  File /a "${BASEDIR}\src\Textures\sound_on.png"
  File /a "${BASEDIR}\src\Textures\soundmusic_off.png"
  SetOutPath $INSTDIR\Textures\bubbles
  File /a "${BASEDIR}\src\Textures\bubbles\board.png"
  File /a "${BASEDIR}\src\Textures\bubbles\texture1.png"
  File /a "${BASEDIR}\src\Textures\bubbles\texture2.png"
  File /a "${BASEDIR}\src\Textures\bubbles\texture3.png"
  SetOutPath $INSTDIR\Textures\marble
  File /a "${BASEDIR}\src\Textures\marble\board.png"
  File /a "${BASEDIR}\src\Textures\marble\texture1.png"
  File /a "${BASEDIR}\src\Textures\marble\texture2.png"
  File /a "${BASEDIR}\src\Textures\marble\texture3.png"
  SetOutPath $INSTDIR\Textures\metal
  File /a "${BASEDIR}\src\Textures\metal\board.png"
  File /a "${BASEDIR}\src\Textures\metal\texture1.png"
  File /a "${BASEDIR}\src\Textures\metal\texture2.png"
  File /a "${BASEDIR}\src\Textures\metal\texture3.png"
  SetOutPath $INSTDIR\Textures\painting
  File /a "${BASEDIR}\src\Textures\painting\board.png"
  File /a "${BASEDIR}\src\Textures\painting\texture1.png"
  File /a "${BASEDIR}\src\Textures\painting\texture2.png"
  File /a "${BASEDIR}\src\Textures\painting\texture3.png"
  SetOutPath $INSTDIR\Textures\rock
  File /a "${BASEDIR}\src\Textures\rock\board.png"
  File /a "${BASEDIR}\src\Textures\rock\texture1.png"
  File /a "${BASEDIR}\src\Textures\rock\texture2.png"
  File /a "${BASEDIR}\src\Textures\rock\texture3.png"
  SetOutPath $INSTDIR\Textures\smily
  File /a "${BASEDIR}\src\Textures\smily\board.png"
  File /a "${BASEDIR}\src\Textures\smily\texture1.png"
  File /a "${BASEDIR}\src\Textures\smily\texture2.png"
  File /a "${BASEDIR}\src\Textures\smily\texture3.png"
  SetOutPath $INSTDIR\Textures\wood
  File /a "${BASEDIR}\src\Textures\wood\board.png"
  File /a "${BASEDIR}\src\Textures\wood\texture1.png"
  File /a "${BASEDIR}\src\Textures\wood\texture2.png"
  File /a "${BASEDIR}\src\Textures\wood\texture3.png"
  ; Install Music
  SetOutPath $INSTDIR\Music
  File /a "${BASEDIR}\src\Music\Cymbals.ogg"
  File /a "${BASEDIR}\src\Music\Earth.ogg"
  File /a "${BASEDIR}\src\Music\Mystery.ogg"
  ; Install Sounds
  SetOutPath $INSTDIR\Sounds
  File /a "${BASEDIR}\src\Sounds\changesetting.ogg"
  File /a "${BASEDIR}\src\Sounds\gamesolved.ogg"
  File /a "${BASEDIR}\src\Sounds\movetoken.ogg"
  File /a "${BASEDIR}\src\Sounds\opengame.ogg"
  File /a "${BASEDIR}\src\Sounds\openmenu.ogg"
  File /a "${BASEDIR}\src\Sounds\quitgame.ogg"
  File /a "${BASEDIR}\src\Sounds\tutorialnext.ogg"
  SetOutPath $INSTDIR\Sounds\bubbles
  File /a "${BASEDIR}\src\Sounds\bubbles\movetoken.ogg"
  SetOutPath $INSTDIR\Sounds\marble
  File /a "${BASEDIR}\src\Sounds\marble\movetoken.ogg"
  SetOutPath $INSTDIR\Sounds\metal
  File /a "${BASEDIR}\src\Sounds\metal\movetoken.ogg"
  SetOutPath $INSTDIR\Sounds\painting
  File /a "${BASEDIR}\src\Sounds\painting\movetoken.ogg"
  SetOutPath $INSTDIR\Sounds\rock
  File /a "${BASEDIR}\src\Sounds\rock\movetoken.ogg"
  SetOutPath $INSTDIR\Sounds\smily
  File /a "${BASEDIR}\src\Sounds\smily\movetoken.ogg"
  SetOutPath $INSTDIR\Sounds\wood
  File /a "${BASEDIR}\src\Sounds\wood\movetoken.ogg"
  ; Install Sounds
  SetOutPath $INSTDIR\locale
  File /a "${BASEDIR}\src\locale\900.lang"
  File /a "${BASEDIR}\src\locale\901.lang"
  File /a "${BASEDIR}\src\locale\999.lang"

SectionEnd

Section "Start Menu Shortcuts" StartMenu

  SectionIn RO
  SetOutPath $INSTDIR ; for working directory
  CreateDirectory "$SMPROGRAMS\${APPNAME}"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk" "$INSTDIR\${APPNAME}.exe" "" "$INSTDIR\${APPNAME}.exe" 0 "" "" "A strategic 3D puzzle game"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall ${APPNAME}.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0 "" "" "Uninstall KhunPahn Installation"  

SectionEnd

Section "Desktop Icon" DesktopIcon

  SetOutPath $INSTDIR ; for working directory
  CreateShortCut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\${APPNAME}.exe" "" "$INSTDIR\${APPNAME}.exe" 0 "" "" "A strategic 3D puzzle game"

SectionEnd

Section "Microsoft Visual C++ Redistributables" VC_Redist

  SetOutPath $TEMP
${If} $Arch == "x64"
  ReadRegDword $R1 HKLM "SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Installed"
  ; Only istall VC_Redist if it is not already installed.
  ; Do not check for version, only for VS2015 (14.0).
  ${If} $R1 != "1"
    File ..\vc_redist.x64.exe  
    ExecWait '"$TEMP\vc_redist.x64.exe" /install /passive /norestart' 
    Delete $TEMP\vc_redist.x64.exe     
  ${EndIf}
${Else}
  ReadRegDword $R1 HKLM "SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x86" "Installed"
  ${If} $R1 != "1"
    File ..\vc_redist.x86.exe  
    ExecWait '"$TEMP\vc_redist.x86.exe" /install /passive /norestart' 
    Delete $TEMP\vc_redist.x86.exe     
  ${EndIf}
${EndIf}

SectionEnd   

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${BinaryFiles} "${APPNAME} binary files."
  !insertmacro MUI_DESCRIPTION_TEXT ${DataFiles} "Texture, music, sound and locale files."
  !insertmacro MUI_DESCRIPTION_TEXT ${StartMenu} "Create start menu."
  !insertmacro MUI_DESCRIPTION_TEXT ${DesktopIcon} "Create desktop icon."
  !insertmacro MUI_DESCRIPTION_TEXT ${VC_Redist} "Install Microsoft Visual C++ Redistributable package 2015 (vc_redist). If unsure install it."
  
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section "-Registry update"

  Var /GLOBAL EstimatedSize
  Var /GLOBAL VersionMajor
  Var /GLOBAL VersionMinor
  Var /GLOBAL DisplayName

  ; Write the uninstall keys for Windows
  ; Calculate estimate size of installation directory
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  StrCpy $EstimatedSize "$0"
  ; Extract major and minor version
  Push "."
  Push "${APPVERSION}"
  Call SplitFirstStrPart
  Pop $VersionMajor
  Pop $VersionMinor
  StrCpy $DisplayName "${APPNAME} ${APPVERSION}"
  ${If} $Arch == "x86"
    ${If} ${RunningX64}
      StrCpy $DisplayName "$DisplayName (32-Bit)"
    ${EndIf}
  ${EndIf}
  
  WriteRegStr   HKLM "${ARP}" "DisplayName"     "$DisplayName"
  WriteRegStr   HKLM "${ARP}" "DisplayVersion"  "${APPVERSION}"
  WriteRegStr   HKLM "${ARP}" "DisplayIcon"     "$INSTDIR\khunphan.exe"
  WriteRegStr   HKLM "${ARP}" "InstallLocation" "$INSTDIR"
  WriteRegStr   HKLM "${ARP}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${ARP}" "Publisher"       "Wolfgang Schwotzer"
  WriteRegDWORD HKLM "${ARP}" "EstimatedSize"   "$EstimatedSize"
  WriteRegDWORD HKLM "${ARP}" "VersionMajor"    "$VersionMajor"
  WriteRegDWORD HKLM "${ARP}" "VersionMinor"    "$VersionMinor"
  WriteRegDWORD HKLM "${ARP}" "NoModify"        1
  WriteRegDWORD HKLM "${ARP}" "NoRepair"        1
  WriteUninstaller "uninstall.exe"  

  System::Call 'Shell32::SHChangeNotify(i ${SHCNE_ASSOCCHANGED}, i ${SHCNF_IDLIST}, p0, p0)'

SectionEnd

;--------------------------------
; Uninstaller

Function un.onInit

  ; Check if any application installed by this installer is running. If so open a user dialog and retry or abort installation.
uninit.checkrun1:
  FindWindow $0 "SDL_app" "${APPNAME} V${APPVERSION}"
  StrCmp $0 0 uninit.notrunning
    MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "${APPNAME} is running. Please close it first" /SD IDRETRY IDRETRY uninit.checkrun1 IDABORT uninit.abort
uninit.notrunning:
  ; Depending on admin status uninstall application for all or current user.
  UserInfo::GetAccountType
  Pop $0
  ${If} $0 == "admin"
    SetShellVarContext all
  ${Else}
    SetShellVarContext current
  ${EndIf}
  ; Estimate by Registry access if this is a 32- or 64-bit installation
  ${If} ${RunningX64}
    SetRegView 64
    ReadRegStr $R0 HKLM "${ARP}" "InstallLocation"
    IfErrors uninit.try32bit uninit.done
uninit.try32bit:  
    SetRegView 32
  ${Else}
    SetRegView Default
  ${EndIf}
  ReadRegStr $R0 HKLM "${ARP}" "InstallLocation"
  IfErrors uninit.dlgabort uninit.done
uninit.dlgabort:
    MessageBox MB_OK "Installation is inconsistent. Aborting Uninstallation."
uninit.abort:
	Abort ; Immediately Quit uninstallation
uninit.done:
  StrCpy $INSTDIR $R0

FunctionEnd

Section "Uninstall" Uninstall
  
  ; Remove registry keys
  DeleteRegKey HKLM "${ARP}"
  
  ; Remove files and uninstaller
  Delete $INSTDIR\locale\*.*
  Delete $INSTDIR\Music\*.*
  Delete $INSTDIR\Sounds\bubbles\*.*
  Delete $INSTDIR\Sounds\marble\*.*
  Delete $INSTDIR\Sounds\metal\*.*
  Delete $INSTDIR\Sounds\painting\*.*
  Delete $INSTDIR\Sounds\rock\*.*
  Delete $INSTDIR\Sounds\smily\*.*
  Delete $INSTDIR\Sounds\wood\*.*
  Delete $INSTDIR\Sounds\*.*
  Delete $INSTDIR\Textures\bubbles\*.*
  Delete $INSTDIR\Textures\marble\*.*
  Delete $INSTDIR\Textures\metal\*.*
  Delete $INSTDIR\Textures\painting\*.*
  Delete $INSTDIR\Textures\rock\*.*
  Delete $INSTDIR\Textures\smily\*.*
  Delete $INSTDIR\Textures\wood\*.*
  Delete $INSTDIR\Textures\*.*
  Delete $INSTDIR\*.*

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${APPNAME}\*.*"
  Delete "$DESKTOP\${APPNAME}.lnk"
  

  ; Remove directories used
  RMDir "$SMPROGRAMS\${APPNAME}"
  RMDir $INSTDIR\locale
  RMDir $INSTDIR\Music
  RMDir $INSTDIR\Sounds\bubbles
  RMDir $INSTDIR\Sounds\marble
  RMDir $INSTDIR\Sounds\metal
  RMDir $INSTDIR\Sounds\painting
  RMDir $INSTDIR\Sounds\rock
  RMDir $INSTDIR\Sounds\smily
  RMDir $INSTDIR\Sounds\wood
  RMDir $INSTDIR\Sounds
  RMDir $INSTDIR\Textures\bubbles
  RMDir $INSTDIR\Textures\marble
  RMDir $INSTDIR\Textures\metal
  RMDir $INSTDIR\Textures\painting
  RMDir $INSTDIR\Textures\rock
  RMDir $INSTDIR\Textures\smily
  RMDir $INSTDIR\Textures\wood
  RMDir $INSTDIR\Textures
  RMDir "$INSTDIR"

  System::Call 'Shell32::SHChangeNotify(i ${SHCNE_ASSOCCHANGED}, i ${SHCNF_IDLIST}, p0, p0)'

SectionEnd

