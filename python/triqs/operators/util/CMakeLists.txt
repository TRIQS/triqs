# All targets below link to triqs and nda_py
link_libraries(triqs nda::nda_py)

SET(PYTHON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/hamiltonians.py
  ${CMAKE_CURRENT_SOURCE_DIR}/observables.py
  ${CMAKE_CURRENT_SOURCE_DIR}/op_struct.py
  ${CMAKE_CURRENT_SOURCE_DIR}/U_matrix.py
)

add_cpp2py_module(extractors)

install(FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/operators/util)
install(TARGETS extractors DESTINATION ${TRIQS_PYTHON_LIB_DEST}/operators/util)


