%global debug_package %{nil}

%define package_xml xrootd-cmstfc-1.5.2
%define xrootd-cmstfc_URL  https://github.com/bbockelm/xrootd-cmstfc

%define package_json xrootd-cmsjson-0.0.1
%define xrootd-cmsjson_URL https://github.com/guyzsarun/xrootd-cmsjson

Name: xrootd-cmstfc
Version: 2.0.0
Release: 1%{?dist}
Summary: CMS TFC plugin for xrootd

Group: System Environment/Daemons
License: BSD
Source0: xrootd-cmstfc.tar.gz
Source1: xrootd-cmsjson.tar.gz

BuildRequires: xerces-c-devel pcre-devel jsoncpp-devel >= 1.9.4
BuildRequires: cmake
Requires: /usr/bin/xrootd pcre xerces-c jsoncpp >= 1.9.4

%package devel
Summary: Development headers and libraries for XRootD CMSTFC plugin
Group: System Environment/Development

%description
%{summary}


%description devel
%{summary}

%prep
%setup -n %{package_xml}
cd ..
gzip -dc /root/rpmbuild/SOURCES/xrootd-cmsjson.tar.gz | tar -xvvf -

%build
pushd $RPM_BUILD_DIR/%{package_xml}
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=%{_prefix}  ..
make VERBOSE=1 %{?_smp_mflags}
popd

pushd $RPM_BUILD_DIR/%{package_json}
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=%{_prefix}  ..
make VERBOSE=1 %{?_smp_mflags}
popd


%install
rm -rf $RPM_BUILD_ROOT
pushd $RPM_BUILD_DIR/%{package_xml}/build
make install DESTDIR=$RPM_BUILD_ROOT
popd

pushd $RPM_BUILD_DIR/%{package_json}/build
make install DESTDIR=$RPM_BUILD_ROOT
popd

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_exec_prefix}/lib/libXrdCmsTfc.so
%{_libdir}/libXrdCmsJson.so

%files devel
%defattr(-,root,root,-)
%{_includedir}/XrdCmsTfc.hh
%{_includedir}/XrdCmsJson.hh

%changelog