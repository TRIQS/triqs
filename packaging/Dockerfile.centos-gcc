FROM centos:7

RUN yum -y install centos-release-scl epel-release && yum -y update
RUN yum -y install \
      blas-devel \
      boost-devel \
      cmake3 \
      devtoolset-7-gcc-c++ \
      fftw-devel \
      gcc-c++ \
      gcc-gfortran \
      git \
      gmp-devel \
      h5py \
      hdf5-devel \
      lapack-devel \
      make \
      mpi4py-openmpi \
      numpy \
      openmpi-devel \
      python-devel \
      python-mako \
      python-matplotlib \
      python-virtualenv \
      scipy

RUN ln -s cmake3 /usr/bin/cmake
ENV PATH=/opt/rh/devtoolset-7/root/usr/bin:${PATH}:/usr/lib64/openmpi/bin PYTHONPATH=/usr/lib64/python2.7/site-packages/openmpi
ENV CC=/opt/rh/devtoolset-7/root/usr/bin/gcc CXX=/opt/rh/devtoolset-7/root/usr/bin/g++
