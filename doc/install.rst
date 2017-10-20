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

#. You follow the latest master commits for all applications, and be certain to
   have all the bug fixes and latest functionality, while perhaps having to change
   some API occasionally. Note that we use Continous Integration (jenkins) to ensure 
   that the master always compiles and passes all tests, both
   for the library and for several public (and private) applications.

#. You use a version tag, e.g. version 1.3, for all applications, and have a
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

You need to install first Cpp2Py and then TRIQS.

We provide hereafter the build instructions in the form of a little bash script.
You can adapt INSTALL_PREFIX, NCORES for your local settings.
Note that, contrary to previous versions of TRIQS,  
the installation directory CMAKE_INSTALL_PREFIX is now mandatory in the cmake configuration.


.. literalinclude:: install.sh
   :language: bash


Environment setup
^^^^^^^^^^^^^^^^^^^

Cpp2Py and TRIQS both provide a little file `cpp2pyvars.sh` and `triqsvars.sh`
to update your environment variables.  

Please add to your .bash_profile (replace INSTALL_PREFIX with the correct path)::

        source INSTALL_PREFIX/share/cpp2pyvars.sh
        source INSTALL_PREFIX/share/triqsvars.sh

This updates PATH and similar variables to that your unix shell find commands like c++2py, libraries and 
cmake automatically finds Cpp2Py and TRIQS in applications.


Further reading
------------------
.. toctree::
   :maxdepth: 1

   installation/install_options
   installation/python
   installation/clang
