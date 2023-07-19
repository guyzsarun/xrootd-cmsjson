FROM ghcr.io/guyzsarun/xrootd-cmsjson:base
LABEL org.opencontainers.image.source https://github.com/guyzsarun/xrootd-cmsjson

COPY . .

RUN mkdir -p build
RUN cd build && cmake .. && make && make install