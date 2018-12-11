Version 2.1
-----------

* [cmake] Adjust cmake-related install dirs to more standard directories
* [cmake] Replace Alps Find Lapack by System FindLAPACK.cmake
* Parse commit messages into changelog. TODO: Add formating/grouping
* [clef] Fix #475
* [clef] Test for products of matsubara frequencies
* [lapack] For basic types change const T & arguments to T, formatting
* [lapack] replaces fortran wrappers
* [lattice] tb class dimension check test
* [det_manip] check_mat_inv() op feedback
* [det_manip] expose precision warning and error bounds
* [array] make ref_counters std::atomic
* [util] time_pt h5 serializ string const &
* [util] time_pt h5 serialization
* Slight adjustments in tail_issue test
* [tail] Add test summarizing issues with high frequency moments
* [tail] Adjust maximum fitting order based on omega_max
* [tail] Change type of omega_max in tail fitting routines to double
* [arrays] Add functionality to substack matrices + test
* [gf] Fix tail ERROR: properly normalize, Small bugfixes
* Add try_change_col_row and complete
* [Jenkins] Keep OSX Brew Includes in both C_INCLUDE_PATH and CPLUS_INCLUDE_PATH
* [jenkins] Add brew includes to OSX CPATH
* [cmake] Print message about modulefile if MODULEPATH is set
* Add test for issue #517
* [BlockGf] Enfore string block-names in BlockGf python constructor
* [BlockGf] FIX BlockGf converter to check that names are convertible to std::string
* [Fourier] Updated Python Fourier test for BlockGf
* [pytriqs] Adjust descriptors for BlockGf Functionality FIX #106
* [cmake,doc] Use CPLUS_INCLUDE_PATH over CPATH to favor -isystem includes
* [cmake] Check that cpp2py and triqs compiler match
* [jenkins] Update llvm to 6.0.1
* [cmake] Only generate but do not install the triqs modulefile
* [lapack] Avoid unnecessary copies in gesvd gelss
* [cmake] Consistently specify PUBLIC/PRIVATE/INTERFACE for target-options and libraries
* Update website in README
* [cmake] Add header install directory to exported triqs target
* [pytriqs] Add property delta (mesh-spacing) to MeshReFreq and Time
* [doc] Minor correction in tail doc
* Fix #638 and refactor code
* [test] mpi zero length vector
* DockerFile Change
* [pytriqs] Add more evaluator for some multivar Gf
* [deb] Add libopenmpi-dev dependence for debian package
* [Jenkins] Update cpp2py to latest version
* [doc] Update logos and add both Maxent and SOM to applications
* [doc] Update mac install instructions after boost option change
* Fix #624
* [gfs] Add functions is_gf_hermitian and make_hermitian + tests
* Add comparison of tight_binding object for c++ and python
* [lattice] Make tight_binding object h5 read and writeable + test
* Add known moments to density calulculation
* [fourier] Allow known_moment fitting for block_gf and block2_gf
* [fourier] Allow fourier transform real -> complex with additional data copy
* Add make_gf_from_fourier<n1,n2>(Gf_multivar, mesh1, mesh2)
* [meshes] Add backward compat for read of bz and lattice meshes
* [fit_tail] Fix assymetry bug in replace_by_tail
* Add wrapping of fit_tail for scalar_valued gf's
* Minor Correction in Fourier Warnings
* [tail] IMPORTANT BUGFIX in known_moment fitting
* Minor correction to 96eb8995
* Clean test
* [array] zero length iterator bugfix
* [cmake] Cleaning, Store TRIQS_MPI information in Config.cmake
* [cmake] Fix cmake+clang linktime problems related to LIBRARY_PATH
* [pytriqs/gf] Fix gf matrix product and add test, Fix #608
* [doc] Remove remaining appearences of ipytriqs executables
* [det_manip] Adjust test requirements, Minor violation for certain lapack versions
* [lattice] Fix h5 read/write for cyclic_lattice mesh
* Merge branch 'master' into unstable
* [doc] Update some old links to corresponding github
* [fourier] Assume vanishing 0th moment in tail-fit, improved error msgs
* [fourier] Allow for mesh shift in real-frequency fourier
* [jenkins] move docs to separate gh-pages repo
* [imfreq] Fix: Remove constexpr for gcc 7.0 / 7.1 compatibility
* [fourier] BUGFIX Fix trapezoidal integration correction in direct fourier
* [c++] Minor cleanings
* [doc] Format and reduce ubuntu package dependencies
* [doc] Change brew install instructions to use python@2
* [jenkins] Support brew installed in other places
* [pytriqs] Add explicit import of h5py.highlevel
* Move methaclass of `pytriqs.gf.Gf` after docstring
* [fourier] fix Error and Warning prints
* [meshes] Comparison operators for lattice/bz domains/meshes, cleaning
* [lattice] storing bz for bzmesh to h5
* [fourier] Add additional static asserts, remove requirement on known_moments to be const_view
* [doc] install guide fixes jouvence
* [doc] Update install instructions
* [doc] Change debian package instructions to master branch
* [doc] Add tags to install.rst
* Update cpp2py
* Update cpp2py
* [doc] Update clang installation instructions to version 6.0
* [cmake] Adjust cpp2py version requirement
* [cmake] Adjust cpp2py version requirement
* Update cpp2py
* Update cpp2py version
* Assume vanishing 0th moment in fit_tail for density calculation
* Add real / imag for block[2] gf and test
* [cmake] Export GIT_EXECUTABLE into TRIQSConfig
* [doc] Update install instructions, Cleaning
* [doc] removing Tail in hdf5 doc and fix tail errors in examples
* [doc] removing more mentionings of the tail
* [doc] changelog + rst fixes
* [doc] remove tail
* [doc] statistics analysis gf tail warning fix (#582)
* Dev lattice wrap (#581)
* Merge branch 'master' into unstable
* [cmake] Fall back TRIQS_VERSION to 2.0
* [cpack] Switch to autogenerated shlib and deps
* Include .git directory in docker build and restore git versioning
* [jenkins] Update cpp2py
* [pytriqs] Fix Flat descriptor for scalar_valued gf
* Update cpp2py requirement in packaging
* [doc] Minor Update install script for master branch
* Remove the requirement that cartesian_product rank > 1
* [cmake] Disable triqs-version detection via git as Jenkins builds in non-git folder
* [cmake] Get full triqs version number
* Adjust precision in fit_tail_matsub
* Fix TRIQS_VERSION
* Fix for gcc

Version 2.0
-----------

* grab the version number from the latest annotated git tag
* [fourier] Add checkfor known moments to fourier
* Move out ipython notebooks to github.com/OParcollet/tail_studies
* Update Dockerfile.build
* add back Jenkinsfile
* Adjust fourier known moments interface, all tests passing
* [gf] Add Pade for scalar_valued function
* [gf] Add evaluator with slice
* [gf] density for bosonic Gf
* [test] gf_density boson test fails! n = infty...
* [pytriqs/gf] fix typo in new add scalar code
* Fourier real, WIP. Dont fit moments for retime
* Add known moments to set_from_fourier functions
* [pytriqs] Small fix in add of gf
* [doc] wip
* [fourier] Adjustments in gf_fourier.py for block_gf and precision
* [cmake] Add -Og flag for debug mode
* [pytriqs] Add map_block function to pytriqs.gf, maps function to every element of Block[2]Gf
* [pytriqs] adding fourier factory functions to python interface + tests
* [pytriqs/gf] Adding functionality for scalar_valued gf in python + tests
* [hdf5] enable compression in c++ h5write of array / array_stack
* More verbose mesh print functionality
* [gf/py] Add g + 1, 1+g for scalar_valued gf. Clean implementation
* [py] Add wrapper for density for scalar_valued
* [gf] Evaluator on BZ.
* [gf] Add Evaluator for g(k,om) : experimental
* [test] tail fit sigma cancellation test
* [tail] Add replace_by_tail without n_min
* Adjust interface of tail_fitter and tail_fit_on_window
* Exposet n_tail_max to user. Change fit_tail API.
* Fix ambiguity in fit_tail for imfreq. Add wrapper in python
* Add fit_and_replace function for backward compatiblity, Add restricted_view(Gf,N), Tests
* Putting tail_fitter into refreq, further cleaning
* disabling gf mpi_gather / scatter functionality in tests
* [tail] Rename get_tail -> fit_tail, further cleaning after creation of tail_fitter class
* [tail] pulling out tail_fitter
* [gf] Add * and sign for statistic_enum
* [doc] remove tail doc for C++
* [gf] Add linear_dims in Python interface
* Bugfix in flatten2d for empty array, cleaning and test
* Fixing fourier imtime and test
*  Fix array comparison of borrow and nonborrowed view
* Add fourier block test
* Add points around beta to Fit Derivative function
* [fourier] add overloads for block/block2, some tests and cleaning
* [fourier] Additional fourier tests and test cleaning
* [tail,fourier] Refactoring fourier matsubara and fourier lattice tests
* [fourier] Adding factory functions for lattice fourier + tests
* Move to order 8
* [fourier,tail] Allowing known moments in direct fourier to python interface
* Descriptor Fourier updaate
* add back density test
* Add working notebook for gtau tail
* [tail] Remove old tail in Python : port tests
* Implement evaluator, adding tests, refactoring/checking failing tests
* BUGFIX in multivariable get_tail call, refactoring and cleaning
* [tail] Remove Old Singularity, further cleaning and fixes
* Moving prod mesh deduction rule and std::pair print functionality
* Add get_tail functions for multivariable Green functions + test
* BUGFIX in imfreq.hpp index map for results
* Add initializier list constructor for rank=3 arrays
* Minor refactoring in density.cpp
* Adjust direct fourier and refactor
* update notebook
* Adjust set_tail_parameters and wrap, reenable direct fourier
* Making the vector with the fitting indeces shared, minor refactoring
* Switch to a vector of indices for the fitting procedure
* Automatically determine number of points in tail and step
* push notebook
* Fix R =1, simplify code.
* [tail] Add wrapper for get_tail with known moments
* Add testing nb : temporary
* [tail] Replacing tail in fourier / density, smaller bugfixes
* Build only Vandermonde on first get_tail call and reuse
* Bugfix, fixed moment fitting working
* [tail] Adjust n_moments automatically
* Adding implementation of  known moments fitting
* [tail] Add tests
* [arrays] Add swap_index_view
* update itertools test for make_product, cleaning
* clean gelss_cache, return lls error, introduce make_product(array<range,N>)
* add gesvd header include to gelss
* Rename lapack test, move gelss_cache into gelss.hpp
* Add gelss_cache object, test and gesvd lapack interface
* tail notebook
* Revert "draft complex_Freq"
* draft complex_Freq
* [travis] Disable clang

Version 1.5
-----------

Changes in installation and cmake files
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To port an application's cmakelists:

* The best is to look at a complete example, ctint tutorial or cthyb.
* Import TRIQS AND Cpp2Py which are 2 separate packages now. 
* We now use modern cmake techniques, i.e. imported targets
* Simply link your target to triqs (imported target).

  Remove ``TRIQS_CXX_DEFINITIONS`` ``TRIQS_INCLUDE_ALL`` and co.
* Obsolete : TRIQSTest are deprecated. Cf cthyb and ctint tutorial for examples.
* build_pytriqs is also removed. Cf test/python in cthyb to set up the environment variable properly

* New Python interface for Gf.

EXPLAIN


API changes
~~~~~~~~~~~

* Remove the gf.local module, simply use gf

  ``from pytriqs.gf.local import ... --> from pytriqs.gf import ...``

* ``g.indices -> g.indices[0]``

* Change gf_struct_t in fundamental_operator_set.hpp from

  ``std::map<std::string,std::vector<std::variant<int, std::string>>>``

  to

  ``std::vector<std::pair<std::string,std::vector<std::variant<int, std::string>>>>``
  
  to properly maintain the order of blocks.
  
* In any Python script, replace e.g.

  ``gf_struct = { "up" : [0, 1], "dn" : [0, 1] }``
  by

  ``gf_struct = [ ["up", [0, 1]], ["dn" : [0, 1]] ]``

Deprecation


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

* The library is installed in C++14 mode.

* New install/build conventions for apps.
  Here are the changes to be made:

  * in _desc.py
  
  1) ``m = module(full_name = 'module_name', app_name='application_a')``
     (instead of ``full_name="pytriqs.applications.application_a.module_name"``)
  2) ``m.use_module("gf", "triqs")``
     (instead of ``m.use_module("gf")``)
  3) ``m.add_include(... with path relative to c++ dir (not relative to current dir))``
     (e.g. ``add_include(solver_core.hpp)`` instead of ``add_include("../c++/solver_core.hpp")``)

  * in CMakeLists.txt

  4) add line include_directories(c++)

  * in python/CMakeLists.txt
  
  5) change python_destination: remove pytriqs.applications...
  6) change triqs_python_extension: change to triqs_python_extension_and_install, and remove second argument
  7) change install DESTINATION : replace variable TRIQS_PYTHON_LIB_DEST with PYTHON_LIB_DEST_ROOT

  * in your python scripts:

  8) replace everywhere ``pytriqs.applications.*`` with ``*``

  A more detailed documentation can be found here: https://triqs.github.io/triqs/1.x/tutorials/using_the_lib/application_structure.html


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

We provide an update script ``porting_tools/update_archive.py`` which should
help you upgrade your archive. The usage is very simple::

  $ python update_archive.py old_archive new_archive

where ``old_archive`` is your old archive to be upgraded and ``new_archive`` is
the name of the new archive. If you encounter problems please post an
issue with a copy of your archive.
