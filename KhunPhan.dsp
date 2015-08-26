# Microsoft Developer Studio Project File - Name="KhunPhan" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=KhunPhan - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KhunPhan.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KhunPhan.mak" CFG="KhunPhan - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KhunPhan - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "KhunPhan - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KhunPhan - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "HAVE_CONFIG_H" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glut32.lib glu32.lib OpenGL32.lib libxml2.lib libpng13.lib SDL.lib SDL_mixer.lib /nologo /subsystem:windows /machine:I386 /out:"khunphan/KhunPhan.exe"

!ELSEIF  "$(CFG)" == "KhunPhan - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "HAVE_CONFIG_H" /D "PNG_DLL" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib OpenGL32.lib glu32.lib SDL.lib SDL_mixer.lib glut32.lib libxml2.lib libpng13d.lib /nologo /subsystem:windows /debug /machine:I386 /out:"khunphan/KhunPhanDbg.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "KhunPhan - Win32 Release"
# Name "KhunPhan - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\khunphan\bdir.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\bstring.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\btexture.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\bthread.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\bthreadfactory.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\bthreadimp.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\btime.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\bwin32threadimp.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\Kamera.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\KhunPhan.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpboard.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpboardGL.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpglutuserinterface.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpidhash.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpnode.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpprogressbar.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpscore.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpsdluserinterface.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstate.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstateaudiosettings.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatecontrolsettings.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatefactory.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatefinish.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstategame.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstategamesolved.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstategraphichint.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstategraphicsettings.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatekeyboardhelp.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatemeinmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatemovetoken.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatescorelist.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstateselectlanguage.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatesettings.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatestartup.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatetutorial1.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatetutorial2.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatetutorial3.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatetutorial4.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatistics.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpthreadsolvecount.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpuibase.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\light.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\main.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\misc1.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\Schild.cpp
# End Source File
# Begin Source File

SOURCE=.\khunphan\Textfeld.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\khunphan\bdir.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\bmp.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\bstring.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\btexture.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\bthread.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\bthreadfactory.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\bthreadimp.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\btime.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\bwin32threadimp.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\confdefs.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\config.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\createTexture.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\Kamera.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\KhunPhan.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpboard.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpboardGL.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpconfig.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpglutuserinterface.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpidhash.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpmenu.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpnode.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpprogressbar.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpscore.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpsdluserinterface.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstate.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstateaudiosettings.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\KPstateContext.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatecontrolsettings.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatefactory.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatefinish.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstategame.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstategamesolved.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstategraphichint.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstategraphicsettings.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatekeyboardhelp.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatemeinmenu.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatemovetoken.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatescorelist.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstateselectlanguage.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatesettings.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatestartup.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatetutorial1.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatetutorial2.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatetutorial3.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatetutorial4.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpstatistics.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpthreadsolvecount.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\KPtime.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\kpuibase.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\LA.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\light.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\misc1.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\Schild.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\sprache.h
# End Source File
# Begin Source File

SOURCE=.\khunphan\Textfeld.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\khunphan\icon.ico
# End Source File
# Begin Source File

SOURCE=.\khunphan\KhunPhan.rc
# End Source File
# End Group
# Begin Group "Language Files"

# PROP Default_Filter "lang"
# Begin Source File

SOURCE=.\khunphan\locale\900.lang
# End Source File
# Begin Source File

SOURCE=.\khunphan\locale\901.lang
# End Source File
# Begin Source File

SOURCE=.\khunphan\locale\999.lang
# End Source File
# End Group
# End Target
# End Project
