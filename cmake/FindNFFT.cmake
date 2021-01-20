#  Copyright Olivier Parcollet 2014
#  Copyright Simons Foundation 2019
#    Author: Nils Wentzell

#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for nfft.
# It sets up : NFFT_INCLUDE_DIR, NFFT_LIBRARIES, NFFT_OLD_API
# Use NFFT_ROOT to specify a particular location
#

if(NFFT_INCLUDE_DIR AND NFFT_LIBRARIES)
  set(NFFT_FIND_QUIETLY TRUE)
endif()

find_path(NFFT_INCLUDE_DIR
  NAMES nfft3.h
  HINTS
    ${NFFT3_ROOT}/include
    ${NFFT_ROOT}/include
    $ENV{NFFT3_ROOT}/include
    $ENV{NFFT_ROOT}/include
    $ENV{NFFT3_BASE}/include
    $ENV{NFFT_BASE}/include
    ENV CPATH
    ENV C_INCLUDE_PATH
    ENV CPLUS_INCLUDE_PATH
    ENV OBJC_INCLUDE_PATH
    ENV OBJCPLUS_INCLUDE_PATH
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
  DOC "Include Directory for NFFT"
)

find_library(NFFT_LIBRARIES
  NAMES nfft3
  HINTS
    ${NFFT_INCLUDE_DIR}/../lib
    ${NFFT3_ROOT}/lib
    ${NFFT_ROOT}/lib
    $ENV{NFFT3_ROOT}/lib
    $ENV{NFFT_ROOT}/lib
    $ENV{NFFT3_BASE}/lib
    $ENV{NFFT_BASE}/lib
    ENV LIBRARY_PATH
    ENV LD_LIBRARY_PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
  DOC "NFFT library"
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NFFT DEFAULT_MSG NFFT_LIBRARIES NFFT_INCLUDE_DIR)

mark_as_advanced(NFFT_INCLUDE_DIR NFFT_LIBRARIES)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp "#include \"nfft3.h\"\nint main(){ nfft_plan p; return p.nfft_flags; }")
try_compile(NFFT_OLD_API ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp COMPILE_DEFINITIONS -I${NFFT_INCLUDE_DIR} )
file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/a.out ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp)

# Interface target
# We refrain from creating an imported target since those cannot be exported
add_library(nfft INTERFACE)
target_link_libraries(nfft INTERFACE ${NFFT_LIBRARIES})
target_include_directories(nfft SYSTEM INTERFACE ${NFFT_INCLUDE_DIR})
target_compile_definitions(nfft INTERFACE $<$<BOOL:${NFFT_OLD_API}>:NFFT_OLD_API>)
