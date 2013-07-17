# boost libraries
# python and mpi have already be set before.
# here add the other boost part we want to compile

# we can not be here if boost is installed.. just check
if(NOT BOOST_SOURCE_DIR)
 message( FATAL_ERROR " cmake error in boost")
endif(NOT BOOST_SOURCE_DIR)

# Extract Boost_VERSION and Boost_LIB_VERSION from version.hpp
# Read the whole file:
#
set(BOOST_VERSION 0)
set(BOOST_LIB_VERSION "")
file(READ "${BOOST_SOURCE_DIR}/boost/version.hpp" _boost_VERSION_HPP_CONTENTS)
# message(STATUS "[ ${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE} ] " "location of version.hpp: ${Boost_INCLUDE_DIR}/boost/version.hpp")

string(REGEX REPLACE ".*#define BOOST_VERSION ([0-9]+).*" "\\1" Boost_VERSION "${_boost_VERSION_HPP_CONTENTS}")
string(REGEX REPLACE ".*#define BOOST_LIB_VERSION \"([0-9_]+)\".*" "\\1" Boost_LIB_VERSION "${_boost_VERSION_HPP_CONTENTS}")

if(NOT "${Boost_VERSION}" STREQUAL "0")
 math(EXPR Boost_MAJOR_VERSION "${Boost_VERSION} / 100000")
 math(EXPR Boost_MINOR_VERSION "${Boost_VERSION} / 100 % 1000")
 math(EXPR Boost_SUBMINOR_VERSION "${Boost_VERSION} % 100")

 set(Boost_ERROR_REASON
  "${Boost_ERROR_REASON}Boost version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}\nBoost include path: ${Boost_INCLUDE_DIR}")
endif(NOT "${Boost_VERSION}" STREQUAL "0")

message(STATUS "[ ${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE} ] "
  "version.hpp reveals boost "
  "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")

 ###
set(BOOST_INCLUDE_DIR ${BOOST_INSTALL_ROOT_DIR}/include  PARENT_SCOPE)

# Test version is high enough ...
if(CMAKE_COMPILER_IS_ICC)
 if ( "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}" VERSION_LESS "1.53.0")
  MESSAGE(FATAL_ERROR "With Intel compiler, you need boost version >= 1.53, while this is ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION} ")
 endif()
endif()

MESSAGE(STATUS "I am using boost sources from ${BOOST_SOURCE_DIR} to compile a mini boost_for_triqs ")

# what to compile ?
SET( BUILD_BOOST_SERIALIZATION ON)
#SET( BUILD_BOOST_FILESYSTEM ON)
SET( BUILD_BOOST_SYSTEM ON)

SET( BUILD_BOOST_FILESYSTEM OFF)
SET (BUILD_BOOST_THREAD ON)
SET (BUILD_BOOST_DATE_TIME OFF )
SET (BUILD_BOOST_PROGRAM_OPTIONS OFF)
SET (BUILD_BOOST_REGEX OFF) 

set(ALPS_BOOST_LIBRARY_NAME "boost_for_triqs" )
include (CompileBoost)

include_directories(${BOOST_SOURCE_DIR}) # this one is for the compilation in this dir
if(TRIQS_BUILD_STATIC)
 set(BOOST_LIBRARY boost_for_triqs mpi ${ALPS_EXTRA_LIBRARIES} PARENT_SCOPE )
else(TRIQS_BUILD_STATIC)
 set(BOOST_LIBRARY boost_for_triqs ${ALPS_EXTRA_LIBRARIES} PARENT_SCOPE )
endif(TRIQS_BUILD_STATIC)
set(BOOST_INCLUDE_DIR ${BOOST_SOURCE_DIR} PARENT_SCOPE) # this is for later compilation in the project

# install the python sources 
SET(PYTHON_SOURCES 
 ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py 
 )

if(TRIQS_BUILD_STATIC)

 set_property(GLOBAL APPEND PROPERTY Modules_actually_compiled mpi)

else(TRIQS_BUILD_STATIC)

 install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/boost)
 install (FILES ${CMAKE_CURRENT_BINARY_DIR}/mpi.so  DESTINATION ${TRIQS_PYTHON_LIB_DEST}/boost)

 # for tests
 FILE  (COPY ${PYTHON_SOURCES} DESTINATION ${CMAKE_BINARY_DIR}/pytriqs/boost)
 execute_process(COMMAND ln -fs ${CMAKE_CURRENT_BINARY_DIR}/mpi.so  ${CMAKE_BINARY_DIR}/pytriqs/boost)

endif(TRIQS_BUILD_STATIC)

if(Install_dev)
 install(DIRECTORY numeric DESTINATION include/boost)
endif(Install_dev)
