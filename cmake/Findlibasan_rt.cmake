# Copyright Nils Wentzell 2018
# Distributed under the GNU GENERAL PUBLIC LICENSE Version 3.0.
# See accompanying file LICENSE or https://www.gnu.org/licenses/gpl-3.0.txt
#
# This cmake find module looks for the LLVM Address Sanitizer Runtime Library
# It sets up : ASAN_RT_LIBRARY

# This module finds the LLVM Address Sanitizer Runtime Library
# latter case skip to the "Boost CMake" section below.  For the former
#
# Use this module by invoking find_package with the form::
#
#   find_package(libasan_rt [REQUIRED])
#
# Results are reported in::
#
#    ASAN_RT_LIBRARY 		Address Sanitizer Runtime Library 

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(name clang_rt.asan_osx_dynamic)
  else()
    set(name clang_rt.asan-x86_64)
  endif()
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(name asan)
else()
  message(FATAL_ERROR "LLVM Address Sanitizer is not available for your compiler")
endif()

find_library(ASAN_RT_LIBRARY
  NAMES ${name}
  PATHS
    ENV LIBRARY_PATH
    ENV LD_LIBRARY_PATH
    /usr/lib 
    /usr/local/lib
    /usr/lib/gcc/*/7
    /usr/lib/gcc/*/*
    /usr/lib/clang/*/lib/linux
    /usr/lib/llvm-*/lib/clang/*/lib/linux
    /usr/local/opt/llvm/lib/clang/*/lib/darwin
)

mark_as_advanced(ASAN_RT_LIBRARY)

# Imported target
add_library(libasan_rt SHARED IMPORTED)
set_property(TARGET libasan_rt PROPERTY IMPORTED_LOCATION ${ASAN_RT_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("Address Sanitizer Runtime Library"
  REQUIRED_VARS ASAN_RT_LIBRARY
  FAIL_MESSAGE "Address Sanitizer Runtime Libraries not found! Consider installing for additional checks!"
)
