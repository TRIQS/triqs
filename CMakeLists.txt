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

#---------------------------------------------------------------------
# Start project
#---------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project(TRIQS VERSION 2.2.1 LANGUAGES C CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

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
  if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 7.0.0)
    message(FATAL_ERROR "The GCC compiler version is ${CMAKE_CXX_COMPILER_VERSION}, but TRIQS requires 7.0 or later")
  endif()
else()
  message(FATAL_ERROR "TRIQS Has to be compiled with g++ (7.x) or clang (>= 5.0.0)")
endif()
try_compile(_CXX_OK ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/cmake/cxx_check.cpp COMPILE_DEFINITIONS -std=c++17)
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

add_compile_options($<$<CONFIG:Debug>:-Og>)
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

#---------------------------------------------------------------------
# Python
#---------------------------------------------------------------------

option(PythonSupport "Build with Python support" ON)
if (PythonSupport)
 set(TRIQS_WITH_PYTHON_SUPPORT 1) # To export the TRIQSConfig file
endif (PythonSupport)

#---------------------------------------------------------------------
# NDA
#---------------------------------------------------------------------
add_subdirectory(nda)

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
configure_file(./cmake/git_hash.cpp.in triqs/git_hash.cpp @ONLY)

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

# -----------------------------------------------------------------------------
# Common option for all targets below (triqs lib, test, pytriqs, doc examples)
# -----------------------------------------------------------------------------
 
include_directories (${PROJECT_SOURCE_DIR})  # to have <triqs/....>

#------------------------
# triqs library
#------------------------

add_subdirectory(triqs)

#------------------------------
# install the itertools and mpi headers
#------------------------------

install(DIRECTORY itertools mpi DESTINATION include FILES_MATCHING PATTERN "*.hpp")

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
install (FILES cmake/Findsanitizer.cmake DESTINATION share/cmake/Modules)

#---------------------------------------
# Fortran bindings
#---------------------------------------

option(BIND_FORTRAN_LOWERCASE "FORTRAN functions are compiled WITHOUT a trailing underscore" OFF)
mark_as_advanced(BIND_FORTRAN_LOWERCASE)
if (BIND_FORTRAN_LOWERCASE)
 set(TRIQS_BIND_FORTRAN_LOWERCASE OFF )
endif (BIND_FORTRAN_LOWERCASE)

# 

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
  set(CPACK_PACKAGE_NAME triqs)
  SET(CPACK_GENERATOR "DEB")
  SET(CPACK_PACKAGE_VERSION ${TRIQS_VERSION})
  SET(CPACK_PACKAGE_CONTACT "https://github.com/TRIQS/triqs")
  EXECUTE_PROCESS(COMMAND dpkg --print-architecture OUTPUT_VARIABLE CMAKE_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
  SET(CPACK_DEBIAN_PACKAGE_DEPENDS "python, python-numpy, python-h5py, python-jinja2, python-mako, python-mpi4py, python-matplotlib, python-scipy, libopenmpi-dev")
  SET(CPACK_DEBIAN_PACKAGE_CONFLICTS "cpp2py")
  SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  SET(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
  INCLUDE(CPack)
endif()
