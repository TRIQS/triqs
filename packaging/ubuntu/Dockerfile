FROM ubuntu:xenial
# TODO: move RELEASE ARG above FROM -- requires newer docker
ARG RELEASE=xenial

RUN apt-get update && apt-get install -y \
      cmake \
      g++ \
      gfortran \
      git \
      hdf5-tools \
      libblas-dev \
      libboost-all-dev \
      libfftw3-dev \
      libgfortran3 \
      libgmp-dev \
      libhdf5-serial-dev \
      liblapack-dev \
      libopenmpi-dev \
      openmpi-bin \
      openmpi-common \
      openmpi-doc \
      python-dev \
      python-h5py \
      python-jinja2 \
      python-mako \
      python-matplotlib \
      python-mpi4py \
      python-numpy \
      python-scipy \
      python-tornado \
      python-virtualenv \
      python-zmq \
      software-properties-common

RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get update && apt-get install -y \
  g++-7
ENV CC=gcc-7 CXX=g++-7

ARG CLANG=5.0
RUN add-apt-repository "deb http://apt.llvm.org/${RELEASE}/ llvm-toolchain-${RELEASE}-${CLANG} main" -y
RUN apt-key adv --fetch-keys http://apt.llvm.org/llvm-snapshot.gpg.key
RUN apt-get update && apt-get install -y \
      clang-${CLANG} \
      libclang-${CLANG}-dev \
      python-clang-${CLANG}
ENV CC=clang-${CLANG} CXX=clang++-${CLANG}

ENV SRC=/work/src BUILD=/work/build INSTALL=/work/install

# cpp2py
RUN git clone https://github.com/TRIQS/cpp2py ${SRC}/cpp2py
WORKDIR ${BUILD}/cpp2py
RUN cmake ${SRC}/cpp2py -DCMAKE_INSTALL_PREFIX=${INSTALL} && make && make install
