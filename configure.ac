dnl Process this file with autoconf to produce a configure script.
dnl Created by Sébastien Le Roux.

AC_PREREQ(2.59)

m4_define(atomes_major_version, 1)
m4_define(atomes_minor_version, 1)
m4_define(atomes_patch_version, 10)
m4_define(atomes_version, atomes_major_version.atomes_minor_version.atomes_patch_version)
m4_define(atomes_url, https://atomes.ipcms.fr)
m4_define(atomes_tar, atomes)

AC_INIT([atomes], [atomes_version], [sebastien.leroux@ipcms.unistra.fr], [atomes_tar], [atomes_url])

AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h])

AC_DEFINE(ATOMES_MAJOR_VERSION, atomes_major_version, [Atomes major version])
AC_SUBST(ATOMES_MAJOR_VERSION, atomes_major_version)
AC_DEFINE(ATOMES_MINOR_VERSION, atomes_minor_version, [Atomes minor version])
AC_SUBST(ATOMES_MINOR_VERSION, atomes_minor_version)
AC_DEFINE(ATOMES_PATCH_VERSION, atomes_patch_version, [Atomes patch version])
AC_SUBST(ATOMES_PATCH_VERSION, atomes_patch_version)

AC_PROG_CC

dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************

GETTEXT_PACKAGE=atomes
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])

dnl IT_PROG_INTLTOOL([0.35.0])

dnl ***************************************************************************
dnl Check for required librairies
dnl ***************************************************************************

PKG_CHECK_MODULES(GTK, [gtk+-3.0 >= 3.16])
dnl PKG_CHECK_MODULES(GTK, [gtk4 >= 4.60])
PKG_CHECK_MODULES(LIBXML2, [libxml-2.0 >= 2.4.0])
PKG_CHECK_MODULES(PANGOFT2, [pangoft2])
PKG_CHECK_MODULES(FFMPEG, [libavcodec libavformat libavutil libswscale])
PKG_CHECK_MODULES(GLU, [glu])
PKG_CHECK_MODULES(EPOXY, [epoxy])

dnl ***************************************************************************
dnl Fortran 90
dnl ***************************************************************************

AC_FC_WRAPPERS

AC_LANG_PUSH([Fortran])
fcflags_save="$FCFLAGS"
AC_PROG_FC([xlf95 fort ifort ifc efc f95 g95 pgf95 lf95 ftn xlf90 f90 pgf90 pghpf epcf90 gfortran gfortran-4 gfortran-4.exe], [90])
FCFLAGS="$fcflags_save"

dnl ----------------------------------------------------------
dnl ----------------------------------------------------------
dnl ----------------------------------------------------------

AC_FC_SRCEXT(f90, FCFLAGS_f90="$FCFLAGS_f90 $FCFLAGS", AC_MSG_ERROR([Cannot compile .f90 files]))
AC_FC_SRCEXT(F90, FCFLAGS_F90="$FCFLAGS_F90 $FCFLAGS", AC_MSG_ERROR([Cannot compile .F90 files]))

#the_compiler_is_IBMS=no
#if test "$FC" = "xlf90" ; then
#	the_compiler_is_IBMS=yes
#fi
#if test "$FC" = "xlf95" ; then
#	the_compiler_is_IBMS=yes
#fi
#if test "$FC" = "mpxlf90" ; then
#	the_compiler_is_IBMS=yes
#fi
#if test "$FC" = "mpxlf95" ; then
#	the_compiler_is_IBMS=yes
#fi
#if test "$FC" = "mpxlf90_r" ; then
#	the_compiler_is_IBMS=yes
#fi
#if test "$FC" = "mpxlf95_r" ; then
#	the_compiler_is_IBMS=yes
#fi

#if test "$the_compiler_is_IBMS" = "yes" ; then
#	FDEFS="-WF,"
#else 
dnl	FDEFS="$DEFS"
#fi
dnl AC_SUBST([FDEFS])

AC_DEFUN([AX_CHECK_COMPILER_FLAGS],
[AC_PREREQ(2.59) dnl for _AC_LANG_PREFIX
AC_MSG_CHECKING([whether _AC_LANG compiler accepts $1])
dnl Some hackery here since AC_CACHE_VAL can't handle a non-literal varname:
AS_LITERAL_IF([$1],
  [AC_CACHE_VAL(AS_TR_SH(ax_cv_[]_AC_LANG_ABBREV[]_flags_$1), [
      ax_save_FLAGS=$[]_AC_LANG_PREFIX[]FLAGS
      _AC_LANG_PREFIX[]FLAGS="$1"
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
        AS_TR_SH(ax_cv_[]_AC_LANG_ABBREV[]_flags_$1)=yes,
        AS_TR_SH(ax_cv_[]_AC_LANG_ABBREV[]_flags_$1)=no)
      _AC_LANG_PREFIX[]FLAGS=$ax_save_FLAGS])],
  [ax_save_FLAGS=$[]_AC_LANG_PREFIX[]FLAGS
   _AC_LANG_PREFIX[]FLAGS="$1"
   AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
     eval AS_TR_SH(ax_cv_[]_AC_LANG_ABBREV[]_flags_$1)=yes,
     eval AS_TR_SH(ax_cv_[]_AC_LANG_ABBREV[]_flags_$1)=no)
   _AC_LANG_PREFIX[]FLAGS=$ax_save_FLAGS])
eval ax_check_compiler_flags=$AS_TR_SH(ax_cv_[]_AC_LANG_ABBREV[]_flags_$1)
AC_MSG_RESULT($ax_check_compiler_flags)
if test "x$ax_check_compiler_flags" = xyes; then
        m4_default([$2], :)
else
        m4_default([$3], :)
fi
])dnl AX_CHECK_COMPILER_FLAGS

AX_CHECK_COMPILER_FLAGS([$FCFLAGS])

AC_FC_LIBRARY_LDFLAGS
AC_FC_FREEFORM
AC_LANG_POP([Fortran])

dnl ***************************************************************************
dnl Check for windows
dnl ***************************************************************************

case $host in
  *mingw* | pw32* | cygwin*)
    platform_win32=yes
    echo "Building win32 application"
    ;;
  *)
    platform_win32=no
    ;;
esac
AM_CONDITIONAL(PLATFORM_WIN32, test x"$platform_win32" = "xyes")

case $host in
  *mingw*)
    native_win32=yes
    echo "Building native win32 application"
    ;;
  *)
    native_win32=no
    ;;
esac
AM_CONDITIONAL(NATIVE_WIN32, test x"$native_win32" = "xyes")

if test "$native_win32" = "yes"; then
  AC_CHECK_TOOL(WINDRES, windres, no)
  if test "$WINDRES" = no; then
    AC_MSG_ERROR([*** Could not find an implementation of windres in your PATH.])
  fi
fi

cat lic > src/version.h
echo "#define FC \""$FC"\"" >> src/version.h
echo "#define FCFLAGS \""$FCFLAGS"\"" >> src/version.h
echo "#define CC \""$CC"\"" >> src/version.h
echo "#define CFLAGS \""$CFLAGS"\"" >> src/version.h

dnl Flatpak 
AC_ARG_ENABLE([flatpak],
	     [  --enable-flatpak        to build the Flatpak])
AM_CONDITIONAL([FLATPAK], [test "x$flatpak" != "xno"])

AC_CONFIG_FILES([
 Makefile
 src/Makefile
])

AC_OUTPUT
