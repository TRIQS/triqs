
 FILE(GLOB TestList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
 FOREACH( TestName1  ${TestList} )
  STRING(REPLACE ".cpp" "" TestName ${TestName1})
  add_executable( ${TestName}  ${CMAKE_CURRENT_SOURCE_DIR}/${TestName}.cpp )
  add_test ( ${TestName} ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
  if (TESTS_C_WITH_VALGRIND)
   add_test ( ${TestName}_valgrind valgrind --error-exitcode=1 ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
  endif()
  add_test( ${TestName}_np2  ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2 ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
  add_test( ${TestName}_np3  ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 3 ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
  add_test( ${TestName}_np4  ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4 ${CMAKE_CURRENT_BINARY_DIR}/${TestName})

 ENDFOREACH( TestName1  ${TestList} )


