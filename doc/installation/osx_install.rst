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

The following installation instructions are for Mac OS X 10.14.3 (Mojave) and should be executed on the terminal.
They may work (possibly with some modifications) for earlier versions of OS X.

1. Install the XCode Command Line Tools: ::

      xcode-select --install

   and make sure the latest XCode SDK is set in your environment: ::

      export SDKROOT=$(xcrun --show-sdk-path)

2. Install `homebrew <http://brew.sh>`_.

   If you already had homebrew installed consider running ``brew doctor`` to resolve potential
   conflicts before continuing. It can further be useful to remove duplicate packages with the command ``brew cleanup``.

3. We recommend using the clang compiler. The following two commands will install the most recent clang compiler ::
     
     brew install llvm
     brew install libomp

   As a next step we make sure the clang compiler is added to your :ref:`environment <environment_vars>`::

     export CC=/usr/local/opt/llvm/clang
     export CXX=/usr/local/opt/llvm/clang++

  To make this choice permanent, add these two lines to your ``$HOME/.bash_profile``.

4. Install the required dependencies using homebrew: ::

     brew install cmake
     brew install boost
     brew install hdf5
     brew install gmp
     brew install fftw
     brew install open-mpi
     brew install python@2
     brew install scipy
     brew install numpy

5. Install the required python packages: ::

    pip install --user --upgrade --force-reinstall --no-binary=h5py h5py
    pip install --user --upgrade --force-reinstall --no-binary=mpi4py mpi4py
    pip install --user --upgrade matplotlib
    pip install --user --upgrade mako

6. If desired, install the `Jupyter <https://jupyter.org/>`_ Notebook using::

    pip install jupyter
