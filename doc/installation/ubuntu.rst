.. index:: ubuntu

.. _Ubuntu :

.. highlight:: bash

Installing required libraries on Ubuntu
=======================================

TRIQS has been installed and tested on Ubuntu 12.04 and 13.04.  Note that there
is a bug in Ubuntu 12.10 (the hdf5 library has been compiled without hdf5
support) so if you are using that version you will have to recompile the hdf5
library. The problem has been reported and is fixed in 13.04.

Install the following packages which are necessary to build TRIQS and use it::

  sudo apt-get install cmake git g++ libgfortran3 gfortran openmpi-bin openmpi-common \
       openmpi-checkpoint libopenmpi-dev libblas-dev liblapack-dev libfftw3-dev libgmp-dev \
       hdf5-tools libhdf5-serial-dev python-h5py libboost1.48-all-dev python-dev \
       python-numpy python-scipy python-virtualenv python-matplotlib doxygen\
       python-tornado python-zmq python-mpi4py

Optionally, you may be interested in:

 * How to *simply* upgrade the ipython notebook with :ref:`virtualenv <virtualenv>` to the latest version?

 * How to use the nice and recommended :ref:`clang <install_clang>` C++ compiler?

..
  * How to use :ref:`Intel <icc>` C++ compiler?

If you wish to compile the documentation locally, you additionally need to install the following package: ::

    pip install sphinxcontrib-doxylink

