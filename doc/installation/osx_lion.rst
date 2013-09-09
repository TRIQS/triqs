.. index:: install_osx_lion

.. highlight:: bash

.. _install_on_osx_lion:

Installing required libraries on Mac OS X
=========================================

This installation guide has been tried on Mountain Lion. It may work for older
versions of Mac OS X but previous versions of Mac OS X are not supported.

NB: The installation of TRIQS under previous versions of OS X requires installing clang (via Xcode).
(On Mountain Lion, clang (llvm) replaces gcc 4.2 as the default C++ compiler).

We strongly recommend the following installation procedure, which provides a clean way to set up all dependencies, so that all
of them are compatible with each other. Only the installation via homebrew is supported for the Mac.

Installation of the dependencies
--------------------------------

1. Install `homebrew <http://mxcl.github.io/homebrew/>`_.

  Run ``brew doctor`` and resolve potential conflicts before continuing.

2. Install XCode (directly from the Mac store). In Preferences/Downloads, install "Command Line tools".

3. Install several packages which are needed: ::
         
     brew install cmake
     brew install gfortran
     brew install  --enable-cxx hdf5 
     brew install gsl
     brew install fftw
     brew install open-mpi
     brew install zmq
     brew install python
     brew install doxygen
     
     #brew formula has been repaired. Temporary using our own
     #until this is back in the master.
     #When 1.55 is out, the regular brew formula should work again ...
     ### brew install boost --without-single --with-mpi --with-c++11
     brew install  http://ipht.cea.fr/triqs/boost.rb  --without-single --with-mpi --with-c++11 -v

5. Install the required python packages: ::
    
    pip install numpy
    pip install h5py
    pip install scipy
    pip install mpi4py
    pip install matplotlib
    pip install tornado
    pip install pyzmq
    pip install jinja2
    pip install ipython
    pip install cython
    # strangely, cython is not in bin, hence not in the path...
    pushd /usr/local/bin && ln -s /usr/local/share/python/cython . && popd

6. If you wish to compile the documentation locally, install sphinx, its dependencies and mathjax: :: 
  
     pip install sphinx
     easy_install pyparsing==1.5.7
     git clone https://github.com/mathjax/MathJax.git MathJax

NB : you need pyparsing <=1.5.7 since apparently v.2.0 works only for python 3.


Possible issues
---------------

If you encounter the following error: ::

    /usr/local/include/ft2build.h:56:38: error: freetype/config/ftheader.h: No such file or directory

in the installation of matplotlib, you need to pass the proper include path. Locate the freetype directory
with the header file and pass the include path through ``CPPFLAGS``: ::

    CPPFLAGS=-I/usr/X11/include/freetype2/ pip install git+https://github.com/matplotlib/matplotlib.git#egg=matplotlib-dev






