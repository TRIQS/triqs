.. index:: install_osx

.. highlight:: bash

.. note:: The recommended way to install TRIQS on Mac OS X is through the :ref:`Anaconda package <triqs_anaconda>`

.. _install_osx:

Prerequisites for the TRIQS Compilation on Mac OS X
===================================================

Disclaimer
----------

We here provide instructions on how to install the dependencies of TRIQS on Mac OS X in order to be able to :ref:`compile TRIQS from source <triqs_compilation>`.

The installation of the scientific libraries that TRIQS depends upon
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

3. Install the required dependencies using homebrew: ::

     brew install cmake
     brew install boost
     brew install hdf5
     brew install gmp
     brew install fftw
     brew install open-mpi
     brew install openblas
     brew install python

4. Install the required python packages: ::

    pip3 install --user --upgrade matplotlib
    pip3 install --user --upgrade mako
    pip3 install --user --upgrade numpy
    pip3 install --user --upgrade scipy
    pip3 install --user --upgrade mpi4py

5. We recommend using the clang compiler by default. The following command will install llvm together with the most recent clang compiler ::

     brew install llvm

   As a next step we make sure the llvm installation is available in your :ref:`environment <environment_vars>` and we set clang as the default compiler ::

     export PATH=/usr/local/opt/llvm/bin:$PATH
     export LIBRARY_PATH=/usr/local/opt/llvm/lib:$LIBRARY_PATH
     export CC=/usr/local/opt/llvm/bin/clang
     export CXX=/usr/local/opt/llvm/bin/clang++

  To make this choice permanent, add these lines to your ``$HOME/.bash_profile``.

You should now be set to :ref:`compile TRIQS from source <triqs_compilation>`.
