# runs a c++ test
# if there is a .ref file a comparison test is done
# Example: add_cpp_test(my_code)
#   where my_code is the cpp executable my_code.ref is the expected output
function(add_cpp_test testname)
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
  # Avoid mpi warning messages for ref-file based tests
  set_property(TEST ${testname_} APPEND PROPERTY ENVIRONMENT OMPI_MCA_btl_base_warn_component_unused=0)
 else()
  add_test(${testname_}${ARGN} ${testcmd})
 endif()

 if(TEST_MPI_NUMPROC)
  set_tests_properties(${testname_} PROPERTIES PROCESSORS ${TEST_MPI_NUMPROC})
 endif(TEST_MPI_NUMPROC)

endfunction(add_cpp_test)

# runs a python test
# Example: add_python_test(my_script)
#   where my_script.py is the script
function(add_python_test)
  cmake_parse_arguments(ARG "" "" "MPI_NUMPROC" ${ARGN})

  set(testfile ${ARGV0}.py)
  set(testname py_${ARGV0})
  set(testenv PYTHONPATH=${CMAKE_BINARY_DIR}/python:${CPP2PY_BINARY_DIR}:${h5_BINARY_DIR}/python:./:$ENV{PYTHONPATH})
  if(SANITIZER_RT_PRELOAD)
    list(APPEND testenv ${SANITIZER_RT_PRELOAD})
  endif()

  foreach(NP ${ARG_MPI_NUMPROC})
    add_test(${testname}_np${NP} ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${ARG_MPI_NUMPROC} ${MPIEXEC_PREFLAGS} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${testfile})
    set_property(TEST ${testname}_np${NP} PROPERTY ENVIRONMENT ${testenv})
  endforeach()

  add_test(${testname} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${testfile})
  set_property(TEST ${testname} PROPERTY ENVIRONMENT ${testenv})

endfunction(add_python_test)
