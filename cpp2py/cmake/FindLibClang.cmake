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

  set(TRIAL_LIBRARY_PATHS
    ENV LIBRARY_PATH
    /usr/local/opt/llvm/lib/
    /usr/local/Cellar/llvm/9.0.0/lib/
    /usr/local/Cellar/llvm/8.0.1/lib/
    /usr/local/Cellar/llvm/8.0.0/lib/
    /usr/local/Cellar/llvm/7.0.0/lib/
    /usr/local/Cellar/llvm/6.0.0/lib/
    /usr/local/Cellar/llvm/5.0.1/lib/
    /usr/local/lib/
    /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/
  )
  find_library(LIBCLANG_LOCATION NAMES libclang.dylib PATHS ${TRIAL_LIBRARY_PATHS} DOC "Location of the libclang library")

  set(LIBCLANG_CXX_FLAGS "" CACHE STRING "Additional flags to be passed to libclang when parsing with clang")

  set(CLANG_COMPILER "clang++")
  set(CLANG_OPT -stdlib=libc++)

else()

  SET(TRIAL_LIBRARY_PATHS
    ENV LIBRARY_PATH
    ENV LD_LIBRARY_PATH
    /usr/lib
    /usr/lib/x86_64-linux-gnu
    /usr/lib/llvm-9/lib
    /usr/lib/llvm-8/lib
    /usr/lib/llvm-7/lib
    /usr/lib/llvm-6.0/lib
    /usr/lib/llvm-5.0/lib
    /usr/lib/llvm-4.0/lib
    /usr/lib64/llvm
  )
  if(NOT LIBCLANG_LOCATION)
    find_library(LIBCLANG_LOCATION NAMES libclang.so PATHS ${TRIAL_LIBRARY_PATHS} DOC "Location of the libclang library")
  endif()

  set(LIBCLANG_CXX_FLAGS "${LIBCLANG_CXX_FLAGS}")
  #SET(LIBCLANG_CXX_FLAGS "-DADD_MAX_ALIGN_T_WORKAROUND ${LIBCLANG_CXX_FLAGS}")

  # Now find the clang compiler ....
  if(NOT CLANG_COMPILER)
    set(TRIAL_CLANG_PATHS
      ENV PATH
      /usr/bin
    )
    find_program(CLANG_COMPILER names clang++ clang++-9 clang++-8 clang++-7 clang++-6.0 clang++-5.0 clang++-4.0 PATHS ${TRIAL_CLANG_PATHS} DOC "Clang compiler (for libclang option)")
  endif()

  if(NOT CLANG_COMPILER)
    message(STATUS "Can not find the Clang compiler, hence can not find the option for libclang")
  endif()

endif()

if(CLANG_COMPILER AND NOT LIBCLANG_CXX_FLAGS)
  set(LIBCLANG_FLAGS_DETECTION_COMMAND "${CLANG_COMPILER} $ENV{CXXFLAGS} -E -x c++ ${CLANG_OPT} -v -")
  separate_arguments(LIBCLANG_FLAGS_DETECTION_COMMAND)
  execute_process(COMMAND ${LIBCLANG_FLAGS_DETECTION_COMMAND}
                  INPUT_FILE /dev/null
                  ERROR_VARIABLE _compiler_output OUTPUT_QUIET)
  #MESSAGE(${_compiler_output})
  string(REGEX MATCH "#include <...> search starts here:\n(.*)End of search list." _matches "${_compiler_output}")
  string(REPLACE "\n" ";" CMAKE_MATCH_1 ${CMAKE_MATCH_1})
  string(REPLACE "(framework directory)" "" CMAKE_MATCH_1 ${CMAKE_MATCH_1})
  separate_arguments(CMAKE_MATCH_1)
  foreach(include_path IN ITEMS ${CMAKE_MATCH_1})
    string(STRIP ${include_path} include_path)
    set(LIBCLANG_CXX_FLAGS "${LIBCLANG_CXX_FLAGS} -isystem${include_path}")
  endforeach()
endif()

find_package_handle_standard_args(LibClang DEFAULT_MSG LIBCLANG_LOCATION LIBCLANG_CXX_FLAGS)
