#  Copyright I. Krivenko, O. Parcollet 2015.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for Libclang and deduce the options
# 

# First find libclang if it is not provided
SET(TRIAL_LIBRARY_PATHS
 /usr/lib 
 /usr/lib/x86_64-linux-gnu
 /usr/lib/llvm-3.6/lib
 /usr/lib/llvm-3.5/lib
 /usr/lib/llvm-3.4/lib
)
if (NOT TRIQS_LIBCLANG_LOCATION)
FIND_LIBRARY(TRIQS_LIBCLANG_LOCATION NAMES libclang.so PATHS ${TRIAL_LIBRARY_PATHS} DOC "Location of the libclang library")
endif()

if (NOT TRIQS_LIBCLANG_LOCATION)
 MESSAGE(STATUS "Can not find libclang")
endif()

# Now find the clang compiler ....
if (NOT CLANG_COMPILER) 
if(${CMAKE_COMPILER_IS_CLANG})
 set(CLANG_COMPILER ${CMAKE_CXX_COMPILER})
else()
 SET(TRIAL_CLANG_PATHS
 /usr/bin 
 )
FIND_PROGRAM(CLANG_COMPILER clang++ ${TRIAL_CLANG_PATHS} DOC "Clang compiler (for libclang option)")
endif(${CMAKE_COMPILER_IS_CLANG})
endif()

if (NOT CLANG_COMPILER)
 MESSAGE(STATUS "Can not find the Clang compiler, hence can not find the option for libclang")
else()
SET(LIBCLANG_FLAGS_DETECTION_COMMAND "${CLANG_COMPILER}" -E -x c++ -v -)
EXECUTE_PROCESS(COMMAND ${LIBCLANG_FLAGS_DETECTION_COMMAND}
                INPUT_FILE /dev/null
                ERROR_VARIABLE _compiler_output OUTPUT_QUIET)
#MESSAGE(${_compiler_output})
STRING(REGEX MATCH "#include <...> search starts here:\n(.*)End of search list." _matches "${_compiler_output}")
STRING(REPLACE "\n" ";" CMAKE_MATCH_1 ${CMAKE_MATCH_1})
STRING(REPLACE "(framework directory)" "" CMAKE_MATCH_1 ${CMAKE_MATCH_1})
separate_arguments(CMAKE_MATCH_1)
FOREACH(include_path IN ITEMS ${CMAKE_MATCH_1})
 STRING(STRIP ${include_path} include_path)
 SET(TRIQS_LIBCLANG_CXX_ADDITIONAL_FLAGS "${TRIQS_LIBCLANG_CXX_ADDITIONAL_FLAGS} -I${include_path}")
ENDFOREACH(include_path IN ITEMS ${CMAKE_MATCH_1})
endif()

#FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibClang DEFAULT_MSG TRIQS_LIBCLANG_LOCATION TRIQS_LIBCLANG_CXX_ADDITIONAL_FLAGS)

