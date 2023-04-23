# This file is derived from https://github.com/YoungWilliamZ/Docker-for-Pintos/blob/master/Dockerfile
FROM ubuntu:16.04

ENV HOME /root
ENV PATH /pintos/utils:$PATH
ENV GDBMACROS $HOME/pintos/src/misc/gdb-macros
USER root
WORKDIR ${HOME}

RUN apt-get update && \
    apt-get install -y \
    curl \
    build-essential \
    gdb \
    xorg-dev \
    bison \
    libgtk2.0-dev \
    psmisc \
    locales \
    locales-all \
    qemu

RUN ln -s /usr/bin/qemu-system-i386 /usr/bin/qemu

RUN curl -L --output bochs-2.6.7.tar.gz https://downloads.sourceforge.net/project/bochs/bochs/2.6.7/bochs-2.6.7.tar.gz && \
    tar -zxvf bochs-2.6.7.tar.gz && \
    cd bochs-2.6.7 && \
    ./configure --with-nogui --enable-gdb-stub && \
    make && \
    make install && \
    cd ../ && \
    rm -fr bochs-2.6.7 && \
    rm bochs-2.6.7.tar.gz

RUN curl "https://bootstrap.pypa.io/pip/2.7/get-pip.py" -o "get-pip.py" && \
    python get-pip.py && \
    pip install python-engineio==3.11.2 && \
    pip install python-socketio==4.4.0 && \
    pip install gdbgui==0.12.0


RUN echo "export PATH=$PATH:/pintos/utils" >> $HOME/.bashrc

COPY ./Dockerfile /root

WORKDIR /pintos
