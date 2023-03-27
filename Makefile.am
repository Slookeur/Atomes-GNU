## Process this file with automake to produce Makefile.in

SUBDIRS = src

atomes_docdir = ${docdir}
atomes_doc_DATA = \
	README.md \
	AUTHORS \
	ChangeLog

atomes_mandir = ${mandir}/man1/
atomes_man_DATA = \
	atomes.1.gz 

install-data-local:
	echo "pkgdatadir= "$(pkgdatadir)
	if test -d $(srcdir)/icons; then \
		$(mkinstalldirs) $(pkgdatadir)/icons; \
		for icon in $(srcdir)/icons/*; do \
			if test -f $$icon; then \
				$(INSTALL_DATA) $$icon $(pkgdatadir)/icons; \
			fi \
		done \
	fi
	if test -d $(srcdir)/pixmaps; then \
		$(mkinstalldirs) $(pkgdatadir)/pixmaps; \
		for pixmap in $(srcdir)/pixmaps/*; do \
			if test -f $$pixmap; then \
				$(INSTALL_DATA) $$pixmap $(pkgdatadir)/pixmaps; \
			else \
				$(mkinstalldirs) $(pkgdatadir)/$$pixmap; \
				for pixma in $$pixmap/*; do \
					if test -f $$pixma; then \
						$(INSTALL_DATA) $$pixma $(pkgdatadir)/$$pixmap; \
					fi \
				done \
			fi \
		done \
	fi
	if test -d $(srcdir)/library; then \
		$(mkinstalldirs) $(pkgdatadir)/library; \
		for librar in $(srcdir)/library/*; do \
			if test -f $$librar; then \
				$(INSTALL_DATA) $$librar $(pkgdatadir)/library; \
			else \
				$(mkinstalldirs) $(pkgdatadir)/$$librar; \
				for libra in $$librar/*; do \
					if test -f $$libra; then \
						$(INSTALL_DATA) $$libra $(pkgdatadir)/$$librar; \
					else \
						$(mkinstalldirs) $(pkgdatadir)/$$libra; \
						for libr in $$libra/*; do \
							if test -f $$libr; then \
								$(INSTALL_DATA) $$libr $(pkgdatadir)/$$libra; \
							fi \
						done \
					fi \
				done \
			fi \
		done \
	fi 
# Bash completion
	if [ ! -d /bash-completion/completions ]; then \
		mkdir -p /bash-completion/completions; \
	fi
	$(INSTALL_DATA) $(srcdir)/atomes-completion.sh $(datadir)/bash-completion/completions/atomes
# Meta info
	if [ ! -d /metainfo ]; then \
		mkdir -p /metainfo; \
	fi
	$(INSTALL_DATA) $(srcdir)/fr.ipcms.atomes.metainfo.xml $(datadir)/metainfo
	appstream-util validate-relax --nonet /metainfo/fr.ipcms.atomes.metainfo.xml
# Desktop file
	sed 's;DATADIR;$(pkgdatadir);1' $(srcdir)/atomes.desktop.in > $(srcdir)/atomes.desktop
	if [ ! -d /applications ]; then \
		mkdir -p /applications; \
	fi
	$(INSTALL_DATA) $(srcdir)/atomes.desktop  $(datadir)/applications/atomes.desktop
	desktop-file-install --vendor="" \
		--dir=/applications -m 644 \
		/applications/atomes.desktop
	update-mime-database /mime &> /dev/null || :
	update-desktop-database &> /dev/null || :
	touch --no-create /icons/hicolor
	if [ -u `which gtk-update-icon-cache` ]; then \
	  	gtk-update-icon-cache -q /icons/hicolor; \
	fi
	
# Remove doc directordy on uninstall
uninstall-local:
	-rm -rf $(pkgdatadir)/icons/*
	-rmdir $(pkgdatadir)/icons
	-rm -rf $(pkgdatadir)/pixmaps/*
	-rmdir $(pkgdatadir)/pixmaps
	-rm -rf $(pkgdatadir)/library/*
	-rmdir $(pkgdatadir)/library
	-rmdir $(pkgdatadir)
	-rm -f $(datadir)/applications/atomes.desktop
	-rm -f $(datadir)/bash-completion/completions/atomes
	-rm -f $(datadir)/metainfo/fr.ipcms.atomes.metainfo.xml
	-rmdir $(atomes_docdir)
	update-mime-database /mime &> /dev/null || :
	update-desktop-database &> /dev/null || :
	touch --no-create /icons/hicolor
	if [ -u `which gtk-update-icon-cache` ]; then \
	  gtk-update-icon-cache -q /icons/hicolor; \
	fi
