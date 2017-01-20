#  Copyright Olivier Parcollet 2014.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for nfft.
# It sets up : NFFT_INCLUDE_DIR, NFFT_LIBRARIES, NFFT_OLD_API
# 

INCLUDE( FindPackageHandleStandardArgs ) 

SET(TRIAL_PATHS
 $ENV{NFFT_ROOT}/include
 ${NFFT_ROOT}/include
 /usr/include
 /usr/local/include
 /opt/local/include
 /sw/include
 )
FIND_PATH(NFFT_INCLUDE_DIR nfft3.h ${TRIAL_PATHS} DOC "Include for NFFT")

SET(TRIAL_LIBRARY_PATHS
 /usr/lib 
 /usr/local/lib
 /opt/local/lib
 /sw/lib
 $ENV{NFFT_ROOT}/lib
 ${NFFT_ROOT}/lib
 )

SET(NFFT_LIBRARIES "NFFT_LIBRARIES-NOTFOUND" CACHE STRING "NFFT library")
# Try to detect the lib
FIND_LIBRARY(NFFT_LIBRARIES nfft3 ${TRIAL_LIBRARY_PATHS} DOC "NFFT library")

mark_as_advanced(NFFT_INCLUDE_DIR)
mark_as_advanced(NFFT_LIBRARIES)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp "#include \"nfft3.h\"\nint main(){ nfft_plan p; return p.nfft_flags; }")
try_compile(NFFT_OLD_API ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp COMPILE_DEFINITIONS -I${NFFT_INCLUDE_DIR} )
file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/a.out ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NFFT DEFAULT_MSG NFFT_LIBRARIES NFFT_INCLUDE_DIR)


