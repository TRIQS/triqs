###################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011-2017 by O. Parcollet, M. Ferrero, A. Antipov
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

# nasty policy for escaping in string....
# cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0005 NEW)
cmake_policy(SET CMP0003 NEW) #cmake --help-policy CMP0003
cmake_policy(SET CMP0018 NEW) # Automatically add -fPIC when necessary

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake) 

#---------------------------------------------------------------------
# Start project
#---------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project (triqs C CXX)

## C & C++ Headers
include(CheckIncludeFile)
include(CheckIncludeFileCXX)

#---------------------------------------------------------------------
# Compiler check 
#---------------------------------------------------------------------

message( STATUS "-------- Checking compiler C++ status-------------")

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.0.0)
    message(FATAL_ERROR "The Clang compiler version is ${CMAKE_CXX_COMPILER_VERSION}, but TRIQS requires 5.0.0 or later")
  endif()
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 7.0.0 OR ${CMAKE_CXX_COMPILER_VERSION} VERSION_EQUAL 8.0.0 OR ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 8.0.0)
    message(FATAL_ERROR "The GCC compiler version is ${CMAKE_CXX_COMPILER_VERSION}, but TRIQS requires version 7.x")
  endif()
else()
  message(FATAL_ERROR "TRIQS Has to be compiled with g++ (7.x) or clang (>= 5.0.0)")
endif()
try_compile(_CXX_OK ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/cxx_check.cpp COMPILE_DEFINITIONS -std=c++17)
if (NOT _CXX_OK)
 message(FATAL_ERROR "The compiler does not support C++17")
endif()

#---------------------------------------------------------------------
# Shared lib (static : not checked).
#---------------------------------------------------------------------

# FIXME : static is not checked by CI : do we what to keep it ?
set(BUILD_SHARED_LIBS ON)
#option(BUILD_SHARED_LIBS "Build shared libraries" ON)
string(COMPARE EQUAL ${BUILD_SHARED_LIBS}  "OFF" TRIQS_BUILD_STATIC)

#---------------------------------------------------------------------
# Build type : Release or debug
#---------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type of build" FORCE)
 mark_as_advanced(CMAKE_BUILD_TYPE)
endif()

add_compile_options(-Wno-register $<$<CONFIG:Debug>:-Og>)
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

#---------------------------------------------------------------------
# Cpp2Py
# Only when Python is supported
#---------------------------------------------------------------------

option(PythonSupport "Build with Python support" ON)
if (PythonSupport)
 set(TRIQS_WITH_PYTHON_SUPPORT 1) # for the triqs_config.h file configuration
 message( STATUS "-------- Cpp2Py detection -------------")
 find_package(Cpp2Py 1.5 REQUIRED)
 if (NOT CMAKE_CXX_COMPILER STREQUAL CPP2PY_CXX_COMPILER)
   message(FATAL_ERROR "For your own safety, you should use the same compiler as the one used to compile Cpp2Py !")
 endif()
endif (PythonSupport)

#---------------------------------------------------------------------
# log info
#---------------------------------------------------------------------

message( STATUS "-------- triqs version and git hash detection -------------")

# Find the TRIQS_GIT_HASH
find_package(Git)

# Get the version number from the latest annotated git tag
execute_process(COMMAND bash "-c" "${GIT_EXECUTABLE} describe | sed \"s/-\\([0-9]*\\)-.*/.\\1/g\""
 WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
 OUTPUT_VARIABLE   TRIQS_VERSION
 OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT TRIQS_VERSION)
  set(TRIQS_VERSION 2.0)
endif()

# Get the Git Hash
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
 WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
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

# ---------------------------------
# RPATH setting
# ---------------------------------
# Make sure the rpath is set fine after installation for libtriqs, module, and executables
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
 set(CMAKE_MACOSX_RPATH 1) # new for cmake 3.x
else()
 set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
 set(CMAKE_SKIP_BUILD_RPATH FALSE)
 set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
endif()

# ---------------------------------
# Resolve Clang Linktime Problems
# CMake will adjust any linker flags from '-L path_to/mylib.so' to -lmylib
# if the proper mylib.so is automatically found by the linker, i.e.
# the directory comes first in LIBRARY_PATH.
# The clang linker however ignores LIBRARY_PATH. By unsetting LIBRARY_PATH
# we enforce absolute paths in the linker flags
# ---------------------------------
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message(STATUS "Unsetting LIBRARY_PATH to avoid cmake+clang linktime problems")
  unset(ENV{LIBRARY_PATH})
endif()

# -----------------------------------------------------------------------------
# C++ standard
# Common for all targets below (triqs lib, test, pytriqs, doc examples)
# Here and not in triqs lib, since it is private.
# It should not be exported or an application could not e.g. use a different c++ standard
# -----------------------------------------------------------------------------

add_compile_options(-std=c++17) 

#------------------------
# Build google test
#------------------------

add_subdirectory(google_test)

# -----------------------------------------------------------------------------
# Common option for all targets below (triqs lib, test, pytriqs, doc examples)
# -----------------------------------------------------------------------------
 
include_directories (${CMAKE_SOURCE_DIR})  # to have <triqs/....>

# Because of triqs_config.h
# FIXME : suppress this triqs_config to have the same installed/build header
# options should be given my -DXXX to the triqs targets, they will be exported.
add_compile_options(-DTRIQS_BUILDING_LIBRARY -I ${CMAKE_BINARY_DIR}/Config)

#------------------------
# triqs library
#------------------------

add_subdirectory(triqs)

#---------------------------------------------------------------------
# pytriqs modules 
#---------------------------------------------------------------------

if (PythonSupport)
 message( STATUS "-------- Preparing python extension modules  -------------")
 add_subdirectory(pytriqs)
endif (PythonSupport)

#------------------------
# tests
#------------------------

enable_testing()

option(Build_Tests "Build the tests of the library " ON)
if (Build_Tests)
 message(STATUS "-------- Preparing tests  -------------")
 add_subdirectory(test)
endif()

#------------------------
# Documentation
#------------------------

option(Build_Documentation "Build the documentation ?" OFF)

if (Build_Documentation)
 message( STATUS "-------- Prepare documentation -------------")
 add_subdirectory (doc)
endif (Build_Documentation)

#-----------------------------------------------
# Install the FindXXX that should be exported
#-----------------------------------------------
install (FILES cmake/FindNFFT.cmake DESTINATION share/cmake/Modules)

#---------------------------------------
# triqs_config.h
#---------------------------------------
message(STATUS "-------- Making triqs_config.h  -------------")

# Prepare some variable
SET(PYTHON_NUMPY_VERSION_LT_17 ${CPP2PY_PYTHON_NUMPY_VERSION_LT_17})

# Fortran bindings
# FIXME : should be remove from config, and PRIVATE to triqs
# fortran interface should not be exposed
# MAKE also LAPACK PRIVATE
option(BIND_FORTRAN_LOWERCASE "FORTRAN functions are compiled WITHOUT a trailing underscore" OFF)
mark_as_advanced(BIND_FORTRAN_LOWERCASE)
if (BIND_FORTRAN_LOWERCASE)
 set(TRIQS_BIND_FORTRAN_LOWERCASE OFF )
endif (BIND_FORTRAN_LOWERCASE)

# 
configure_file(./cmake/triqs_config.h.in Config/triqs_config.h @ONLY)

#---------------------------------------
# TRIQSConfig
#---------------------------------------
message( STATUS "-------- Making TRIQSConfig.cmake -------------")

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 configure_file(./cmake/TRIQSConfig.cmake.in Config/TRIQSConfig.cmake @ONLY)
else()
 configure_file(./cmake/TRIQSConfig.cmake.in Config/TRIQSConfig1.cmake @ONLY)
 # I assume there is a basic python available
 EXECUTE_PROCESS(COMMAND python -c  "import re;open('${CMAKE_CURRENT_BINARY_DIR}/Config/TRIQSConfig.cmake','w').write(re.sub(r';*\\s*-framework\\s+([a-zA-Z_][a-zA-Z_0-9]*)',r' \"-framework \\1\"',open('${CMAKE_CURRENT_BINARY_DIR}/Config/TRIQSConfig1.cmake').read()))" , ConfigFile)
endif()

configure_file(./cmake/TRIQSConfig-version.cmake.in Config/TRIQSConfig-version.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Config/TRIQSConfig.cmake DESTINATION lib/cmake/triqs)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Config/TRIQSConfig-version.cmake DESTINATION lib/cmake/triqs)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Config/triqs_config.h DESTINATION include/triqs)

#------------------------
# Test macros
#------------------------

configure_file(./cmake/FindTriqsTest.cmake.in Config//FindTriqsTest.cmake @ONLY)
configure_file(./cmake/run_test.cmake.in Config/run_test.cmake @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Config/FindTriqsTest.cmake DESTINATION share/cmake/Modules)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Config/run_test.cmake DESTINATION share/cmake/Modules)

#--------------------------------------------------------
#   Remind the user how to set up his/her variables
#--------------------------------------------------------

# Configure and install the file to source to setup the environment variables 
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/triqsvars.sh.in ${CMAKE_CURRENT_BINARY_DIR}/triqsvars.sh @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/triqs.modulefile.in ${CMAKE_CURRENT_BINARY_DIR}/triqs.modulefile @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/triqsvars.sh DESTINATION  ${CMAKE_INSTALL_PREFIX}/share)
MESSAGE(STATUS "***************************************************************")
MESSAGE(STATUS "* Use :                                                        ")
MESSAGE(STATUS "*   source ${CMAKE_INSTALL_PREFIX}/share/triqsvars.sh          ")
MESSAGE(STATUS "*   to set up the environment variables                        ") 
if(DEFINED ENV{MODULEPATH})
  MESSAGE(STATUS "*                                                              ")
  MESSAGE(STATUS "* Consider copying ${CMAKE_CURRENT_BINARY_DIR}/triqs.modulefile")
  MESSAGE(STATUS "*   into your environment module directories                   ")
endif()
MESSAGE(STATUS "***************************************************************")

#--------------------------------------------------------
# Packaging
#--------------------------------------------------------
option(BUILD_DEBIAN_PACKAGE "Build a deb package" OFF)
if(BUILD_DEBIAN_PACKAGE)
  if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    message(FATAL_ERROR "CMAKE_INSTALL_PREFIX must be /usr for packaging")
  endif()
  SET(CPACK_GENERATOR "DEB")
  SET(CPACK_PACKAGE_VERSION ${TRIQS_VERSION})
  SET(CPACK_PACKAGE_CONTACT "https://github.com/TRIQS/triqs")
  EXECUTE_PROCESS(COMMAND dpkg --print-architecture OUTPUT_VARIABLE CMAKE_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
  SET(CPACK_DEBIAN_PACKAGE_DEPENDS "python, python-numpy, python-h5py, python-jinja2, python-mako, python-mpi4py, python-matplotlib, python-scipy, libopenmpi-dev, cpp2py (>= 1.5)")
  SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  SET(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
  INCLUDE(CPack)
endif()
