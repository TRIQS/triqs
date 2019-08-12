.. index:: environment_vars

.. _environment_vars:
.. highlight:: bash

Unix environment variables
==========================

Environment variables are local to each shell session and influence the way your system and
the installed software behaves. The PATH variable, for example, contains a colon-separated
list of directories (such as /usr/bin) that are searched for executable programs. 

In order to access the value of an environment variable, prepend a $-sign.
To print, e.g., the value of PATH we run::

   echo $PATH

If we want to change the value of an environment variable we have to use the command ``export``.
To prepend, e.g., the directory ``/home/user/bin`` to PATH, we run::

   export PATH=/home/user/bin:$PATH

One can list all environment variables of the active shell by running::

   printenv

If you want to set the value for an environment variable automatically, it is the best practice
to add the corresponding ``export`` command to the file `~/.bash_profile <https://en.wikipedia.org/wiki/Bash_(Unix_shell)#Startup_scripts>`_ (or `~/.zprofile <http://zsh.sourceforge.net/FAQ/zshfaq03.html#l19>`_)


  .. note:: Some environment variables on Mac OS differ slightly from the ones on conventional UNIX systems (e.g. LD_LIBRARY_PATH)


Common environment variables
----------------------------

+-------------------------------+---------------------------------------------------------------+
| Environment Variable          | Brief Description                                             |
+===============================+===============================================================+
| PATH                          | List of directories searched for executables                  |
+-------------------------------+---------------------------------------------------------------+
| USER                          | The current username                                          |
+-------------------------------+---------------------------------------------------------------+
| HOME                          | Home directory of the current user                            |
+-------------------------------+---------------------------------------------------------------+
| PWD                           | Path to the current working directory                         |
+-------------------------------+---------------------------------------------------------------+

Environment variables for software developement
-----------------------------------------------

+-------------------------------+---------------------------------------------------------------------------------------+
| Environment Variable          | Brief Description                                                                     |
+===============================+=======================================================================================+
| CC                            | The C-compiler executable                                                             |
+-------------------------------+---------------------------------------------------------------------------------------+
| CXX                           | The C++-compiler executable                                                           |
+-------------------------------+---------------------------------------------------------------------------------------+
| CFLAGS                        | Additional compiler flags for C                                                       |
+-------------------------------+---------------------------------------------------------------------------------------+
| CXXFLAGS                      | Additional compiler flags for C++                                                     |
+-------------------------------+---------------------------------------------------------------------------------------+
| LDFLAGS                       | Additional flags for the linker                                                       |
+-------------------------------+---------------------------------------------------------------------------------------+
| LIBRARY_PATH                  | List of directories searched for libraries by the linker                              |
+-------------------------------+---------------------------------------------------------------------------------------+
| LD_LIBRARY_PATH               | List of directories searched for dynamic libraries                                    |
+-------------------------------+---------------------------------------------------------------------------------------+
| CPATH                         | List of directories searched for C/C++ header files (treated like -I includes)        |
+-------------------------------+---------------------------------------------------------------------------------------+
| CPLUS_INCLUDE_PATH            | List of directories searched for C/C++ header files (treated like -isystem includes)  |
+-------------------------------+---------------------------------------------------------------------------------------+
| PYTHONPATH                    | List of directories searched for python modules/packages                              |
+-------------------------------+---------------------------------------------------------------------------------------+

Environment variables affecting OpenMP
--------------------------------------

Certain environment variables will affect the behavior of OpenMP.
In particular they allow you to set the number of OpenMP threads that will be spawned.

.. warning:: This is of particular importance when using MPI, as each MPI rank will potentially spawn
             an equal number of OpenMP threads. If you intend to use as many MPI ranks as cores are available
             you should `export OMP_NUM_THREADS=1`

+-------------------------------+---------------------------------------------------------------------------------------+
| Environment Variable          | Brief Description                                                                     |
+===============================+=======================================================================================+
| OMP_NUM_THREADS               | Specifies globally the number of OpenMP threads to use                                |
+-------------------------------+---------------------------------------------------------------------------------------+
| MKL_NUM_THREADS               | Specifies the number of OpenMP threads to use only within the Intel MKL library       |
+-------------------------------+---------------------------------------------------------------------------------------+
| OPENBLAS_NUM_THREADS          | Specifies the number of OpenMP threads to use within the OpenBlas library             |
+-------------------------------+---------------------------------------------------------------------------------------+
