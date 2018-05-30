.. index:: ubuntu

.. _Ubuntu :

.. highlight:: bash

Installing required libraries on Ubuntu
===================================================

TRIQS has been installed and tested on Ubuntu 16.04 and later. Earlier versions are not supported.

Install the following packages which are necessary to build TRIQS and use it::

  sudo apt-get install libboost-all-dev cmake git g++ libgfortran3 gfortran openmpi-bin \
       openmpi-common openmpi-doc libopenmpi-dev libblas-dev liblapack-dev libfftw3-dev \
       libgmp-dev hdf5-tools libhdf5-serial-dev python-h5py python-dev python-numpy \ 
       python-scipy python-jinja2 python-virtualenv python-matplotlib python-tornado \
       python-zmq python-mpi4py python-mako

If you wish to use the ipython/jupyter notebook with triqs, you need to install additional packages depending on your Ubuntu version.

* For Ubuntu versions 16.04 and 16.10 please install::

    sudo apt-get install ipython ipython-notebook 

* For Ubuntu 17.04 and later please install::

    sudo apt-get install python-notebook 


C++ compiler
------------

The default compiler on e.g. Ubuntu 16.04 LTS is gcc 5.4.0, which is too old to compile the latest version of TRIQS.

There are two options:

* Upgrade gcc to version 7 using the Ubuntu Toolchain test builds with the following commands::

    sudo apt-get install software-properties-common
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install g++-7
  
  In a next step, set the C/C++ compiler for your :ref:`environment <environment_vars>`::

    export CC=gcc-7
    export CXX=g++-7

  To make this choice permanent, add these two lines to your ``$HOME/.bash_profile``.

.. _clang_ubuntu :
* When developing applications with TRIQS, we recommend to use the clang compiler instead.
  As a first step you will need to install the g++-7 package as described in the first set of commands above.
  In a next step we install version 5.0 of clang and libclang.::
  
    source /etc/lsb-release
    sudo add-apt-repository 'deb http://apt.llvm.org/$DISTRIB_CODENAME/ llvm-toolchain-$DISTRIB_CODENAME-5.0 main' -y
    curl -L https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo apt-get update && sudo apt-get install clang-5.0 libclang-5.0-dev python-clang-5.0
    
  Finally, we set the C/C++ compiler::

    export CC=clang-5.0
    export CXX=clang++-5.0

  To make this choice permanent, add these two lines to your ``$HOME/.bash_profile``.
