# Start configuration
cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project(CPP2PY VERSION 1.6.0 LANGUAGES C CXX)
get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)

# Default to Release build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Type of build" FORCE)
endif()
message(STATUS "-------- BUILD-TYPE: ${CMAKE_BUILD_TYPE} -------------")

# Build static library
set(BUILD_SHARED_LIBS OFF)

# Check CMAKE_INSTALL_PREFIX : must be provided and absolute
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR (NOT IS_ABSOLUTE ${CMAKE_INSTALL_PREFIX}))
 message(FATAL_ERROR "CMAKE_INSTALL_PREFIX must be specified and must be an absolute path.\n There is no default.\n Current value : ${CMAKE_INSTALL_PREFIX}\n.  e.g. ... -DCMAKE_INSTALL_PREFIX=$HOME/triqs_install")
endif()
message(STATUS "Installation directory will be ${CMAKE_INSTALL_PREFIX}")
set(CPP2PY_BINARY_DIR ${PROJECT_BINARY_DIR} CACHE STRING "Binary directory of the Cpp2Py Project")

# add ./cmake to the path for module
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# log info
message( STATUS "-------- cpp2py version and git hash detection -------------")

find_package(Git) 

# Get the Git Hash
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
 WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
 OUTPUT_VARIABLE   CPP2PY_GIT_HASH
 OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Cpp2py version : ${CPP2PY_VERSION}")
message(STATUS "Git hash : ${CPP2PY_GIT_HASH}")

# LIBCLANG : try to set LIBCLANG_LOCATION and LIBCLANG_CXX_FLAGS automatically
MESSAGE(STATUS "-------- LibClang detection -------------")

find_package(LibClang)

MESSAGE(STATUS "LibClang location: ${LIBCLANG_LOCATION}")
MESSAGE(STATUS "LibClang additional flags: ${LIBCLANG_CXX_FLAGS}")

# allow user to modify
set(LIBCLANG_CXX_FLAGS "${LIBCLANG_CXX_FLAGS}" CACHE STRING "Additional flags to be passed to libclang when parsing with clang")
set(LIBCLANG_LOCATION "${LIBCLANG_LOCATION}" CACHE STRING "Location of the libclang library")

# Detect Python
find_package(Python)
install(TARGETS python_and_numpy EXPORT cpp2py-targets)

# subdirs
add_subdirectory(bin)            # Executables
add_subdirectory(cmake)          # Exported Cpp2Py-Config
add_subdirectory(include)
add_subdirectory(lib)

# The python modules
add_subdirectory(cpp2py)
add_subdirectory(cpp2rst)
add_subdirectory(cpp2cxx)

#--------------------------------------------------------
#   Libclang Warning
#--------------------------------------------------------

if (NOT LIBCLANG_LOCATION)
 MESSAGE(STATUS "**************************************************************************************")
 MESSAGE(STATUS "**************************          WARNING           ********************************")
 MESSAGE(STATUS "   ")
 MESSAGE(STATUS "    Can not find libclang                                                           ")
 MESSAGE(STATUS "    You can use cpp2py to compile a code, but c++2py, c++2rst, c++2cxx will not work")
 MESSAGE(STATUS "   ")
 MESSAGE(STATUS "**************************          WARNING           ********************************")
 MESSAGE(STATUS "**************************************************************************************")
endif()

#--------------------------------------------------------
#   Remind the user how to set up his/her variables
#--------------------------------------------------------

if(NOT IS_SUBPROJECT)
  # Configure and install the file to source to setup the environment variables
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cpp2pyvars.sh.in ${CMAKE_CURRENT_BINARY_DIR}/cpp2pyvars.sh @ONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cpp2py.modulefile.in ${CMAKE_CURRENT_BINARY_DIR}/cpp2py.modulefile @ONLY)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cpp2pyvars.sh DESTINATION  ${CMAKE_INSTALL_PREFIX}/share)
  message(STATUS "***************************************************************")
  message(STATUS "* Use :                                                        ")
  message(STATUS "*   source ${CMAKE_INSTALL_PREFIX}/share/cpp2pyvars.sh         ")
  message(STATUS "*   to set up the environment variables                        ")
  if(DEFINED ENV{MODULEPATH})
    message(STATUS "*                                                              ")
    message(STATUS "* Consider adding ${CMAKE_CURRENT_BINARY_DIR}/cpp2py.modulefile")
    message(STATUS "*   to your environment modules                                ")
  endif()
  message(STATUS "***************************************************************")
endif()

#--------------------------------------------------------
# Packaging
#--------------------------------------------------------
option(BUILD_DEBIAN_PACKAGE "Build a deb package" OFF)
if(BUILD_DEBIAN_PACKAGE)
  if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    message(FATAL_ERROR "CMAKE_INSTALL_PREFIX must be /usr for packaging")
  endif()
  set(CPACK_PACKAGE_NAME cpp2py)
  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_VERSION ${CPP2PY_VERSION})
  set(CPACK_PACKAGE_CONTACT "https://github.com/TRIQS/cpp2py")
  execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "python, python-mako, python-numpy, python-h5py, python-scipy")
  set(CPACK_DEBIAN_PACKAGE_CONFLICTS "triqs (>= 2.1)")
  SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  SET(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
  include(CPack)
endif()
