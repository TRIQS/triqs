.. index:: installation

.. highlight:: bash

.. _installation:

Installation 
============
 
Prerequisite
------------

The TRIQS library relies on a certain number of standard libraries described in
the :ref:`list of requirements <requirements>`. Here are instructions to install
these necessary libraries on two standard systems:

.. toctree::
   :maxdepth: 1
     
   installation/ubuntu
   installation/osx_lion

Installation steps
------------------

#. Download the sources of TRIQS from github into a ``src`` directory::

     $ git clone git@github.com:TRIQS/triqs.git src

#. Create an empty build directory where you will compile the code::

     $ mkdir build && cd build

#. In the build directory call cmake specifying where the TRIQS library should be installed::

     $ cmake -DCMAKE_INSTALL_PREFIX=path_to_install_directory ../src

#. Compile the code, run the tests and install the library::

     $ make
     $ make test
     $ make install

#. After the installation TRIQS has been installed using the standard UNIX
   scheme:

   * executables in ``path_to_install_directory/bin``.

   * c++ and python libraries in ``path_to_install_directory/lib``.

   * c++ headers in ``path_to_install_directory/include/triqs``.
 
   * shared items in ``path_to_install_directory/share/triqs``.


Further details
---------------

.. toctree::
   :maxdepth: 1
   
   installation/python
   installation/clang
   installation/intel
   installation/install_options
   installation/changelog

