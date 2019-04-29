find_package(Test)
enable_testing()

option(TESTS_C_WITH_VALGRIND "Run all the tests also through valgrind" OFF)
include_directories(${CMAKE_SOURCE_DIR}/google_test)

macro(all_tests)

 file(GLOB TestList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
 foreach(TestName1 ${TestList})
  string(REPLACE ".cpp" "" TestName ${TestName1})
  add_executable(${TestName} ${CMAKE_CURRENT_SOURCE_DIR}/${TestName}.cpp)
  target_link_libraries(${TestName} triqs gtest)
  add_cpp_test(${TestName})
  if (TESTS_C_WITH_VALGRIND)
   add_test (${TestName}_valgrind valgrind --error-exitcode=1 --suppressions=${CMAKE_SOURCE_DIR}/test/triqs/valgrind.supp ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
  endif()
 endforeach(TestName1  ${TestList})

endmacro()

add_all_subdirectories_with_cmakelist()


# ---------------------------------
# Build C++ examples of the doc
# ---------------------------------

FILE(GLOB_RECURSE ExampleList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/doc/*.cpp)

# build all the examples in their respective directory
 foreach(example ${ExampleList})
  get_filename_component (f ${example} NAME_WE)
  get_filename_component (d ${example} DIRECTORY)
  add_executable(doc_${f} ${CMAKE_CURRENT_SOURCE_DIR}/${example})
  target_link_libraries(doc_${f} triqs )
  # set_target_properties(doc_${f} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${d})
 endforeach()


