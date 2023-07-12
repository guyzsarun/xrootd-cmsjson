FROM almalinux:8

RUN yum -y update
RUN dnf group install -y "Development Tools"

COPY . .