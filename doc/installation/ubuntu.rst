.. index:: ubuntu

.. _Ubuntu :

.. highlight:: bash

Installing required libraries on Ubuntu 14.04LTS
===================================================

TRIQS has been installed and tested on Ubuntu 14.04. Earlier versions are not supported.

Install the following packages which are necessary to build TRIQS and use it::

  sudo apt-get install libboost-all-dev cmake git g++ libgfortran3 gfortran openmpi-bin openmpi-common \
       openmpi-checkpoint libopenmpi-dev libblas-dev liblapack-dev libfftw3-dev libgmp-dev \
       hdf5-tools libhdf5-serial-dev python-h5py python-dev python-numpy python-scipy python-jinja2 \
       python-virtualenv python-matplotlib python-tornado python-zmq python-mpi4py python-mako \


* If you wish to *simply* upgrade the ipython notebook to the latest version,
  use :ref:`virtualenv <virtualenv>`.


C++ compiler [developers only]
---------------------------------

The default compiler on  Ubuntu 14.04LTS is gcc 4.8.1, which compiles TRIQS in C++11 mode only.

This is sufficient for a basic usage of the library.

More advanced users, in particular developers, however should rather use a C++14 compiler since:

* it leads to better error messages in many cases in C++ (due to the triqs/clef library to be precise).

* a few new parts of the library (in alpha stage) are C++14 only.

There are two options:


* Upgrade the gcc to 4.9.2 in Ubuntu 14.04, using the official package, which can be easily done with the commands::

    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install g++-4.9

    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.9
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 40 --slave /usr/bin/g++ g++ /usr/bin/g++-4.8

  This procedure installs gcc 4.9, and (in the last lines) sets up the default compiler (g++, gcc) to point 
  to the 4.9 version. The TRIQS developers uses this routinely on several machines, it does not affect the rest of the distribution.

* Install clang 3.6, as packaged in Ubuntu.
  

Building the documentation
-------------------------------

* To build the complete documentation, you need to run c++2doc.py, hence the libclang tool with the clang compiler::

    sudo apt-get install clang-3.6 clang-format-3.6  libclang-3.6-dev  libclang-common-3.6-dev libclang1-3.6:amd64   python-clang-3.6


