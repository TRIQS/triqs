.. index:: install_options

.. highlight:: bash

.. _install_options:

Customizing installation: cmake options
---------------------------------------

Specifying the compiler or compile/link flags
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To specify the compiler with cmake one may use the CXX, CXXFLAGS variables, e.g. ::

   CXX=clang++ CXXFLAGS=XXXX cmake path_to_TRIQS_source_directory  ..... 

Customize the installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can then customize the installation options using the :program:`ccmake` program::

  ccmake .

This opens an interface with your main installation options shown ::

 BUILD_SHARED_LIBS                ON
 Build_Documentation              OFF
 Build_Triqs_General_Tools_Test   ON    
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
 

