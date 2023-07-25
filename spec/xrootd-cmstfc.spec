%global debug_package %{nil}

Name: xrootd-cmstfc
Version: 2.0.0
Release: 3%{?dist}
Summary: CMS TFC plugin for xrootd

Group: System Environment/Daemons
License: BSD
%define xrootd-cmstfc_URL  https://github.com/bbockelm/xrootd-cmstfc
%define xrootd-cmsjson_URL https://github.com/guyzsarun/xrootd-cmsjson
Source0: xrootd-cmstfc.tar.gz
Source1: xrootd-cmsjson.tar.gz

BuildRequires: xrootd-libs-devel xerces-c-devel pcre-devel jsoncpp-devel >= 1.9.4
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
%setup -n xrootd-cmstfc
cd ..
rm -rf xrootd-cmsjson
mkdir -p xrootd-cmsjson
cd xrootd-cmsjson
tar -xf /root/rpmbuild/SOURCES/xrootd-cmsjson.tar.gz .

%build
pushd $RPM_BUILD_DIR/xrootd-cmstfc
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=%{_prefix}  ..
make VERBOSE=1 %{?_smp_mflags}
popd

pushd $RPM_BUILD_DIR/xrootd-cmsjson
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=%{_prefix}  ..
make VERBOSE=1 %{?_smp_mflags}
popd


%install
rm -rf $RPM_BUILD_ROOT
pushd $RPM_BUILD_DIR/xrootd-cmstfc/build
make install DESTDIR=$RPM_BUILD_ROOT
popd

pushd $RPM_BUILD_DIR/xrootd-cmsjson/build
make install DESTDIR=$RPM_BUILD_ROOT
popd

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_libdir}/libXrdCmsTfc.so
%{_libdir}/libXrdCmsJson.so

%files devel
%defattr(-,root,root,-)
%{_includedir}/XrdCmsTfc.hh
%{_includedir}/XrdCmsJson.hh

%changelog