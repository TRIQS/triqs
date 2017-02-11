#  Copyright Olivier Parcollet 2010.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for fftw.
# It sets up : FFTW_INCLUDE_DIR, FFTW_LIBRARIES
# 

SET(TRIAL_PATHS
 $ENV{FFTW_ROOT}/include
 ${FFTW_ROOT}/include
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
FIND_PATH(FFTW_INCLUDE_DIR fftw3.h ${TRIAL_PATHS} DOC "Include for FFTW")

SET(TRIAL_LIBRARY_PATHS
 $ENV{FFTW_ROOT}/lib
 ${FFTW_ROOT}/lib
 ${FFTW_INCLUDE_DIR}/../lib
 ENV LIBRARY_PATH
 ENV LD_LIBRARY_PATH
 /usr/lib 
 /usr/local/lib
 /opt/local/lib
 /sw/lib
 )

SET(FFTW_LIBRARIES "FFTW_LIBRARIES-NOTFOUND" CACHE STRING "FFTW library")
# Try to detect the lib
FIND_LIBRARY(FFTW_LIBRARIES fftw3 ${TRIAL_LIBRARY_PATHS} DOC "FFTW library")

mark_as_advanced(FFTW_INCLUDE_DIR)
mark_as_advanced(FFTW_LIBRARIES)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW DEFAULT_MSG FFTW_LIBRARIES FFTW_INCLUDE_DIR)

