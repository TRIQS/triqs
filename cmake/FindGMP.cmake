#  Copyright Olivier Parcollet 2010.
#  Copyright Simons Foundation 2019
#    Author: Nils Wentzell

#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# This module looks for gmp.
# It sets up : GMP_INCLUDE_DIR, GMP_LIBRARIES GMPXX_LIBRARIES
# Use GMP_ROOT to specify a particular location
#

if(GMP_INCLUDE_DIR AND GMP_LIBRARIES)
 set(GMP_FIND_QUIETLY TRUE)
endif()

find_path(GMP_INCLUDE_DIR
  NAMES gmp.h
  HINTS
    ${GMP_ROOT}/include
    $ENV{GMP_ROOT}/include
    $ENV{GMP_BASE}/include
    ENV CPATH
    ENV C_INCLUDE_PATH
    ENV CPLUS_INCLUDE_PATH
    ENV OBJC_INCLUDE_PATH
    ENV OBJCPLUS_INCLUDE_PATH
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
  DOC "Include Directory for GMP"
)

find_library(GMP_LIBRARIES
  NAMES gmp libgmp
  HINTS
    ${GMP_INCLUDE_DIR}/../lib
    ${GMP_ROOT}/lib
    $ENV{GMP_ROOT}/lib
    $ENV{GMP_BASE}/lib
    ENV LIBRARY_PATH
    ENV LD_LIBRARY_PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
  DOC "GMP library"
)

find_library(GMPXX_LIBRARIES
  NAMES gmpxx libgmpxx
  HINTS
    ${GMP_INCLUDE_DIR}/../lib
    ${GMP_ROOT}/lib
    $ENV{GMP_ROOT}/lib
    $ENV{GMP_BASE}/lib
    ENV LIBRARY_PATH
    ENV LD_LIBRARY_PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
  DOC "GMPXX library"
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GMP DEFAULT_MSG GMP_LIBRARIES GMPXX_LIBRARIES GMP_INCLUDE_DIR)

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARIES GMPXX_LIBRARIES)

# Add Interface targets for gmp and gmpxx
# We refrain from creating an imported target since those cannot be exported

add_library(gmp INTERFACE)
target_link_libraries(gmp INTERFACE ${GMP_LIBRARIES})
target_include_directories(gmp SYSTEM INTERFACE ${GMP_INCLUDE_DIR})

add_library(gmpxx INTERFACE)
target_link_libraries(gmpxx INTERFACE ${GMPXX_LIBRARIES})
target_include_directories(gmpxx SYSTEM INTERFACE ${GMP_INCLUDE_DIR})
