.. index:: clang


.. highlight:: bash

.. _install_clang:
Using clang compiler
==========================

**clang/llvm** is one of the very best C++ compilers, with gcc.
It is open source, on linux and OS X (where it is now the standard compiler provided by Apple).

*clang* compiler is **highly recommended** for anyone developing in C++ since : 

 * It is standard compliant (C++11).
 * It has very nice and useful error messages, much nicer than intel or gcc (even thought gcc is catching up...).
   It really makes little sense to develop with e.g. gcc 4.6 today, specially with a library like TRIQS, 
   which uses a lot of metaprogramming.

Note however that for *production* compilations, gcc, specially recent versions, still produces in general
a slighly more optimal code for TRIQS applications, also the difference is small.

It is highly recommended to use the latest stable on linux (3.3), and the default compiler on Os X (Mountain lion).

Get and install clang on linux
---------------------------------------------

Precompiled versions are available at the  `LLVM dowload page <http://llvm.org/releases/download.html>`_, 
use it. Just untar them, and add the directory in your path.

In any case, it is also very easy to compile clang from source, just follow the 
`instructions <http://clang.llvm.org/get_started.html#build>`_.

Usage
--------------

To use clang ::

   CXX=clang++ cmake path_to_TRIQS_source_directory .... others options ...

.. note::

   On Os X, the options -std=c++11 and -stdlib=libc++ are automatically added by the TRIQS installation script.

