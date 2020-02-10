#  Copyright Olivier Parcollet 2010.
#  Copyright Simons Foundation 2019
#    Author: Nils Wentzell

#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for gsl.
# It sets up : GSL_INCLUDE_DIR, GSL_LIBRARIES
# Use GSL_ROOT to specify a particular location
#

find_path(GSL_INCLUDE_DIR
  NAMES gsl_math.h
  PATHS
    ${GSL_ROOT}/include
    $ENV{GSL_ROOT}/include
    $ENV{GSL_BASE}/include
    ENV CPATH
    ENV C_INCLUDE_PATH
    ENV CPLUS_INCLUDE_PATH
    ENV OBJC_INCLUDE_PATH
    ENV OBJCPLUS_INCLUDE_PATH
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
  DOC "Include Directory for GSL"
)

find_library(GSL_LIBRARIES
  NAMES gsl libgsl
  PATHS
    ${GSL_INCLUDE_DIR}/../lib
    ${GSL_ROOT}/lib
    $ENV{GSL_ROOT}/lib
    $ENV{GSL_BASE}/lib
    ENV LIBRARY_PATH
    ENV LD_LIBRARY_PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
  DOC "GSL library"
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GSL DEFAULT_MSG GSL_LIBRARIES GSL_INCLUDE_DIR)

mark_as_advanced(GSL_INCLUDE_DIR GSL_LIBRARIES)

# Interface target
# We refrain from creating an imported target since those cannot be exported
add_library(gsl INTERFACE)
target_link_libraries(gsl INTERFACE ${GSL_LIBRARIES})
target_include_directories(gsl SYSTEM INTERFACE ${GSL_INCLUDE_DIR})
