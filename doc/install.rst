.. index:: installation

.. highlight:: bash

.. _installation:

Installation 
============

TRIQS and its applications are provided *à la carte*: 
after you have installed the TRIQS toolkit, you will be able to easily install 
various TRIQS-based applications: impurity solvers, realistic DMFT tools, ...

This page describes the installation of the TRIQS toolkit itself. The installation of the applications
is described in their respective documentation.

Prerequisites
-------------

The TRIQS library relies on a certain number of standard libraries and tools described in
the :ref:`list of requirements <requirements>`. Beware in particular to the :ref:`C++ compilers<require_cxx_compilers>`
and to :ref:`Scientific Python tools<python_install>`.  
Here are instructions to install
these necessary libraries on two standard systems:

.. toctree::
   :maxdepth: 1
     
   installation/ubuntu
   installation/osx_lion

Installation steps
------------------

#. Download the sources of TRIQS from github into a ``src`` directory::

     $ git clone https://github.com/TRIQS/triqs.git src

#. Create an empty build directory where you will compile the code::

     $ mkdir build && cd build

#. In the build directory call cmake specifying where the TRIQS library should be installed::

     $ cmake -DCMAKE_INSTALL_PREFIX=path_to_install_directory ../src

   If you omit ``CMAKE_INSTALL_PREFIX``, the default
   ``path_to_install_directory`` is the subdirectory *INSTALL_DIR* in your build
   directory. More configuration options are described in the
   :ref:`cmake options <install_options>`.

#. Compile the code, run the tests and install the library::

     $ make
     $ make test
     $ make install

   .. note:: 
       
      We remind you that on multicore machine, make's option -jN (where N is the number of cores)
      greatly speeds up the compilation, e.g. make -j8 on an 8 core machine...

#. After the installation TRIQS has been installed using the standard UNIX
   scheme:

   * executables in ``path_to_install_directory/bin``.

   * c++ and python libraries in ``path_to_install_directory/lib``.

   * c++ headers in ``path_to_install_directory/include/triqs``.
 
   * shared items (e.g. the documentation) in ``path_to_install_directory/share/triqs``.


Further reading
------------------
.. toctree::
   :maxdepth: 1
   
   installation/install_options
   installation/python
   installation/clang
