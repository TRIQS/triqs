.. index:: install_options

.. highlight:: bash

.. _install_options:

Customizing your installation: cmake options
---------------------------------------------

Specifying the compiler or compile/link flags
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To specify the compiler with cmake one may use the CXX, CXXFLAGS variables, e.g.::

   CXX=clang++ CXXFLAGS=XXXX cmake path_to_TRIQS_source_directory ...

cmake options
^^^^^^^^^^^^^^

+-----------------------------------------------+------------------------------------------------+
| Options                                       | Syntax                                         |
+===============================================+================================================+
| Specify the installation directory            | -DCMAKE_INSTALL_PREFIX=install_path            |
+-----------------------------------------------+------------------------------------------------+
| Special python interpreter                    | -DPYTHON_INTERPRETER=path/python               |
+-----------------------------------------------+------------------------------------------------+
| Build the documentation locally               | -DBuild_Documentation=ON                       |
+-----------------------------------------------+------------------------------------------------+
