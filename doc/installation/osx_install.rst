.. index:: install_osx

.. highlight:: bash

.. _install_osx:

Installing required libraries on Mac OS X
=========================================

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

  Run ``brew doctor`` to resolve potential conflicts before continuing.
  Consider further to remove old versions your packages with the command ``brew cleanup``.

2. We recommend using the clang compiler. When using it the first time, you will be prompted to
   install the clang command line tools. After this is done, proceed to step 3.
   The following two commands will install the most recent clang compiler and load it into your environment: ::
     
     brew install llvm
     brew link --force llvm

   As a next step we make sure the C/C++ compiler is set for your current :ref:`environment <environment_vars>`::

     export CC=clang
     export CXX=clang++

  To make this choice permanent, add these two lines to your ``$HOME/.bash_profile``.

3. Install several packages which are needed: ::

     brew install cmake
     brew install --without-single boost
     brew install hdf5
     brew install gsl
     brew install gmp
     brew install fftw
     brew install open-mpi
     brew install zmq
     brew install python@2

4. Install the required python packages: ::

    pip install numpy
    pip install --no-binary=h5py h5py
    pip install scipy
    pip install --no-binary=mpi4py mpi4py
    pip install matplotlib
    pip install tornado
    pip install pyzmq
    pip install jinja2
    pip install mako

4. If desired, install the `Jupyter <https://jupyter.org/>`_ Notebook using::

    pip install jupyter
