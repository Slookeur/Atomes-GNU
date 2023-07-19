## Process this file with automake to produce Makefile.in

atomes_datadir = $(DESTDIR)$(datadir)
atomes_pkgdatadir = $(DESTDIR)$(pkgdatadir)
atomes_iconsdir = $(atomes_datadir)/pixmaps
atomes_desktopdir = $(atomes_datadir)/applications

install-data-local:
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
	
# Remove doc directordy on uninstall
uninstall-local:
	-rm -rf $(atomes_pkgdatadir)/pixmaps/*
	-rmdir $(atomes_pkgdatadir)/pixmaps
	-rm -rf $(atomes_pkgdatadir)/library/*
	-rmdir $(atomes_pkgdatadir)/library
	-rmdir $(atomes_pkgdatadir)
