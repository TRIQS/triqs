# install Python files
set(python_sources ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/atom_diag)

# install generated header
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/atom_diag.wrap.hxx DESTINATION include/triqs/atom_diag)
