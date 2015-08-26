####### kdevelop will overwrite this part!!! (begin)##########

SUBDIRS = marble metal wood painting smily bubbles rock 

EXTRA_DIST = characters.png logo.png music_on.png sound_on.png soundmusic_off.png 

install-data-local:
	$(mkinstalldirs) $(datadir)/khunphan/Textures/
	$(INSTALL_DATA) $(srcdir)/characters.png $(datadir)/khunphan/Textures/characters.png
	$(mkinstalldirs) $(datadir)/khunphan/Textures/
	$(INSTALL_DATA) $(srcdir)/logo.png $(datadir)/khunphan/Textures/logo.png
	$(mkinstalldirs) $(datadir)/khunphan/Textures/
	$(INSTALL_DATA) $(srcdir)/music_on.png $(datadir)/khunphan/Textures/music_on.png
	$(mkinstalldirs) $(datadir)/khunphan/Textures/
	$(INSTALL_DATA) $(srcdir)/sound_on.png $(datadir)/khunphan/Textures/sound_on.png
	$(mkinstalldirs) $(datadir)/khunphan/Textures/
	$(INSTALL_DATA) $(srcdir)/soundmusic_off.png $(datadir)/khunphan/Textures/soundmusic_off.png

uninstall-local:
	-rm -f $(datadir)/khunphan/Textures/characters.png
	-rm -f $(datadir)/khunphan/Textures/logo.png
	-rm -f $(datadir)/khunphan/Textures/music_on.png
	-rm -f $(datadir)/khunphan/Textures/sound_on.png
	-rm -f $(datadir)/khunphan/Textures/soundmusic_off.png

####### kdevelop will overwrite this part!!! (end)############
