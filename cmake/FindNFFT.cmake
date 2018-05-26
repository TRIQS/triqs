#  Copyright Olivier Parcollet 2014.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for nfft.
# It sets up : NFFT_INCLUDE_DIR, NFFT_LIBRARY, NFFT_OLD_API
# 

INCLUDE( FindPackageHandleStandardArgs ) 

SET(TRIAL_PATHS
 $ENV{NFFT_ROOT}/include
 ${NFFT_ROOT}/include
 ENV CPATH
 ENV C_INCLUDE_PATH
 ENV CPLUS_INCLUDE_PATH
 ENV OBJC_INCLUDE_PATH
 ENV OBJCPLUS_INCLUDE_PATH
 /usr/include
 /usr/local/include
 /opt/local/include
 /sw/include
 )
FIND_PATH(NFFT_INCLUDE_DIR nfft3.h ${TRIAL_PATHS} DOC "Include for NFFT")

SET(TRIAL_LIBRARY_PATHS
 $ENV{NFFT_ROOT}/lib
 ${NFFT_ROOT}/lib
 ${NFFT_INCLUDE_DIR}/../lib
 ENV LIBRARY_PATH
 ENV LD_INCLUDE_PATH
 /usr/lib 
 /usr/lib/x86_64-linux-gnu
 /usr/local/lib
 /opt/local/lib
 /sw/lib
 )

SET(NFFT_LIBRARY "NFFT_LIBRARY-NOTFOUND" CACHE STRING "NFFT library")
# Try to detect the lib
FIND_LIBRARY(NFFT_LIBRARY nfft3 ${TRIAL_LIBRARY_PATHS} DOC "NFFT library")

mark_as_advanced(NFFT_INCLUDE_DIR)
mark_as_advanced(NFFT_LIBRARY)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp "#include \"nfft3.h\"\nint main(){ nfft_plan p; return p.nfft_flags; }")
try_compile(NFFT_OLD_API ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp COMPILE_DEFINITIONS -I${NFFT_INCLUDE_DIR} )
file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/a.out ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp)

# Imported target

add_library(nfft UNKNOWN IMPORTED)
set_property(TARGET nfft PROPERTY IMPORTED_LOCATION ${NFFT_LIBRARY})
set_property(TARGET nfft PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${NFFT_INCLUDE_DIR})
set_property(TARGET nfft PROPERTY PROPERTY INTERFACE_COMPILE_DEFINITIONS $<$<BOOL:${NFFT_OLD_API}>:NFFT_OLD_API>)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NFFT DEFAULT_MSG NFFT_LIBRARY NFFT_INCLUDE_DIR)


