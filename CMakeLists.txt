###################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2013 by O. Parcollet, M. Ferrero, A. Antipov
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

#version number of the library
set (TRIQS_VERSION "1.5")

# Path for local cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Where is triqs source
set(TRIQS_SOURCE_DIR "${CMAKE_SOURCE_DIR}")

# a default version : by default installation in the build dir :
# useful for developer : everything is at the same place, easy to clean.
# pass -DCMAKE_INSTALL_PREFIX to overrule
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/INSTALL_DIR" CACHE PATH "TRIQS install prefix" )
message(STATUS "Installation directory is ${CMAKE_INSTALL_PREFIX}")

# Forbid compilation in the source dir... to force good practice
# and because I have not checked it (copy of various file before compilation may mess it up)
# option ALLOW_COMPILATION_IN_SOURCES for brew...
if (NOT ALLOW_COMPILATION_IN_SOURCES)
 if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
  message(FATAL_ERROR "On site compilation is not recommended. Use -DALLOW_COMPILATION_IN_SOURCES to overrule this message")
 endif (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
endif()

# nasty policy for escaping in string....
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0005 NEW)
cmake_policy(SET CMP0003 NEW) #cmake --help-policy CMP0003

cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project (AllPrograms C CXX)

## C & C++ Headers
include(CheckIncludeFile)
include(CheckIncludeFileCXX)

# Make sure the rpath is set fine after installation
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# First check the C++ compiler
find_package(CompilerCheck)

# Reuse later for C17 switch
# Add C11 or C14 flag depending on compiler detection
#option(USE_CPP17 "Use C++17 mode (Put OFF to compile in C+14 backward compatibility mode) " ON)
#mark_as_advanced(USE_CPP17)
#if (NOT USE_CPP17)
# MESSAGE(STATUS "*********************************************************************************")
# MESSAGE(STATUS "**                                                                             **")
# MESSAGE(STATUS "**                          Compiling in C++11 mode                            **")
# MESSAGE(STATUS "**                                                                             **")
# MESSAGE(STATUS "**                          !!! Warning !!!!                                   **")
# MESSAGE(STATUS "**                                                                             **")
# MESSAGE(STATUS "**               C++11 mode is deprecated.                                     **")
# MESSAGE(STATUS "**               Some new features of the library are desactivated             **")
# MESSAGE(STATUS "**               C++14 is the default in TRIQS >= 1.4                          **")
# MESSAGE(STATUS "**               C++11 support will be removed in release 1.5 or 1.6           **")
# MESSAGE(STATUS "**               UPGRADE YOUR COMPILER,  Cf documentation/install              **")
# MESSAGE(STATUS "*********************************************************************************")
# set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS}  " -std=c++11 -DTRIQS_CPP11")
#else()
set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS}  " -std=c++1y ")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
#endif()

# Turn the memory check
option(CHECK_MEMORY "Turn on memory check" OFF)
if (CHECK_MEMORY)
  # These options must be activated for the lib AND any code using it or they will give false positive
 set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS} " -DTRIQS_ARRAYS_DEBUG_COUNT_MEMORY -DTRIQS_ARRAYS_CHECK_WEAK_REFS")
endif()

# Include TRIQS cmake macros
find_package(TriqsMacros)

# Static built ? TO BE RECHECKED ...
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
string(COMPARE EQUAL ${BUILD_SHARED_LIBS}  "OFF" TRIQS_BUILD_STATIC)
if (TRIQS_BUILD_STATIC)
 message(STATUS "STATIC Built  ")
else (TRIQS_BUILD_STATIC)
 message(STATUS "DYNAMIC Built ")
endif (TRIQS_BUILD_STATIC)

# RPATH setting
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
 set(CMAKE_MACOSX_RPATH 1) # new for cmake 3.x
else()
 set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
 set(CMAKE_SKIP_BUILD_RPATH FALSE)
 set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
 set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# #################################
# Python support
# #################################
# TEST AND DEBUG THE OFF OPTION
# option is set first time, once for all. No change.
option(PythonSupport "Build with Python support" ON)
if (PythonSupport)

 set(TRIQS_WITH_PYTHON_SUPPORT 1) # for the triqs_config.h file configuration

 option(Python_use_mpi4py "Use mpi4py (if OFF, use boost.mpi)" ON)

 find_package(Python REQUIRED)
 if (NOT PYTHONLIBS_FOUND)
  message (FATAL_ERROR "Cannot find python. You can use -DWithoutPython to build a pure C++ version.")
 endif()

 # added from issue 181. Does not work on Mac, because of -framework CoreFoundation.
 if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  #MESSAGE( ${PYTHON_EXTRA_LIBS})
 separate_arguments(PYTHON_EXTRA_LIBS)
 #MESSAGE( ${PYTHON_EXTRA_LIBS})
endif()

 set (TRIQS_LIBRARY_PYTHON ${PYTHON_LIBRARY} ${PYTHON_EXTRA_LIBS})
 set (TRIQS_INCLUDE_PYTHON ${PYTHON_NUMPY_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS})

 include_directories(SYSTEM ${TRIQS_INCLUDE_PYTHON} )
 #include_directories(SYSTEM ${PYTHON_NUMPY_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS})
 #link_libraries( ${TRIQS_LIBRARY_PYTHON})

 message (STATUS "Numpy include in ${PYTHON_NUMPY_INCLUDE_DIR}")

 # The C API of numpy has changed with 1.7.0, the macro is a version switch in a few files of the libs.
 if (PYTHON_NUMPY_VERSION VERSION_LESS "1.7.0")
  set(TRIQS_NUMPY_VERSION_LT_17 1)
 endif()

 # Installation : Final destination of the python modules
 string(REGEX REPLACE ".*/lib" "lib" TRIQS_PYTHON_LIB_DEST_ROOT ${PYTHON_SITE_PKG} )
 set( TRIQS_PYTHON_LIB_DEST ${TRIQS_PYTHON_LIB_DEST_ROOT}/pytriqs )
 message(STATUS "Python modules will be installed in ${CMAKE_INSTALL_PREFIX}/${TRIQS_PYTHON_LIB_DEST}")

 # build pytriqs scripts
 add_subdirectory(shells bin)

endif (PythonSupport)

# #################################
# MPI
# #################################

message( STATUS "-------- MPI detection -------------")
find_package(MPI)
if (NOT MPI_CXX_FOUND)
 message(FATAL_ERROR "TRIQS requires MPI")
endif()
set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS}  ${MPI_CXX_COMPILE_FLAGS}  ${MPI_C_COMPILE_FLAGS})
#add_definitions( ${MPI_CXX_COMPILE_FLAGS}  ${MPI_C_COMPILE_FLAGS})
include_directories(SYSTEM ${MPI_CXX_INCLUDE_PATH})
#link_libraries( ${MPI_CXX_LIBRARIES})
set(TRIQS_LIBRARY_MPI ${MPI_CXX_LIBRARIES})
set(TRIQS_INCLUDE_MPI ${MPI_CXX_INCLUDE_PATH})
set(TRIQS_COMPILE_FLAGS_MPI ${MPI_CXX_COMPILE_FLAGS}  ${MPI_C_COMPILE_FLAGS})
message(STATUS "MPI CXX compiler : ${MPI_CXX_COMPILER}")
message(STATUS "MPI_COMPILE_FLAGS : ${TRIQS_COMPILE_FLAGS_MPI} ")
message(STATUS "MPI_CXX_INCLUDE_PATH : ${MPI_CXX_INCLUDE_PATH}")

# #################################
# Boost detection
# #################################

message( STATUS "-------- Boost detection -------------")

# if static build, request static lib
if(TRIQS_BUILD_STATIC)
 set(Boost_USE_STATIC_LIBS        ON)
else()
 set(Boost_USE_STATIC_LIBS        OFF)
endif()

# icc requires a higher version
if(CMAKE_COMPILER_IS_ICC)
 set(BOOST_VERSION 1.53)
else()
 set(BOOST_VERSION 1.46)
endif()
find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS ) #mpi serialization system)
if (NOT Boost_FOUND)
 message(FATAL_ERROR "Boost not found")
endif()

mark_as_advanced(Boost_DIR)

include_directories(${Boost_INCLUDE_DIRS})
set(BOOST_LIBRARY ${Boost_LIBRARIES})
set(BOOST_INCLUDE_DIR ${Boost_INCLUDE_DIRS})

set(TRIQS_LIBRARY_BOOST ${Boost_LIBRARIES})
set(TRIQS_INCLUDE_BOOST ${Boost_INCLUDE_DIRS})

message(STATUS  "Boost include dir: ${BOOST_INCLUDE_DIR}")
message(STATUS  "Boost libraries: ${BOOST_LIBRARY}")

# #################################
# max_align_t bug detection
# #################################
try_compile(_max_align_t_is_correct ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/max_align_t_test.cpp COMPILE_DEFINITIONS  -std=c++11 )
if (NOT _max_align_t_is_correct)
  MESSAGE(STATUS "Adding workaround for max_align_t bug (in clang normally)")
 set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS}  -DTRIQS_ADD_MAX_ALIGN_T_WORKAROUND )
else ()
  MESSAGE(STATUS "Checked max_align_t. No workaround needed")
endif()

# #################################
# on 64 bit machines
# #################################
if (CMAKE_SIZEOF_VOID_P EQUAL 8) # for 64 bits machines
 message( STATUS "64 bit machine : Adding -fpic ")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic" )
 set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS} -fpic)
endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

# #################################
# Lapack
# #################################
message( STATUS "-------- Lapack detection -------------")
if (NOT MKLROOT)
#option(LAPACK_64_BIT "Use 64-bit version of LAPACK" OFF)
#set (BIND_FORTRAN_INTEGER_8 ${LAPACK_64_BIT})
if (NOT LAPACK_FOUND)
 find_package(Lapack)
endif (NOT LAPACK_FOUND)
if (REQUIRE_PTHREAD)
 set(LAPACK_LIBRARY "${LAPACK_LIBRARY};${PTHREAD_LIBRARY}")
 set(LAPACK_LIBRARIES "${LAPACK_LIBRARIES};${PTHREAD_LIBRARY}")
endif (REQUIRE_PTHREAD)

set(LAPACK_LIBS ${LAPACK_LIBRARY} ${BLAS_LIBRARY} ${LAPACK_LINKER_FLAGS} CACHE STRING "Flags to link Lapack and Blas")

set(TRIQS_LIBRARY_LAPACK ${LAPACK_LIBS})
if(MAC_VECLIB)
 set(TRIQS_LIBRARY_LAPACK "-framework Accelerate ${TRIQS_LIBRARY_LAPACK}")
endif(MAC_VECLIB)

else(NOT MKLROOT)
 # MKL
 # we use the LP64 mode, not IL64 mode which probably require porting the blas wrappers for MKL_INT and co
 message (STATUS "Using MKL with root : ${MKLROOT}")
 set(TRIQS_LIBRARY_LAPACK "-Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_core.a ${MKLROOT}/lib/intel64/libmkl_sequential.a -Wl,--end-group -lpthread -lm")
if(CMAKE_COMPILER_IS_ICC)
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${MKLROOT}/include")
else()
 # must be GNU
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -I${MKLROOT}/include")
endif()
endif(NOT MKLROOT)

message(STATUS "Lapack libraries : ${TRIQS_LIBRARY_LAPACK}")
#link_libraries(${TRIQS_LIBRARY_LAPACK})

# #################################
# HDF5
# #################################
message( STATUS "-------- HDF5 detection -------------")
# on weiss, it is 2.8.2 and we should not put HL, on 12.04 we need to put it...
if ( ${CMAKE_VERSION} VERSION_LESS "2.8.6") # CHECK THIS BOUND, where are the cmake changelogs ??
 find_package(HDF5 REQUIRED C )
else(${CMAKE_VERSION} VERSION_LESS "2.8.6")
 find_package(HDF5 REQUIRED C HL)
endif(${CMAKE_VERSION} VERSION_LESS "2.8.6")
if(NOT HDF5_FOUND)
 message(FATAL_ERROR "Require hdf5 1.8.2 or higher. Set HDF5_HOME")
endif()
if(HDF5_IS_PARALLEL)
 message(FATAL_ERROR "parallel(MPI) hdf5 is detected. The standard version is preferred.")
endif(HDF5_IS_PARALLEL)
if(HDF5_HL_LIBRARIES)    # CMake 3.6.0 and later puts libhdf5_hl into a separate variable
 list(APPEND HDF5_LIBRARIES ${HDF5_HL_LIBRARIES})
 list(REMOVE_DUPLICATES HDF5_LIBRARIES)
endif(HDF5_HL_LIBRARIES)
message( STATUS " HDF5_LIBRARIES = ${HDF5_LIBRARIES}")
mark_as_advanced(HDF5_DIR) # defined somewhere else ? what is it ?

include_directories (SYSTEM ${HDF5_INCLUDE_DIR})
#link_libraries (${HDF5_LIBRARIES}) 
set(TRIQS_LIBRARY_HDF5  ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES})
set(TRIQS_INCLUDE_HDF5 ${HDF5_INCLUDE_DIR})
set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS} ${HDF5_DEFINITIONS})
set(TRIQS_HDF5_DIFF_EXECUTABLE ${HDF5_DIFF_EXECUTABLE})
GET_FILENAME_COMPONENT(TRIQS_HDF5_COMMAND_PATH ${TRIQS_HDF5_DIFF_EXECUTABLE} PATH)

# Get hdf5 version and add a definition HDF5_LT_189 if version < 1.8.9
execute_process(COMMAND ${HDF5_DIFF_EXECUTABLE} --version OUTPUT_VARIABLE diff_output)
string(REGEX MATCHALL "[0-9]+.[0-9]+.[0-9]+" HDF5_VERSION "${diff_output}")
message(STATUS "hdf5 version: ${HDF5_VERSION}")
if (PythonSupport)
if (NOT "${PYTHON_H5PY_HDF5VERSION}" STREQUAL "${HDF5_VERSION}")
 message(FATAL_ERROR "The HDF5 version of the h5py (${PYTHON_H5PY_HDF5VERSION}) is different from the hdf5 library found for C++ (${HDF5_VERSION}). You MUST have the same version.")
endif()
endif()

# #################################
# Other libs
# #################################
# GMP
message( STATUS "-------- GMP detection -------------")
find_package(GMP)
include_directories(SYSTEM ${GMP_INCLUDE_DIR})
#link_libraries(${GMP_LIBRARIES})
set(TRIQS_LIBRARY_GMP ${GMP_LIBRARIES} ${GMPXX_LIBRARIES})
set(TRIQS_INCLUDE_GMP ${GMP_INCLUDE_DIR})

# FFTW
message( STATUS "-------- FFTW detection -------------")
find_package(FFTW)
include_directories(SYSTEM ${FFTW_INCLUDE_DIR})
#link_libraries( ${FFTW_LIBRARIES})
set(TRIQS_LIBRARY_FFTW ${FFTW_LIBRARIES})
set(TRIQS_INCLUDE_FFTW ${FFTW_INCLUDE_DIR})

# remove the possible horrible pthread bug on os X !!( on gcc, old, before clang... is it really needed now ???)
# check for clang compiler ?? on gcc, os X snow leopard, it MUST be set
# since _REENTRANT is mysteriously set and this leads to random stalling of the code....
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS} -pthread )
endif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# is it useful ?
if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type of build" FORCE)
 mark_as_advanced(CMAKE_BUILD_TYPE)
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Fortran bindings
option(BIND_FORTRAN_LOWERCASE "FORTRAN functions are compiled WITHOUT a trailing underscore" OFF)
mark_as_advanced(BIND_FORTRAN_LOWERCASE)
if (BIND_FORTRAN_LOWERCASE)
 set(TRIQS_BIND_FORTRAN_LOWERCASE OFF )
endif (BIND_FORTRAN_LOWERCASE)

message( STATUS "-------- git and hash detection -------------")
# git
Find_package(Git3) # our local version of FindGit...
if (GIT_FOUND)
 GIT_TREE_INFO ( ${CMAKE_SOURCE_DIR} "GIT")
 message(STATUS "Git hash ${GIT_HASH}")
 message(STATUS "Git branch :  ${GIT_BRANCHES}")
 set (TRIQS_GIT_HASH ${GIT_HASH})
endif (GIT_FOUND)

# Compute WhoAmI and hostname
# Change this : make a version module
EXECUTE_PROCESS(COMMAND hostname OUTPUT_VARIABLE myHostName RESULT_VARIABLE returncode OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS(COMMAND id -un OUTPUT_VARIABLE WhoAmI RESULT_VARIABLE returncode OUTPUT_STRIP_TRAILING_WHITESPACE)
set(TRIQS_HOSTNAME ${myHostName})
set(TRIQS_COMPILEDBY ${WhoAmI})


#
enable_testing()

##------------------------
# Documentation
##------------------------
option(Build_Documentation "Build the documentation ?" OFF)

#------------------------------------------------------
# link libs for shared libs or for pytriqs
#------------------------------------------------------
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 set(TRIQS_LINK_LIBS_PYTHON "-undefined dynamic_lookup")
else()
 set(TRIQS_LINK_LIBS_PYTHON ${TRIQS_LIBRARY_PYTHON})
endif()

set(TRIQS_LINK_LIBS
${TRIQS_LIBRARY_BOOST}
${TRIQS_LINK_LIBS_PYTHON}
${TRIQS_LIBRARY_MPI}
${TRIQS_LIBRARY_HDF5}
${TRIQS_LIBRARY_LAPACK}
${TRIQS_LIBRARY_FFTW}
${TRIQS_LIBRARY_GMP}
${TRIQS_LIBRARY_GSL}
)

# General include header
# remove this dep to C++
include_directories(${TRIQS_SOURCE_DIR})
# for the generated headers for python convertions
include_directories(${CMAKE_BINARY_DIR}/include/)
# Add it to triqs definitions

#--------------------------------
# General C++ compilation flags
#--------------------------------

#message( STATUS "TRIQS_CXX_DEFINITIONS = ${TRIQS_CXX_DEFINITIONS}")

# needed for utility/first_include.hpp that includes either the triqs_config.h installed in the proper directory
# (Case of an installed lib) or the triqs_config.h just generated in the built dir (when compiling the lib itself).
add_definitions ( ${TRIQS_CXX_DEFINITIONS} -DTRIQS_BUILDING_LIBRARY -I ${CMAKE_BINARY_DIR}/Config)

#------------------------
# Not used in the main code, only in TRIQSConfig and wrapper_desc_generator configuration
#------------------------
# for people who want to quickly add everything TRIQS has detected...
set(TRIQS_LIBRARY_ALL ${TRIQS_LIBRARY} ${TRIQS_LIBRARY_BOOST} ${TRIQS_LIBRARY_PYTHON} ${TRIQS_LIBRARY_MPI} ${TRIQS_LIBRARY_HDF5} ${TRIQS_LIBRARY_LAPACK} ${TRIQS_LIBRARY_FFTW} ${TRIQS_LIBRARY_GMP} ${TRIQS_LIBRARY_GSL} )
set(TRIQS_INCLUDE_ALL ${TRIQS_INCLUDE} ${TRIQS_INCLUDE_BOOST} ${TRIQS_INCLUDE_PYTHON} ${TRIQS_INCLUDE_MPI} ${TRIQS_INCLUDE_HDF5} ${TRIQS_INCLUDE_LAPACK} ${TRIQS_INCLUDE_FFTW} ${TRIQS_INCLUDE_GMP} ${TRIQS_INCLUDE_GSL} )
list (REMOVE_DUPLICATES TRIQS_INCLUDE_ALL)

#---------------------------------------------------------------------
# LIBCLANG
#---------------------------------------------------------------------
MESSAGE(STATUS "-------- LibClang detection -------------")

# Try to set TRIQS_LIBCLANG_LOCATION and TRIQS_LIBCLANG_CXX_ADDITIONAL_FLAGS automatically
find_package(LibClang)
MESSAGE(STATUS "LibClang location: ${TRIQS_LIBCLANG_LOCATION}")
MESSAGE(STATUS "LibClang additional flags: ${TRIQS_LIBCLANG_CXX_ADDITIONAL_FLAGS}")
set(TRIQS_LIBCLANG_CXX_ADDITIONAL_FLAGS "${TRIQS_LIBCLANG_CXX_ADDITIONAL_FLAGS}" CACHE STRING "Additional flags to be passed to libclang when parsing with clang")
set(TRIQS_LIBCLANG_LOCATION "${TRIQS_LIBCLANG_LOCATION}" CACHE STRING "Location of the libclang library")
#---------------------------------------------------------------------
# pytriqs modules : MUST be before TRIQS, to have the py_converters
#---------------------------------------------------------------------

if (PythonSupport)
 message( STATUS "-------- Preparing python extension modules  -------------")
 add_subdirectory(${TRIQS_SOURCE_DIR}/pytriqs )
endif (PythonSupport)

#------------------------
# General triqs libraries
#------------------------

add_subdirectory(${TRIQS_SOURCE_DIR}/triqs triqs)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 list (REMOVE_DUPLICATES TRIQS_LINK_LIBS)
endif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#---------------------------------------------------------------------
# Clang-tidy
#---------------------------------------------------------------------

find_program(CLANG_TIDY "clang-tidy" PATHS $ENV{PATH})
if(CLANG_TIDY)
 MESSAGE(STATUS "-------- clang-tidy detected. Making target -------------")
 string(REGEX REPLACE "([^;]+)" "-I/\\1" ALL_INCLUDES "${TRIQS_INCLUDE_ALL}")
 set(tmp ${TRIQS_CXX_DEFINITIONS}) # SHOULD WE separate_arguments always ?
 separate_arguments(tmp)
 add_custom_target(
    clang-tidy
    COMMAND ${CLANG_TIDY}
    ${CMAKE_SOURCE_DIR}/test/triqs/arrays/algorithms.cpp
    -checks=*
    -header-filter='.*hpp'
    --
    ${tmp}
    ${ALL_INCLUDES} -I ${CMAKE_SOURCE_DIR}
    )
endif()

#------------------------
# tests
#------------------------

option(Build_Tests "Build the tests of the library " ON)
if (Build_Tests)
 message( STATUS "-------- Preparing tests  -------------")
 add_subdirectory(${TRIQS_SOURCE_DIR}/test )
endif()

##------------------------
# Tools
##------------------------
add_subdirectory(tools)

#------------------------
# Documentation
#------------------------

if (Build_Documentation)
 message( STATUS "-------- Prepare documentation -------------")
 add_subdirectory (${TRIQS_SOURCE_DIR}/doc )
 #add_dependencies(docs_sphinx py_sources)
 message( STATUS "----------------------------------------")
endif (Build_Documentation)

#------------------------
# FindTRIQS and FindNFFT need to be installed
#------------------------

install (FILES ${CMAKE_SOURCE_DIR}/cmake/FindTRIQS.cmake ${CMAKE_SOURCE_DIR}/cmake/FindNFFT.cmake DESTINATION share/triqs/cmake)

#------------------------
# TRIQS cmake file and config.h
#------------------------

message( STATUS "-------- Making TRIQSConfig.cmake -------------")
get_property(ALL_DEFS GLOBAL PROPERTY TRIQS_COMPIL_DEFINITIONS)
configure_file(./cmake/triqs_config.h.in Config/triqs_config.h @ONLY)

file(READ ${CMAKE_SOURCE_DIR}/shells/pytriqs.sh.in BUILD_PYTRIQS_SCRIPT)
file(READ ${CMAKE_SOURCE_DIR}/shells/ipytriqs.sh.in BUILD_IPYTRIQS_SCRIPT)
file(READ ${CMAKE_SOURCE_DIR}/shells/ipytriqs_notebook.sh.in BUILD_IPYTRIQS_NOTEBOOK_SCRIPT)

set(EXPORT_PYTHONPATH "export PYTHONPATH=\${CMAKE_CURRENT_BINARY_DIR}:\${TRIQS_PATH}/\${TRIQS_PYTHON_LIB_DEST_ROOT}:\$PYTHONPATH")
string(CONFIGURE "${BUILD_PYTRIQS_SCRIPT}" BUILD_PYTRIQS_SCRIPT @ONLY)
string(REPLACE "\"" "\\\"" BUILD_PYTRIQS_SCRIPT "${BUILD_PYTRIQS_SCRIPT}")
string(CONFIGURE "${BUILD_IPYTRIQS_SCRIPT}" BUILD_IPYTRIQS_SCRIPT @ONLY)
string(REPLACE "\"" "\\\"" BUILD_IPYTRIQS_SCRIPT "${BUILD_IPYTRIQS_SCRIPT}")
string(CONFIGURE "${BUILD_IPYTRIQS_NOTEBOOK_SCRIPT}" BUILD_IPYTRIQS_NOTEBOOK_SCRIPT @ONLY)
string(REPLACE "\"" "\\\"" BUILD_IPYTRIQS_NOTEBOOK_SCRIPT "${BUILD_IPYTRIQS_NOTEBOOK_SCRIPT}")
unset(EXPORT_PYTHONPATH)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 configure_file(./cmake/TRIQSConfig.cmake.in Config/TRIQSConfig.cmake @ONLY)
else()
 configure_file(./cmake/TRIQSConfig.cmake.in Config/TRIQSConfig1.cmake @ONLY)
 find_package(Python) # even in the case we use no python, cf issue #40
 EXEC_PYTHON_SCRIPT( "import re;open('${CMAKE_CURRENT_BINARY_DIR}/Config/TRIQSConfig.cmake','w').write(re.sub(r';*\\s*-framework\\s+([a-zA-Z_][a-zA-Z_0-9]*)',r' \"-framework \\1\"',open('${CMAKE_CURRENT_BINARY_DIR}/Config/TRIQSConfig1.cmake').read()))" , ConfigFile)
endif()

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Config/TRIQSConfig.cmake DESTINATION share/triqs/cmake)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Config/triqs_config.h DESTINATION include/triqs)
install(FILES ${CMAKE_SOURCE_DIR}/cmake/FindTRIQS.cmake DESTINATION share/triqs/cmake)

configure_file(./cmake/FindTriqsTest.cmake.in Config//FindTriqsTest.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Config/FindTriqsTest.cmake DESTINATION share/triqs/cmake)
configure_file(./cmake/run_test.cmake.in Config/run_test.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Config/run_test.cmake DESTINATION share/triqs/cmake)

#------------------------
# Packaging
#------------------------

# make the source package
set(TAR_SRC_FILE ${CMAKE_CURRENT_BINARY_DIR}/triqs_src.tgz)
set(TAR_SRC_DIRBUILD ${CMAKE_CURRENT_BINARY_DIR}/triqs_src_tgz_build)
add_custom_command( OUTPUT ${TAR_SRC_FILE} COMMAND mkdir -p  ${TAR_SRC_DIRBUILD} && cd ${TAR_SRC_DIRBUILD} && ${Subversion_SVN_EXECUTABLE} export --force ${Project_WC_URL} triqs_src && tar cvzf ${TAR_SRC_FILE} triqs_src)
add_custom_target(src_tgz DEPENDS ${TAR_SRC_FILE})

# .deb and dmg
INCLUDE(InstallRequiredSystemLibraries)

# Prepare the postinstall and preremoval for debian
configure_file(${TRIQS_SOURCE_DIR}/cmake/prerm.in ${CMAKE_CURRENT_BINARY_DIR}/prerm)
configure_file(${TRIQS_SOURCE_DIR}/cmake/postinst.in ${CMAKE_CURRENT_BINARY_DIR}/postinst)

set(CPACK_PACKAGE_NAME "triqs")
set(CPACK_PACKAGE_FILE_NAME "triqs")
set(CPACK_PACKAGE_VENDOR "The TRIQS collaboration")
set(CPACK_DEBIAN_PACKAGE_SECTION "Science")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TRIQS, a Toolbox for Research in Quantum Interacting Systems")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_CONTACT "Olivier Parcollet <olivier.parcollet@cea.fr>")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "9")
set(CPACK_PACKAGE_VERSION_PATCH "1-2")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "TRIQS_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_STRIP_FILES "bin/pytriqs")
set(CPACK_SOURCE_STRIP_FILES "")
set(CPACK_PACKAGE_EXECUTABLES "pytriqs" "pytriqs")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "openmpi-bin(>=1.4), libblas-dev (>=1.2), ipython (>=0.12), libfftw3-dev (>=3.2), hdf5-tools (>=1.8), python-matplotlib (>=1.1) ")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA ${CMAKE_CURRENT_BINARY_DIR}/prerm;${CMAKE_CURRENT_BINARY_DIR}/postinst)
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 set(CPACK_GENERATOR TGZ DEB)
endif(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

INCLUDE(CPack)

