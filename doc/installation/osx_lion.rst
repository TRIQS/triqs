.. index:: install_osx_lion

.. highlight:: bash

.. _install_on_osx_lion:

Installing required libraries on Mac OS X [EXPERIMENTAL]
==============================================================

Disclaimer
-------------

We provide here some instructions to install and use TRIQS on OS X.

While the installation of TRIQS itself is as straightforward as on Linux systems, 
the installation of the standard scientific libraries used by TRIQS 
(mpi, hdf5, boost, fftw, ipython, ....) is not as simple as in e.g. Debian/Ubuntu, 
where they are packaged with the distribution itself (i.e. "part of the system").

This general problem of scientific computing on OS X is clearly
illustrated by the large number of third-party attempts
to provide "easy" installation of scientific libraries : 
brew, macports, fink, Enthought Python distribution.

In our experience, none of this solution is perfect, nor complete or stable : they are still 
very far from the quality and stability of a Linux distribution like Debian/Ubuntu.
It *almost* works, but at the end, there are some issues, sometimes severe,
in the way scientific librairies are installed.
(e.g. currently the default version of mpi and hdf5 installed by brew are in conflict : 
the simple mpi "Hello World"  crashes when linked with hdf5_cpp).

Moreover, because there is no notion of "distribution" (except in Enthought, which unfortunately is incomplete),
the versions of the libraries are always changing e.g. in brew.
As a result, the installation instructions may work on one day, and suddenly stop to work
the day after.

While this has a priori **nothing to do with TRIQS** and its applications, it clearly impacts its installation and usage.
We are looking for a more robust solution to this OS X installation mess; **help welcome !**.

In the following, we describe an installation procedure which worked (at least one day),
on 10.8 and 10.9 (at least on the Mac of one of the developer !).


Installation of the dependencies
--------------------------------

We describe an installation procedure which is known to have worked at least one day,
on 10.8 and 10.9, (at least on the Mac of one of the developer !).

1. Install `homebrew <http://mxcl.github.io/homebrew/>`_.

  Run ``brew doctor`` and resolve potential conflicts before continuing.

2. Install XCode (directly from the Mac store). In Preferences/Downloads, install "Command Line tools".

3. Install several packages which are needed: ::
        
     brew tap homebrew/science  
     brew install cmake
     brew install gfortran
     brew install  --enable-cxx hdf5 
     brew install gsl
     brew install fftw
     brew install open-mpi
     brew install zmq
     brew install python

     #brew formula has been repaired, since boost installation of mpi.python is a complete mess
     #which needs to be fixed manually (except in Debian/Ubuntu where it is correct).
     ### brew install boost --without-single --with-mpi --with-c++11
     brew install  http://ipht.cea.fr/triqs/formulas/boost.rb  --without-single --with-mpi --with-c++11 -v

5. Install the required python packages: ::
    
    pip install numpy
    pip install h5py
    pip install scipy
    pip install mpi4py
    pip install matplotlib
    pip install breathe
    pip install sphinxcontrib-doxylink
    pip install tornado
    pip install pyzmq
    pip install jinja2
    pip install ipython

6. If you wish to compile the documentation locally, install sphinx, its dependencies and mathjax: :: 
  
     brew install doxygen
     pip install sphinx
     easy_install pyparsing==1.5.7
     git clone https://github.com/mathjax/MathJax.git MathJax

NB : you need pyparsing <=1.5.7 since apparently v.2.0 works only for python 3.

7. If you wish to build the documentation locally, 
   configure TRIQS with the option -DPython_use_mpi4py=ON    (workaround boost.mpi.python bug).

8. **Set up** the environment variable, e.g. in your ~/.bash_profile (workaround for issue #43) ::
     
     export HDF5_DEBUG="all"

    or your code will crash when launched without mpirun 
    (due to a bug in hdf5 C++/ openmpi, nothing to do with TRIQS, so we can not fix it). 

Possible issues
---------------

* If you encounter the following error: ::

    /usr/local/include/ft2build.h:56:38: error: freetype/config/ftheader.h: No such file or directory

  in the installation of matplotlib, you need to pass the proper include path. Locate the freetype directory
  with the header file and pass the include path through ``CPPFLAGS``: ::

    CPPFLAGS=-I/usr/X11/include/freetype2/ pip install git+https://github.com/matplotlib/matplotlib.git#egg=matplotlib-dev






