#  Copyright Olivier Parcollet 2010 
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

FILE(GLOB_RECURSE SOURCES *.rst)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${CMAKE_CURRENT_BINARY_DIR}/conf.py)
EXECUTE_PROCESS(COMMAND sh -c "mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/html")
SET(DOC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
include(${TRIQS_SOURCE_DIR}/cmake/BuildSphinx.cmake)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ COMPONENT documentation DESTINATION share/doc/triqs/user_manual
 FILES_MATCHING 
 PATTERN "*.html" 
 PATTERN "*.png"
 PATTERN "*.js"
 PATTERN "_*"
 PATTERN "*.png" 
 PATTERN "*.gif" 
 PATTERN "*.xsl" 
 PATTERN "*.css"
 PATTERN "*.pdf"
 PATTERN "*.py"
 PATTERN "*.txt"
 PATTERN "*.bib"
 PATTERN ".svn" EXCLUDE 
 PATTERN "CVS" EXCLUDE
 )


if (TRIQS_BUILD_STATIC)
 add_dependencies(docs_sphinx bin/${ExecutableName} ) # need the executable to build the doc
else (TRIQS_BUILD_STATIC)
 GET_PROPERTY(PYTHON_DYNAMIC_MODULES_LIST GLOBAL  PROPERTY PYTHON_DYNAMIC_MODULES_LIST)
 foreach (mod ${PYTHON_DYNAMIC_MODULES_LIST})
  add_dependencies(docs_sphinx ${mod}) 
 endforeach (mod ${PYTHON_DYNAMIC_MODULES_LIST})

endif (TRIQS_BUILD_STATIC)

if (Build_PDF_Documentation)
 install(FILES ${sphinx_top_pdf} DESTINATION share/doc/triqs/user_manual/)
endif()

