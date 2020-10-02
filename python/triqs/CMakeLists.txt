# 
set(TRIQS_PYTHON_LIB_DEST ${CPP2PY_PYTHON_LIB_DEST_ROOT}/triqs)

# Create a temporary copy of the python modules so that we can run before installation with the test
FILE(GLOB_RECURSE all_py_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py )
foreach(f ${all_py_files})
 configure_file(${f} ${f} COPYONLY)
endforeach()

SET(PYTHON_SOURCES
 ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
 ${CMAKE_CURRENT_BINARY_DIR}/version.py
 ${CMAKE_CURRENT_BINARY_DIR}/cmake_info.py
 ${CMAKE_CURRENT_BINARY_DIR}/magic.py
 )

configure_file(cmake_info.py.in cmake_info.py)
configure_file(version.py.in version.py)
configure_file(utility/mpi.py.in utility/mpi.py)
configure_file(magic.py.in magic.py @ONLY)

install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST})

# All targets below link to triqs and nda_py
link_libraries(triqs nda::nda_py)

# Remove some warnings on clang and gcc for Python ?
If("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
 add_compile_options(-Wno-deprecated-writable-strings)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
 add_compile_options(-Wno-literal-suffix) 
endif()

# ---------------------------------------------

add_subdirectory(archive)
#add_subdirectory(arrays)
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

add_subdirectory(atom_diag)
