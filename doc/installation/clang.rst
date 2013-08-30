.. index:: clang

.. highlight:: bash

.. _install_clang:

Using the clang compiler
========================

**clang/llvm** is one of the very best C++ compilers, with gcc. It is open
source on linux and OS X (where it is now the standard compiler provided by
Apple).

The **clang** compiler is *highly recommended* for anyone developing in C++ since: 

* It is standard compliant (C++11).
* It has very nice and useful error messages, much nicer than intel or gcc (even thought gcc is catching up).
  It really makes little sense to develop with e.g. gcc 4.6 today, especially with a library like TRIQS, 
  which uses a lot of metaprogramming.

Note, however, that for *production* compilations, gcc, especially recent versions, still produces in general
a slighly more optimal code for TRIQS applications, even if the difference is small.

It is highly recommended to use the latest stable version on linux (3.3), and the default compiler on OS X (Mountain lion).

Get and install clang on linux
------------------------------

Precompiled versions are available at the `LLVM dowload page <http://llvm.org/releases/download.html>`_.
Just untar them and add the directory in your path.

It is also very easy to compile **clang** from source, just follow the 
`instructions <http://clang.llvm.org/get_started.html#build>`_.

Usage
-----

In order to use clang when building TRIQS::

   CXX=clang++ cmake -DCMAKE_INSTALL_PREFIX=path_to_install_directory ../src + other options

.. note::

   On OS X, the options -std=c++11 and -stdlib=libc++ are automatically added by the TRIQS
   installation script.

