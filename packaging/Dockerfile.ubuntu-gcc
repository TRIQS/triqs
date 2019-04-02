FROM ubuntu:bionic
ARG RELEASE=bionic

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
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
      libhdf5-dev \
      liblapack-dev \
      libopenmpi-dev \
      openmpi-bin \
      openmpi-common \
      openmpi-doc \
      python-dev \
      python-h5py \
      python-mako \
      python-matplotlib \
      python-mpi4py \
      python-numpy \
      python-scipy

ENV CC=gcc CXX=g++
