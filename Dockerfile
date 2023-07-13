FROM almalinux:8

WORKDIR /xrootd

RUN yum -y update && \
    yum install -y dnf

RUN dnf group install -y "Development Tools"
RUN dnf -y install \
        cmake \
        curl-devel \
        diffutils \
        file \
        fuse-devel \
        gcc-c++ \
        git \
        json-c-devel \
        krb5-devel \
        libtool \
        libuuid-devel \
        libxml2-devel \
        make \
        openssl-devel \
        python3-devel \
        python3-setuptools \
        readline-devel \
        systemd-devel \
        zlib-devel

RUN yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm \
         https://repo.opensciencegrid.org/osg/3.6/osg-3.6-el8-release-latest.rpm

RUN yum install -y  xrootd \
                    python3-xrootd \
                    xrootd-devel

RUN mkdir -p libs
RUN git clone https://github.com/open-source-parsers/jsoncpp.git ./libs/jsoncpp
RUN mkdir -p ./libs/jsoncpp/build && cd ./libs/jsoncpp/build && cmake -DBUILD_STATIC_LIBS=ON  .. && make && make install

COPY . .