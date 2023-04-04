Name:           atomes
%global upname Atomes-GNU
Version:        1.1.11
Release:        5%{?dist}
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
BuildRequires: pkgconfig(pangoft2)
BuildRequires: pkgconfig(glu)
BuildRequires: pkgconfig(epoxy)
BuildRequires: pkgconfig(libavutil)
BuildRequires: pkgconfig(libavcodec)
BuildRequires: pkgconfig(libavformat)
BuildRequires: pkgconfig(libswscale)

# Runtime requirements
Requires: gtk3
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
%configure
#make %%{?_smp_mflags}
%make_build

%install
%make_install

%check
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop
appstream-util validate-relax --nonet %{buildroot}%{_metainfodir}/fr.ipcms.%{name}.appdata.xml

%files
%license COPYING
%{_bindir}/%{name}
%{_datadir}/doc/%{name}/
%{_mandir}/man1/%{name}.1.gz
%{_datadir}/%{name}/
%{_datadir}/bash-completion/completions/%{name}
%{_datadir}/applications
%{_datadir}/mime
%{_datadir}/icons/hicolor/scalable/apps/%{name}.svg
%{_datadir}/icons/hicolor/scalable/mimetypes/%{name}-workspace.svg
%{_datadir}/icons/hicolor/scalable/mimetypes/%{name}-project.svg
%{_metainfodir}/fr.ipcms.%{name}.appdata.xml

%changelog
* Tue Apr 04 2023 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.11-5
- Revised package

* Thu Mar 30 2023 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.11-4
- Revised package

* Thu Mar 30 2023 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.11-3
- Revised package

* Wed Mar 29 2023 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.11-2
- Revised package

* Wed Mar 29 2023 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.11-1
- Revised package

* Mon Mar 27 2023 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.11-0
- Bug correction

* Fri Nov 04 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.10-1
- Bug correction

* Mon Oct 31 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.9-3
- Revised package

* Fri Oct 28 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.9-2
- Revised package

* Fri Oct 28 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.9-1
- Bug corrections

* Fri Oct 28 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.8-3
- Revised package

* Fri Oct 28 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.8-2
- Revised package

* Thu Oct 27 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.8-1
- Bug corrections

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
- Bug corrections

* Tue Oct 11 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.6-1
- Bug corrections & improvements

* Fri Sep 23 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.5-1
- Bug correction

* Thu Sep 22 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.4-1
- Bug corrections

* Thu Sep 15 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.3-1
- Bug corrections

* Wed Sep 14 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.2-1
- Bug corrections

* Tue Sep 13 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.1-1
- Bug correction

* Mon Aug 29 2022 Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr> - 1.1.0-1
- Initial release of the Atomes program v1.1.0
