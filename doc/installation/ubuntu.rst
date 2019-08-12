.. index:: ubuntu

.. _Ubuntu :

.. highlight:: bash

Installing required libraries on Ubuntu
===================================================

TRIQS has been installed and tested on Ubuntu 16.04 and later. Earlier versions are not supported.

Install the following packages which are necessary to build TRIQS and use it::

  sudo apt-get install cmake g++ gfortran git hdf5-tools \
       libblas-dev libboost-all-dev libfftw3-dev libgfortran3 \
       libhdf5-serial-dev libgmp-dev liblapack-dev libopenmpi-dev \
       python-dev python-h5py python-mako python-matplotlib \
       python-mpi4py python-numpy python-scipy

If you wish to use the ipython/jupyter notebook with triqs, you need to install additional packages depending on your Ubuntu version.

* For Ubuntu versions 16.04 and 16.10 please install::

    sudo apt-get install ipython ipython-notebook 

* For Ubuntu 17.04 and later please install::

    sudo apt-get install python-notebook 


C++ compiler
------------

The default compiler on e.g. Ubuntu 16.04 (xenial) is gcc 5.4.0, which is too old to compile the latest version of TRIQS.

.. note::
        The instructions below address Ubuntu 16.04 (xenial). In Ubuntu 18.04 (bionic), please consider using the respective standard apt packages instead.

We recommend to upgrade to gcc version 7 using the Ubuntu Toolchain test builds with the following commands::

    sudo apt-get install software-properties-common
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install g++-7
  
In a second step, set the C/C++ compiler for your :ref:`environment <environment_vars>`::

    export CC=gcc-7
    export CXX=g++-7

You can make this choice permanent by adding these two lines to your ``$HOME/.bash_profile``.

.. _ubuntu_cpp2py:
C++2py
------

For full functionality of the C++2Py Tool, a recent version of the libclang library is required.
While a recent version is packaged with Ubuntu 18.04 (bionic), more steps are required on Ubuntu 16.04 (xenial).

* As a first step you will need to install the g++-7 package as described in the first set of commands above.

* In a second step we install version 6.0 of clang and libclang from the `apt.llvm.org <apt.llvm.org>`_ repositories::
  
    source /etc/lsb-release
    sudo add-apt-repository 'deb http://apt.llvm.org/$DISTRIB_CODENAME/ llvm-toolchain-$DISTRIB_CODENAME-6.0 main' -y
    curl -L https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo apt-get update && sudo apt-get install clang-6.0 libclang-6.0-dev python-clang-6.0

If you are using the Ubuntu 16.04 (xenial) :ref:`debian packages <ubuntu_debian>`, you should permanently set the following :ref:`environment variables <environment_vars>`::

    export LIBCLANG_LOCATION=/usr/lib/llvm-6.0/lib/libclang.so
    export LIBCLANG_CXX_FLAGS="-I/usr/include/x86_64-linux-gnu/c++/7"

.. _ubuntu_issues:
Issues
------

If you encounter compile-time error messages such as::

        fatal error: mpi.h: No such file or directory
        fatal error: H5public.h: No such file or directory
        
make sure that the include directories of your openmpi and hdf5 installation are in the CPLUS_INCLUDE_PATH :ref:`environment variable <environment_vars>`::

        export CPLUS_INCLUDE_PATH=/usr/include/openmpi:/usr/include/hdf5/serial/:$CPLUS_INCLUDE_PATH
