# install Python files
set(python_sources 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/bz_patch.py
  ${CMAKE_CURRENT_SOURCE_DIR}/point.py
  ${CMAKE_CURRENT_SOURCE_DIR}/super_lattice.py
  ${CMAKE_CURRENT_SOURCE_DIR}/tight_binding.py
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.py
)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/lattice)

# install generated header
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lattice_tools.wrap.hxx DESTINATION include/triqs/lattice)
