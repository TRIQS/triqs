
SET(RUN_TEST ${CMAKE_BINARY_DIR}/build_pytriqs ) 
#if (TRIQS_BUILD_STATIC)
# FILE(WRITE ${RUN_TEST} "PYTHONPATH=${CMAKE_BINARY_DIR}:$PYTHONPATH  ${PYTHON_INTERPRETER} $1")
# #FILE(WRITE ${RUN_TEST} "PYTHONPATH=${CMAKE_BINARY_DIR}:$PYTHONPATH  ${CMAKE_BINARY_DIR}/bin/pytriqs $1")
# execute_process( COMMAND chmod u+x ${RUN_TEST} )
#endif (TRIQS_BUILD_STATIC)

SET(ExecutableNameForTest ${RUN_TEST})
 
# runs pytriqs script 
# Example: 
#   add_triqs_test_script(ExampleTest)
#
macro(add_triqs_test testname ) 
 enable_testing()
 add_test(${testname}
  ${CMAKE_COMMAND}
  -Dname=${testname}
  -Dcmd=${ExecutableNameForTest}
  -Dinput=${CMAKE_CURRENT_SOURCE_DIR}/${testname}.py
  -P ${TRIQS_SOURCE_DIR}/cmake/run_test.cmake
  )
endmacro(add_triqs_test)

# runs pytriqs script > output 
# and compares output with script.output
# Example: 
#   add_triqs_test_script(ExampleTest)
#   where ExampleTest.py is the script and ExampleTest.output is the expected output
#
macro(add_triqs_test_txt testname ) 
 enable_testing()
 add_test(${testname}
  ${CMAKE_COMMAND}
  -Dname=${testname}
  -Dcmd=${ExecutableNameForTest}
  -Dinput=${CMAKE_CURRENT_SOURCE_DIR}/${testname}.py
  -Dreference=${CMAKE_CURRENT_SOURCE_DIR}/${testname}.output
  -P ${TRIQS_SOURCE_DIR}/cmake/run_test.cmake
  )
endmacro(add_triqs_test_txt)

# - runs python_with_DMFT script 
# - compares the file named testname_output.h5 
#   with the file testname.output.h5 of the source directory
# The comparison with made with h5diff_options 
# all remainings arguments are the objects to which to restrict the comparison (default = no restriction)
# Example : 
#   add_triqs_test_hdf(ExampleTestH5 "-r" )
#   where ExampleTestH5.py produces to be compared with ExampleTestH5_output.h5 with option "-r" and for all objects
#
macro(add_triqs_test_hdf testname h5diff_options)
 set(h5diff_objects ${ARGN}) # grab additionnal arguments !
 enable_testing()
 add_test(${testname}
  ${CMAKE_COMMAND}
  -Dname=${testname}
  -Dcmd=${ExecutableNameForTest}
  -Dinput=${CMAKE_CURRENT_SOURCE_DIR}/${testname}.py
  -DoutputName=${testname}.output.h5
  -Dreference=${CMAKE_CURRENT_SOURCE_DIR}/${testname}.output.h5
  -DH5_DIFF_EXECUTABLE=${HDF5_DIFF_EXECUTABLE} 
  -DH5_DIFF_OPTIONS=${h5diff_options} 
  -DH5_DIFF_OBJECTS=${h5diff_objects} 
  -P ${TRIQS_SOURCE_DIR}/cmake/run_test.cmake
  )
endmacro(add_triqs_test_hdf)


