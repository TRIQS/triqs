###################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011-2017 by O. Parcollet, M. Ferrero, A. Antipov
# Copyright (C) 2019-2020 Simons Foundation
#    authors: N. Wentzell
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
###################################################################################

#---------------------------------------------------------------------
# Preliminary : path, policy
#---------------------------------------------------------------------

# Check CMAKE_INSTALL_PREFIX : must be provided and absolute
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR (NOT IS_ABSOLUTE ${CMAKE_INSTALL_PREFIX}))
 message(FATAL_ERROR "CMAKE_INSTALL_PREFIX must be specified and must be an absolute path.\n There is no default.\n Current value : ${CMAKE_INSTALL_PREFIX}\n.  e.g. ... -DCMAKE_INSTALL_PREFIX=$HOME/triqs_install")
endif()
message(STATUS "Installation directory will be ${CMAKE_INSTALL_PREFIX}")

#---------------------------------------------------------------------
# Start project
#---------------------------------------------------------------------

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
cmake_policy(VERSION ${CMAKE_VERSION})

project(TRIQS VERSION 4.0.0 LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# version displayed in the documentation
set(BRANCH_FOR_DOC unstable)

## C & C++ Headers
include(CheckIncludeFile)
include(CheckIncludeFileCXX)

#---------------------------------------------------------------------
# Compiler check 
#---------------------------------------------------------------------

message( STATUS "-------- Checking compiler C++ status-------------")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.0.0)
    message(FATAL_ERROR "The Clang compiler version is ${CMAKE_CXX_COMPILER_VERSION}, but TRIQS requires 19.0.0 or later")
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14.0.0)
    message(FATAL_ERROR "The GCC compiler version is ${CMAKE_CXX_COMPILER_VERSION}, but TRIQS requires 14.0.0 or later")
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "IntelLLVM")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 2023.1.0)
    message(FATAL_ERROR "The IntelLLVM compiler version is ${CMAKE_CXX_COMPILER_VERSION}, but TRIQS requires 2023.1.0 or later")
  endif()
else()
  message(FATAL_ERROR "TRIQS Has to be compiled with g++ (>= 14.0), clang (>= 19.0) or IntelLLVM (>= 2023.1.0). ${CMAKE_CXX_COMPILER_ID} is not supported.")
endif()

#---------------------------------------------------------------------
# CMake Options
#---------------------------------------------------------------------

# Default to Release build type
if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE Release CACHE STRING "Type of build" FORCE)
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Python Support
option(PythonSupport "Build with Python support" ON)
# Documentation
option(Build_Documentation "Build the documentation ?" OFF)
if(Build_Documentation AND NOT PythonSupport)
  message(FATAL_ERROR "Build_Documentation=ON requires PythonSupport to be enabled")
endif()

# Benchmarks
option(Build_Benchs "Build Benchmarks" OFF)

# Testing
option(Build_Tests "Build tests" ON)
if(Build_Tests)
  enable_testing()
endif()

# Build shared libraries by default
option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" ON)

# Build a Debian package; also selects the dist-packages Python install layout
option(BUILD_DEBIAN_PACKAGE "Build a deb package" OFF)

#---------------------------------------------------------------------
# Global Compilation Settings
#---------------------------------------------------------------------

# Export the list of compile-commands into compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Disable compiler extensions
set(CMAKE_CXX_EXTENSIONS OFF)

# TRIQS uses no C++20 modules. Disable module scanning (default ON for C++20+ with Ninja):
# it injects transient @<obj>.modmap flags into compile_commands.json that break clair-c2py
# binding generation when it replays those commands. No-op on CMake < 3.28.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

#---------------------------------------------------------------------
# log info
#---------------------------------------------------------------------

message( STATUS "-------- triqs version and git hash detection -------------")

# Find the TRIQS_GIT_HASH
find_package(Git)

# Get the Git Hash
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
 WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
 OUTPUT_VARIABLE   TRIQS_GIT_HASH
 OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Compute TRIQS_HOSTNAME and TRIQS_COMPILEDBY
execute_process(COMMAND hostname 
    OUTPUT_VARIABLE TRIQS_HOSTNAME 
    RESULT_VARIABLE returncode 
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(COMMAND id -un 
    OUTPUT_VARIABLE TRIQS_COMPILEDBY
    RESULT_VARIABLE returncode
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Triqs version : ${TRIQS_VERSION}")
message(STATUS "Git hash : ${TRIQS_GIT_HASH}")
message(STATUS "Hostname : ${TRIQS_HOSTNAME}")
message(STATUS "Compiled by : ${TRIQS_COMPILEDBY}")

# Compile a small file with the hash
configure_file(./cmake/git_hash.cpp.in c++/triqs/git_hash.cpp @ONLY)

#---------------------------------------------------------------------
# Define GNU standard installation directories
#---------------------------------------------------------------------

include(GNUInstallDirs)

# ---------------------------------
# RPATH setting
# ---------------------------------
# Make sure the rpath is set fine after installation for libtriqs, module, and executables
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}")
  set(CMAKE_MACOSX_RPATH 1)
else()
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
endif()

# ---------------------------------
# Resolve Clang Linktime Problems
# CMake will adjust any linker flags from '-L path_to/mylib.so' to -lmylib
# if the proper mylib.so is automatically found by the linker, i.e.
# the directory comes first in LIBRARY_PATH.
# The clang linker however ignores LIBRARY_PATH.
# We thus explicitly add the content of LIBRARY_PATH to the LDFLAGS
# FIXME For future cmake versions we should populate the
# INTERFACE_LINK_DIRECTORIES of the triqs target
# ---------------------------------
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND DEFINED ENV{LIBRARY_PATH})
  string(REPLACE ":" ";" LINK_DIRS $ENV{LIBRARY_PATH})
  foreach(dir ${LINK_DIRS})
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -L${dir}")
    string(APPEND CMAKE_MODULE_LINKER_FLAGS " -L${dir}")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -L${dir}")
  endforeach()
endif()

# -------------------------------------------------------
# Create an Interface target for compiler warnings
# -------------------------------------------------------
add_library(triqs_warnings INTERFACE)
target_compile_options(triqs_warnings
  INTERFACE
    -Wall
    -Wextra
    -Wfloat-conversion
    -Wpedantic
    -Wno-sign-compare
    $<$<CXX_COMPILER_ID:GNU>:-Wno-comma-subscript>
    $<$<CXX_COMPILER_ID:GNU>:-Wno-psabi> # Disable notes about ABI changes
    $<$<CXX_COMPILER_ID:GNU>:-Wshadow=local>
    $<$<CXX_COMPILER_ID:GNU>:-Wno-attributes>
    $<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated-declarations>
    $<$<CXX_COMPILER_ID:AppleClang,Clang,IntelLLVM>:-Wno-deprecated-comma-subscript>
    $<$<CXX_COMPILER_ID:AppleClang,Clang,IntelLLVM>:-Wno-unknown-warning-option>
    $<$<CXX_COMPILER_ID:AppleClang,Clang,IntelLLVM>:-Wshadow>
    $<$<CXX_COMPILER_ID:AppleClang,Clang,IntelLLVM>:-Wno-gcc-compat>
    $<$<CXX_COMPILER_ID:AppleClang,Clang,IntelLLVM>:-Wno-c++26-extensions>
    $<$<CXX_COMPILER_ID:AppleClang,Clang,IntelLLVM>:-Wno-c++26-compat>
    $<$<CXX_COMPILER_ID:AppleClang,Clang,IntelLLVM>:-Wno-c2y-extensions> # __COUNTER__ in SCOPE_EXIT
    $<$<CXX_COMPILER_ID:IntelLLVM>:-Wno-tautological-constant-compare>
)

#---------------------------------------------------------------------
# External Dependencies (nda, h5, itertools, gtest, ...)
#---------------------------------------------------------------------

add_subdirectory(deps)

#---------------------------------------------------------------------
# Python variables (provided by the c2py dependency, resolved first in deps/)
#---------------------------------------------------------------------
# c2py runs find_package(Python) and promotes Python_EXECUTABLE together with the
# PYTHON_LIB_DEST_ROOT install-layout variable into the cache. Map them onto the public
# TRIQS_PYTHON_* names exported from TRIQSConfig.cmake for downstream applications.
if(PythonSupport)
  set(TRIQS_PYTHON_EXECUTABLE ${Python_EXECUTABLE} CACHE FILEPATH "Python executable")
  set(TRIQS_PYTHON_LIB_DEST_ROOT ${PYTHON_LIB_DEST_ROOT} CACHE PATH "Python library destination")
  message(STATUS "Python modules install path: ${CMAKE_INSTALL_PREFIX}/${TRIQS_PYTHON_LIB_DEST_ROOT}")
endif()

#------------------------
# triqs library
#------------------------

add_subdirectory(c++/triqs)

#---------------------------------------------------------------------
# Python modules 
#---------------------------------------------------------------------

if(PythonSupport)
 set(TRIQS_WITH_PYTHON_SUPPORT 1) # To export the TRIQSConfig file
 message( STATUS "-------- Preparing python extension modules  -------------")
 add_subdirectory(python/triqs)
endif()

#------------------------
# build tests
#------------------------

if(Build_Tests)
 message(STATUS "-------- Preparing tests  -------------")
 add_subdirectory(test)
endif()

#------------------------
# build benchmarks
#------------------------

if(Build_Benchs AND NOT ASAN AND NOT UBSAN)
  add_subdirectory(benchmarks)
endif()

#------------------------
# Documentation
#------------------------

if(Build_Documentation)
 message( STATUS "-------- Prepare documentation -------------")
 add_subdirectory(doc)
endif()

#-----------------------------------------------
# Install the FindXXX that should be exported
#-----------------------------------------------
install (FILES cmake/FindNFFT.cmake DESTINATION share/cmake/Modules)
install (FILES cmake/Findsanitizer.cmake DESTINATION share/cmake/Modules)
install (FILES cmake/extract_flags.cmake DESTINATION share/cmake/Modules)

#---------------------------------------
# Fortran bindings
#---------------------------------------

option(BIND_FORTRAN_LOWERCASE "FORTRAN functions are compiled WITHOUT a trailing underscore" OFF)
mark_as_advanced(BIND_FORTRAN_LOWERCASE)
if(BIND_FORTRAN_LOWERCASE)
 set(TRIQS_BIND_FORTRAN_LOWERCASE OFF)
endif(BIND_FORTRAN_LOWERCASE)

#---------------------------------------
# TRIQSConfig
#---------------------------------------
message( STATUS "-------- Making TRIQSConfig.cmake -------------")

configure_file(./cmake/TRIQSConfig.cmake.in Config/TRIQSConfig.cmake @ONLY)
configure_file(./cmake/TRIQSConfig-version.cmake.in Config/TRIQSConfig-version.cmake @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/Config/TRIQSConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/triqs)
install(FILES ${PROJECT_BINARY_DIR}/Config/TRIQSConfig-version.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/triqs)

#--------------------------------------------------------
#   Remind the user how to set up his/her variables
#--------------------------------------------------------

if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr" AND NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")

  # Configure and install the file to source to setup the environment variables
  configure_file(${PROJECT_SOURCE_DIR}/triqsvars.sh.in ${PROJECT_BINARY_DIR}/triqsvars.sh @ONLY)
  configure_file(${PROJECT_SOURCE_DIR}/triqs.modulefile.in ${PROJECT_BINARY_DIR}/triqs.modulefile @ONLY)

  install(FILES ${PROJECT_BINARY_DIR}/triqsvars.sh DESTINATION share/triqs)
  message(STATUS "***************************************************************")
  message(STATUS "* Use :                                                        ")
  message(STATUS "*   source ${CMAKE_INSTALL_PREFIX}/share/triqs/triqsvars.sh    ")
  message(STATUS "*   to set up the environment variables                        ")
  if(DEFINED ENV{MODULEPATH})
    message(STATUS "*                                                              ")
    message(STATUS "* Consider copying ${PROJECT_BINARY_DIR}/triqs.modulefile      ")
    message(STATUS "*   into your environment module directories                   ")
  endif()
  message(STATUS "***************************************************************")
endif()

#--------------------------------------------------------
# Packaging
#--------------------------------------------------------
if(BUILD_DEBIAN_PACKAGE)
  if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    message(FATAL_ERROR "CMAKE_INSTALL_PREFIX must be /usr for packaging")
  endif()
  set(CPACK_PACKAGE_NAME triqs)
  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_VERSION ${TRIQS_VERSION})
  set(CPACK_PACKAGE_CONTACT "https://github.com/TRIQS/triqs")
  execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "python3, python3-numpy, python3-mako, python3-mpi4py, python3-matplotlib, python3-scipy, libopenmpi-dev")
  set(CPACK_DEBIAN_PACKAGE_CONFLICTS "cpp2py")
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
  INCLUDE(CPack)
endif()
