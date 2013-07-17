#  Copyright Olivier Parcollet 2010 
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

FILE(GLOB_RECURSE SOURCES *.rst)

# The compiler flags for autocompile
SET( link_libs "${LAPACK_LIBS}  ${BOOST_LIBRARY} ${ALPS_EXTRA_LIBRARIES}")
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 list (REMOVE_DUPLICATES link_libs)
ENDIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
string(REPLACE "boost_for_triqs" " " TMP "${link_libs}")
string(REPLACE "debug" " " TMP "${TMP}")
string(REPLACE "optimized" " " TMP "${TMP}")

if(BOOST_SOURCE_DIR)
 SET(  link_boost_for_triqs "-lboost_for_triqs")
EXECUTE_PROCESS(COMMAND sh -c "ln -sf ${CMAKE_BINARY_DIR}/foreignlibs/boost/libboost_for_triqs.so ${CMAKE_CURRENT_BINARY_DIR}")
else(BOOST_SOURCE_DIR)
 SET(  link_boost_for_triqs " ")
endif(BOOST_SOURCE_DIR)
SET(DOC_AUTOCOMPILE_INCLUDE ${BOOST_SOURCE_DIR} ${CMAKE_SOURCE_DIR}  ${ALPS_INCLUDE_DIRS} ${ALPS_EXTRA_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/foreignlibs  )
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
SET(DOC_AUTOCOMPILE_LINK "-L ${CMAKE_BINARY_DIR}/foreignlibs/boost ${link_boost_for_triqs} -L ${CMAKE_BINARY_DIR}/triqs -ltriqs -L ${CMAKE_BINARY_DIR}/triqs -ltriqs_utility ${TMP} ")
SET(DOC_AUTOCOMPILE_DEFINITIONS -std=c++0x -DBOOST_PP_VARIADICS=1 -stdlib=libc++ -I/System/Library/Frameworks/vecLib.framework/Versions/A/Headers)
else()
SET(DOC_AUTOCOMPILE_LINK "-L ${CMAKE_BINARY_DIR}/foreignlibs/boost ${link_boost_for_triqs} -L ${CMAKE_BINARY_DIR}/triqs -ltriqs -L ${CMAKE_BINARY_DIR}/triqs -ltriqs_utility ${TMP} -Wl,-rpath=${CMAKE_CURRENT_BINARY_DIR} ")
SET(DOC_AUTOCOMPILE_DEFINITIONS -std=c++0x -DBOOST_PP_VARIADICS=1 )
endif()
SET(DOC_AUTOCOMPILE_LD_LIBRARY_PATH " ${CMAKE_BINARY_DIR}/foreignlibs/boost/:${CMAKE_BINARY_DIR}/triqs ")

EXECUTE_PROCESS(COMMAND sh -c "ln -sf ${CMAKE_BINARY_DIR}/triqs/libtriqs.so ${CMAKE_CURRENT_BINARY_DIR}")
EXECUTE_PROCESS(COMMAND sh -c "ln -sf ${CMAKE_BINARY_DIR}/triqs/libtriqs_utility.so ${CMAKE_CURRENT_BINARY_DIR}")

# generating the conf.py
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${CMAKE_CURRENT_BINARY_DIR}/conf.py)
EXECUTE_PROCESS(COMMAND sh -c "mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/html")
SET(DOC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
SET(DOC_EXT "_cpp")
include(${TRIQS_SOURCE_DIR}/cmake/BuildSphinx.cmake)

add_all_subdirectories_with_cmakelist()

# Build the doxygen 
SET(DOXYGEN_HTML_OUTPUT ./html/doxy_triqs)
find_package(Doxygen REQUIRED)

get_property(DOXYGEN_SOURCES GLOBAL PROPERTY DOXYGEN_SOURCES)
string(REPLACE ";" "  " DOXYGEN_SOURCES_LIST "${DOXYGEN_SOURCES}")
message(STATUS "Doxygen sources are ${DOXYGEN_SOURCES}") 

# Prepare the Doxyfile
configure_file(${TRIQS_SOURCE_DIR}/cmake/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

SET(doxy_top ${CMAKE_CURRENT_BINARY_DIR}/doxy.log)
add_custom_command (OUTPUT ${doxy_top} DEPENDS ${DOXYGEN_SOURCES} COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile > ${doxy_top} )
add_custom_target(docs_doxy ALL DEPENDS ${doxy_top})
add_dependencies(docs_sphinx_cpp docs_doxy) # we must first have doxygen process the sources to link to them
add_dependencies(docs_sphinx_cpp triqs boost_for_triqs ) # add also mpi ?

if (Build_PDF_Documentation)
 # not needed, no link to doxygen in the pdf ...
 #add_dependencies(docs_sphinx_pdf_cpp docs_doxy) # we must first have doxygen process the sources to link to them
 add_dependencies(docs_sphinx_pdf_cpp triqs boost_for_triqs ) # add also mpi ?
 install(FILES ${sphinx_top_pdf} DESTINATION share/doc/triqs/developer_manual/)
endif()

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ COMPONENT documentation DESTINATION share/doc/triqs/developer_manual
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

