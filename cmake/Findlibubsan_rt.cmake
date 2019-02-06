# Copyright Nils Wentzell 2018
# Distributed under the GNU GENERAL PUBLIC LICENSE Version 3.0.
# See accompanying file LICENSE or https://www.gnu.org/licenses/gpl-3.0.txt
#
# This cmake find module looks for the LLVM Undefined Behavior Sanitizer Runtime Library
# It sets up : UBSAN_RT_LIBRARY

# This module finds the LLVM Undefined Behavior Sanitizer Runtime Library
# latter case skip to the "Boost CMake" section below.  For the former
#
# Use this module by invoking find_package with the form::
#
#   find_package(libubsan_rt [REQUIRED])
#
# Results are reported in::
#
#    UBSAN_RT_LIBRARY 			Undefined Behavior Sanitizer Runtime Library
#    [UBSAN_MINIMAL_RT_LIBRARY] 	Minimal version of UBSan Runtime, To be used in combination with Asan

set(TRIAL_PATHS
  ENV LIBRARY_PATH
  ENV LD_LIBRARY_PATH
  /usr/lib
  /usr/local/lib
  /usr/lib/gcc/7/*
  /usr/lib/gcc/*/*
  /usr/lib/clang/*/lib/linux
  /usr/lib/llvm-*/lib/clang/*/lib/linux
  /usr/local/opt/llvm/lib/clang/*/lib/darwin
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(name clang_rt.ubsan_osx_dynamic)
  else()
    set(name clang_rt.ubsan_standalone-x86_64)
  endif()
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(name ubsan)
else()
  message(FATAL_ERROR "Undefined Behavior Sanitizer is not available for your compiler")
endif()
  
find_library(UBSAN_RT_LIBRARY
  NAMES ${name}
  PATHS ${TRIAL_PATHS}
)

mark_as_advanced(UBSAN_RT_LIBRARY)

# Try to find UBSan Minimal Runtime for Clang
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(name libclang_rt.ubsan_minimal_osx_dynamic)
  else()
    set(name clang_rt.ubsan_minimal-x86_64)
  endif()

  find_library(UBSAN_MINIMAL_RT_LIBRARY
    NAMES ${name}
    PATHS ${TRIAL_PATHS}
  )
  mark_as_advanced(UBSAN_MINIMAL_RT_LIBRARY)
endif()

# Imported target
add_library(libubsan_rt SHARED IMPORTED)
set_property(TARGET libubsan_rt PROPERTY IMPORTED_LOCATION ${UBSAN_RT_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("Undefined Behavior Sanitizer Runtime Library"
  REQUIRED_VARS UBSAN_RT_LIBRARY
  FAIL_MESSAGE "Undefined Behavior Sanitizer Runtime Libraries not found! Consider installing for additional checks!"
)
