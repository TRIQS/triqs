find_package(Test)
enable_testing()

# Copy h5 files to binary dir
FILE(GLOB all_h5_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h5)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${all_h5_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

option(TESTS_C_WITH_VALGRIND "Run all the tests also through valgrind" OFF)

macro(all_tests)

 file(GLOB TestList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
 foreach(TestName1 ${TestList})
  string(REPLACE ".cpp" "" TestName ${TestName1})
  add_executable(${TestName} ${CMAKE_CURRENT_SOURCE_DIR}/${TestName}.cpp)
  target_link_libraries(${TestName} triqs gtest triqs_warnings)
  add_cpp_test(${TestName})
  if (TESTS_C_WITH_VALGRIND)
   add_test (${TestName}_valgrind valgrind --error-exitcode=1 --suppressions=${PROJECT_SOURCE_DIR}/test/triqs/valgrind.supp ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
  endif()
 endforeach(TestName1  ${TestList})

endmacro()

add_all_subdirectories_with_cmakelist()
