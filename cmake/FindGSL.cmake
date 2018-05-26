#  Copyright Olivier Parcollet 2010.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for fftw.
# It sets up : GSL_INCLUDE_DIR, GSL_LIBRARIES
# 

SET(TRIAL_PATHS
 $ENV{GSL_ROOT}/include
 ${GSL_ROOT}/include
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
FIND_PATH(GSL_INCLUDE_DIR gsl/gsl_math.h ${TRIAL_PATHS} DOC "Include for GSL")

SET(TRIAL_LIBRARY_PATHS
 $ENV{GSL_ROOT}/lib
 ${GSL_ROOT}/lib
 ${GSL_INCLUDE_DIR}/../lib
 ENV LIBRARY_PATH
 ENV LD_LIBRARY_PATH
 /usr/lib 
 /usr/local/lib
 /opt/local/lib
 /sw/lib
 )

SET(GSL_LIBRARIES "GSL_LIBRARIES-NOTFOUND" CACHE STRING "GSL library")
# Try to detect the lib
FIND_LIBRARY(GSL_LIBRARIES gsl ${TRIAL_LIBRARY_PATHS} DOC "GSL library")

mark_as_advanced(GSL_INCLUDE_DIR)
mark_as_advanced(GSL_LIBRARIES)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(GSL DEFAULT_MSG GSL_LIBRARIES GSL_INCLUDE_DIR)

