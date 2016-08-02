find_package(Test)
enable_testing()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 link_libraries( ${TRIQS_LIBRARY_PYTHON} )
endif()

option(TESTS_C_WITH_VALGRIND "Run all the tests also through valgrind" OFF)
link_libraries(gtest)

macro(all_tests)

 FILE(GLOB TestList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
 FOREACH( TestName1  ${TestList} )
  STRING(REPLACE ".cpp" "" TestName ${TestName1})
  add_executable( ${TestName}  ${CMAKE_CURRENT_SOURCE_DIR}/${TestName}.cpp )
  add_cpp_test( ${TestName}    )
  if (TESTS_C_WITH_VALGRIND)
   add_test ( ${TestName}_valgrind valgrind --error-exitcode=1 --suppressions=${CMAKE_SOURCE_DIR}/test/triqs/valgrind.supp ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
  endif()
 ENDFOREACH( TestName1  ${TestList} )

endmacro()

add_all_subdirectories_with_cmakelist()

