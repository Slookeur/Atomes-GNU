## Process this file with automake to produce Makefile.in

SUBDIRS = src

atomes_datadir = $(DESTDIR)$(datadir)
atomes_pkgdatadir = $(DESTDIR)$(pkgdatadir)
atomes_desktopdir = $(DESTDIR)$(datadir)/applications
atomes_metadir = $(DESTDIR)$(datadir)/metainfo
atomes_bashdir = $(DESTDIR)$(datadir)/bash-completion/completions

atomes_docdir = $(DESTDIR)$(docdir)
atomes_doc_DATA = \
	README.md \
	AUTHORS \
	ChangeLog

atomes_mandir = $(DESTDIR)$(mandir)/man1/
atomes_man_DATA = \
	atomes.1.gz 

install-data-local:
	if test -d $(srcdir)/icons; then \
		$(mkinstalldirs) $(atomes_pkgdatadir)/icons; \
		for icon in $(srcdir)/icons/*; do \
			if test -f $$icon; then \
				$(INSTALL_DATA) $$icon $(atomes_pkgdatadir)/icons; \
			fi \
		done \
	fi
	if test -d $(srcdir)/pixmaps; then \
		$(mkinstalldirs) $(atomes_pkgdatadir)/pixmaps; \
		for pixmap in $(srcdir)/pixmaps/*; do \
			if test -f $$pixmap; then \
				$(INSTALL_DATA) $$pixmap $(atomes_pkgdatadir)/pixmaps; \
			else \
				$(mkinstalldirs) $(atomes_pkgdatadir)/$$pixmap; \
				for pixma in $$pixmap/*; do \
					if test -f $$pixma; then \
						$(INSTALL_DATA) $$pixma $(atomes_pkgdatadir)/$$pixmap; \
					fi \
				done \
			fi \
		done \
	fi
	if test -d $(srcdir)/library; then \
		$(mkinstalldirs) $(atomes_pkgdatadir)/library; \
		for librar in $(srcdir)/library/*; do \
			if test -f $$librar; then \
				$(INSTALL_DATA) $$librar $(atomes_pkgdatadir)/library; \
			else \
				$(mkinstalldirs) $(atomes_pkgdatadir)/$$librar; \
				for libra in $$librar/*; do \
					if test -f $$libra; then \
						$(INSTALL_DATA) $$libra $(atomes_pkgdatadir)/$$librar; \
					else \
						$(mkinstalldirs) $(atomes_pkgdatadir)/$$libra; \
						for libr in $$libra/*; do \
							if test -f $$libr; then \
								$(INSTALL_DATA) $$libr $(atomes_pkgdatadir)/$$libra; \
							fi \
						done \
					fi \
				done \
			fi \
		done \
	fi 
# Bash completion
	if [ ! -d $(atomes_bashdir) ]; then \
		$(mkinstalldirs) $(atomes_bashdir); \
	fi
	$(INSTALL_DATA) $(srcdir)/atomes-completion.sh $(atomes_bashdir)/atomes
# Meta info
	if [ ! -d $(atomes_metadir) ]; then \
		$(mkinstalldirs) $(atomes_metadir); \
	fi
	$(INSTALL_DATA) $(srcdir)/fr.ipcms.atomes.metainfo.xml $(atomes_metadir)
	appstream-util validate-relax --nonet $(atomes_metadir)/fr.ipcms.atomes.metainfo.xml
# Desktop file
	sed 's;DATADIR;$(atomes_pkgdatadir);1' $(srcdir)/atomes.desktop.in > $(srcdir)/atomes.desktop
	if [ ! -d $(atomes_desktopdir) ]; then \
		$(mkinstalldirs) $(atomes_desktopdir); \
	fi
	$(INSTALL_DATA) $(srcdir)/atomes.desktop  $(atomes_desktopdir)/atomes.desktop
	desktop-file-install --vendor="" \
		--dir=/applications -m 644 \
		$(atomes_desktopdir)/atomes.desktop
	update-mime-database /mime &> /dev/null || :
	update-desktop-database &> /dev/null || :
	touch --no-create /icons/hicolor
	if [ -u `which gtk-update-icon-cache` ]; then \
	  	gtk-update-icon-cache -q /icons/hicolor; \
	fi
	
# Remove doc directordy on uninstall
uninstall-local:
	-rm -rf $(atomes_pkgdatadir)/icons/*
	-rmdir $(atomes_pkgdatadir)/icons
	-rm -rf $(atomes_pkgdatadir)/pixmaps/*
	-rmdir $(atomes_pkgdatadir)/pixmaps
	-rm -rf $(atomes_pkgdatadir)/library/*
	-rmdir $(atomes_pkgdatadir)/library
	-rmdir $(atomes_pkgdatadir)
	-rm -f $(atomes_desktopdir)/atomes.desktop
	-rm -f $(atomes_metadir)/fr.ipcms.atomes.metainfo.xml
	-rm -f $(atomes_bashdir)/atomes
	-rmdir $(atomes_docdir)
	update-mime-database /mime &> /dev/null || :
	update-desktop-database &> /dev/null || :
	touch --no-create /icons/hicolor
	if [ -u `which gtk-update-icon-cache` ]; then \
	  gtk-update-icon-cache -q /icons/hicolor; \
	fi
