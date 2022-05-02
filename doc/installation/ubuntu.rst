.. index:: ubuntu

.. _Ubuntu :

.. highlight:: bash

Installing required libraries on Ubuntu
=======================================

TRIQS has been installed and tested on Ubuntu 20.04 and later. Earlier versions are not supported.

Install the following packages which are necessary to build TRIQS and use it::

  sudo apt-get install cmake g++-10 gfortran git hdf5-tools \
       libblas-dev libboost-dev libfftw3-dev libgfortran5 \
       libgmp-dev libhdf5-dev liblapack-dev libopenmpi-dev \
       python3-dev python3-mako python3-matplotlib \
       python3-mpi4py python3-numpy python3-scipy

If you wish to use the jupyter notebook with triqs you can install it using::

    sudo apt-get install jupyter-notebook

.. _ubuntu_clang:

For full functionality of the C++2Py Tool, please install clang and the libclang library::

    sudo apt-get install clang libclang-dev


.. _ubuntu_issues:
Issues
------

If you encounter compile-time error messages such as::

        fatal error: mpi.h: No such file or directory
        fatal error: H5public.h: No such file or directory
        
make sure that the include directories of your openmpi and hdf5 installation are in the CPLUS_INCLUDE_PATH :ref:`environment variable <environment_vars>`::

        export CPLUS_INCLUDE_PATH=/usr/include/openmpi:/usr/include/hdf5/serial/:$CPLUS_INCLUDE_PATH
