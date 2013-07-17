# Create a temporary copy of the python modules so that we can run before installation with the test
FILE(GLOB_RECURSE all_py_files RELATIVE ${CMAKE_SOURCE_DIR}/pytriqs *.py )
SET(py_copy_tar  ${CMAKE_BINARY_DIR}/py_copy.tar)
add_custom_command (OUTPUT ${py_copy_tar} DEPENDS ${all_py_files}
 COMMAND cd ${CMAKE_SOURCE_DIR}/pytriqs && tar cf ${py_copy_tar} ${all_py_files} && cd ${CMAKE_BINARY_DIR}/pytriqs && tar xf ${py_copy_tar} )
add_custom_target(py_copy ALL DEPENDS ${py_copy_tar})

find_package(CythonModuleMacro)

# Add C11 flag
# Put this up when boost python will be removed (or fixed for C++11).
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_BINARY_DIR}/version.py
  )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.py.in ${CMAKE_CURRENT_BINARY_DIR}/version.py)

install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST})

add_all_subdirectories_with_cmakelist()

