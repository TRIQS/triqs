.. index:: clang

.. _clang:

.. highlight:: bash

Using clang compiler
==========================

clang++ compiler is **highly recommended** for anyone developing in C++ since : 

* It has very nice  and useful error messages, much nicer than intel or gcc.
* It implements most of the new C++11 standard.

clang/llvm is now the standard compiler on OS X, but it is open source
and works very well on linux too.

Version
---------

* TRIQS compiles on clang, version 3.0 and later.

* Recommended version is 3.1 (latest stable).

Get and install clang
------------------------

If you have precompiled version in the  `LLVM dowload page <http://llvm.org/releases/download.html>`_, 
use it. E.g. on Ubuntu 12.04 LTS ::

 wget http://llvm.org/releases/3.1/clang+llvm-3.1-x86_64-linux-ubuntu_12.04.tar.gz
 tar xzf clang+llvm-3.1-x86_64-linux-ubuntu_12.04
 sudo mv clang+llvm-3.1-x86_64-linux-ubuntu_12.04 /opt/clang
 # add /opt/clang/bin to your path
 # use clang++...

Otherwise, it is in fact quite easy to compile clang from source, just follow the 
`instructions <http://clang.llvm.org/get_started.html#build>`_.

Usage
--------------

In the current version, when compiling with clang, say (provided clang++ is your path of course) ::

   CXX=clang++ cmake path_to_TRIQS_source_directory .... others options ...


