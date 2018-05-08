#  Copyright Olivier Parcollet 2017.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# This module looks for sphinx documentation tool 
# and define a function that prepares the Makefile for sphinx-build

find_program(SPHINXBUILD_EXECUTABLE
 NAMES sphinx-build 
 PATHS /usr/bin /opt/local/bin /usr/local/bin #opt/sphinx-doc/bin
 PATH_SUFFIXES  bin
 )

if (NOT SPHINXBUILD_EXECUTABLE)
 message(FATAL_ERROR "I cannot find sphinx to build the triqs documentation")
endif()

execute_process(
      COMMAND "${SPHINXBUILD_EXECUTABLE}" --version
      OUTPUT_VARIABLE SPHINXBUILD_VERSION
      ERROR_VARIABLE  SPHINXBUILD_VERSION
    )
if (SPHINXBUILD_VERSION MATCHES "[Ss]phinx.* ([0-9]+\\.[0-9]+(\\.|b)[0-9]+)")
  set (SPHINXBUILD_VERSION "${CMAKE_MATCH_1}")
endif()

if (SPHINXBUILD_VERSION VERSION_EQUAL 1.6.3)
 message(FATAL_ERROR "sphinx-build found at ${SPHINXBUILD_EXECUTABLE} but version 1.6.3 has a bug. Upgrade sphinx.")
else()
 message(STATUS "sphinx-build program found at ${SPHINXBUILD_EXECUTABLE} with version ${SPHINXBUILD_VERSION}")
endif ()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SPHINX DEFAULT_MSG SPHINXBUILD_EXECUTABLE)

mark_as_advanced( SPHINXBUILD_EXECUTABLE )
