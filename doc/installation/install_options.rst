.. index:: install_options

.. highlight:: bash

.. _install_options:

Customizing your installation
-----------------------------

The installation of TRIQS can be customized using either :ref:`environment variables <environment_vars>` such
as ``CXX`` or via a set of cmake options described below.

cmake options
^^^^^^^^^^^^^^

+-----------------------------------------------+---------------------------------------------------------------+
| Options                                       | Syntax                                                        |
+===============================================+===============================================================+
| Specify the installation directory (required) | -DCMAKE_INSTALL_PREFIX=install_path                           |
+-----------------------------------------------+---------------------------------------------------------------+
| Choose to build TRIQS in Debug mode           | -DCMAKE_BUILD_TYPE=Debug                                      |
+-----------------------------------------------+---------------------------------------------------------------+
| Specify additional compiler flags             | -DCMAKE_CXX_FLAGS='...'                                       |
+-----------------------------------------------+---------------------------------------------------------------+
| Specify additional linker flags               | -DCMAKE_EXE_LINKER_FLAGS='...'                                |
+-----------------------------------------------+---------------------------------------------------------------+
| Turn of the compilation of tests              | -DBuild_Tests=OFF                                             |
+-----------------------------------------------+---------------------------------------------------------------+
| Build the documentation locally               | -DBuild_Documentation=ON                                      |
+-----------------------------------------------+---------------------------------------------------------------+
