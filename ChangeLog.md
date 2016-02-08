
Version 1.4
-----------

Documentation
~~~~~~~~~~~~~

* Replace Doxygen by the local simpler tool c++2doc.

Installation
~~~~~~~~~~~~

* boost mpi is no longer detected by triqs. 
  You can either add 2 lines to
  detect it in CMake, or port your apps to triqs::mpi.
  To port an apps using mpi :
 
  * detect boost mpi lib, and link to it, using e.g. in CMakeLists :

    find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS mpi serialization system)
    link_libraries(${Boost_LIBRARIES})

  * include "boost/mpi.hpp" if needed.

* The library is installed in C++14 mode by default. Put the flag ``USE_CPP14`` to false
  to get to C+11 backward compatibility mode.

* New install/build conventions for apps.
  Here are the changes to be made:

  * in _desc.py
  1) m = module(full_name = 'module_name', app_name='application_a')
      (instead of full_name="pytriqs.applications.application_a.module_name")
  2) m.use_module("gf", "triqs")
      (instead of m.use_module("gf"))
  3) m.add_include(... with path relative to c++ dir (not
  relative to current dir))
     (e.g. add_include(solver_core.hpp) instead of add_include("../c++/solver_core.hpp")

  * in CMakeLists.txt
  4) add line include_directories(c++)

  * in python/CMakeLists.txt
  5) change python_destination: remove pytriqs.applications...
  6) change triqs_python_extension: change to triqs_python_extension_and_install, and remove second argument
  7) change install DESTINATION : replace variable TRIQS_PYTHON_LIB_DEST with PYTHON_LIB_DEST_ROOT

  * in your python scripts:
  8) replace everywhere pytriqs.applications.*** with ***

  A more detailed documentation can be found here: https://triqs.ipht.cnrs.fr/1.x/reference/using_the_lib/application_structure.html


Many-body operators
~~~~~~~~~~~~~~~~~~~

* They are now polymorphic, they can handle complex and real case.
  many_body_operator is not a template any more.
  many_body_operator_generic<ScalarType> is the general case.

* The are in triqs::operators namespace (moved from utility).

Green's functions
~~~~~~~~~~~~~~~~~

* Added support for complex G(tau). By default G(tau) is complex.

* Accordingly G(iw) is defined by default on the full (positive and negative) frequency axis.
  Plotting is affected.

* [API break] As a consequence G.data is twice as big. Now G.data[0] refers to the
  smallest negative frequency so older codes using direct data access will break.
  The n^th frequency can be retrieved with G(n) which is the recommended usage as it is
  independent from the storage details.

* Fourier transforms have been updated.

* When writing to HDF5:

  * If the imaginary part of G(tau) is zero, then G(tau) is written as a real array (to gain space)
    otherwise it is written as a complex array
  * If the negative and positive frequencies of G(iw) are all equivalent through G(-iw) = G(iw)*
    then only the positive frequencies are saved (to gain space). Otherwise the full axis is
    written
  * Note that the behavior described above is only followed when writing to the HDF5 file. When
    a G(tau) or a G(iw) is read from the HDF5, it will always be turned into a complex G(tau)
    or a G(iw) on the full frequency axis.


Tests
~~~~~

* All c++ tests have been ported to Google test.

* The c++ test tools have been reorganized into ``triqs/test_tools``. The google test sources are there
  as well as two headers ``gfs.hpp`` and ``arrays.hpp`` with macros to easily compare arrays
  and Green's functions within a google test code.

* The test cmake files have been simplified. There are just two macros to run a test:

  * ``add_cpp_test(testname)`` that adds a c++ test.
  * ``add_python_test(testname)`` that adds a python test.
  * From an external project these macros are called ``triqs_add_cpp_test`` and
    ``triqs_add_python_test``.

* For both tests, if there is a file ``testname.ref`` then the output of the test,
  written in a file ``testname.out`` will be compared to ``testname.ref`` and the test
  will fail if they are different.

* It is now up to the user to check a result against a reference archive. As a convention,
  the reference archives end with .ref.h5 and the test generates a file .out.h5. The
  program h5diff is no longer used by the library (h5diff had several limitations). The
  python tests have been changed accordingly.

* The module ``pytriqs.utility.comparison_tests`` has functions that make it easy to
  compare arrays and green's functions in a python script.

* The module ``pytriqs.utility.h5diff`` allows to compare two archives.




Version 1.3
-----------

Installation
~~~~~~~~~~~~

* TRIQS depends on a number of third-party Sphinx extensions to build its documentation.
  These extensions are now installed under `${CMAKE_INSTALL_PREFIX}/share/triqs/sphinxext`, in order to let TRIQS-based application to reuse the extensions and to inherit the visual style of the docs.
* Preinstalled Sphinx extension sphinxcontrib-doxylink is not needed anymore to build the documentation.

Green functions
~~~~~~~~~~~~~~~

* New Python method `DOS.copy()`.
* New convenience functions in module `pytriqs.gf.local.tools`,

  * `inverse()`: compute the inverse of a Green's function;
  * `delta()`: compute Delta_iw from G0_iw;
  * `dyson()`: solve Dyson's equation for given two of G0_iw, G_iw and Sigma_iw to yield the third;
  * `tail_fit()`: fit the tails of Sigma_iw and optionally G_iw;
  * `read_gf_from_txt()`/`write_gf_to_txt()`: read/write a GfReFreq from/to text files.

Many-body operators
~~~~~~~~~~~~~~~~~~~

* HDF5 support.
* Various utilities related to the many-body operators are organized in a separate sub-package `pytriqs.operators.util`. It includes the following modules,

  * `hamiltonian`: commonly-used Hamiltonians, Slater, Kanamori, density-density;
  * `U_matrix` (moved from dft_tools): construction of interaction matrices;
  * `observables`: operators of commonly used observables (S_z, S^2, L_z, L^2, L*S and so on);
  * `op_struct`: auxiliary functions to work with spin and orbital indices.

Monte-Carlo tools
~~~~~~~~~~~~~~~~~

* Function `mc_generic::start()` returns different exit codes depending on the way it has been stopped, by receiving a signal or by `stop_callback()`.

Determinant manipulation
~~~~~~~~~~~~~~~~~~~~~~~~

* New functions `insert2()`, `remove2()`, `change_row()` and `change_col()`, which are combinations of `try_*` and `complete_operation()`.
* New functions `insert2_at_end()`, `remove_at_end()` and `remove2_at_end()`.
* Optimized version of `change_one_row_and_one_col()` and other minor optimizations.

HDF5
~~~~

* Support for `unsigned long long`.
* The lists and tuples in Python are now written by hdf_archive with keys as "0", "1", "2", "3", instead of "00000000", "00000001", "0000002", "0000003", ...
  This is necessary for C++ compatibility, where `std::vector<T>` is written in HDF5 as a subgroup with object named by their number, for generic T (i.e. not `int`, `double`, `std::string`).
  This change is backward compatible: old archive can be read by new code, but would break h5diff.

c++2py
~~~~~~

* Support for rich comparison methods.
* New C++ attribute `ignore_in_python` that prevents a class method from wrapping in Python.
* One can use two special line prefixes, `type:` and `default:` when documenting a member of the parameter class. They allow to override the corresponding fields in the generated table.
* Converters for `unsigned int`, `unsigned long` and `unsigned long long`.
* Converter for `std::tuple` (from any Python sequence/to `tuple()`).
* `std::pair` is now converted to `tuple()`.

Tests
~~~~~

* The Google Test framework (https://github.com/google/googletest) has been adopted by the project. Some of C++ test have been ported to it.
* New header file `test_tools.hpp` with convenience functions and additional GTest macros for C++ testing.

Miscellaneous
~~~~~~~~~~~~~

* Added a reference to a Computer Physics Communications paper about TRIQS.
* Numerous updates to the documentation.


Version 1.2
------------------

C++
~~~

* New wrapping of boost random generators. No effect on API.
* HDF5 cleaned, now using only C library (not C++) for simpler installation.

Python
~~~~~~

* New wrapper tool to glue C++ and Python. Cython is gone.

Green Functions
~~~~~~~~~~~~~~~

* `transpose()` method now returns a NEW green function, like `conjugate()`, etc...
* Suppress the possibility to save/load in HDF5 for a tail alone (must be part of a GF).



Version 1.1
-----------

* New constructors for the gf [api change]
* Fix for gf expression templates
* The gf tails now have fixed size to avoid mpi problems
* Fixes in gf expression templates
* New python random generator interface
* Fixes for issues #11, #18, #25


Version 1.0
-----------

There have been changes from versions 0.x to 1.0.0 that will most likely have
consequences for your scripts and archives.

Python classes
~~~~~~~~~~~~~~

The number of python classes in the old 0.x releases was increasing with no
clear naming convention. In TRIQS 1.0 we have unified the naming of the classes
following the `PEP naming conventions
<http://www.python.org/dev/peps/pep-0008/#naming-conventions>`_:

* Package and module names: lowercase with underscores
* Class names: CapWords
* Function names: lowercase with underscores
* Function and method arguments: lowercase with underscores

Archives
~~~~~~~~

We provide :download:`an update script <scripts/update_archive.py>` which should
help you upgrade your archive. The usage is very simple::

  $ python update_archive.py old_archive new_archive

where ``old_archive`` is your old archive to be upgraded and ``new_archive`` is
the name of the new archive. If you encounter problems please post an
issue with a copy of your archive.
