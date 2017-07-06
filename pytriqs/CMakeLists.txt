# 
set(TRIQS_PYTHON_LIB_DEST ${CPP2PY_PYTHON_LIB_DEST_ROOT}/pytriqs)

# Create a temporary copy of the python modules so that we can run before installation with the test
FILE(GLOB_RECURSE all_py_files RELATIVE ${CMAKE_SOURCE_DIR}/pytriqs *.py )
SET(py_copy_tar  ${CMAKE_BINARY_DIR}/py_copy.tar)
add_custom_command (OUTPUT ${py_copy_tar} DEPENDS ${all_py_files}
 COMMAND cd ${CMAKE_SOURCE_DIR}/pytriqs && tar cf ${py_copy_tar} ${all_py_files} && cd ${CMAKE_BINARY_DIR}/pytriqs && tar xf ${py_copy_tar} )
add_custom_target(py_copy ALL DEPENDS ${py_copy_tar})

SET(PYTHON_SOURCES
 ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
 ${CMAKE_CURRENT_BINARY_DIR}/version.py
 ${CMAKE_CURRENT_BINARY_DIR}/cmake_info.py
 ${CMAKE_CURRENT_BINARY_DIR}/magic.py
 )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake_info.py.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_info.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.py.in ${CMAKE_CURRENT_BINARY_DIR}/version.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/utility/mpi.py.in ${CMAKE_CURRENT_BINARY_DIR}/utility/mpi.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/magic.py.in ${CMAKE_CURRENT_BINARY_DIR}/magic.py @ONLY)

install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST})
install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST})

# all targets below link to triqs
link_libraries(triqs)

# Remove some warnings on clang and gcc for Python ?
If("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
 add_compile_options(-Wno-deprecated-writable-strings)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
 add_compile_options(-Wno-literal-suffix) 
endif()

# ---------------------------------------------

add_subdirectory(archive)
add_subdirectory(arrays)
add_subdirectory(dos)
add_subdirectory(fit)
add_subdirectory(gf)
add_subdirectory(lattice)
add_subdirectory(operators)
add_subdirectory(plot)
add_subdirectory(random_generator)
add_subdirectory(statistics)
add_subdirectory(sumk)
add_subdirectory(utility)

