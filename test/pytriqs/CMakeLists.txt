enable_testing()
find_package(Test)

# Create a temporary copy of the python modules so that we can run before installation with the test
FILE(GLOB_RECURSE all_py_files RELATIVE ${CMAKE_SOURCE_DIR}/test/pytriqs *.py )
SET(py_copy_tar  ${CMAKE_BINARY_DIR}/test/py_copy_test.tar)
add_custom_command (OUTPUT ${py_copy_tar} DEPENDS ${all_py_files}
 COMMAND cd ${CMAKE_SOURCE_DIR}/test/pytriqs && tar cf ${py_copy_tar} ${all_py_files} && cd ${CMAKE_BINARY_DIR}/test/pytriqs && tar xf ${py_copy_tar} )
add_custom_target(py_copy_test ALL DEPENDS ${py_copy_tar})

add_custom_target(wrap_clean_test COMMAND find ${CMAKE_BINARY_DIR} -name \"*.cpp\" |xargs rm)

add_all_subdirectories_with_cmakelist()

