#
# spec file for package [spectemplate]
#
# Copyright (c) 2010 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#
# norootforbuild

Name:           texiteasy
Version:        0.2.4
Release:        0
Summary:        Simple and Powerful Latex Editor

Group:          Productivity/Publishing/PDF
License:        GPL-3
Url:            https://github.com/Bramas/texiteasy
Source:         %{name}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
#BuildRoot: 		%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  gcc gcc-c++
%if 0%{?mandriva_version}
BuildRequires:  X11-devel
BuildRequires:  qt4-devel >= 4.7
BuildRequires:  poppler-qt4-devel
%else
%if 0%{?fedora_version} || 0%{?rhel_version} || 0%{?centos_version}
BuildRequires: qt4-devel >= 4.7 
BuildRequires: poppler
BuildRequires: poppler-qt
BuildRequires: poppler-qt-devel
%else
BuildRequires: libqt4-devel >= 4.7
BuildRequires: libpoppler-qt4-devel
%endif
%endif
Requires:       texlive-latex



%description
Simple and Powerful Latex Editor.

%prep
%setup -q


%build 
QTDIR=%{_libdir}/qt4
%if 0%{?mandriva_version} > 2006  
QTDIR=%{_prefix}/lib/qt4/  
%endif  
PATH=$QTDIR/bin:$PATH
LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
DYLD_LIBRARY_PATH=$QTDIR/lib:$DYLD_LIBRARY_PATH
export QTDIR PATH LD_LIBRARY_PATH DYLD_LIBRARY_PATH
PREFIX=%{buildroot}%{_prefix}
qmake texiteasy.pro
make %{?_smp_mflags} INSTALL_ROOT=%{buildroot}

%install
make INSTALL_ROOT=%{buildroot} install

#%clean
#%{__rm} -rf %{buildroot}
# rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/texiteasy
%doc

%changelog