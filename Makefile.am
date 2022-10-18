## Process this file with automake to produce Makefile.in

SUBDIRS = src

sysconfdir = /etc

atomes_docdir = ${docdir}
atomes_doc_DATA = \
	README.md \
	AUTHORS \
	ChangeLog

atomes_mandir = ${mandir}/man1/
atomes_man_DATA = \
	atomes.1.gz 

EXTRA_DIST = $(atomes_doc_DATA) $(atomes_man_DATA)

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/icons; then \
		$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/icons; \
		for icon in $(srcdir)/icons/*; do \
			if test -f $$icon; then \
				$(INSTALL_DATA) $$icon $(DESTDIR)$(pkgdatadir)/icons; \
			fi \
		done \
	fi
	if test -d $(srcdir)/pixmaps; then \
		$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
		for pixmap in $(srcdir)/pixmaps/*; do \
			if test -f $$pixmap; then \
				$(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
			else \
				$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/$$pixmap; \
				for pixma in $$pixmap/*; do \
					if test -f $$pixma; then \
						$(INSTALL_DATA) $$pixma $(DESTDIR)$(pkgdatadir)/$$pixmap; \
					fi \
				done \
			fi \
		done \
	fi
	if test -d $(srcdir)/library; then \
		$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/library; \
		for librar in $(srcdir)/library/*; do \
			if test -f $$librar; then \
				$(INSTALL_DATA) $$librar $(DESTDIR)$(pkgdatadir)/library; \
			else \
				$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/$$librar; \
				for libra in $$librar/*; do \
					if test -f $$libra; then \
						$(INSTALL_DATA) $$libra $(DESTDIR)$(pkgdatadir)/$$librar; \
					else \
						$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/$$libra; \
						for libr in $$libra/*; do \
							if test -f $$libr; then \
								$(INSTALL_DATA) $$libr $(DESTDIR)$(pkgdatadir)/$$libra; \
							fi \
						done \
					fi \
				done \
			fi \
		done \
	fi 
	if [ ! -d $(DESTDIR)$(datadir)bash-completion/completions ]; then \
		mkdir -p $(DESTDIR)$(datadir)/bash-completion/completions; \
	fi
	$(INSTALL_DATA) $(srcdir)/atomes-completion.sh $(DESTDIR)$(datadir)/bash-completion/completions/atomes
	if [ ! -d $(DESTDIR)$(datadir)/metainfo ]; then \
		mkdir -p $(DESTDIR)$(datadir)/metainfo; \
	fi
	$(INSTALL_DATA) $(srcdir)/fr.ipcms.atomes.metainfo.xml $(DESTDIR)$(datadir)/metainfo
	appstream-util validate-relax --nonet $(DESTDIR)$(datadir)/metainfo/fr.ipcms.atomes.metainfo.xml
	sed 's;DATADIR;$(pkgdatadir);1' $(srcdir)/atomes.desktop.in > $(srcdir)/atomes.desktop
	$(INSTALL_DATA) $(srcdir)/atomes.desktop $(DESTDIR)$(pkgdatadir)
	if [ ! -d $(DESTDIR)$(datadir)/applications ]; then \
		mkdir -p $(DESTDIR)$(datadir)/applications; \
	fi
	$(INSTALL_DATA) $(DESTDIR)$(pkgdatadir)/atomes.desktop  $(DESTDIR)$(datadir)/applications/atomes.desktop
	desktop-file-install --vendor="" \
		--dir=$(DESTDIR)$(datadir)/applications -m 644 \
		$(DESTDIR)$(datadir)/applications/atomes.desktop
	update-mime-database $(DESTDIR)$(datadir)/mime &> /dev/null || :
	update-desktop-database &> /dev/null || :
	touch --no-create $(DESTDIR)$(datadir)/icons/hicolor
	if [ -u `which gtk-update-icon-cache` ]; then \
	  	gtk-update-icon-cache -q $(DESTDIR)$(datadir)/icons/hicolor; \
	fi
	
# Remove doc directordy on uninstall
uninstall-local:
	-rm -rf $(DESTDIR)$(pkgdatadir)/icons/*
	-rmdir $(DESTDIR)$(pkgdatadir)/icons
	-rm -rf $(DESTDIR)$(pkgdatadir)/pixmaps/*
	-rmdir $(DESTDIR)$(pkgdatadir)/pixmaps
	-rm -rf $(DESTDIR)$(pkgdatadir)/library/*
	-rmdir $(DESTDIR)$(pkgdatadir)/library
	-rm -f $(DESTDIR)$(pkgdatadir)/*
	-rmdir $(DESTDIR)$(pkgdatadir)
	-rm -f $(DESTDIR)$(atomes_docdir)/*
	-rmdir $(DESTDIR)$(atomes_docdir)
	-rm -f $(DESTDIR)$(atomes_mandir)/man1/atomes.*
	-rm -f $(DESTDIR)$(datadir)/applications/atomes.desktop
	-rm -f $(DESTDIR)$(datadir)/bash-completion/completion/atomes
	-rm -f $(DESTDIR)$(datadir)/metainfo/fr.ipcms.atomes.metainfo.xml
	update-mime-database $(DESTDIR)$(datadir)/mime &> /dev/null || :
	update-desktop-database &> /dev/null || :
	touch --no-create $(DESTDIR)$(datadir)/icons/hicolor
	if [ -u `which gtk-update-icon-cache` ]; then \
	  gtk-update-icon-cache -q $(DESTDIR)$(datadir)/icons/hicolor; \
	fi
