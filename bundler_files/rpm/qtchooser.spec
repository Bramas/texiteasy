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

Name:           qtchooser
Version:        0.1
Release:        0
Summary:        QtChooser

Group:          System/Libraries
License:        GPL
Url:            https://github.com/qtproject/qtchooser
Source:         %{name}.zip
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

BuildRequires:  gcc gcc-c++

%description
Simple and Powerful Latex Editor.

%prep
%setup -q


%build 
./configure
make

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