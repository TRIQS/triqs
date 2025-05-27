include(external_dependency.cmake)

# Add your dependencies with the function
#
#   external_dependency(name
#       [VERSION <version-number>]
#       [GIT_REPO <url>]
#       [GIT_TAG <tag>]
#       [BUILD_ALWAYS]
#       [EXCLUDE_FROM_ALL]
#   )
#
# Resolve the dependency using the following steps in order.
# If a step was successful, skip the remaining ones.
#
#  1. Use find_package(name [<version-number>])
#     to locate the package in the system.
#     Skip this step if Build_Deps option is set.
#  2. Try to find a directory containing the sources
#     at ${CMAKE_CURRENT_SOURCE_DIR}/name and
#     ${CMAKE_SOURCE_DIR}/deps/name. If found
#     build it as a cmake sub-project.
#  3. If GIT_REPO is provided, git clone the sources,
#     and build them as a cmake sub-project.
#
# Addtional options:
#
#  GIT_TAG - Use this keyword to specify the git-tag, branch or commit hash
#
#  BUILD_ALWAYS - If set, this dependency will always be built from source
#                 and will never be searched in the system.
#
#  EXCLUDE_FROM_ALL - If set, targets of the dependency cmake subproject
#                     will not be included in the ALL target of the project.
#                     In particular the dependency will not be installed.

if(NOT DEFINED Build_Deps)
  set(Build_Deps "Always" CACHE STRING "Do we build dependencies from source? [Never/Always/IfNotFound]")
else()
  set(Build_Deps_Opts "Never" "Always" "IfNotFound")
  if(NOT ${Build_Deps} IN_LIST Build_Deps_Opts)
    message(FATAL_ERROR "Build_Deps option should be either 'Never', 'Always' or 'IfNotFound'")
  endif()
  set(Build_Deps ${Build_Deps} CACHE STRING "Do we build dependencies from source? [Never/Always/IfNotFound]")
  if(NOT Build_Deps STREQUAL "Always" AND (ASAN OR UBSAN))
    message(WARNING "For builds with llvm sanitizers (ASAN/UBSAN) it is recommended to use -DBuild_Deps=Always to avoid false positives.")
  endif()
endif()

# -- Cpp2Py --
if(PythonSupport OR Build_Documentation)
 triqs_external_dependency(Cpp2Py
   GIT_REPO https://github.com/TRIQS/cpp2py
   VERSION 3.3
   GIT_TAG main
 )
 execute_process(COMMAND ${CPP2PY_PYTHON_EXECUTABLE} -c "import mpi4py" RESULT_VARIABLE failed ERROR_QUIET)
 if(failed)
   message(FATAL_ERROR "Failed to import mpi4py Python module.")
 endif()
endif()

# -- GTest --
triqs_external_dependency(GTest
  GIT_REPO https://github.com/google/googletest
  GIT_TAG main
  BUILD_ALWAYS
  EXCLUDE_FROM_ALL
)

# -- google-bench --
if(Build_Benchs)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "")
  triqs_external_dependency(google-bench
    GIT_REPO https://github.com/google/benchmark
    GIT_TAG main
    BUILD_ALWAYS
    EXCLUDE_FROM_ALL
  )
endif()

# -- fmt --
set(FMT_INSTALL ON)
triqs_external_dependency(fmt
  GIT_REPO https://github.com/fmtlib/fmt
  GIT_TAG 11.1.4
)
target_compile_options(fmt PRIVATE $<$<CXX_COMPILER_ID:IntelLLVM>:-Wno-tautological-constant-compare>)

# -- itertools --
triqs_external_dependency(itertools
  GIT_REPO https://github.com/TRIQS/itertools
  VERSION 1.3
  GIT_TAG unstable
  GIT_HASH 913f1ef0361e7f26c97419dc3cf8fe71d6f3ec21
)

# -- h5 --
triqs_external_dependency(h5
  GIT_REPO https://github.com/TRIQS/h5
  VERSION 1.3
  GIT_TAG unstable
  GIT_HASH ea2199931e171cc1c629e41017b43152ce88a93d
)

# -- mpi --
triqs_external_dependency(mpi
  GIT_REPO https://github.com/TRIQS/mpi
  VERSION 1.3
  GIT_TAG unstable
  GIT_HASH f0a19543450fee9d253878c66f10d6a8ba2e222e
)

# -- nda --
triqs_external_dependency(nda
  GIT_REPO https://github.com/TRIQS/nda
  VERSION 1.3
  GIT_TAG unstable
  GIT_HASH 9efdcc922ea3dcccb701fc6d4df1b6f39a492c49
) 

# -- cppdlr --
triqs_external_dependency(cppdlr
  GIT_REPO https://github.com/flatironinstitute/cppdlr
  VERSION 1.1
  GIT_TAG main
  GIT_HASH c9eba2fc0ef9546746fe75c0cb41605118a122ce
)
