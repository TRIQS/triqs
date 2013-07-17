#  Copyright Olivier Parcollet 2010.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# This module looks for sphinx documentation tool 
# and define a function that prepares the Makefile for sphinx-build

FIND_PROGRAM(SPHINXBUILD_EXECUTABLE
 NAMES sphinx-build sphinx-build-2.6
 PATHS $ENV{HOME}/bin /usr/bin /opt/local/bin 
 PATH_SUFFIXES  bin
 )

if (NOT SPHINXBUILD_EXECUTABLE)
 MESSAGE(FATAL_ERROR "I can not find sphinx to build the triqs documentation")
else (NOT SPHINXBUILD_EXECUTABLE)
 MESSAGE(STATUS "sphinx-build program found at ${SPHINXBUILD_EXECUTABLE} ")
endif (NOT SPHINXBUILD_EXECUTABLE)

# a little hack to make the doc being compiled by pytriqs itself !
# so that autodoc loading works...
if (TRIQS_BUILD_STATIC)
  SET(SPHINXBUILD_EXECUTABLE PYTHONPATH=${CMAKE_BINARY_DIR}  ${CMAKE_BINARY_DIR}/bin/pytriqs ${SPHINXBUILD_EXECUTABLE})
else (TRIQS_BUILD_STATIC)
  SET(SPHINXBUILD_EXECUTABLE ${CMAKE_BINARY_DIR}/run_pytriqs_for_test.sh ${SPHINXBUILD_EXECUTABLE})
endif (TRIQS_BUILD_STATIC)

# handle the QUIETLY and REQUIRED arguments and set SPHINXBUILD_FOUND to TRUE if 
# all listed variables are TRUE

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SPHINX DEFAULT_MSG SPHINXBUILD_EXECUTABLE)

MARK_AS_ADVANCED( SPHINXBUILD_EXECUTABLE )

SET(sphinx_top ${CMAKE_CURRENT_BINARY_DIR}/html/contents.html)
add_custom_command(OUTPUT ${sphinx_top} DEPENDS ${SOURCES} COMMAND ${SPHINXBUILD_EXECUTABLE} -c . -b html ${DOC_SOURCE} html)
add_custom_target(docs_sphinx${DOC_EXT} ALL DEPENDS ${sphinx_top} ${DOC_SOURCE})

if (Build_PDF_Documentation)
 SET(sphinx_top_pdf ${CMAKE_CURRENT_BINARY_DIR}/latex/TRIQS.pdf)
 add_custom_command(OUTPUT ${sphinx_top_pdf} DEPENDS ${SOURCES} COMMAND ${SPHINXBUILD_EXECUTABLE} -c . -b latex ${DOC_SOURCE} latex && cd latex
  && pdflatex -interaction=nonstopmode TRIQS.tex && pdflatex -interaction=nonstopmode TRIQS.tex)
 add_custom_target(docs_sphinx_pdf${DOC_EXT} ALL DEPENDS ${sphinx_top_pdf} ${DOC_SOURCE})
endif()

