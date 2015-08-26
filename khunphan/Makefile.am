####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = khunphan

## INCLUDES were found outside kdevelop specific part

khunphan_SOURCES = bdir.cpp kpsdluserinterface.cpp kpglutuserinterface.cpp kpuibase.cpp btime.cpp kpstatescorelist.cpp kpscore.cpp btexture.cpp kpstatistics.cpp kpprogressbar.cpp kpthreadsolvecount.cpp blinuxthreadimp.cpp bthreadimp.cpp bthreadfactory.cpp bthread.cpp kpstatemovetoken.cpp kpstatetutorial4.cpp kpstatetutorial3.cpp kpstatetutorial2.cpp kpstatetutorial1.cpp kpstategamesolved.cpp kpstateselectlanguage.cpp kpstategame.cpp kpstatekeyboardhelp.cpp kpstateaudiosettings.cpp kpstategraphichint.cpp kpstategraphicsettings.cpp kpstatecontrolsettings.cpp kpstatesettings.cpp kpstatefinish.cpp kpstatemeinmenu.cpp kpstatefactory.cpp kpstatestartup.cpp kpmenu.cpp kpstate.cpp light.cpp main.cpp kpconfig.cpp kpboardGL.cpp Kamera.cpp Textfeld.cpp Schild.cpp kpidhash.cpp kpnode.cpp misc1.cpp kpboard.cpp KhunPhan.cpp bstring.cpp 
khunphan_LDADD   = -lpthread -lglut -lGLU -lGL -lxml2 -lpng

SUBDIRS = docs Textures Music Sounds locale 

EXTRA_DIST = config.h bstring.cpp bstring.h KhunPhan.cpp KhunPhan.h kpboard.cpp kpboard.h misc1.cpp misc1.h kpnode.h kpnode.cpp kpidhash.h kpidhash.cpp Schild.cpp Schild.h sprache.h Textfeld.cpp Textfeld.h Kamera.cpp Kamera.h kpboardGL.h kpboardGL.cpp kpconfig.cpp kpconfig.h main.cpp light.cpp light.h KPstateContext.h kpstate.cpp kpstate.h kpmenu.cpp kpmenu.h kpstatestartup.cpp kpstatestartup.h kpstatefactory.cpp kpstatefactory.h kpstatemeinmenu.cpp kpstatemeinmenu.h kpstatefinish.cpp kpstatefinish.h kpstatesettings.cpp kpstatesettings.h kpstatecontrolsettings.cpp kpstatecontrolsettings.h kpstategraphicsettings.cpp kpstategraphicsettings.h kpstategraphichint.cpp kpstategraphichint.h kpstateaudiosettings.cpp kpstateaudiosettings.h kpstatekeyboardhelp.cpp kpstatekeyboardhelp.h kpstategame.cpp kpstategame.h kpstateselectlanguage.cpp kpstateselectlanguage.h kpstategamesolved.cpp kpstategamesolved.h kpstatetutorial1.cpp kpstatetutorial1.h kpstatetutorial2.cpp kpstatetutorial2.h kpstatetutorial3.cpp kpstatetutorial3.h kpstatetutorial4.cpp kpstatetutorial4.h kpstatemovetoken.cpp kpstatemovetoken.h bthread.cpp bthread.h bthreadfactory.cpp bthreadfactory.h bthreadimp.cpp bthreadimp.h blinuxthreadimp.cpp blinuxthreadimp.h kpthreadsolvecount.cpp kpthreadsolvecount.h kpprogressbar.cpp kpprogressbar.h KPtime.h kpstatistics.cpp kpstatistics.h btexture.cpp btexture.h kpscore.cpp kpscore.h kpstatescorelist.cpp kpstatescorelist.h btime.cpp btime.h kpuibase.cpp kpuibase.h kpglutuserinterface.cpp kpglutuserinterface.h kpsdluserinterface.cpp kpsdluserinterface.h bdir.cpp bdir.h bwin32threadimp.cpp bwin32threadimp.h KhunPhan.rc 

####### kdevelop will overwrite this part!!! (end)############
# set the include path found by configure

INCLUDES= $(all_includes) -I/usr/include/libxml2 -DLINUX -DDATADIR=\"$(datadir)\"

# the library search path.
khunphan_LDFLAGS = $(all_libraries) 
                                                                                
