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

$ ./output/json.out /store/test/14c5c58e-00c2-4660-bf90-a963b86388e1.root

```
