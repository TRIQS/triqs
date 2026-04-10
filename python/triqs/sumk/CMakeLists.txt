# install Python files
set(python_sources 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/sumk_discrete_from_lattice.py
  ${CMAKE_CURRENT_SOURCE_DIR}/sumk_discrete.py
)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/sumk)
