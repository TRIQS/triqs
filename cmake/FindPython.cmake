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
#  - PYTHON_INCLUDE_DIRS : include for compilation
#  - PYTHON_NUMPY_INCLUDE_DIR : include for compilation with numpy
#  - PYTHON_LIBRARY : link flags 
#  - PYTHON_SITE_PKG : path to the standard packages of the python interpreter
#  - PYTHON_EXTRA_LIBS :  libraries which must be linked in when embedding
#  - PYTHON_LINK_FOR_SHARED :  linking flags needed when building a shared lib for external modules

set(PYTHON_MINIMAL_VERSION 2.6.5)

MESSAGE( STATUS "-------- Python detection -------------")

# IF PYTHON_INTERPRETER is not defined, try to find a python
if (NOT PYTHON_INTERPRETER )
 find_program(PYTHON_INTERPRETER python PATHS $ENV{PATH})
endif (NOT PYTHON_INTERPRETER)
if (NOT PYTHON_INTERPRETER)
 MESSAGE(FATAL_ERROR "No python interpreter found")
endif(NOT PYTHON_INTERPRETER)

MESSAGE (STATUS "Python interpreter ${PYTHON_INTERPRETER}")
#
# The function EXEC_PYTHON_SCRIPT executes the_script in  python interpreter
# and set the variable of output_var_name in the calling scope
#
FUNCTION ( EXEC_PYTHON_SCRIPT the_script output_var_name)
 EXECUTE_PROCESS(COMMAND ${PYTHON_INTERPRETER} -c "${the_script}" 
  OUTPUT_VARIABLE res RESULT_VARIABLE returncode OUTPUT_STRIP_TRAILING_WHITESPACE)
 IF (NOT returncode EQUAL 0)
  MESSAGE(FATAL_ERROR "The script : ${the_script} \n did not run properly in the Python interpreter. Check your python installation.") 
 ENDIF (NOT returncode EQUAL 0)
 SET( ${output_var_name} ${res} PARENT_SCOPE)
ENDFUNCTION (EXEC_PYTHON_SCRIPT)

#
# Check the interpreter and its version
#
EXEC_PYTHON_SCRIPT ("import sys, string; print sys.version.split()[0]" PYTHON_VERSION)
STRING(COMPARE GREATER ${PYTHON_MINIMAL_VERSION} ${PYTHON_VERSION} PYTHON_VERSION_NOT_OK)
IF (PYTHON_VERSION_NOT_OK)
 MESSAGE(FATAL_ERROR "Python intepreter version is ${PYTHON_VERSION} . It should be >= ${PYTHON_MINIMAL_VERSION}")
ENDIF (PYTHON_VERSION_NOT_OK)

EXEC_PYTHON_SCRIPT ("import distutils " nulle) # check that distutils is there...
EXEC_PYTHON_SCRIPT ("import numpy" nulle) # check that numpy is there...
EXEC_PYTHON_SCRIPT ("import h5py" nulle) # check that h5py is there...
EXEC_PYTHON_SCRIPT ("import scipy" nulle) # check that scipy is there...
MESSAGE(STATUS "Python interpreter and modules are ok : version ${PYTHON_VERSION}" )

#
# Check for Python include path
#
EXEC_PYTHON_SCRIPT ("import distutils ; from distutils.sysconfig import * ; print distutils.sysconfig.get_python_inc()"  PYTHON_INCLUDE_DIRS )
message(STATUS "PYTHON_INCLUDE_DIRS =  ${PYTHON_INCLUDE_DIRS}" )
mark_as_advanced(PYTHON_INCLUDE_DIRS)
FIND_PATH(TEST_PYTHON_INCLUDE patchlevel.h PATHS ${PYTHON_INCLUDE_DIRS} NO_DEFAULT_PATH)
if (NOT TEST_PYTHON_INCLUDE)
 message (ERROR "The Python herader files have not been found. Please check that you installed the Python headers and not only the interpreter.")
endif (NOT TEST_PYTHON_INCLUDE)
mark_as_advanced(TEST_PYTHON_INCLUDE)

#
# include files for numpy
#
EXEC_PYTHON_SCRIPT ("import numpy;print numpy.get_include()" PYTHON_NUMPY_INCLUDE_DIR)
MESSAGE(STATUS "PYTHON_NUMPY_INCLUDE_DIR = ${PYTHON_NUMPY_INCLUDE_DIR}" )
mark_as_advanced(PYTHON_NUMPY_INCLUDE_DIR)

#
# include files for numpy
#
EXEC_PYTHON_SCRIPT ("import numpy;print numpy.version.version" PYTHON_NUMPY_VERSION)
MESSAGE(STATUS "PYTHON_NUMPY_VERSION = ${PYTHON_NUMPY_VERSION}" )
mark_as_advanced(PYTHON_NUMPY_VERSION)

#
# Check for site packages
#
EXEC_PYTHON_SCRIPT ("from distutils.sysconfig import * ;print get_python_lib(0,0)"
 PYTHON_SITE_PKG)
MESSAGE(STATUS "PYTHON_SITE_PKG = ${PYTHON_SITE_PKG}" )
mark_as_advanced(PYTHON_SITE_PKG)

#
# Check for Python library path
 #
 #EXEC_PYTHON_SCRIPT ("import string; from distutils.sysconfig import * ;print string.join(get_config_vars('VERSION'))"  PYTHON_VERSION_MAJOR_MINOR)         
 EXEC_PYTHON_SCRIPT ("import string; from distutils.sysconfig import *; print '%s/config' % get_python_lib(0,1)" PYTHON_LIBRARY_BASE_PATH)
 EXEC_PYTHON_SCRIPT ("import string; from distutils.sysconfig import *; print 'libpython%s' % string.join(get_config_vars('VERSION'))" PYTHON_LIBRARY_BASE_FILE)
 IF(BUILD_SHARED_LIBS)
  FIND_FILE(PYTHON_LIBRARY NAMES "${PYTHON_LIBRARY_BASE_FILE}.so" PATHS ${PYTHON_LIBRARY_BASE_PATH})
  IF(NOT PYTHON_LIBRARY)
   FIND_FILE(PYTHON_LIBRARY NAMES "${PYTHON_LIBRARY_BASE_FILE}.a" PATHS ${PYTHON_LIBRARY_BASE_PATH})
  ENDIF(NOT PYTHON_LIBRARY)
 ELSE(BUILD_SHARED_LIBS)
  FIND_FILE(PYTHON_LIBRARY NAMES "${PYTHON_LIBRARY_BASE_FILE}.a" PATHS ${PYTHON_LIBRARY_BASE_PATH})
 ENDIF(BUILD_SHARED_LIBS)
 MESSAGE(STATUS "PYTHON_LIBRARY = ${PYTHON_LIBRARY}" )
 mark_as_advanced(PYTHON_LIBRARY)

 #
 # Cython
 #
 EXEC_PYTHON_SCRIPT("from Cython.Compiler import Version as V;print V.version.split('.')[1]" PYTHON_CYTHON_VERSION )
 MESSAGE(STATUS "Found cython 0.${PYTHON_CYTHON_VERSION}.xxx")
 # check version here 

 #
 # libraries which must be linked in when embedding
 #
 EXEC_PYTHON_SCRIPT ("from distutils.sysconfig import * ;print (str(get_config_var('LOCALMODLIBS')) + ' ' + str(get_config_var('LIBS'))).strip()"
  PYTHON_EXTRA_LIBS)
 MESSAGE(STATUS "PYTHON_EXTRA_LIBS =${PYTHON_EXTRA_LIBS}" )
 mark_as_advanced(PYTHON_EXTRA_LIBS)

 #
 # Looking for ipython ... (optional)
 # not very useful... in static case, we should not allow ipython anyway I guess...
 EXECUTE_PROCESS(COMMAND ${PYTHON_INTERPRETER} -c "try :\n import IPython\n print 1\nexcept:\n print 0" OUTPUT_VARIABLE TRIQS_IPYTHON_DETECTED RESULT_VARIABLE returncode OUTPUT_STRIP_TRAILING_WHITESPACE)
 if (TRIQS_IPYTHON_DETECTED) 
  MESSAGE(STATUS "IPython found")
 else (TRIQS_IPYTHON_DETECTED)
  MESSAGE(STATUS "IPython NOT FOUND ")
 endif (TRIQS_IPYTHON_DETECTED)

 #
 # linking flags needed when embedding (building a shared lib)
 # To BE RETESTED
 #
 #EXEC_PYTHON_SCRIPT ("from distutils.sysconfig import *;print get_config_var('LINKFORSHARED')"
 # PYTHON_LINK_FOR_SHARED)
 #MESSAGE(STATUS "PYTHON_LINK_FOR_SHARED =  ${PYTHON_LINK_FOR_SHARED}" )
 #mark_as_advanced(PYTHON_LINK_FOR_SHARED)

 # Correction on Mac
 #IF(APPLE)
 #    SET (PYTHON_LINK_FOR_SHARED -u _PyMac_Error -framework Python)
 #    SET (PYTHON_LINK_MODULE -bundle -undefined dynamic_lookup)
 #ELSE(APPLE)
 #    SET (PYTHON_LINK_MODULE -shared)
 #ENDIF(APPLE)

 set (PYTHONLIBS_FOUND TRUE) #${PYTHON_FOUND})

 # used in BOOST ONLY !!
 FUNCTION(PYTHON_ADD_MODULE _NAME )
  OPTION(PYTHON_ENABLE_MODULE_${_NAME} "Add module ${_NAME}" TRUE)
  OPTION(PYTHON_MODULE_${_NAME}_BUILD_SHARED "Add module ${_NAME} shared" ${BUILD_SHARED_LIBS})

  IF(PYTHON_ENABLE_MODULE_${_NAME})
   IF(PYTHON_MODULE_${_NAME}_BUILD_SHARED)
    SET(PY_MODULE_TYPE MODULE)
   ELSE(PYTHON_MODULE_${_NAME}_BUILD_SHARED)
    SET(PY_MODULE_TYPE STATIC)
    SET_PROPERTY(GLOBAL  APPEND  PROPERTY  PY_STATIC_MODULES_LIST ${_NAME})
   ENDIF(PYTHON_MODULE_${_NAME}_BUILD_SHARED)

   SET_PROPERTY(GLOBAL  APPEND  PROPERTY  PY_MODULES_LIST ${_NAME})
   ADD_LIBRARY(${_NAME} ${PY_MODULE_TYPE} ${ARGN})
   #    TARGET_LINK_LIBRARIES(${_NAME} ${PYTHON_LIBRARIES})

  ENDIF(PYTHON_ENABLE_MODULE_${_NAME})
 ENDFUNCTION(PYTHON_ADD_MODULE)
MESSAGE( STATUS "--------------------------------------------")

