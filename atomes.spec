Name:           atomes
%global upname Atomes-GNU
Version:        1.1.7
Release:        6%{?dist}
Summary:        An atomistic toolbox
License:        AGPL-3.0-or-later
Source0:        https://github.com/Slookeur/%{upname}/archive/refs/tags/v%{version}.tar.gz
# Source1:        ./v%%{version}.tar.gz.asc
# Source2:        %%{name}.gpg
URL:            https://%{name}.ipcms.fr/

BuildRequires: gnupg2
BuildRequires: make
BuildRequires: automake
BuildRequires: autoconf
BuildRequires: pkgconf-pkg-config
BuildRequires: gcc
BuildRequires: gcc-gfortran
BuildRequires: libgfortran
BuildRequires: desktop-file-utils
BuildRequires: libappstream-glib

# pkg-config 
BuildRequires: pkgconfig(gtk+-3.0)
BuildRequires: pkgconfig(libxml-2.0)
BuildRequires: pkgconfig(glu)
BuildRequires: pkgconfig(glut)
BuildRequires: pkgconfig(epoxy)
BuildRequires: pkgconfig(libavutil)
BuildRequires: pkgconfig(libavcodec)
BuildRequires: pkgconfig(libavformat)
BuildRequires: pkgconfig(libswscale)

# For Suse-based linux
%if 0%{?suse_version}
BuildRequires: update-desktop-files
%endif
 
Requires: gtk3
Requires: freeglut
Requires: mesa-libGLU

Provides: %{name} = %{version}-%{release}

%description
Atomes: a toolbox to analyze, to visualize 
and to create/edit three-dimensional atomistic models.
It offers a workspace that allows to have many projects opened simultaneously.
The different projects in the workspace can exchange data: 
analysis results, atomic coordinates...
Atomes also provides an advanced input preparation system 
for further calculations using well known molecular dynamics codes:

    Classical MD: DLPOLY and LAMMPS
    ab-initio MD: CPMD and CP2K
    QM-MM MD: CPMD and CP2K

To prepare the input files for these calculations is likely to be the key, 
and most complicated step towards MD simulations.
Atomes offers a user-friendly assistant to help and guide the scientist
step by step to achieve this crucial step.

%prep
# %%{gpgverify} --keyring='%%{SOURCE2}' --signature='%%{SOURCE1}' --data='%%{SOURCE0}'
%autosetup -n %{upname}-%{version}

%build
%configure --prefix=/usr
make `%{?_smp_mflags}`

%install
%make_install

%check
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop
appstream-util validate-relax --nonet %{buildroot}%{_metainfodir}/fr.ipcms.%{name}.metainfo.xml

%files
%license COPYING
%{_bindir}/%{name}
%{_datadir}/doc/%{name}/
%{_datadir}/%{name}/
%{_datadir}/bash-completion/completions/%{name}
%{_datadir}/applications/%{name}.desktop
%{_mandir}/man1/%{name}.1.gz
%{_metainfodir}/fr.ipcms.%{name}.metainfo.xml

%changelog
* Fri Oct 21 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.7-6
- Revised package

* Wed Oct 19 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.7-5
- Revised package

* Wed Oct 19 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.7-4
- Revised package

* Wed Oct 19 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.7-3
- Revised package

* Tue Oct 18 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.7-2
- Revised package

* Thu Oct 13 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.7-1
- Bug correction:
  callback.c: save workspace
  atom_action.c: on remove, trigger re-calculate bonding

* Tue Oct 11 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.6-1
- Bug correction: 
  w_library.c: lib_preview_plot = NULL;
- Improvements:
  main.c: -h
  glview.c: selection delay on mouse pressed/released

* Fri Sep 23 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.5-1
- Bug correction:
  w_search.c: selection not to crash if atom_win is closed

* Thu Sep 22 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.4-1
- Bug correction: 
  read_coord.c: Windows to handle properly EOL symbols
  m_curve.c: Windows to not crash when destroying label widget
  glview.c: correct GWARNING messages on widget scale

* Thu Sep 15 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.3-1
- Bug correction: 
  read_opengl.c: correct 'read_atom_b'
  initring.c: coorect 'send_rings_opengl_'

* Wed Sep 14 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.2-1
- Bug correction: 
  atom_action.c: recompute bonding on passivate
  bdcall.c: do not trigger menu init on passivate

* Tue Sep 13 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.1-1
- Bug correction: 
  double click on workspace

* Mon Aug 29 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.0-1
- Initial release of the Atomes program v1.1.0
