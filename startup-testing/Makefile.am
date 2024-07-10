# Setup the name of the program to be generated, using:
bin_PROGRAMS = atomes_startup_testing

atomes_startup_testing_LDADD = $(LGTK_LIBS) $(GLU_LIBS) $(EPOXY_LIBS)
if WINDOWS
  atomes_startup_testing_LDADD += winatomes.o

winatomes.o: ../misc/win/atomes.rc
	$(WINDRES) $< $@		
endif

# Create a variable to store the CFLAGS for all the required libraries
LIBS_CFLAGS = $(LGTK_CFLAGS) $(GLU_CFLAGS) $(EPOXY_CFLAGS)

if GTK3
  GTK_VERSION=-DGTK3
else
  GTK_VERSION=-DGTK4
endif

# Use AM_CPPFLAGS to define additional preprocessor flags:
if WINDOWS
AM_CPPFLAGS = $(GTK_VERSION) -DGTKGLAREA
else
PACKAGE_CPPFLAGS = -DPACKAGE_PREFIX=\""$(datadir)/atomes"\" 
if LINUX
AM_CPPFLAGS = -DLINUX $(GTK_VERSION) -DGTKGLAREA $(PACKAGE_CPPFLAGS)
else
AM_CPPFLAGS = -DOSX $(GTK_VERSION) -DGTKGLAREA $(PACKAGE_CPPFLAGS)
endif
endif

inc = -I../src/ -I../src/opengl/ -I.
includes = $(inc) -DGTK_DISABLE_DEPRECATED

# Use AM_CFLAGS to define additional C flags:
AM_CFLAGS = $(includes) $(LIBS_CFLAGS)

atomes_startup_testing_src = startup_testing.c

atomes_startup_testing_includes = config.h version.h

clean: 
	-rm -f *.o

# List the files required to build the program
atomes_startup_testing_SOURCES = $(atomes_startup_testing_src)
