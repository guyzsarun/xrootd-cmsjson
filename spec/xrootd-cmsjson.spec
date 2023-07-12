
Name: xrootd-json
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
BuildRequires: xrootd-libs-devel xerces-c-devel pcre-devel
BuildRequires: cmake
Requires: /usr/bin/xrootd pcre xerces-c

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

%cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .
make VERBOSE=1 %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_libdir}/libXrdCmsjson.so.*
%{_libdir}/libXrdCmsjson.so

%files devel
%defattr(-,root,root,-)
%{_includedir}/XrdCmsjson.hh

%changelog

