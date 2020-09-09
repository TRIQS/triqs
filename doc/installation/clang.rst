.. index:: clang

.. highlight:: bash

.. _install_clang:

Using the clang compiler
========================

**clang/llvm** alongside gcc is one of most modern C++ compilers. 
It is open source with contributers such as Apple and Google, and is currently the default compiler on Mac OS.

We *highly recommend* the **clang** compiler for anyone developing C++ since:

* It is compliant with the most recent C++ language standards.
* It has expressive and clear error messages, superior to intel and gcc.
* It is a developed as a drop-in replacement for the more wide-spread gcc (i.e. almost identical usage)

Note, however, that for *production* compilations, gcc, especially recent versions, still produces in general
a slightly more optimal code for TRIQS applications, even if the differences are small.

It is highly recommended to use the latest stable version on Linux and the default compiler on Mac OS.

Get and install clang on linux
------------------------------

Precompiled versions are available at the `LLVM dowload page <http://llvm.org/releases/download.html>`_.
We have provided :ref:`instructions <ubuntu_clang>` on how to install a recent clang on Ubuntu.

Alternatively, for the more experienced user, the most recent version of **clang** can be
`compiled from source <http://clang.llvm.org/get_started.html#build>`_.

Usage
-----

In order to use clang for building TRIQS and its applications, one should::

   export CC=clang
   export CXX=clang++

before the cmake configuration step in the install instructions.

libclang
----------

Some tools for developers (such as the `Cpp2Py <http://github.com/triqs/cpp2py>`_ wrapping tool) make use of
the libclang library (commonly installed with llvm/clang) and its Python bindings (usually not given).

The latter can be installed either via the package manager of your distribution
or into a :ref:`Python virtual environment <python_virtualenv>` via::

  pip install clang
