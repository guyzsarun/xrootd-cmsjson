
Name: xrootd-cmsjson
Version: 0.0.1
Release: 3%{?dist}
Summary: CMS JSON plugin for xrootd

Group: System Environment/Daemons
License: BSD
URL: https://github.com/bbockelm/xrootd-cmsjson
# Generated from:
# git-archive master | gzip -7 > ~/rpmbuild/SOURCES/xrootd-lcmaps.tar.gz
Source0: %{name}.tar.gz
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires: cmake jsoncpp-devel >= 1.9.4
Requires: /usr/bin/xrootd, jsoncpp >= 1.9.4

%package devel
Summary: Development headers and libraries for Xrootd CMSjson plugin
Group: System Environment/Development

%description
%{summary}

%description devel
%{summary}

%prep
%setup -q -c -n %{name}-%{version}

%build
%cmake .
make VERBOSE=1 %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_libdir}/libXrdCmsJson.so

%files devel
%defattr(-,root,root,-)
%{_includedir}/XrdCmsJson.hh

%changelog

