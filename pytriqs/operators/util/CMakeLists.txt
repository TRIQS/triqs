
SET(PYTHON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/hamiltonians.py
  ${CMAKE_CURRENT_SOURCE_DIR}/observables.py
  ${CMAKE_CURRENT_SOURCE_DIR}/op_struct.py
  ${CMAKE_CURRENT_SOURCE_DIR}/U_matrix.py
  ${CMAKE_CURRENT_BINARY_DIR}/extractors.so
)

triqs_python_extension(extractors extractors)
add_dependencies(python_wrap_extractors python_wrap_operators)

install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/operators/util)
