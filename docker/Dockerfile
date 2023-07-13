FROM xrootd-base-image:latest

COPY . .

RUN mkdir -p build
RUN cd build && cmake .. && make
