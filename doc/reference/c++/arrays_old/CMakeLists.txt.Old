#  Copyright Olivier Parcollet 2010 
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# grab all the rst files here....
EXECUTE_PROCESS(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name "*.rst" OUTPUT_VARIABLE res ) 
string(REPLACE "\n" ";" res ${res})     # remove the newlines
SET(SOURCES ${SOURCES} ${res})          # adding to the previous sources
SET(SOURCES ${SOURCES} PARENT_SCOPE)    # put it up

# the doxygen doc 
SET(DOXYGEN_SOURCES 
 ${TRIQS_SOURCE_DIR}/triqs/arrays/h5/simple_read_write.hpp
 ${TRIQS_SOURCE_DIR}/triqs/arrays/h5/array_proxy.hpp
 ${TRIQS_SOURCE_DIR}/triqs/arrays/h5/array_stack.hpp
 )
SET(DOXYGEN_NAME arrays)
SET(DOXYGEN_HTML_OUTPUT ../html/doxy_arrays)

include(${TRIQS_SOURCE_DIR}/cmake/BuildDoxygenDocs.cmake)

