.. highlight:: python
.. role:: red
.. role:: blue

Conventions for TRIQS applications
========================================

Let us suppose you want to build your own application named 'application_a'. If you want to use the full functionalities of TRIQS and its c++2py tools, we advise you to follow the conventions below:


Source structure 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

      **SRC_DIR**/c++: :red:`where you put your c++ code`

      **SRC_DIR**/c++/foobar/toto_a1.hpp (with class toto_a1 in namespace foobar)

      **SRC_DIR**/python: :red:`where you put your python code`

      **SRC_DIR**/python/module_a1_desc.py (descriptor for :blue:`module_a1` with  python class TotoA1 wrapping c++ class toto_a1)

          in particular, contains::

           module = module_(full_name = "module_a1", doc = "doc of module_a1",   app_name="application_a")

           module.add_include("foobar/toto_a1.hpp") (i.e. everything is relative to **SRC_DIR**/c++/)

           module.use_module("module_b1", "application_b") (#adds **BUILD_DIR**/include/application_b/py_converters and **INSTALL_DIR**/include/application_b/py_converters to module_path_list)

           module.generate_code()

           ...

      **SRC_DIR**/doc: :red:`where you put your documentation`

      **SRC_DIR**/doc/reference

      **SRC_DIR**/doc/reference/contents.rst


          in particular, contains::

          .. automodule:: application_a.module_a1

      **SRC_DIR**/test: :red:`where you put your tests`

      **SRC_DIR**/test/c++

      **SRC_DIR**/test/python/TotoA1.py 
      
         in particular, contains::
          
            import application_a.module_a1 ...

      **SRC_DIR**/test/python/TotoA1.ref.h5

----------------------------------------------------      

Install structure
~~~~~~~~~~~~~~~~~~

      Reminder:

      **INSTALL_DIR**/include/triqs/

      **INSTALL_DIR**/include/triqs/py_converters

      **INSTALL_DIR**/include/triqs/py_converters/gf.hpp (for instance)

      **INSTALL_DIR**/lib/python2.7/dist-packages/triqs/gf/local/gf.so (for instance)

      Your application should be installed with the following structure:

      **INSTALL_DIR**/include/:red:`application_a`/

      **INSTALL_DIR**/include/:red:`application_a`/foobar/toto_a1.hpp (with relative includes when applies)

      **INSTALL_DIR**/include/:red:`application_a`/

      **INSTALL_DIR**/include/:red:`application_a`/foobar/toto_a1.hpp (with relative includes when applies)

      **INSTALL_DIR**/include/:red:`application_a`/py_converters/:blue:`module_a1`.hpp (with  includes relative to **INSTALL_DIR**/include/ i.e. starting with ":red:`application_a`/foobar/toto_a1.hpp

      **INSTALL_DIR**/lib/python2.7/dist-packages/:red:`application_a`/:blue:`module_a1`.so (importable via  import :red:`application_a`.:blue:`module_a1` in a triqs prompt)

      **INSTALL_DIR**/lib/python2.7/dist-packages/:red:`application_a`/my_functions.py

----------------------------------------------------      

Build structure
~~~~~~~~~~~~~~~~

      **BUILD_DIR**/build_triqs

      **BUILD_DIR**/c++/

      **BUILD_DIR**/python/

      **BUILD_DIR**/python/:red:`application_a`/:blue:`module_a1`.so

      **BUILD_DIR**/python/:red:`application_a`/my_functions.py

      **BUILD_DIR**/python/:red:`application_a`/__init__.py

      **BUILD_DIR**/python/:red:`application_a`/module_a1_wrap.cpp #should contain paths relative to **SRC_DIR**/c++ (e.g. #include "foobar/toto_a1.hpp")

      **BUILD_DIR**/include/:red:`application_a`/py_converters/:blue:`module_a1`.hpp (contains paths relative to **SRC_DIR**/c++ if yet another module in the same

      **BUILD_DIR**/include/:red:`application_a`/py_converters/:blue:`module_a1`.hpp.to_be_installed (contains paths correct for the installation folder)

      **BUILD_DIR**/doc/

      **BUILD_DIR**/test/python/TotoA1.ref.h5 (copied via CMakeLists)

