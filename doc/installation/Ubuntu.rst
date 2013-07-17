.. index:: Ubuntu

.. _Ubuntu :

.. highlight:: bash

Installation on Ubuntu (12.04 LTS)
####################################################

TRIQS can be installed on Ubuntu 12.04 LTS.

NB : Ubuntu 12.10: there is a bug in Ubuntu 12.10 (hdf5 compiled without hdf5 support).
On Ubuntu 12.10, you have to recompile hdf5. The problem has been reported and will be fixed in 13.04.

#. Install the following packages which are necessary to build TRIQS and use it::

     sudo apt-get install cmake git g++ libgfortran3 gfortran openmpi-bin openmpi-common \
       openmpi-checkpoint libopenmpi-dev libblas-dev liblapack-dev libfftw3-dev libgmp-dev \
       hdf5-tools libhdf5-serial-dev python-h5py libboost1.48-all-dev python-dev \
       python-numpy python-scipy python-virtualenv python-matplotlib doxygen\
       python-tornado python-zmq

#. Download the code::

     git clone git://github.com/TRIQS/TRIQS.git path_to_triqs_sources

#. Create a build directory (different from the TRIQS source directory nor a subdirectory thereof) ::

    mkdir triqs_build && cd triqs_build

#. Configure ::

    cmake path_to_triqs_sources -DCMAKE_INSTALL_PREFIX=path_to_install_directory

   NB: the install directory is optional and defaults to `triqs_build/INSTALL_DIR`. It is
   however usually a good idea to have an install directory outside `triqs_build` (after
   a successful installation `triqs_build` can be deleted). The cmake call should end with::

     -- Build files have been written to: *path_to_triqs_build*

   OPTIONAL: To install the documentation, install Sphinx and doxygen: ::

    sudo easy_install -U Sphinx
    sudo apt-get install doxygen

   and add::

    -DBuild_Documentation=ON

   to the above cmake command.

#. Build TRIQS, test it and install it with (N being the number of cores of your machine) ::

    make -jN   && make test && make -jN install

   NB: the tests SingleSite and CDMFT-v2 take longer than the other ones (30s to 1 min).

#. If you use Wien2TRIQS, please complete the installation as described :ref:`here <wien2k_inst>`.




#. You can now :ref:`start ... <get_started>`

#. Optionally, you may be interested in:

 * How to *simply* upgrade the ipython notebook with :ref:`virtualenv <virtualenv>` to the latest version?

 * How to use the nice :ref:`clang <clang>` C++ compiler?

 * How to use :ref:`Intel <icc>` C++ compiler?

.. warning:: For the branch 1.0, you need the latest version of Cython (>=.17, downloadable on Cython.org). If Cython is not found, specify the location of the executable with the option -DCYTHON_EXECUTABLE. 
