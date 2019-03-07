SET(PythonBuildExecutable ${PYTHON_INTERPRETER})

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
  set(testcmd ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${TEST_MPI_NUMPROC} ${MPIEXEC_PREFLAGS} ${testcmd} ${MPIEXEC_POSTFLAGS})
 else(TEST_MPI_NUMPROC)
  set(testname_ ${testname})
 endif(TEST_MPI_NUMPROC)

 if(EXISTS ${testref})
  file(COPY ${testref} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  add_test(${testname_}
   ${CMAKE_COMMAND}
   -Dname=${testname_}${ARGN}
   -Dcmd=${testcmd}
   -Dreference=${testref}
   -P ${CMAKE_BINARY_DIR}/Config/run_test.cmake
  )
 else()
  add_test(${testname_}${ARGN} ${testcmd})
 endif()

 if(TEST_MPI_NUMPROC)
  set_tests_properties(${testname_} PROPERTIES PROCESSORS ${TEST_MPI_NUMPROC})
 endif(TEST_MPI_NUMPROC)

endmacro(add_cpp_test)

# runs a python test
# Example: add_python_test(my_script)
#   where my_script.py is the script
macro(add_python_test testname)

 add_test(${testname} ${PythonBuildExecutable} ${CMAKE_CURRENT_SOURCE_DIR}/${testname}.py)
 set_property(TEST ${testname} PROPERTY ENVIRONMENT PYTHONPATH=${CMAKE_BINARY_DIR}:${CPP2PY_BINARY_DIR}:./:$ENV{PYTHONPATH})

endmacro(add_python_test)
