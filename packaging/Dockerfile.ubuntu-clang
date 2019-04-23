FROM ubuntu:bionic
ARG RELEASE=bionic
ARG LLVM=6.0

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
      clang-${LLVM} \
      cmake \
      g++ \
      gfortran \
      git \
      hdf5-tools \
      libblas-dev \
      libboost-all-dev \
      libclang-${LLVM}-dev \
      libfftw3-dev \
      libgfortran3 \
      libgmp-dev \
      libhdf5-dev \
      liblapack-dev \
      libopenmpi-dev \
      openmpi-bin \
      openmpi-common \
      openmpi-doc \
      python-clang-${LLVM} \
      python-dev \
      python-h5py \
      python-mako \
      python-matplotlib \
      python-mpi4py \
      python-numpy \
      python-scipy \
      python-sphinx \
      python-nbsphinx

ENV CC=clang-${LLVM} CXX=clang++-${LLVM}

ADD http://releases.llvm.org/6.0.1/libcxx-6.0.1.src.tar.xz /tmp/
ADD http://releases.llvm.org/6.0.1/libcxxabi-6.0.1.src.tar.xz /tmp/
RUN mkdir /tmp/build && cd /tmp/build && \
      tar -C /tmp -xf /tmp/libcxx-6.0.1.src.tar.xz && \
      tar -C /tmp -xf /tmp/libcxxabi-6.0.1.src.tar.xz && \
      cmake /tmp/libcxxabi-6.0.1.src -DLLVM_CONFIG_PATH=/usr/bin/llvm-config-${LLVM} -DCMAKE_INSTALL_PREFIX=/usr/lib/llvm-${LLVM} -DLIBCXXABI_LIBCXX_PATH=/tmp/libcxx-6.0.1.src && make -j2 && make install && \
      rm -rf * && \
      cmake /tmp/libcxx-6.0.1.src -DLLVM_CONFIG_PATH=/usr/bin/llvm-config-${LLVM} -DCMAKE_INSTALL_PREFIX=/usr/lib/llvm-${LLVM} -DLIBCXX_CXX_ABI=libcxxabi -DLIBCXX_CXX_ABI_INCLUDE_PATHS=/tmp/libcxxabi-6.0.1.src/include && make -j2 install && \
      rm -rf /tmp/libcxx* /tmp/build
ENV CXXFLAGS="-stdlib=libc++" LD_LIBRARY_PATH=/usr/lib/llvm-${LLVM}/lib
