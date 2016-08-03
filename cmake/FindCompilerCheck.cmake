#  Copyright Olivier Parcollet 2012
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)


if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
 set(gnu_compiler_version_min "5.1.0")
 if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS gnu_compiler_version_min)
   set(line_of_star "\n************************** FATAL ERROR ************************************\n")
   MESSAGE( FATAL_ERROR "${line_of_star}You are using the ${CMAKE_CXX_COMPILER_ID} compiler but your compiler is too old :\n TRIQS requires version >= ${gnu_compiler_version_min} while you have ${CMAKE_CXX_COMPILER_VERSION}\n  ${line_of_star}")
 endif()
 set(CMAKE_COMPILER_IS_GCC TRUE )
 set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS}  " -Wno-literal-suffix ")

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")

 try_run(run_result_var compile_result_var ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/test_clang.cpp
         COMPILE_DEFINITIONS -std=c++14 RUN_OUTPUT_VARIABLE compiler_version)
 if (NOT compile_result_var)
  MESSAGE (FATAL_ERROR "Clang compiler does not compile simple tests. Error was ${COMPILE_OUTPUT_VARIABLE}")
 endif()

 MESSAGE(STATUS "Using clang version ${compiler_version}")

 set(CMAKE_COMPILER_IS_CLANG TRUE )
 set(compiler_name "clang")

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")

 EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
  OUTPUT_VARIABLE compiler_version RESULT_VARIABLE returncode OUTPUT_STRIP_TRAILING_WHITESPACE)
 set(CMAKE_COMPILER_IS_ICC TRUE )
 set(compiler_version_min "14.0.0")
 set(compiler_name "Intel icc")
 #string(REGEX REPLACE "[^0-9]*([0-9]+\\.[0-9]\\.[0-9]).*" "\\1" compiler_version ${_compiler_output})

else ()
 set(compiler_version_min "0.0")
 set(compiler_group 0)
 set(line_of_star "\n************************** WARNING  ************************************\n")
 MESSAGE( WARNING "${line_of_star}  Compiler not recognized by TRIQS : TRIQS may compile .. or not ${line_of_star}")
 #message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()

MESSAGE( STATUS "Compiler is ${compiler_name} with version ${compiler_version}")

# on OS X : for clang, add the infamous -stdlib=libc++
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 if (CMAKE_COMPILER_IS_CLANG)
  #set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ ")
  set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS}  " -stdlib=libc++  -Wno-deprecated-writable-strings ")
  MESSAGE(STATUS "Adding compilation flags -stdlib=libc++ ")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ ")
 else (CMAKE_COMPILER_IS_CLANG)
  MESSAGE( WARNING "${line_of_star}You are on Os X but your are not using clang. This is NOT recommended...${line_of_star}")
 endif (CMAKE_COMPILER_IS_CLANG)
ENDIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# for icc, add the very infamous flags or calculation of boost::math bessel function are wrong !!
# tested on boost 1.53
IF(CMAKE_COMPILER_IS_ICC)
 set(TRIQS_WORKAROUND_INTEL_COMPILER_14_BUGS 1)
 set(BOOST_MATH_DISABLE_STD_FPCLASSIFY 1)
 #set(TRIQS_CXX_DEFINITIONS ${TRIQS_CXX_DEFINITIONS} -DTRIQS_WORKAROUND_INTEL_COMPILER_BUGS -DBOOST_MATH_DISABLE_STD_FPCLASSIFY)
 #add_definitions( -DTRIQS_WORKAROUND_INTEL_COMPILER_BUGS -DBOOST_MATH_DISABLE_STD_FPCLASSIFY)
 MESSAGE(STATUS " Adding compilation flags -DTRIQS_WORKAROUND_INTEL_COMPILER_BUGS -DBOOST_MATH_DISABLE_STD_FPCLASSIFY")
ENDIF(CMAKE_COMPILER_IS_ICC)
