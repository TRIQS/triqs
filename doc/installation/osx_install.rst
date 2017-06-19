.. index:: install_osx

.. highlight:: bash

.. _install_osx:

Installing required libraries on Mac OS X [EXPERIMENTAL]
==============================================================

Disclaimer
-------------

Here we provide the installation instructions in order to use TRIQS on OS X.

While the installation of TRIQS itself is as straightforward as on Linux systems,
the installation of the standard scientific libraries used by TRIQS
(mpi, hdf5, boost, fftw, ipython, ....) is not as simple as in e.g. Debian/Ubuntu,
where they are packaged with the distribution itself (i.e. "part of the system").

This general problem of scientific computing on OS X is clearly illustrated by the
large number of third-party attempts to provide "easy" installation of scientific libraries,
such as brew, macports, fink, or the Enthought Python distribution.

In our experience, none of these solutions is perfect, nor complete or stable: they are still
very far from the quality and stability of a Linux distribution like Debian/Ubuntu.
It *almost* works, but at the end, there are some issues, sometimes severe, in the way
scientific librairies are installed.

Moreover, because there is no notion of "distribution", the versions of the libraries are
always changing e.g. in brew. As a result, the installation instructions may work on one day,
and suddenly stop to work the day after.

While this has a priori **nothing to do with TRIQS** and its applications, it clearly impacts its installation and usage.
We are looking for a more robust solution to this OS X installation mess; **help welcome !**.

Installation of the dependencies
--------------------------------

The following installation instructions are for Mac OS X 10.9 (Mavericks). They may work (possibly with some
modifications) for earlier versions of OS X.

1. Install `homebrew <http://mxcl.github.io/homebrew/>`_.

  Run ``brew doctor`` and resolve potential conflicts before continuing.

2. We recommend using the clang compiler. When using it the first time, you will be prompted to
   install the clang command line tools. After this is done, proceed to step 3.

3. Install several packages which are needed: ::

     brew tap homebrew/science
     brew install cmake
     brew install --with-mpi --with-python --without-single boost
     brew install hdf5
     brew install gsl
     brew install gmp
     brew install fftw
     brew install open-mpi
     brew install zmq
     brew install python

5. Install the required python packages: ::

    pip install numpy
    pip install --no-binary=h5py h5py
    pip install scipy
    pip install --no-binary=mpi4py mpi4py
    pip install matplotlib
    pip install tornado
    pip install pyzmq
    pip install jinja2
    pip install ipython
    pip install mako

6. If you wish to compile the documentation locally, install sphinx, its dependencies, libclang Python bindings, and MathJax: ::

     pip install sphinx
     pip install pyparsing
     pip install clang
     git clone https://github.com/mathjax/MathJax.git MathJax

Possible issues
---------------

* If you experience ImportErrors in Python, make sure that ::

    /usr/local/bin

  occurs before /usr/bin in your path. Otherwise you may be using the system's default version of Python.
  As a result, some packages, although previously installed with ``pip install``, may not be found.

* If you encounter the following error: ::

    /usr/local/include/ft2build.h:56:38: error: freetype/config/ftheader.h: No such file or directory

  in the installation of matplotlib, you need to pass the proper include path. Locate the freetype directory
  with the header file and pass the include path through ``CPPFLAGS``: ::

    CPPFLAGS=-I/usr/X11/include/freetype2/ pip install git+https://github.com/matplotlib/matplotlib.git#egg=matplotlib-dev

* When building the doc, you may experience the following sphinx error: ::

    ValueError: unknown locale: UTF-8

  In this case, make sure to add the following lines to your ``.bash_profile``: ::

    export lc_all=en_us.utf-8
    export lang=en_us.utf-8







