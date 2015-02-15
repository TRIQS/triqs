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
       python-virtualenv python-matplotlib doxygen python-tornado python-zmq python-mpi4py python-mako \


* If you wish to *simply* upgrade the ipython notebook to the latest version, 
  use :ref:`virtualenv <virtualenv>`.


C++ compiler [developers only]
---------------------------------

The default compiler on  Ubuntu 14.04LTS is gcc 4.8.1, which compiles TRIQS properly in C++11 mode.

This is sufficient for most users.

Developers however should rather use a C++14 compiler since : 

* it leads to better error messages in many cases in C++ (due to the triqs/clef library to be precise).

* a few new parts of the library (in alpha stage) are C++14 only.

There are two options:

* Install clang 3.5, as packaged in Ubuntu.
 
* Upgrade the gcc to 4.9.2 in Ubuntu 14.04, which can be easily done with the commands::

    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install g++-4.9
    
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.9
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 40 --slave /usr/bin/g++ g++ /usr/bin/g++-4.8


Building the documentation
-------------------------------

* If you wish to compile the documentation locally, you additionally need to install the following package: ::

    sudo pip install sphinxcontrib-doxylink

