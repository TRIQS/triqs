
 FILE(GLOB TestList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
 FOREACH( TestName1  ${TestList} )
  STRING(REPLACE ".cpp" "" TestName ${TestName1})
  add_executable( ${TestName}  ${CMAKE_CURRENT_SOURCE_DIR}/${TestName}.cpp )
  add_test ( ${TestName} ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
  if (TESTS_C_WITH_VALGRIND)
   add_test ( ${TestName}_valgrind valgrind --error-exitcode=1 ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
  endif()

  set(TEST_MPI_NUMPROC 2)
  add_cpp_test(${TestName})
  set(TEST_MPI_NUMPROC 3)
  add_cpp_test(${TestName})
  set(TEST_MPI_NUMPROC 4)
  add_cpp_test(${TestName})

 ENDFOREACH( TestName1  ${TestList} )


