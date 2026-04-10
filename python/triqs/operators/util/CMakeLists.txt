# install Python files
set(python_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/hamiltonians.py
  ${CMAKE_CURRENT_SOURCE_DIR}/observables.py
  ${CMAKE_CURRENT_SOURCE_DIR}/op_struct.py
  ${CMAKE_CURRENT_SOURCE_DIR}/U_matrix.py
)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/operators/util)
