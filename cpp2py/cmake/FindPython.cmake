#  Copyright Olivier Parcollet 2012
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
#  Python settings :
#
#  This module checks that :
#  - the python interpreter is working and version >= 2.6.5
#  - it has modules : distutils, numpy
#
#  This module defines the variables
#  - PYTHON_INTERPRETER : name of the python interpreter
#  - PYTHON_VERSION_MAJOR : Python major version found e.g. 2
#  - PYTHON_VERSION_MINOR : Python version found e.g. 7
#  - PYTHON_INCLUDE_DIRS : include for compilation
#  - PYTHON_NUMPY_INCLUDE_DIR : include for compilation with numpy
#  - PYTHON_LIBRARY : link flags
#  - PYTHON_SITE_PKG : path to the standard packages of the python interpreter
#  - PYTHON_EXTRA_LIBS :  libraries which must be linked in when embedding
#  - PYTHON_LINK_FOR_SHARED :  linking flags needed when building a shared lib for external modules

set(PYTHON_MINIMAL_VERSION 2.6.5)
include(FindPackageHandleStandardArgs)

message( STATUS "-------- Python detection -------------")

# IF PYTHON_INTERPRETER is not defined, try to find a python
if(NOT PYTHON_INTERPRETER)
 find_program(PYTHON_INTERPRETER python PATHS $ENV{PATH})
endif(NOT PYTHON_INTERPRETER)
if(NOT PYTHON_INTERPRETER)
 message(FATAL_ERROR "No python interpreter found")
endif(NOT PYTHON_INTERPRETER)

message(STATUS "Python interpreter ${PYTHON_INTERPRETER}")
#
# The function EXEC_PYTHON_SCRIPT executes the_script in  python interpreter
# and set the variable of output_var_name in the calling scope
#
FUNCTION( EXEC_PYTHON_SCRIPT the_script output_var_name)
 EXECUTE_PROCESS(COMMAND ${PYTHON_INTERPRETER} -c "${the_script}"
  OUTPUT_VARIABLE res RESULT_VARIABLE returncode OUTPUT_STRIP_TRAILING_WHITESPACE)
 if(NOT returncode EQUAL 0)
  message(FATAL_ERROR "The script : ${the_script} \n did not run properly in the Python interpreter. Check your python installation.")
 endif(NOT returncode EQUAL 0)
 SET( ${output_var_name} ${res} PARENT_SCOPE)
ENDFUNCTION(EXEC_PYTHON_SCRIPT)

 #
 # Check the interpreter and its version
 #
 EXEC_PYTHON_SCRIPT("import sys, string; print sys.version.split()[0]" PYTHON_VERSION)
 if(${PYTHON_VERSION} VERSION_LESS ${PYTHON_MINIMAL_VERSION})
  message(FATAL_ERROR "Python intepreter version is ${PYTHON_VERSION} . It should be >= ${PYTHON_MINIMAL_VERSION}")
 endif()
 EXEC_PYTHON_SCRIPT("import sys; print sys.version_info[0]" PYTHON_VERSION_MAJOR)
 EXEC_PYTHON_SCRIPT("import sys; print sys.version_info[1]" PYTHON_VERSION_MINOR)
 
 EXEC_PYTHON_SCRIPT("import mako.template" nulle) # check that Mako is there...
 EXEC_PYTHON_SCRIPT("import distutils " nulle) # check that distutils is there...
 EXEC_PYTHON_SCRIPT("import numpy" nulle) # check that numpy is there...
 EXEC_PYTHON_SCRIPT("import h5py" nulle) # check that h5py is there...
 EXEC_PYTHON_SCRIPT("import scipy" nulle) # check that scipy is there...
  
 #EXEC_PYTHON_SCRIPT("import mpi4py" nulle) # check that mpi4py is there...
 
 # Adjust this
 if(Build_Documentation)
  EXEC_PYTHON_SCRIPT("import clang.cindex" nulle) # check that libclang is there...
 endif(Build_Documentation)
 message(STATUS "Python interpreter and modules are ok : version ${PYTHON_VERSION}")
 
 #
 # Check for Python include path
 #
 EXEC_PYTHON_SCRIPT("import distutils ; from distutils.sysconfig import * ; print distutils.sysconfig.get_python_inc()"  PYTHON_INCLUDE_DIRS)
 message(STATUS "PYTHON_INCLUDE_DIRS =  ${PYTHON_INCLUDE_DIRS}")
 mark_as_advanced(PYTHON_INCLUDE_DIRS)
 find_path(TEST_PYTHON_INCLUDE patchlevel.h PATHS ${PYTHON_INCLUDE_DIRS} NO_DEFAULT_PATH)
 if(NOT TEST_PYTHON_INCLUDE)
  message(ERROR "The Python herader files have not been found. Please check that you installed the Python headers and not only the interpreter.")
 endif(NOT TEST_PYTHON_INCLUDE)
 mark_as_advanced(TEST_PYTHON_INCLUDE)
 
 #
 # HDF5 version used by h5py
 #
 EXEC_PYTHON_SCRIPT("import h5py;print h5py.version.hdf5_version" PYTHON_H5PY_HDF5VERSION)
 set(PYTHON_H5PY_HDF5VERSION ${PYTHON_H5PY_HDF5VERSION} CACHE STRING "the hdf5 version that h5py was compiled against" FORCE)
 message(STATUS "PYTHON_H5PY_HDF5VERSION = ${PYTHON_H5PY_HDF5VERSION}")
 
 #
 # include files for numpy
 #
 EXEC_PYTHON_SCRIPT("import numpy;print numpy.get_include()" PYTHON_NUMPY_INCLUDE_DIR)
 message(STATUS "PYTHON_NUMPY_INCLUDE_DIR = ${PYTHON_NUMPY_INCLUDE_DIR}")
 mark_as_advanced(PYTHON_NUMPY_INCLUDE_DIR)
 
 #
 # include files for numpy
 #
 EXEC_PYTHON_SCRIPT("import numpy;print numpy.version.version" PYTHON_NUMPY_VERSION)
 message(STATUS "PYTHON_NUMPY_VERSION = ${PYTHON_NUMPY_VERSION}")
 mark_as_advanced(PYTHON_NUMPY_VERSION)
 
 #
 # Check for site packages
 #
 EXEC_PYTHON_SCRIPT("from distutils.sysconfig import * ;print get_python_lib(0,0)"
  PYTHON_SITE_PKG)
 message(STATUS "PYTHON_SITE_PKG = ${PYTHON_SITE_PKG}")
 mark_as_advanced(PYTHON_SITE_PKG)
 
 #
 # Check for Python library path
 #
 EXEC_PYTHON_SCRIPT("import string; from distutils.sysconfig import *; print '%s' % get_python_lib(0,1)" PYTHON_LIBRARY_BASE_PATH)
 EXEC_PYTHON_SCRIPT("import string; from distutils.sysconfig import *; print 'libpython%s' % string.join(get_config_vars('VERSION'))" PYTHON_LIBRARY_BASE_FILE)
 set( PYTHON_LIBRARY_SEARCH_PATHS "${PYTHON_LIBRARY_BASE_PATH}/.." "${PYTHON_LIBRARY_BASE_PATH}/../x86_64-linux-gnu" "${PYTHON_LIBRARY_BASE_PATH}/../i386-linux-gnu")
 if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  find_file(PYTHON_LIBRARY NAMES "${PYTHON_LIBRARY_BASE_FILE}.dylib" PATHS ${PYTHON_LIBRARY_SEARCH_PATHS})
 else()
  find_file(PYTHON_LIBRARY NAMES "${PYTHON_LIBRARY_BASE_FILE}.so" PATHS ${PYTHON_LIBRARY_SEARCH_PATHS})
 endif()
 message(STATUS "PYTHON_LIBRARY = ${PYTHON_LIBRARY}")
 mark_as_advanced(PYTHON_LIBRARY)

 #
 # libraries which must be linked in when embedding
 #
 EXEC_PYTHON_SCRIPT("from distutils.sysconfig import * ;print (str(get_config_var('LOCALMODLIBS')) + ' ' + str(get_config_var('LIBS'))).strip()"
  PYTHON_EXTRA_LIBS)
 message(STATUS "PYTHON_EXTRA_LIBS =${PYTHON_EXTRA_LIBS}")
 mark_as_advanced(PYTHON_EXTRA_LIBS)

 # FIXME : No clue why this is here ?? Why separate_arguments ?
 # added from issue 181. Does not work on Mac, because of -framework CoreFoundation.
 if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  separate_arguments(PYTHON_EXTRA_LIBS)
 endif()

 set(PYTHONLIBS_FOUND TRUE)

 # Define python_and_numpy interface target
 add_library(python_and_numpy INTERFACE)
 target_link_libraries(python_and_numpy INTERFACE ${PYTHON_LIBRARY} "${PYTHON_EXTRA_LIBS}")
 target_include_directories(python_and_numpy SYSTEM INTERFACE ${PYTHON_INCLUDE_DIRS} ${PYTHON_NUMPY_INCLUDE_DIR})
 target_compile_options(python_and_numpy INTERFACE -Wno-register) # Some version of Python.h still use register

 # The C API of numpy has changed with 1.7.0, the macro is a version switch in a few files of the libs.
 if(PYTHON_NUMPY_VERSION VERSION_LESS "1.7.0")
   set_property(TARGET python_and_numpy PROPERTY INTERFACE_COMPILE_DEFINITIONS PYTHON_NUMPY_VERSION_LT_17)
 endif()

 # Disable the inclusion of the deprecated Numpy API headers starting from 1.8.0. This seems to be buggy for earlier versions.
 if(PYTHON_NUMPY_VERSION VERSION_GREATER "1.8.0")
   set_property(TARGET python_and_numpy PROPERTY INTERFACE_COMPILE_DEFINITIONS NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION)
 endif()

 # Installation : Final destination of the python modules
 if(BUILD_DEBIAN_PACKAGE)
   set(PYTHON_LIB_DEST_DIR dist-packages)
 else()
   set(PYTHON_LIB_DEST_DIR site-packages)
 endif()
 set(PYTHON_LIB_DEST_ROOT lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/${PYTHON_LIB_DEST_DIR} CACHE PATH "Install directory for Python Modules relative to CMAKE_INSTALL_PREFIX")
 message(STATUS "Python modules will be installed in ${CMAKE_INSTALL_PREFIX}/${PYTHON_LIB_DEST_ROOT}")

