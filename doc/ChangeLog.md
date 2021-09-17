(changelog)=


# Changelog

This document describes the main changes in TRIQS.


## Version 3.1.0


### Green function meshes (C++ only)


* The Green functions are now templated directly on the mesh, while the previous tags have been removed.
  This has little consequence in practice, except for the special case of brillouin_zone (cf below).

* The meshes are regrouped in triqs::mesh namespace

* Some meshes have been abbreviated (with backward compat. aliases).

Before we had : 

```cpp
 auto m = gf_mesh<imfreq>{....};
 auto g = gf<imfreq>{m, {1,1}};
```

Now we have simply

```cpp
 auto m = mesh::imfreq{....};
 auto g = gf<imfreq>{m, {1,1}}; // as before
```

* Due to a name collision, the mesh over brillouin_zone has been renamed `brzone`.
  So now : 

  * brillouin_zone is the triqs::lattice::brillouin_zone representing the Brillouin Zone
  * triqs::mesh::brzone is the mesh on such object

Before : 
```cpp
  auto m = gf_mesh<brillouin_zone>{....};
  auto g = gf<cartesian_product<brillouin_zone,imfreq>> {{m, ...}, ....}; 
```
Now : 

```cpp
  auto m = mesh::brzone{....};
  auto g = gf<prod<brzone,imfreq>>{{m, ...}, ....};
```

* Backward compatibility help : 

 * gf_mesh<T> is aliased to T, with deprecation.

 * Regex Vim to replace the gf_mesh : 
  ```
  :bufdo %s/gf_mesh<\(.\{-}\)>/mesh::\1/gce|w 
  ```
 
* brillouin_zone : the codes have to be modified manually (Find/Replace with confirm. Replace only in gf/gf_mesh template).

 * Aliases are provided for cartesian_product (renamed prod), cyclic_lattice (renamed torus)
   but it does not take care of the direct construction, which has to be modified. 

```
   auto m = cartesian_product {mesh1, mesh2};
```
   can be rewritten simply
```
   auto m = mesh1 * mesh2; // Simpler
   auto m = prod {mesh1, mesh2}; // Ok too.
```


## Version 3.0.1

TRIQS Version 3.0.1 is a patch release that fixes a number of
issues. In particular a crucial issue in relation to the atom_diag
class was resolved.

We provide a more detailed description of the changes and fixes below.

### Issue 819 - Atom Diag

For certain Hamiltonians and orderings of the fundamental_operator_set
the atom_diag class would yield incorrect eigenenergies. Please refer to
issue [#819](https://github.com/TRIQS/triqs/issues/819) for more details.

We provide a simple mechanism to detect if a given model was affected by the issue.
By setting the runtime environment variable `CHECK_ISSUE819` to an arbitrary value with e.g. `export CHECK_ISSUE819=1`
before starting the calculation, TRIQS will now throw an exception if the model was affected before.

This bug was fixed in pull request [#820](https://github.com/TRIQS/triqs/pull/820).

### General
* Add block_matrix to cpp2py converter table
* Add missing includes in utility/variant_extensions.hpp
* Add missing #pragma once in utility/variant_extensions.hpp
* Increase tolerance in test_delta_infty
* Use unittest assert methods for better errors

### array
* For triqs::arrays::vector operator /= fall back to array solution FIX #793
* Fix issue with slicing of array and matrix proxies

### Gf
* Fix issue in block_gf / block2_gf scalar arithmetic + test
* Fix issue in operator= for default constructed block2_gf
* Extend gf_base test to cover slicing in combination with gf evaluation

### cmake
* Change required cpp2py branch to 2.0.x
* Correction issue in TRIQSConfig.cmake.in
* Treat cmake options before adding build dirs, fail if Build_Documentation AND NOT PythonSupport
* Link triqs_py against proper namespaced cpp2py target
* Smaller fixes for nopython build

### jenkins
* Updates for new jenkins node
* Switch osx to gcc-11

### doc
* Updates to install.rst
* Extend conda installation instruction
* oplot: Replace remaining occurances of RI by mode
* In osx install instructions install numpy scipy and mpi4py through pip3 instead of brew

Contributors: DerWeh, Alexander Hampel, Igor Krivenko, Henri Menke, Dylan Simon, Hugo U. R. Strand, Nils Wentzell, Rok Žitko


## Version 3.0.0

TRIQS Version 3.0.0 is a major release that
* introduces compatibility with Python 3 (Python 2 no longer supported)
* separates out the [h5](https://github.com/triqs/h5), [mpi](https://github.com/triqs/mpi) and [itertools](https://github.com/triqs/itertools) functionality of the TRIQS library
* fixes several library issues

A guide with instructions on how to port your application can be found [here](https://github.com/TRIQS/triqs/blob/unstable/doc/porting_to_triqs3.md).
We provide a detailed list of the changes below.

We thank all the people who have contributed to this release: Alexander Hampel, Olivier Parcollet, Malte Schueler, Hiroshi Shinaoka, Dylan Simon, Nils Wentzell, Manuel Zingl, Rok Žitko

### Renamings

The separation of various TRIQS library functionalities into their
own repositories made it necessary to rename various Python modules
as well C++ header files. In addition, we have decided to
rename the top-level Python module of TRIQS from 'pytriqs' to 'triqs'.
These renamings can be automated using the [porting script](https://github.com/TRIQS/triqs/blob/unstable/porting_tools/port_to_triqs3)
that we provide. Please refer to our [porting guide](https://github.com/TRIQS/triqs/blob/unstable/doc/porting_to_triqs3.md) for more
detailed instructions.

### Dependency Management

We are managing the interdependencies of the various
library components [cpp2py](https://github.com/triqs/cpp2py), [h5](https://github.com/triqs/h5), [mpi](https://github.com/triqs/mpi) and [itertools](https://github.com/triqs/h5) using cmake.
Per default cmake will pull those dependencies from their corresponding
GitHub repositories, build them, and install these components together
with TRIQS, unless they are found in your system.
This behavior can be altered using the additional cmake options

* `-DBuild_Deps="Always"` - Always build dependencies, do not try to find them
* `-DBuild_Deps="Never"` - Never build dependencies, but find them instead

during the configuration step.

### h5py dependency

The dependency on h5py has been a source of of problems in setting up
the TRIQS library for many users. We are glad to report that TRIQS 3.0
is no longer dependent on h5py, and instead provides the necessary
Python bindings directly in the [h5](https://github.com/triqs/h5) repository,
which per default is built and installed together with TRIQS.

### Removal of deprecated API

Some parts of the TRIQS library API that have been deprecated in earlier releases
were removed. This includes,
```
InverseFourier, set_from_inverse_fourier, make_gf_from_inverse_fourier, triqs.applications.XXX, g.N1, g.N2, g.beta, g.statistic, g.indicesL, g.indicesR
```
for the Python API and
```
block_gf.domain(), block_gf.mesh(), get_gf_data_shape, get_target_shape
```
in C++.
Note that the [porting script](https://github.com/TRIQS/triqs/blob/unstable/porting_tools/port_to_triqs3) will replace some of these with the proper syntax.


### General
* Port TRIQS and Cpp2Py from Python 2 to Python 3
* Remove dependency on h5py package
* Depend only on header-part of Boost library
* Rename the pytriqs module to triqs
* Add porting script porting_tools/port_to_triqs3
* Split of libpython dependent part of libtriqs into seperate library libtriqs_py
* Remove redundant headers boost_serialization.hpp and serialization.hpp
* Use std::complex_literals::operatori and remove custom operator_j
* Remove triqs/utility/variant.hpp and use std::variant instead
* Make sure that histograms are == comparable in both C++ and Python + Test
* Remove redundant ostream operator<< for std::array, conflicting with nda
* Removal of deprecated parts of Python API
* Removal of deprecated parts of C++ API
* Avoid use of optional.value() for improved osx compatibility
* Disable stacktrace implementation on Mac OS due to failure on OSX10.15 (Catalina)
* Various improvements to triqs++ compiler wrapper
* Fix issues with simultaneous execution of various c++ tests
* Fix bug in lattice tools dos function
* Cpp2pyInfo class no longer requires a base-class
* Regenerate GPL Copyright headers
* Various smaller bugfixes / improvements

### atom_diag
* Make python classes AtomDiagReal and AtomDiagComplex serializable using h5
* Test h5 serialization of atom_diag real/cplx in c++
* Allow mpi broadcasting of AtomDiag objects using the h5 serialization
* In atom_diag h5_read / h5_write, make sure to treat all member variables
* Add operator== for atom_diag objects for c++20 compatible compilers
* Fix fill_first_eigenstate_of_subspace for case of zero subspaces


### cmake
* Bump TRIQS Version number to 3.0.0
* Bump cmake version requirement to 3.3.2
* Add external_dependency.cmake file with function for external dependency management
* Make cpp2py, h5, mpi, itertools, gtest a dependency using add_external_dependency
* Remove dependence on Boost serialization and clean cmake
* Add deps/CMakeLists.txt file to manage dependencies
* In extract_flags.cmake, protect extraction of -L/usr/lib and -I/usr/include
* Disable warnings about comma operator in array subscript
* Extend extract_flags.cmake to treat compiler-specific generator expressions properly
* In extract_flags allow extraction of BUILD_INTERFACE flags
* Fix install permissions for triqs++ compiler wrapper
* No longer add '-Og' for debug builds
* Use -std=c++20 compiler flag if available
* Add -fconcepts flag only for gcc in c++17 mode
* Split off libpython-dependent code into seperate libtriqs_py library
* Suppress certain mpi warnings for ref-file based tests
* Print file differences for failing ref-file based tests
* Use PROJECT_SOURCE/BINARY_DIR over CMAKE_XXX_DIR
* Use namespaced targets for dependencies mpi/itertools/h5
* Add a warning if ASAN/UBSAN is used without Build_Deps=Always
* Use cmake_policy(VERSION ..) over cmake_policy(SET ..)
* Various cleanings in c++/triqs/CMakeLists.txt
* In TRIQSConfig.cmake, properly find target dependencies with find_dependency
* Refactor extract_flags macro and helper functions into cmake/extract_flags.cmake
* Explicitly set CMake Policy CMP0057 and CMP0074 to NEW
* Adjust add_python_test to take MPI_NUMPROC argument for mpi testing
* Use PYTHON_EXECUTABLE over PYTHON_INTERPRETER to be consistent with gtest and pybind
* Make sure to define H5_USE_110_API when compiling against hdf5 1.12
* Consider both PKG_ROOT and PKG_BASE env variables in find-modules for NFFT, GMP, FFTW
* Properly set MATHJAX_PATH in url fallback solution
* Create separate targets for gmp and gmpxx and only link against the first
* Add macro add_mpi_test and make sure test executables are found by mpich
* Allow finding cpp2py in the system
* Use CPP2PY_XXX python variables over bare ones to avoid issues with preinstalled cpp2py
* Use FindGit.cmake in external_dependency.cmake, Improve error message on clone failure

### detmanip
* Remove regenerate on singular det, Throw in regeneration

### doc
* Add porting guide: doc/porting_to_triqs3.md
* Update Ubuntu install instructions for TRIQS 3
* added NRGLjubjana_interface to the list of impurity solvers
* updated applications page to include new hubbardi version 2.2
* Fix example code green_retime.py
* Generate extended toctree for manual documentation pages
* Update easybuild install instructions for 3.0.0 images
* Change the Mathjax fallback solution to use version 2.7.8 @ github
* Add advice on VERSION changes in top-level CMakeLists.txt to porting guide
* Update documentation in histograms python module
* Remove h5py from the list of dependencies
* Fix various c++ and python examples after Python3 port
* Update installation page to include Section on Anaconda packages
* Consistently use r/w/a over old hdf5 file modes H5F_ACC_TRUNC ...
* Change order in OSX install instructions to not use brew clang for the pip no-binary installs
* Fix CC/CXX export commands in osx install instructions
* Bump cmake version requirement to actual required version
* Update link to install page in README to use latest stable version
* Adjust introduction on install page, recommend use of stable releases
* Update list of requirements
* Various smaller improvements

### gf
* Disable automatic tailfit in fourier transform of real-frequency Green functions
* Deprecate the use of string indices
* Allow pytriqs Gf initialization with g[:] = RHS and add test FIX #773
* Provide is_gf_hermitian in python also for imtime and tensor_valued<4>
* Compare block_names also in c++ functions assert_block[2]_gfs_are_close
* Fix warning about improper np.array access pattern
* Properly compare gf indices in tests, fix improper indices construction in various spots
* Fix defintion of density matrix in density(gf_const_view<legendre> gl)
* Check that meshes are compatible in lshift assignment
* When constructing Gf from data, be sure to check compatibility to the mesh
* Allow fourier transform of real-valued imag/real time Green functions
* Add comparison operators and output operators to the gf_indices type
* Add deduction guides for gf{gf_expr} statements
* Bugfix in block_gf(mesh, gf_struct) constructor for scalar_valued gf
* Add imaginary time versions of make_hermitian to the pytriqs gf factories
* Fix wrapper for make_adjoint_mesh(gf_mesh<retime>)
* Change default tolerance of is_gf_hermitian from 1e-13 -> 1e-12
* Fix make_hermitian and is_gf_hermitian for real-valued Green functions
* Fix bug in return type of positive_freq_view(gf&) and add test FIX #764
* Store block_names in block_gf and block2_gf as list of str instead of nparray

### jenkins
* Align Dockerfiles with app4triqs skeleton
* Install lldb debugger into ubuntu and centos base images
* Set CC and CXX env variables in centos-gcc base image
* Set BUILD_ID to avoid caching
* Restore and correct PYTHONPATH for apps
* Force hdf5 1.10 for OSX builds
* add workaround for centos buggy openmpi
* Mark test failures as UNSTABLE
* Add upstream triggers for external dependencies
* Provide vim in packaging base images for error analysis
* Switch to centos build to centos 8
* Update build scripts for Python3

### operators
* Change comparison of many_body_operator_generic to const

### packaging
* Various updates/improvements to memory sanitizer compatible Docker environment (packaging/Dockerfile.msan)
* Generate and test debian packages for Ubuntu 20.04, adjust dependencies


## Version 2.2.3

TRIQS Version 2.2.3 is a bug-fix release.
In addition to various smaller fixes we improved
the usability of TRIQS in conjunction with the LLVM
Memory Sanitizer and the usability of the triqs++
compiler-wrapper.

A detailed description of the changes is provided below.

### General
* In the triqs++ compiler wrapper put any additional flags before cxxflags and ldflags
* Update memory sanitizer docker image with latest development version
* Fix memory sanitizer positive in triqs/arrays/linalg/eigenelements.hpp
* Remove packaging directory from dockerignore

### cmake
* Fix install permissions for triqs++ compiler wrapper
* Be sure to add -Lpath_to_install/lib to triqs++ ldflags
* In extract_flags.cmake, protect extraction of -L/usr/lib and fix filtering of -I/usr/include
* Extend extract_flags.cmake to treat compiler-specific generator expressions properly
* Use extract_flags.cmake of triqs 3.0, properly protect agsinst -I/usr/include flags
* Make sure to supress certain mpi warnings in ref-file based tests

### cpp2py
* Be sure to properly treat both rvalues and lvalues in c2py functions
* Fix type usage in py_converter for integral types

### gf
* Properly import all mesh-types in python/triqs/gf/__init__.py
* Provide is_gf_hermitian in python also for imtime and tensor_valued<4>
* Wrap make_hermitian also for tensor_valued and imtime Green functions

### jenkins
* oxs: force hdf5 1.10


## Version 2.2.2

TRIQS Version 2.2.2 introduces various smaller bug fixes
and adjustments to the documentation. In particular we
now provide a section on the newly added Anaconda packages
on the install page. These packages are the recommended
way to set up TRIQS on Mac OS.
We further removed the dependence of TRIQS on any of the
compiled parts of the boost library.

A more detailed description of the changes is provided below.

### doc
* Add NRGLjubjana_interface to the list of impurity solvers
* Updated Applications page to include new hubbardi version 2.2
* Add section on Anaconda packages to install page
* Fix CC/CXX export commands in osx install instructions

### gf
* Fix construction of gf_indices in various places

### cmake
* Remove dependence on Boost serialization
* Add macro add_mpi_test to improve compatibility against mpich
* Allow a compiler different from TRIQS, but explicitly warn the user

### General
* Fix error in triqs++ ld flags
* Disable stacktrace in OSX error messages due to problems with Mac OS 10.15 (Catalina)
* Fix bug in return type of positive_freq_view(gf&) and add test
* Fix bug in lattice tools dos function
* Remove redundant headers boost_serialization.hpp and serialization.hpp
=======


## Version 2.2.1

TRIQS Version 2.2.1 introduces minor adjustments
to the documentation. It further improves compatibility
for older osx versions and fixes the triqs++ compiler wrapper.

We provide a more detailed description of the changes below.

### doc

* On OSX Install scipy/numpy via pip rather than homebrew for Python2 compatibility
* For debian packages do not source /usr/share/triqsvars.sh
* Bump cmake version requirement to actual required version
* Update link to install page in README to use latest stable version

### cmake
* Fix issues in flag extraction for triqs++ compiler wrapper

### General
* Avoid use of optional.value() for improved osx compatibility


## Version 2.2.0

TRIQS Version 2.2.0 introduces substantial performance
improvements by reworking the array memory allocation and
by further changing the behavior of view types of arrays and
Green functions. The Release also introduces structural
changes to the mpi and itertools components to make them
usable as standalone components.
Beyond that, this release fixes several library issues
and improves the documentation. We provide a more detailed
description of the changes below.

### Make range and itertools functionality standalone

We have pulled the functionality for integer ranges
and the range-adapting functionality (product, enumerate, ...)
out of the TRIQS namespaces and made the headers standalone.
This functionality now lives in the itertools namespace
and resides in the top-level itertools folder.
Tests can be found in test/itertools.


### Make mpi functionality standalone and change namespace

We have pulled the generic mpi functionality
(communicator, gather(basic_type), reduce(basic_type), ...)
out of the triqs::mpi namespace and made the headers standalone.
This functionality now lives in the mpi namespace and resides
in the top-level mpi folder. Tests can be found in test/mpi.
In particular, includes of the header files triqs/mpi/base.hpp and
triqs/mpi/vector.hpp should now be replaced with with includes to
the according file in the mpi directory.


### Non-owning views (C++)

We have changed the behavior of view-types within TRIQS.
Prior to TRIQS 2.2 any view (gf_view, array_view, ...) would make sure that
the data of the underlying object (gf, array, ...) is kept alive
until all views and the object itself are destroyed. This was achieved
through a so-called reference-counting mechanism that lead to
substantial performance penalties in some cases. In TRIQS 2.2 view-types
no longer keep the underlying memory alive and are thus only valid as long as
the base-object is valid.
Let us illustrate this with two examples:

Example 1: Returning view-types

```c++
array_view<int, 1> f() { return array<int, 1>{1, 2, 3}; }
```

Here, the `array<int, 1>` is immediately destroyed at the end of the function body.
The view-type that we return is thus immediately invalid. We should instead return
an `array<int, 1>`.

```c++
array<int, 1> f() { return array<int, 1>{1, 2, 3}; }
```

Example 2: Constructing a view from a temporary

```c++
array_view<int, 1> av{ zeros<int>(10) };
```

Here we construct an `array_view<int 1>` from a temporary of type `array<int, 1>`.
The temporary will be destroyed immediately after the construction,
invalidating the view. We should instead be creating an `array<int, 1>`

```c++
array<int, 1> arr{ zeros(10) };
```

Note that no unnecessary copies will be made in the code above.

Our analysis has shown that in practice this change in behavior rarely affects existing applications.
You can however automatically detect any problems related to this change by using the dynamic analyzer checks described
in the following.

### Dynamic Analyzer Checks (ASAN, UBSAN)

The clang and gcc compiler provide so-called dynamic analyzer checks that allow us to
detect various memory access errors and occurances of undefined behavior.
We have simplified the process of running these checks for TRIQS and any application based
on the app4triqs skeleton by adding additional options to the cmake configuration.
In order to run these checks you will need to configure TRIQS and your application
with -DASAN=ON and/or -DUBSAN=ON.

### No longer install googletest

The build-system of the TRIQS library will no longer install the googletest project as this can
lead to conflicts with system-provided packages. Applications should instead compile and link against
their own version of googletest.
This is done for example in the [app4triqs](https://github.com/TRIQS/app4triqs) application skeleton.

### app4triqs

Starting with TRIQS Version 2.2.0 we will issue compatible releases of the [app4triqs](https://github.com/TRIQS/app4triqs) application skeleton.
These will serve as a basis to the corresponding compatible releases of many of our official applications.
Making adjustments to the project structure directly within [app4triqs](https://github.com/TRIQS/app4triqs)
allows us to easily propagate these changes to all compatible applications.

### arrays
* All views are now borrowed (non-owning) by default
* Major Test cleanings
* Fix make_regular for array_expr
* Allow a=0 for regular arrays
* Initialize std::complex memory blocks by calls to calloc
* Explicitly delete rebind to rvalues for view types
* Move arrays::range to itertools::range
* Reduce hdf5 compression level from 8->1 for higher performance

### cmake
* We now generate and install a triqs++ compiler wrapper
* gtest is no longer installed by TRIQS but should now be compile by the applications (cf. app4triqs)
* Install Findsanitizer.cmake - It locates sanitizer runtime libraries and interface targets
* We now LD_PRELOAD the proper sanitizer runtime libraries when Python tests are executed
* Allow compilation also with the parallel version of hdf5
* Consistently use PROJECT_SOURCE_DIR and PROJECT_BINARY_DIR over the CMAKE_XXX equivalents
* extend add_cpp2py_module function to allow optional header dependencies
* Specify boost library components explicitly, Avoid using BoostConfig.cmake
* Synchronize and clean find_package modules for FFTW GMP GSL and NFFT
* Explicitly prepend `py_` to all python tests
* Adjust the project name and export extra version info
* C++ standard is now propagated through the triqs target
* Create INTERFACE targets for triqs dependencies and export them
  (e.g. triqs::mpi, triqs::blas_lapack)
* Make the remaining python ref tests standalone and adjust FindTest.cmake
* FIX clang+cmake linktime problems
* Add parse_commit script to parse the commit messages into a tag-grouped format
* Add script to generate copyright headers generically
* Use only is_gf_v, is_block_gf_v, Add is_instantiation_of_v<TMPL, T>
* Properly check signbit in ostream operator of real_or_complex FIX #729
* Fix all positives of address and undefined sanitizer
* Add macros FORCEINLINE, TERMINATE, EXPECTS, ASSERT, ENSURES to macros.hpp
* Change the License for the itertools, mpi and cpp2y directories to Apache 2
* Remove and Clean usage of forward compat header c14.hpp and c17.hpp
* Adjust fundamental operator set to respect order in gf_struct #342

### doc
* Major overhaul of the documentation file structure
* Add documentation for fit_legendre function
* Include a markdown cheat sheet in the feature request issue template
* Add issue_templates for bug report and feature request
* Update the website for TRIQS-based applications
* Document functions make_hermitian, is_gf_hermitian, Refactor and clean
* Improve C++ Api documentation of make_zero_tail, fit_tail and fit_hermitian_tail
* Add a page about how to contribute to TRIQS
* Add GreenFunction concept
* Add a brief description for the use of singularity containers
* Add information on OpenMP specific environment variables
* Improve documentation for Dichotomy function
* Update osx install instructions to fully load llvm install into environment
* Minor refactoring of installation page, Update singularity weblinks
* Add warning about compilation memory usage to install page
* Extensions to triqs.css file
* Set up fallback solution in FindMathJax.cmake
* Update the description of versioning
* Add C++2Rst generated rst files, Make the regeneration optional with target docs_cpp2rst
* Update OSX install instructions
* Clean old python-package dependencies from install instructions and docker files
* Update homebrew formula and osx install instructions

### fourier
* Fix generic make_gf_from_fourier for block_gf and block2_gf
* Make direct fourier (ImTime -> Matsubara) checks less rigid
* Issue warning in k-mesh fourier for non-diagonal periodization matrix

### gf
* Generalize is_gf_hermitian and make_hermitian for imtime and rank<4> green functions
* Explicitly check is_within_boundary for index_to_linear and index_to_point in Debug mode
* Remove modulo operation from cluster_mesh::operator[]
* Remove unnecessary modulo operations from index_to_linear of cluster_mesh FIX #725
* Rename slice_t to value_t for all gf target types
* Adjust the the delta(g) function in pytriqs.gf.tools after tail adjustments
* Fix gf_indices construction in partial evaluation of Green functions

### h5
* Protect the write vector of size 0 in h5
* Add missing h5_write/read for vector<long>

### itertools
* Add range slicing capabilities with slice(Range,start,end) + test
* Add range striding capabilities with stride(Range,step) + test
* Move chunk_range into itertools, add omp_chunk (omp version of mpi_chunk) to itertools/omp_chunk.hpp
* Add a doc example for enumerate

### jenkins
* Add sanitization build including both asan and ubsan
* Make linux builds use requirements.txt
* try to exclude large git packs from build
* Switch ubuntu-clang build to libc++ packages
* Add Dockerfile supporting msan with mkl

### mc_generic
* We added separate functions warmup/accumulate

### mpi
* Add range-adapting function mpi::chunk(Range) that distributes over all mpi ranks
* Encapsulate generic mpi functionality in its own mpi namespace
* Adjust slice_range/length to match the API of range(N,M,1) where M is excluded
* Add a generic mpi_reduce_in_place function
* Correction in mpi_reduce for compatibility against older mpis
* Allow conversions mpi::communicator <-> boost::mpi::communicator if boost/mpi.hpp included
* Rename slice_range->chunk_range and slice_length->chunk_length
* Add std::string broadcasting

### lattice
* Store orbital positions and names for bravais_lattice, FIX #724
* Add BrillouinZone member to tight-binding-lattice

### operators
* Add is_almost_zero(precision) function to many_body_operator FIX #745
* Rename reduction_t -> data_t in fundamental op. set

### packaging
* Add Easybuild script for TRIQS release 2.1.1

### pytriqs
* Add missing imports of 'warnings' python module
* Deprecate use of 'inverse' in fourier function names
* Restore compatibility for old numpy versions
* Important Bugfix in Gf * Mat and Mat * Gf arithmetic
* Wrap fit_tail and fit_hermitian_tail for BlockGf
* Generalize implementation of map_block for lambdas of general return type
* Add utility class capture_stdout to capture output
* Make sure to specify module dependencies in all desc files
* Extend make_zero_tail functionality to work on (im/re)time Green functions
* Extend mpi.bcast(x) to bcast(x, root = 0) and recv() to recv(source = 0)

### General
* Reduce hdf5 compression level from 8->1 for higher performance
  (negligible change in file sizes, substantial change in write/read times)


## Version 2.1.1

TRIQS Version 2.1.1 introduces minor adjustments
to the cmake files and the documentation.
It further provides a build script for the
EasyBuild framework.

### jenkins
* Manually pip install numpy first on osx
* Rename docker repo to packaging

### doc
* Use short version of binder link in install page
* Fix github logo

### cmake
* Synchronize and clean find_package modules for FFTW GMP GSL and NFFT
* Allow compilation also with the parallel version of hdf5

### General
* Easyconfig Build Script for TRIQS release 2.1.1


## Version 2.1.0

TRIQS Version 2.1.0 is a bugfix release.
It resolves a variety of issues. In particular it addresses
a series of stability issues with the new treatment of
the high-frequency tails of the Green Function as it was
introduced in Version 2.0.0.

When upgrading to TRIQS 2.1.0 be sure to account for the updated
install instructions. The cpp2py tool will now be automatically
installed together with TRIQS and should no longer be installed
seperately. The corresponding cpp2py deb package should thus
also be removed.

### gf
* Introduce g.target_space() to generate an idx range + test
* Add density function for real-frequency gf/block_gf
* GfReFreq density density_zero_T + test
* Add functions is_gf_hermitian and make_hermitian + tests
* Important fix in pytriqs/gf matrix product and add test, Fix #608

### array
* Add functionality to substack (vstack) matrices + test
* Improve thread-safety of array_views (atomic counter)
* Bugfix for iterators of length zero

### clef
* Fix Issue #475
* Additional Tests for products of matsubara frequencies

### BlockGf
* Enfore string block-names in BlockGf python constructor
* FIX in BlockGf converter to check that names are convertible to std::string

### cmake
* Fix a problem with improper usage of LIBRARY_PATH when using clang
* Integrate cpp2py as a cmake sub-project
* Check that compiler is either clang or gcc, Cleanup some intel remains
* FindNFFT.cmake and FindTest.cmake are now installed to share/cmake/Modules
* Adjust cmake-related install dirs to more standard directories
* Replace Alps Find Lapack by System FindLAPACK.cmake
* Check that cpp2py and triqs compiler match
* Only generate but do not install the TRIQS modulefile
* Export GIT_EXECUTABLE into TRIQSConfig

### lapack
* replace fortran wrappers
* A fix that avoids unnecessary copies in gesvd gelss

### lattice
* Make tight_binding object h5 read and writeable + test
* Fix h5 read/write for cyclic_lattice mesh

### tail
* Fix tail least-squares error: properly normalize, Small bugfixes
* Fix assymetry bug in replace_by_tail
* Fix in BlockGf functionality of make_zero_tail
* Add fit_hermitian_tail function that enforces G(i,j)(iw) = conj(G(j,i)(-iw))
* Add tail_issues test summarizing any resolved problems with high frequency moments
* Adjust maximum fitting order based on largest fitted frequency
* Important Bugfix in known_moment fitting

### det_manip
* Add try_change_col_row and complete
* expose precision warning and error bounds

### atom_diag
* Incorporate patch for empty subspaces
* We now expose n_min, n_max and f_shell + test

### mc_tools
* Assert that a move has been registered in attempt() FIX #663
* collect_results: Inform user that we are waiting for other mpi-threads
* make measure timer optional, Fix #650

### doc
* Add warning about compilation memory usage to install page
* Commit generated rst files, add an optional target for regeneration (docs_cpp2rst)
* Add documentation for the fit_tail functionality, update to fourier docs
* Change brew install instructions to use python@2
* Updated install instructions after cpp2py inclusion
* Update install script for master branch

### General
* Fix the delta(g) function in pytriqs.gf.tools after tail adjustments
* Generalize fit_[hermitian_]tail for BlockGf, Some cleanining in make_gf_from_fourier
* time_pt h5 serialization
* Add REQUIRES macro for clang/gcc to start using concept-style template constraints
* Bugfix in linear interpolation, adjust rouding error margin, FIX #668
* Add missing HDFArchiveInert import to pytriqs.archive FIX #671
* Fix MeshPoint arithmetic in Python FIX #557
* Fix backward compat for mesh_brillouin_zone h5_read
* Generalize the density(G, tail) function to block Gfs
* Add range generating function product_range(i,j,k,..) + test
* Add python function make_zero_tail in accordance with cpp implementation
* Add key-word argument for center of SemiCircular
* Generalize legendre functionality for arbitrary target types
* Fix Issue #649, #638, #624, #581, #594
* Add comparison of tight_binding object for c++ and python
* Add known moments argument to density calulculation
* Add make_gf_from_fourier<n1,n2>(Gf_multivar, mesh1, mesh2)
* Add wrapping of fit_tail for scalar_valued gf's
* Add real / imag for block[2] gf and test

### pytriqs
* Add fit_legendre(gt,order) function + test
* Adjust descriptors for BlockGf Functionality FIX #106
* Fix of Flat descriptor for scalar_valued gf

### fourier
* Fix generic make_gf_from_fourier for block_gf and block2_gf
* Make direct fourier (ImTime -> Matsubara) checks less rigid
* Updated Python Fourier test for BlockGf
* Allow known_moment fitting for block_gf and block2_gf
* Assume vanishing 0th moment in tail-fit, improved error msgs


## Version 2.0.0

### fourier,tail
* Allowing known moments in direct fourier to python interface

### cmake
* Add -Og flag for debug mode

### hdf5
* enable compression in c++ h5write of array / array_stack

### arrays
* Add swap_index_view

### doc
* wip
* remove tail doc for C++

### gf/py
* Add g + 1, 1+g for scalar_valued gf. Clean implementation

### py
* Add wrapper for density for scalar_valued

### General
* grab the version number from the latest annotated git tag
* Move out ipython notebooks to github.com/OParcollet/tail_studies
* Update Dockerfile.build
* add back Jenkinsfile
* Adjust fourier known moments interface, all tests passing
*  Fourier real, WIP. Dont fit moments for retime
* Add known moments to set_from_fourier functions
* More verbose mesh print functionality
* Adjust interface of tail_fitter and tail_fit_on_window
* Exposet n_tail_max to user. Change fit_tail API.
* Fix ambiguity in fit_tail for imfreq. Add wrapper in python
* Add fit_and_replace function for backward compatiblity, Add restricted_view(Gf,N), Tests
* Putting tail_fitter into refreq, further cleaning
* disabling gf mpi_gather / scatter functionality in tests
* Bugfix in flatten2d for empty array, cleaning and test
* Fixing fourier imtime and test
*  Fix array comparison of borrow and nonborrowed view
* Add fourier block test
* Add points around beta to Fit Derivative function
* Move to order 8
* Descriptor Fourier updaate
*  add back density test
* Add working notebook for gtau tail
* Implement evaluator, adding tests, refactoring/checking failing tests
* BUGFIX in multivariable get_tail call, refactoring and cleaning
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
* Add testing nb : temporary
* Build only Vandermonde on first get_tail call and reuse
* Bugfix, fixed moment fitting working
* Adding implementation of  known moments fitting
* update itertools test for make_product, cleaning
* clean gelss_cache, return lls error, introduce make_product(array<range,N>)
* add gesvd header include to gelss
* Rename lapack test, move gelss_cache into gelss.hpp
*  Add gelss_cache object, test and gesvd lapack interface
* tail notebook
* Revert "draft complex_Freq"
* draft complex_Freq

### travis
* Disable clang

### gf
* Add Pade for scalar_valued function
* Add evaluator with slice
* density for bosonic Gf
* Evaluator on BZ.
* Add Evaluator for g(k,om) : experimental
* Add * and sign for statistic_enum
* Add linear_dims in Python interface

### tail
* Add replace_by_tail without n_min
* Rename get_tail -> fit_tail, further cleaning after creation of tail_fitter class
* pulling out tail_fitter
* Remove old tail in Python : port tests
* Remove Old Singularity, further cleaning and fixes
* Add wrapper for get_tail with known moments
* Replacing tail in fourier / density, smaller bugfixes
* Adjust n_moments automatically
* Add tests

### pytriqs
* Small fix in add of gf
* Add map_block function to pytriqs.gf, maps function to every element of Block[2]Gf
* adding fourier factory functions to python interface + tests

### pytriqs/gf
* fix typo in new add scalar code
* Adding functionality for scalar_valued gf in python + tests

### test
* gf_density boson test fails! n = infty...
* tail fit sigma cancellation test

### tail,fourier
* Refactoring fourier matsubara and fourier lattice tests

###fourier
* Add checkfor known moments to fourier
* Adjustments in gf_fourier.py for block_gf and precision
* add overloads for block/block2, some tests and cleaning
* Additional fourier tests and test cleaning
* Adding factory functions for lattice fourier + tests

## Version 1.5


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


### API changes

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


## Version 1.4

### Documentation

* Replace Doxygen by the local simpler tool c++2doc.

### Installation


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


### Many-body operators


* They are now polymorphic, they can handle complex and real case.
  many_body_operator is not a template any more.
  many_body_operator_generic<ScalarType> is the general case.

* The are in triqs::operators namespace (moved from utility).

### Green's functions

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


### Tests

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




## Version 1.3

### Installation

* TRIQS depends on a number of third-party Sphinx extensions to build its documentation.
  These extensions are now installed under `${CMAKE_INSTALL_PREFIX}/share/triqs/sphinxext`, in order to let TRIQS-based application to reuse the extensions and to inherit the visual style of the docs.
* Preinstalled Sphinx extension sphinxcontrib-doxylink is not needed anymore to build the documentation.

### Green functions

* New Python method `DOS.copy()`.
* New convenience functions in module `pytriqs.gf.local.tools`,

  * `inverse()`: compute the inverse of a Green's function;
  * `delta()`: compute Delta_iw from G0_iw;
  * `dyson()`: solve Dyson's equation for given two of G0_iw, G_iw and Sigma_iw to yield the third;
  * `tail_fit()`: fit the tails of Sigma_iw and optionally G_iw;
  * `read_gf_from_txt()`/`write_gf_to_txt()`: read/write a GfReFreq from/to text files.

### Many-body operators

* HDF5 support.
* Various utilities related to the many-body operators are organized in a separate sub-package `pytriqs.operators.util`. It includes the following modules,

  * `hamiltonian`: commonly-used Hamiltonians, Slater, Kanamori, density-density;
  * `U_matrix` (moved from dft_tools): construction of interaction matrices;
  * `observables`: operators of commonly used observables (S_z, S^2, L_z, L^2, L*S and so on);
  * `op_struct`: auxiliary functions to work with spin and orbital indices.

### Monte-Carlo tools

* Function `mc_generic::start()` returns different exit codes depending on the way it has been stopped, by receiving a signal or by `stop_callback()`.

### Determinant manipulation

* New functions `insert2()`, `remove2()`, `change_row()` and `change_col()`, which are combinations of `try_*` and `complete_operation()`.
* New functions `insert2_at_end()`, `remove_at_end()` and `remove2_at_end()`.
* Optimized version of `change_one_row_and_one_col()` and other minor optimizations.

### HDF5

* Support for `unsigned long long`.
* The lists and tuples in Python are now written by hdf_archive with keys as "0", "1", "2", "3", instead of "00000000", "00000001", "0000002", "0000003", ...
  This is necessary for C++ compatibility, where `std::vector<T>` is written in HDF5 as a subgroup with object named by their number, for generic T (i.e. not `int`, `double`, `std::string`).
  This change is backward compatible: old archive can be read by new code, but would break h5diff.

### c++2py

* Support for rich comparison methods.
* New C++ attribute `ignore_in_python` that prevents a class method from wrapping in Python.
* One can use two special line prefixes, `type:` and `default:` when documenting a member of the parameter class. They allow to override the corresponding fields in the generated table.
* Converters for `unsigned int`, `unsigned long` and `unsigned long long`.
* Converter for `std::tuple` (from any Python sequence/to `tuple()`).
* `std::pair` is now converted to `tuple()`.

### Tests

* The Google Test framework (https://github.com/google/googletest) has been adopted by the project. Some of C++ test have been ported to it.
* New header file `test_tools.hpp` with convenience functions and additional GTest macros for C++ testing.

### Miscellaneous

* Added a reference to a Computer Physics Communications paper about TRIQS.
* Numerous updates to the documentation.


## Version 1.2

### C++

* New wrapping of boost random generators. No effect on API.
* HDF5 cleaned, now using only C library (not C++) for simpler installation.

### Python

* New wrapper tool to glue C++ and Python. Cython is gone.

### Green Functions

* `transpose()` method now returns a NEW green function, like `conjugate()`, etc...
* Suppress the possibility to save/load in HDF5 for a tail alone (must be part of a GF).



### Version 1.1

* New constructors for the gf [api change]
* Fix for gf expression templates
* The gf tails now have fixed size to avoid mpi problems
* Fixes in gf expression templates
* New python random generator interface
* Fixes for issues #11, #18, #25


### Version 1.0

There have been changes from versions 0.x to 1.0.0 that will most likely have
consequences for your scripts and archives.

### Python classes

The number of python classes in the old 0.x releases was increasing with no
clear naming convention. In TRIQS 1.0 we have unified the naming of the classes
following the `PEP naming conventions
<http://www.python.org/dev/peps/pep-0008/#naming-conventions>`_:

* Package and module names: lowercase with underscores
* Class names: CapWords
* Function names: lowercase with underscores
* Function and method arguments: lowercase with underscores

### Archives

We provide an update script ``porting_tools/update_archive.py`` which should
help you upgrade your archive. The usage is very simple::

  $ python update_archive.py old_archive new_archive

where ``old_archive`` is your old archive to be upgraded and ``new_archive`` is
the name of the new archive. If you encounter problems please post an
issue with a copy of your archive.
