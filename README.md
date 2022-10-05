# xrootd-cmsjson

## Install jsoncpp
```
git clone https://github.com/open-source-parsers/jsoncpp.git
cd jsoncpp/
mkdir -p build
cd build/
cmake -DCMAKE_BUILD_TYPE=release -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DARCHIVE_INSTALL_DIR=. -G "Unix Makefiles" ..
make
make install

```

## Test manually xrootd-cmsjson
```
$ cd xrootd-cmsjson

$ g++ -g ~/xrootd-cmsjson/main.cc -o ~/output/json.out ~/xrootd-cmsjson/src/XrdCmsJson.cc -I /root/xrootd-json -l jsoncpp -I /usr/include/xrootd -l XrdUtils

$ ~/output/json.out /store/test/14c5c58e-00c2-4660-bf90-a963b86388e1.root

```

### Expected results
lfn: /store/test/xrootd/T2_XX_Test/store/test.root
pfn: srm://my.domain.ch/srm/managerv2?SFN=/pnfs/domain.ch/xrootd/disk/data/T2/store/test.root



## Test manually xrootd-cmstfc

```
$ g++ -g xml_main.cc -o ~/output/test.out ~/xrootd-cmstfc/src/XrdCmsTfc.cc -I /usr/include/xrootd -I /root/xrootd-cmstfc -l xerces-c-3.2 -l XrdUtils -l pcre

$ ./output/test.out

```

## Test storage.xml in cmssw
```
export SITECONFIG_PATH=/my_siteconf_path/SITECONF
export CMS_PATH=/my_siteconf_path/
```