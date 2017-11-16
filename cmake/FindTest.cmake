SET(PythonBuildExecutable python)

# runs a c++ test
# if there is a .ref file a comparison test is done
# Example: add_cpp_test(my_code)
#   where my_code is the cpp executable my_code.ref is the expected output
macro(add_cpp_test testname)
 enable_testing()

 set(testcmd ${CMAKE_CURRENT_BINARY_DIR}/${testname}${ARGN})
 set(testref ${CMAKE_CURRENT_SOURCE_DIR}/${testname}.ref)

 # run this test via mpirun if TEST_MPI_NUMPROC is set
 if(TEST_MPI_NUMPROC)
  set(testname_ ${testname}_np${TEST_MPI_NUMPROC})
  set(testcmd ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${TEST_MPI_NUMPROC} ${testcmd})
 else(TEST_MPI_NUMPROC)
  set(testname_ ${testname})
 endif(TEST_MPI_NUMPROC)

 if (EXISTS ${testref})

  file(COPY ${testref} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

  add_test(${testname_}
   ${CMAKE_COMMAND}
   -Dname=${testname_}${ARGN}
   -Dcmd=${testcmd}
   -Dreference=${testref}
   -P ${CMAKE_BINARY_DIR}/Config/run_test.cmake
  )

 else (EXISTS ${testref})

  add_test(${testname_}${ARGN} ${testcmd})

 endif (EXISTS ${testref})

 if(TEST_MPI_NUMPROC)
  set_tests_properties(${testname_} PROPERTIES PROCESSORS ${TEST_MPI_NUMPROC})
 endif(TEST_MPI_NUMPROC)

endmacro(add_cpp_test)

# runs a python test
# if there is a .ref file a comparison test is done
# Example: add_python_test(my_script)
#   where my_script.py is the script and my_script.ref is the expected output
macro(add_python_test testname)
 enable_testing()

 set(testcmd ${PythonBuildExecutable})
 set(testref ${CMAKE_CURRENT_SOURCE_DIR}/${testname}.ref)

 # run this test via mpirun if TEST_MPI_NUMPROC is set
 if(TEST_MPI_NUMPROC)
  set(testname_ ${testname}_np${TEST_MPI_NUMPROC})
  set(testcmd ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${TEST_MPI_NUMPROC} ${testcmd})
 else(TEST_MPI_NUMPROC)
  set(testname_ ${testname})
 endif(TEST_MPI_NUMPROC)

 if (EXISTS ${testref})

  file(COPY ${testref} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

  add_test(${testname_}
   ${CMAKE_COMMAND}
   -Dname=${testname_}
   -Dcmd=${testcmd}
   -Dinput=${CMAKE_CURRENT_SOURCE_DIR}/${testname}.py
   -Dreference=${testref}
   -P ${CMAKE_BINARY_DIR}/Config/run_test.cmake
  )

 else (EXISTS ${testref})

  add_test(${testname_}
   ${CMAKE_COMMAND}
   -Dname=${testname_}
   -Dcmd=${testcmd}
   -Dinput=${CMAKE_CURRENT_SOURCE_DIR}/${testname}.py
   -P ${CMAKE_BINARY_DIR}/Config/run_test.cmake
  )

 endif (EXISTS ${testref})

 set_property(TEST ${testname_} PROPERTY ENVIRONMENT PYTHONPATH=${CMAKE_BINARY_DIR}:./:$ENV{PYTHONPATH} )

 if(TEST_MPI_NUMPROC)
  set_tests_properties(${testname_} PROPERTIES PROCESSORS ${TEST_MPI_NUMPROC})
 endif(TEST_MPI_NUMPROC)

endmacro(add_python_test)
