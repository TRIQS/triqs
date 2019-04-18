#  Copyright I. Krivenko, O. Parcollet 2015.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for Libclang and deduce the options
# 

include(FindPackageHandleStandardArgs)
# First find libclang if it is not provided

if( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

  SET(TRIAL_LIBRARY_PATHS
   ENV LIBRARY_PATH
   /usr/local/opt/llvm/lib/
   /usr/local/Cellar/llvm/8.0.0/lib/
   /usr/local/Cellar/llvm/7.0.0/lib/
   /usr/local/Cellar/llvm/6.0.0/lib/
   /usr/local/Cellar/llvm/5.0.1/lib/
   /usr/local/lib/
   /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/
  )
 #if (NOT LIBCLANG_LOCATION)
   FIND_LIBRARY(LIBCLANG_LOCATION NAMES libclang.dylib PATHS ${TRIAL_LIBRARY_PATHS} DOC "Location of the libclang library")
   #endif()
 
  set(LIBCLANG_CXX_FLAGS "" CACHE STRING "Additional flags to be passed to libclang when parsing with clang")

  set(CLANG_COMPILER "clang++")
  set(CLANG_OPT -stdlib=libc++ )

else()

SET(TRIAL_LIBRARY_PATHS
 ENV LIBRARY_PATH
 ENV LD_LIBRARY_PATH
 /usr/lib 
 /usr/lib/x86_64-linux-gnu
 /usr/lib/llvm-6.0/lib
 /usr/lib/llvm-5.0/lib
 /usr/lib/llvm-4.0/lib
 /usr/lib64/llvm
)
if (NOT LIBCLANG_LOCATION)
FIND_LIBRARY(LIBCLANG_LOCATION NAMES libclang.so PATHS ${TRIAL_LIBRARY_PATHS} DOC "Location of the libclang library")
endif()

SET(LIBCLANG_CXX_FLAGS "${LIBCLANG_CXX_FLAGS}")
#SET(LIBCLANG_CXX_FLAGS "-DADD_MAX_ALIGN_T_WORKAROUND ${LIBCLANG_CXX_FLAGS}")

# Now find the clang compiler ....
if (NOT CLANG_COMPILER) 
 SET(TRIAL_CLANG_PATHS
  ENV PATH
  /usr/bin 
 )
FIND_PROGRAM(CLANG_COMPILER names clang++ clang++-6.0 clang++-5.0 clang++-4.0 PATHS ${TRIAL_CLANG_PATHS} DOC "Clang compiler (for libclang option)")
endif()

if (NOT CLANG_COMPILER)
 MESSAGE(STATUS "Can not find the Clang compiler, hence can not find the option for libclang")
endif()

endif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if (NOT LIBCLANG_CXX_FLAGS)
if (CLANG_COMPILER)
SET(LIBCLANG_FLAGS_DETECTION_COMMAND "${CLANG_COMPILER} $ENV{CXXFLAGS} -E -x c++ ${CLANG_OPT} -v -")
SEPARATE_ARGUMENTS(LIBCLANG_FLAGS_DETECTION_COMMAND)
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
 SET(LIBCLANG_CXX_FLAGS "${LIBCLANG_CXX_FLAGS} -I${include_path}")
ENDFOREACH(include_path IN ITEMS ${CMAKE_MATCH_1})
endif (CLANG_COMPILER)
endif()
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibClang DEFAULT_MSG LIBCLANG_LOCATION LIBCLANG_CXX_FLAGS)

