.. index:: install_options

.. highlight:: bash

.. _install_options:

Customizing installation : cmake options
----------------------------------------------------

Specifying the compiler or compile/link flags
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To specify the compiler with cmake one may use the CXX, CXXFLAGS variables, e.g. ::

   CXX=clang++ CXXFLAGS cmake path_to_TRIQS_source_directory  ..... 

Customize the installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can then customize the installation options using the :program:`ccmake` program::

  ccmake .

This opens an interface with your main installation options shown ::

 BUILD_SHARED_LIBS                ON
 Build_CTHyb                      ON
 Build_Documentation              OFF
 Build_HubbardI                   ON
 Build_Triqs_General_Tools_Test   ON    
 Build_Wien2k                     ON      
 CMAKE_INSTALL_PREFIX             /home/parcolle/BUILD2/Triqs_dynamic/INSTALL_DIR 
 Install_dev                      OFF   
 LAPACK_LIBS                      /usr/lib/liblapack.so;/usr/lib/libblas.so;/usr/lib/libpthread.so;/usr/lib/libblas.so
 PYTHON_INTERPRETER               /usr/bin/python   
 
You can for instance change the following options:

  * ``CMAKE_INSTALL_PREFIX``: This is the directory :file:`path_to_TRIQS_install_directory` where you want to install TRIQS.
  * ``Build_CTHyb``: Build the continuous-time hybridization impurity solver.
  * ``Build_HubbardI``: Build a simple HubbardI solver.
  * ``Build_Wien2k``: Build the interface to **WIEN2K**. 
  * ``Build_Documentation``: Get the documentation locally in :file:`path_to_TRIQS_install_directory/share/doc`.

You may change all other installation options (like locations of libraries, the choice of compilers, etc.) in the advanced mode, by typing 't' after having opened the *ccmake* interface.

After having corrected your options you may build, test and install TRIQS as described in :ref:`installation`.
 
.. _install_without_boost:

Installation with your own version of boost  [-DBOOST_INSTALL_DIR=...]
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have a sufficiently recent version of boost already installed in your system you may
link TRIQS with it instead of compiling a new version of boost from sources.

In this case, the cmake instruction is ::

   cmake path_to_TRIQS_source_directory -DBOOST_INSTALL_DIR=root_path_to_Boost_installation ....


The cmake option `DBOOST_INSTALL_DIR` is the path to the **boost** libraries. 
The default value is  : 
 
 * BOOST_INSTALL_DIR : /usr 

So if boost is at the right place, you can just say::

   cmake path_to_TRIQS_source_directory

**Obviously, you should not specify `DBOOST_SOURCE_DIR` in this case!**
.. _static_dyn :

Static versus dynamic linking [-DBUILD_SHARED_LIBS=OFF]
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TRIQS can be compiled in two different ways: 

* **dynamic linking**  [default] : 
  
  This is the standard python procedure and the default choice. 
  All the C++ codes are compiled in shared libraries and loaded dynamically at runtime by the python interpreter.
 
  With this technique however, you have to properly set the PYTHONPATH environment variable to the TRIQS installation directory in order 
  for python to be able to find the module. To avoid errors, TRIQS provides a 
  tiny script :file:`path_to_TRIQS_install_directory/bin/pytriqs` that sets up this variable for you and launch the python interpreter.

* **static linking** :

  An alternative is to compile a new interpreter statically.

  Here the C++ modules are linked with libpython2.x.a to create a *new python interpreter*  :file:`path_to_TRIQS_install_directory/bin/pytriqs`
  that contains these modules as *built-in* modules.

  Though it is not as standard and extensible as the dynamic linking, this technique has several advantages :

    * It avoids opening dynamic libs, which simplifies code profiling for example.
    * It may be more stable on some cluster machines (the boost.mpi modules in this case is also linked statically).

In any case, both the scripts and the way to call them remain the same in both cases::
 
  pytriqs myscript.py

The choice is made with the cmake option ::
   
 -DBUILD_SHARED_LIBS=OFF/ON
