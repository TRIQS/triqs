FROM centos:7

RUN yum -y install centos-release-scl epel-release && yum -y update
RUN yum -y install \
      blas-devel \
      boost-devel \
      cmake \
      devtoolset-7-gcc-c++ \
      fftw-devel \
      gcc-c++ \
      gcc-gfortran \
      git \
      gmp-devel \
      h5py \
      hdf5-devel \
      lapack-devel \
      lapack-devel \
      make \
      mpi4py-openmpi \
      numpy \
      openmpi-devel \
      python-devel \
      python-jinja2 \
      python-mako \
      python-matplotlib \
      python-tornado \
      python-virtualenv \
      python-zmq \
      scipy

ENV PATH=/opt/rh/devtoolset-7/root/usr/bin:${PATH}:/usr/lib64/openmpi/bin PYTHONPATH=/usr/lib64/python2.7/site-packages/openmpi

ENV SRC=/work/src BUILD=/work/build INSTALL=/work/install

# cpp2py
RUN git clone https://github.com/TRIQS/cpp2py ${SRC}/cpp2py
WORKDIR ${BUILD}/cpp2py
RUN cmake ${SRC}/cpp2py -DCMAKE_INSTALL_PREFIX=${INSTALL} && make && make install
