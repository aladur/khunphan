bin_PROGRAMS = khunphan

khunphan_SOURCES = bdir.cpp kpsdluserinterface.cpp kpglutuserinterface.cpp \
	kpuibase.cpp btime.cpp kpstatescorelist.cpp kpscore.cpp btexture.cpp \
	kpstatistics.cpp kpprogressbar.cpp kpthreadsolvecount.cpp \
	bposixthreadimp.cpp bwin32threadimp.cpp bthreadimp.cpp \
	bthreadfactory.cpp bthread.cpp kpstatemovetoken.cpp \
	kpstatetutorial4.cpp kpstatetutorial3.cpp kpstatetutorial2.cpp \
	kpstatetutorial1.cpp kpstategamesolved.cpp kpstateselectlanguage.cpp \
	kpstategame.cpp kpstatekeyboardhelp.cpp kpstateaudiosettings.cpp \
	kpstategraphichint.cpp kpstategraphicsettings.cpp \
	kpstatecontrolsettings.cpp kpstatesettings.cpp kpstatefinish.cpp \
	kpstatemeinmenu.cpp kpstatefactory.cpp kpstatestartup.cpp kpmenu.cpp \
	kpstate.cpp light.cpp main.cpp kpconfig.cpp kpboardGL.cpp Kamera.cpp \
	Textfeld.cpp Schild.cpp kpidhash.cpp kpnode.cpp misc1.cpp \
	kpboard.cpp KhunPhan.cpp bstring.cpp kpstatelighttest.cpp

khunphan_CXXFLAGS  = @GLU_CFLAGS@ @GLUT_CFLAGS@ @XML_CPPFLAGS@ @LIBPNG_CFLAGS@
khunphan_LDADD     = @GLU_LIBS@ @GLUT_LIBS@ @XML_LIBS@ @LIBPNG_LIBS@

EXTRA_DIST = config.h bstring.h KhunPhan.h kpboard.h misc1.h kpnode.h \
	kpidhash.h Schild.h sprache.h Textfeld.h Kamera.h kpboardGL.h \
	kpconfig.h  light.h KPstateContext.h kpstate.h kpmenu.h \
	kpstatestartup.h kpstatefactory.h kpstatemeinmenu.h \
	kpstatefinish.h kpstatesettings.h kpstatecontrolsettings.h \
	kpstategraphicsettings.h kpstategraphichint.h kpstateaudiosettings.h \
	kpstatekeyboardhelp.h kpstategame.h kpstateselectlanguage.h \
	kpstategamesolved.h kpstatetutorial1.h kpstatetutorial2.h \
	kpstatetutorial3.h kpstatetutorial4.h kpstatemovetoken.h bthread.h \
	bthreadfactory.h bthreadimp.h bposixthreadimp.h kpthreadsolvecount.h \
	kpprogressbar.h KPtime.h kpstatistics.h btexture.h kpscore.h \
	kpstatescorelist.h btime.h kpuibase.h kpglutuserinterface.h \
	kpsdluserinterface.h bdir.h bwin32threadimp.h resource.h \
	bdelete.h bsingle.h kpstatelighttest.h \
	KhunPhan.rc icon.ico

AM_CPPFLAGS= $(all_includes) -DLINUX -DDATADIR=\"$(datadir)\"
#khunphan_LDFLAGS = $(all_libraries) 

nobase_dist_pkgdata_DATA = \
	Music/Cymbals.ogg Music/Earth.ogg Music/Mystery.ogg \
	Sounds/changesetting.ogg Sounds/gamesolved.ogg \
	Sounds/movetoken.ogg Sounds/opengame.ogg \
	Sounds/openmenu.ogg Sounds/quitgame.ogg \
	Sounds/tutorialnext.ogg \
	Sounds/bubbles/movetoken.ogg \
	Sounds/marble/movetoken.ogg \
	Sounds/metal/movetoken.ogg \
	Sounds/painting/movetoken.ogg \
	Sounds/rock/movetoken.ogg \
	Sounds/smily/movetoken.ogg \
	Sounds/wood/movetoken.ogg \
	Textures/characters.png Textures/logo.png \
	Textures/music_on.png Textures/soundmusic_off.png \
	Textures/sound_on.png \
	Textures/bubbles/board.png Textures/bubbles/texture1.png \
	Textures/bubbles/texture2.png Textures/bubbles/texture3.png \
	Textures/marble/board.png Textures/marble/texture1.png \
	Textures/marble/texture2.png Textures/marble/texture3.png \
	Textures/metal/board.png Textures/metal/texture1.png \
	Textures/metal/texture2.png Textures/metal/texture3.png \
	Textures/painting/board.png Textures/painting/texture1.png \
	Textures/painting/texture2.png Textures/painting/texture3.png \
	Textures/rock/board.png Textures/rock/texture1.png \
	Textures/rock/texture2.png Textures/rock/texture3.png \
	Textures/smily/board.png Textures/smily/texture1.png \
	Textures/smily/texture2.png Textures/smily/texture3.png \
	Textures/wood/board.png Textures/wood/texture1.png \
	Textures/wood/texture2.png Textures/wood/texture3.png \
	locale/900.lang locale/901.lang locale/999.lang
                                                                                
