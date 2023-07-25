FROM almalinux:8
MAINTAINER Sarun Nuntaviriyakul <sarun.nuntaviriyakul@cern.ch>
LABEL org.opencontainers.image.source https://github.com/guyzsarun/xrootd-cmsjson

WORKDIR /xrootd

RUN yum -y update && \
    yum install -y dnf \
        https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm \
        https://repo.opensciencegrid.org/osg/3.6/osg-3.6-el8-release-latest.rpm \
        epel-release

RUN dnf group install -y "Development Tools"

# CMS json dependency
RUN yum install -y  cmake \
                    wget \
                    xrootd \
                    python3-devel \
                    python3-xrootd \
                    xrootd-devel \
                    https://cbs.centos.org/kojifiles/packages/jsoncpp/1.9.4/4.el8s/x86_64/jsoncpp-1.9.4-4.el8s.x86_64.rpm  \
                    https://cbs.centos.org/kojifiles/packages/jsoncpp/1.9.4/4.el8s/x86_64/jsoncpp-devel-1.9.4-4.el8s.x86_64.rpm

RUN pip3 install pytest

COPY . .

RUN mkdir -p build
RUN cd build && cmake .. && make && make install