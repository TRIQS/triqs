#  Copyright Olivier Parcollet 2010 
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Sphinx sources
#set_property(GLOBAL APPEND PROPERTY SPHINX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/det_manip.rst) 

# Doxygen sources
set_property(GLOBAL APPEND PROPERTY DOXYGEN_SOURCES ${TRIQS_SOURCE_DIR}/triqs/gfs/gf.hpp)

