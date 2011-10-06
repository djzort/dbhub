Summary: DB Hub
Name: dbhub
Version: 0.451
Release: 1%{?dist}
Source0: %{name}-%{version}.tar.gz
License: GPL
Group: System Environment/Daemons
Packager: Pavel O. Zhukov <gelios@gmail.com>
BuildRoot: %{_tmppath}/%{name}-%{version}-buildroot
Summary: A poweful Direct Connect hub for Linux/Unix
Vendor: Pavel O. Zhukov

%description
A powerfull DirectConnect hub software for unix systems.
Uses plain text database.
Plugins support.
Included antiflood and antispam systems.

%prep
%setup -q

%build
./configure --prefix=/usr --enable-switch_user --enable-perl
make

%install
make DESTDIR="$RPM_BUILD_ROOT" install

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/bin/dbhub
%doc COPYING AUTHORS
%doc DB-Manual/config.html

%changelog
* Wed Dec 5 2007 Pavel O. Zhukov <gelios@gmail.com> - 0.450-1
- new upstream version
