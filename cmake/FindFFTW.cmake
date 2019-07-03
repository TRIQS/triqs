#  Copyright Olivier Parcollet 2010.
#  Copyright Simons Foundation 2019
#    Author: Nils Wentzell

#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for fftw.
# It sets up : FFTW_INCLUDE_DIR, FFTW_LIBRARIES
# Use FFTW3_ROOT to specify a particular location
#

if(FFTW_INCLUDE_DIR AND FFTW_LIBRARIES)
  set(FFTW_FIND_QUIETLY TRUE)
endif()

find_path(FFTW_INCLUDE_DIR
  NAMES fftw3.h
  PATHS
    ${FFTW3_ROOT}/include
    ${FFTW_ROOT}/include
    $ENV{FFTW3_ROOT}/include
    $ENV{FFTW_ROOT}/include
    ENV CPATH
    ENV C_INCLUDE_PATH
    ENV CPLUS_INCLUDE_PATH
    ENV OBJC_INCLUDE_PATH
    ENV OBJCPLUS_INCLUDE_PATH
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
  DOC "Include Directory for FFTW"
)

find_library(FFTW_LIBRARIES
  NAMES fftw3
  PATHS
    ${FFTW_INCLUDE_DIR}/../lib
    ${FFTW3_ROOT}/lib
    ${FFTW_ROOT}/lib
    $ENV{FFTW3_ROOT}/lib
    $ENV{FFTW_ROOT}/lib
    ENV LIBRARY_PATH
    ENV LD_LIBRARY_PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
  DOC "FFTW library"
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW DEFAULT_MSG FFTW_LIBRARIES FFTW_INCLUDE_DIR)

mark_as_advanced(FFTW_INCLUDE_DIR FFTW_LIBRARIES)

# Interface target
# We refrain from creating an imported target since those cannot be exported
add_library(fftw INTERFACE)
target_link_libraries(fftw INTERFACE ${FFTW_LIBRARIES})
target_include_directories(fftw SYSTEM INTERFACE ${FFTW_INCLUDE_DIR})
