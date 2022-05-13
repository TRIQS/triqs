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
 external_dependency(Cpp2Py
   GIT_REPO https://github.com/TRIQS/cpp2py
   VERSION 2.0
   GIT_TAG master
 )
 execute_process(COMMAND ${CPP2PY_PYTHON_EXECUTABLE} -c "import mpi4py" RESULT_VARIABLE failed ERROR_QUIET)
 if(failed)
   message(FATAL_ERROR "Failed to import mpi4py Python module.")
 endif()
endif()

# -- GTest --
external_dependency(GTest
  GIT_REPO https://github.com/google/googletest
  GIT_TAG main
  BUILD_ALWAYS
  EXCLUDE_FROM_ALL
)

# -- google-bench --
if(Build_Benchs)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "")
  external_dependency(google-bench
    GIT_REPO https://github.com/google/benchmark
    GIT_TAG v1.5.2
    BUILD_ALWAYS
    EXCLUDE_FROM_ALL
  )
endif()

# -- itertools --
external_dependency(itertools
  GIT_REPO https://github.com/TRIQS/itertools
  VERSION 1.1
  GIT_TAG unstable
)

# -- mpi --
external_dependency(mpi
  GIT_REPO https://github.com/TRIQS/mpi
  VERSION 1.1
  GIT_TAG unstable
)

# -- h5 --
external_dependency(h5
  GIT_REPO https://github.com/TRIQS/h5
  VERSION 1.1
  GIT_TAG unstable
)

# -- nda --
external_dependency(nda
  GIT_REPO https://github.com/TRIQS/nda
  VERSION 1.1
  GIT_TAG DEV_CUDA_LINALG_MIGUEL
)
