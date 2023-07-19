FROM xrootd-base-image:latest
LABEL org.opencontainers.image.source https://github.com/guyzsarun/xrootd-cmsjson

COPY . .

RUN mkdir -p build
RUN cd build && cmake .. && make && make install