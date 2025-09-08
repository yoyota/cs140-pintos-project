# This file is derived from https://github.com/YoungWilliamZ/Docker-for-Pintos/blob/master/Dockerfile
FROM ubuntu:16.04

ENV HOME=/root
ENV PATH=/pintos/utils:$PATH
ENV GDBMACROS=$HOME/pintos/src/misc/gdb-macros
USER root
WORKDIR ${HOME}

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    gdb \
    xorg-dev \
    bison \
    libgtk2.0-dev \
    psmisc \
    locales \
    locales-all \
    wget \
    qemu-system-x86

RUN ln -s /usr/bin/qemu-system-i386 /usr/bin/qemu

RUN apt-get install -y \
    libncursesw5 \
    libncurses5-dev

COPY ./src/misc /misc
RUN cd /misc && \
    sh ./bochs-2.6.11-build.sh /usr/local


RUN wget https://bootstrap.pypa.io/pip/2.7/get-pip.py && \
    python get-pip.py && \
    pip install python-engineio==3.11.2 && \
    pip install python-socketio==4.4.0 && \
    pip install gdbgui==0.12.0 Brotli==1.0.9


COPY ./Dockerfile /root

WORKDIR /pintos
