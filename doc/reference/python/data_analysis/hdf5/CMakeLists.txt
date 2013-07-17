#  Copyright Olivier Parcollet 2010 
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# grab all the rst files here....
EXECUTE_PROCESS(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name "*.rst" OUTPUT_VARIABLE res ) 
string(REPLACE "\n" ";" res ${res})     # remove the newlines
SET(SOURCES ${SOURCES} ${res})          # adding to the previous sources
SET(SOURCES ${SOURCES} PARENT_SCOPE)    # put it up

