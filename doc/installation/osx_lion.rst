.. index:: install_on_osx_lion

.. _install_on_osx_lion:
.. highlight:: bash

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

     #AFTER THE FIX IN BOOST AND BREW : IN PROGRESS
     #brew install boost --without-single --with-mpi --with-c++11
     
4. Now install virtualenv: ::

    pip install virtualenv
    virtualenv mypython

Using virtualenv provides a clean way to set up a different python environment for each user.
The first line in the instructions above installs the package and the second creates the virtual
environment, which is created in ``$HOME/mypython``.
Make sure to permanently add (prepend) ``$HOME/mypython/bin`` to your path by adding ::

    export PATH = $HOME/mypython/bin:$PATH

to your ``.bashrc file`` (assuming bash). Within a new shell, check that

    which python
    which pip
    which easy_install

yield the ones located in ``$HOME/mypython``.

5. Install the required python packages: ::
    
    pip install numpy
    pip install h5py
    pip install scipy
    pip install mpi4py
    pip install matplotlib
    #pip install git+https://github.com/matplotlib/matplotlib.git#egg=matplotlib-dev
    pip install tornado
    pip install pyzmq
    pip install jinja2
    pip install ipython
    pip install cython


6. If you wish to compile the documentation locally, install sphinx, its dependencies and mathjax: :: 
  
     pip install sphinx
     easy_install pyparsing==1.5.7
     git clone git://github.com/mathjax/MathJax.git MathJax

NB : you need pyparsing <=1.5.7 since apparently v.2.0 works only for python 3.

7. Download the latest `sources of boost <http://www.boost.org/users/download/>`_  and untar them into a directory ``BOOST_SRC``.

Possible issues
---------------

If you encounter the following error: ::

    /usr/local/include/ft2build.h:56:38: error: freetype/config/ftheader.h: No such file or directory

in the installation of matplotlib, you need to pass the proper include path. Locate the freetype directory
with the header file and pass the include path through ``CPPFLAGS``: ::

    CPPFLAGS=-I/usr/X11/include/freetype2/ pip install git+https://github.com/matplotlib/matplotlib.git#egg=matplotlib-dev






