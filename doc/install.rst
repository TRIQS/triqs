.. index:: installation

.. highlight:: bash

.. _installation:

Installation 
============

TRIQS and its applications are provided *a la carte*:
after you have installed the TRIQS library, you will be able to easily install
various TRIQS-based applications: impurity solvers, tools for DFT+DMFT
calculations, etc.

This page describes the installation of the TRIQS library itself. The
installation procedure of the applications is described on their respective
sites, under 'Install'.

Philosophy
----------

The TRIQS project is in perpetual dynamic evolution such that we can get our
developments directly to users straight out of the oven.

However, we also understand that some users may not wish to constantly update
their codes, and are happy to use an older but perhaps more stable version. 

To this end, we propose two options to the user:
1) You follow the latest master commits for all applications, and be certain to
have all the bug fixes and latest functionality, while perhaps having to change
some API occasionally, or
2) You use a version tag, e.g. version 1.2, for all applications, and have a
stable version of the code, while not having the latest bug fixes and
functionality.

Prerequisites
-------------

The TRIQS library relies on a certain number of standard libraries and tools
described in the :ref:`list of requirements <requirements>`. Please pay
particular attention to the :ref:`C++ compilers<require_cxx_compilers>` and to
:ref:`Python and the scientific python tools<python_install>`.  Here are instructions to
install these necessary libraries on two standard systems:

.. toctree::
   :maxdepth: 1

   installation/ubuntu
   installation/osx_install

.. note:: Please ensure that you have all the dependencies installed before proceeding further!

Installation steps
------------------

#. Download the sources of TRIQS from github into a ``triqs.src`` directory::

     $ git clone https://github.com/TRIQS/triqs.git triqs.src

   If you wish to follow Option 2 as described under Philosophy, you should
   instead download the latest tagged version from the `GitHub releases page 
   <https://github.com/TRIQS/triqs/releases>`_.

#. Create an empty ``triqs.build`` directory where you will compile the code::

     $ mkdir triqs.build && cd triqs.build

#. In the build directory call ``cmake`` specifying where the TRIQS library should be installed::

     $ cmake -DCMAKE_INSTALL_PREFIX=path_to_install_directory ../triqs.src

   If you omit ``CMAKE_INSTALL_PREFIX``, the default
   ``path_to_install_directory`` is the subdirectory *INSTALL_DIR* in your build
   directory. More configuration options are described in the
   :ref:`cmake options <install_options>`.

#. Compile the code, run the tests and install the library::

     $ make
     $ make test
     $ make install

   .. note:: 

      On multicore machine, the make option -jN (where N is the number of cores)
      greatly speeds up the compilation, e.g. make -j8 on an 8 core machine.

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
