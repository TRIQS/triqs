#  Copyright Olivier Parcollet 2014.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for nfft.
# It sets up : NFFT_INCLUDE_DIR, NFFT_LIBRARY, NFFT_OLD_API
# 

include(FindPackageHandleStandardArgs)

set(TRIAL_PATHS
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
find_path(NFFT_INCLUDE_DIR nfft3.h ${TRIAL_PATHS} DOC "Include for NFFT")

set(TRIAL_LIBRARY_PATHS
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

set(NFFT_LIBRARY "NFFT_LIBRARY-NOTFOUND" CACHE STRING "NFFT library")
# Try to detect the lib
find_library(NFFT_LIBRARY nfft3 ${TRIAL_LIBRARY_PATHS} DOC "NFFT library")

mark_as_advanced(NFFT_INCLUDE_DIR)
mark_as_advanced(NFFT_LIBRARY)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp "#include \"nfft3.h\"\nint main(){ nfft_plan p; return p.nfft_flags; }")
try_compile(NFFT_OLD_API ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp COMPILE_DEFINITIONS -I${NFFT_INCLUDE_DIR} )
file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/a.out ${CMAKE_CURRENT_BINARY_DIR}/nfft_api_test.cpp)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NFFT DEFAULT_MSG NFFT_LIBRARY NFFT_INCLUDE_DIR)

# Interface target
# We refrain from creating an imported target since those cannot be exported
add_library(nfft INTERFACE)
target_link_libraries(nfft INTERFACE ${NFFT_LIBRARY})
target_include_directories(nfft INTERFACE ${NFFT_INCLUDE_DIR})
target_compile_definitions(nfft INTERFACE $<$<BOOL:${NFFT_OLD_API}>:NFFT_OLD_API>)
