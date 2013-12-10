#  Copyright Olivier Parcollet 2012 
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
#
# Compilers are sorted by group of interoperability
# group 1 : fully compliant C++11 compiler
#           gcc 4.8.1 and higher, clang >= 3.3
# group 1 : older gcc, ok with workaround
# group 2 : Intel 14.0 : almost C++11, but with Intel workaround
# group 3 : Intel 13.0 : messy compiler...
# group 0 : other

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
 
 EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} --version  
  OUTPUT_VARIABLE _compiler_output RESULT_VARIABLE returncode OUTPUT_STRIP_TRAILING_WHITESPACE)
 set(compiler_version_min "4.8.1")
 set(compiler_name "gcc")
 string(REGEX REPLACE ".*([2-5]\\.[0-9]\\.[0-9]).*" "\\1" compiler_version ${_compiler_output})

 if(NOT compiler_version VERSION_LESS "4.9.0")
  set (compiler_is_c14 ON)
 endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")

 EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} --version
  OUTPUT_VARIABLE _compiler_output RESULT_VARIABLE returncode OUTPUT_STRIP_TRAILING_WHITESPACE)
 set(CMAKE_COMPILER_IS_CLANG TRUE )
 set(compiler_name "clang")
 IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Apple which does not has the official clang version number ... 
  string(REGEX REPLACE ".*LLVM ([2-5]\\.[0-9]).*" "\\1" compiler_version ${_compiler_output})
 else()
  string(REGEX REPLACE ".*([2-5]\\.[0-9]).*" "\\1" compiler_version ${_compiler_output})
 endif()
 #message(STATUS "Compiler --version output = ${_compiler_output}")
 message(STATUS "Compiler version = ${compiler_version}")
 set( compiler_version_min "3.2")

 # does not always work ... To be fixed when clang 3.4 officially released and on OS X.
 if(NOT compiler_version VERSION_LESS "3.4")
  set (compiler_is_c14 ON)
 endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
 
 EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion  
  OUTPUT_VARIABLE compiler_version RESULT_VARIABLE returncode OUTPUT_STRIP_TRAILING_WHITESPACE)
 set(CMAKE_COMPILER_IS_ICC TRUE )
 set(compiler_version_min "14.0.0")
 set(compiler_name "Intel icc")
 #string(REGEX REPLACE "[^0-9]*([0-9]+\\.[0-9]\\.[0-9]).*" "\\1" compiler_version ${_compiler_output})

 # for intel 14.0 /test 
 link_libraries( -lomp_db )

else ()
 set(compiler_version_min "0.0")
 set(compiler_group 0)
 set(line_of_star "\n************************** WARNING  ************************************\n")
 MESSAGE( WARNING "${line_of_star}  Compiler not recognized by TRIQS : TRIQS may compile .. or not ${line_of_star}") 
 #message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()


MESSAGE( STATUS "Compiler is ${compiler_name} with version ${compiler_version}")

# Check version 
if(compiler_version VERSION_LESS ${compiler_version_min} )
 set(line_of_star "\n************************** FATAL ERROR ************************************\n")
 # MESSAGE( FATAL_ERROR "${line_of_star}You are using the ${compiler_name} compiler but your compiler is too old :\n TRIQS requires version >= ${compiler_version_min} while you have ${compiler_version}\n  ${line_of_star}")
endif(compiler_version VERSION_LESS ${compiler_version_min} )

# Now add some definitions ...

# on OS X : for clang, add the infamous -stdlib=libc++
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 if (CMAKE_COMPILER_IS_CLANG) 
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ ")
  MESSAGE(STATUS "Adding compilation flags -stdlib=libc++ ")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-writable-strings ")
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


