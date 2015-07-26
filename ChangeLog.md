New in version 1.4
==================

Installation
------------

* boost mpi is no longer detected by triqs. 
  You can either add 2 lines to
  detect it in CMake, or port your apps to triqs::mpi.
  To port an apps using mpi :
 
  * detect boost mpi lib, and link to it, using e.g. in CMakeLists :

    find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS mpi serialization system)
    link_libraries(${Boost_LIBRARIES})

  * include "boost/mpi.hpp" if needed.



New in version 1.3
==================

Installation
------------

* TRIQS depends on a number of third-party Sphinx extensions to build its documentation.
  These extensions are now installed under `${CMAKE_INSTALL_PREFIX}/share/triqs/sphinxext`, in order to let TRIQS-based application to reuse the extensions and to inherit the visual style of the docs.
* Preinstalled Sphinx extension sphinxcontrib-doxylink is not needed anymore to build the documentation.

Green functions
---------------

* New Python method `DOS.copy()`.
* New convenience functions in module `pytriqs.gf.local.tools`,

  * `inverse()`: compute the inverse of a Green's function;
  * `delta()`: compute Delta_iw from G0_iw;
  * `dyson()`: solve Dyson's equation for given two of G0_iw, G_iw and Sigma_iw to yield the third;
  * `tail_fit()`: fit the tails of Sigma_iw and optionally G_iw;
  * `read_gf_from_txt()`/`write_gf_to_txt()`: read/write a GfReFreq from/to text files.

Many-body operators
-------------------

* HDF5 support.
* Various utilities related to the many-body operators are organized in a separate sub-package `pytriqs.operators.util`. It includes the following modules,

  * `hamiltonian`: commonly-used Hamiltonians, Slater, Kanamori, density-density;
  * `U_matrix` (moved from dft_tools): construction of interaction matrices;
  * `observables`: operators of commonly used observables (S_z, S^2, L_z, L^2, L*S and so on);
  * `op_struct`: auxiliary functions to work with spin and orbital indices.

Monte-Carlo tools
-----------------

* Function `mc_generic::start()` returns different exit codes depending on the way it has been stopped, by receiving a signal or by `stop_callback()`.

Determinant manipulation
------------------------

* New functions `insert2()`, `remove2()`, `change_row()` and `change_col()`, which are combinations of `try_*` and `complete_operation()`.
* New functions `insert2_at_end()`, `remove_at_end()` and `remove2_at_end()`.
* Optimized version of `change_one_row_and_one_col()` and other minor optimizations.

HDF5
----

* Support for `unsigned long long`.
* The lists and tuples in Python are now written by hdf_archive with keys as "0", "1", "2", "3", instead of "00000000", "00000001", "0000002", "0000003", ...
  This is necessary for C++ compatibility, where `std::vector<T>` is written in HDF5 as a subgroup with object named by their number, for generic T (i.e. not `int`, `double`, `std::string`).
  This change is backward compatible: old archive can be read by new code, but would break h5diff.

c++2py
------

* Support for rich comparison methods.
* New C++ attribute `ignore_in_python` that prevents a class method from wrapping in Python.
* One can use two special line prefixes, `type:` and `default:` when documenting a member of the parameter class. They allow to override the corresponding fields in the generated table.
* Converters for `unsigned int`, `unsigned long` and `unsigned long long`.
* Converter for `std::tuple` (from any Python sequence/to `tuple()`).
* `std::pair` is now converted to `tuple()`.

Tests
-----

* The Google Test framework (https://github.com/google/googletest) has been adopted by the project. Some of C++ test have been ported to it.
* New header file `test_tools.hpp` with convenience functions and additional GTest macros for C++ testing.

Miscellaneous
-------------

* Added a reference to a Computer Physics Communications paper about TRIQS.
* Numerous updates to the documentation.

New in version 1.2
==================

C++
---

* New wrapping of boost random generators. No effect on API.

Python
------

* New wrapper tool to glue C++ and Python. Cython is gone.

Green Functions
---------------

* `transpose()` method now returns a NEW green function, like `conjugate()`, etc...
* Suppress the possibility to save/load in HDF5 for a tail alone (must be part of a GF).


