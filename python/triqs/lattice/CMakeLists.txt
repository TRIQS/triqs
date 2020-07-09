SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/bz_patch.py
  ${CMAKE_CURRENT_SOURCE_DIR}/super_lattice.py
  ${CMAKE_CURRENT_SOURCE_DIR}/tight_binding.py
)

# 
add_cpp2py_module(lattice_tools)

install(FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/lattice)
install(TARGETS lattice_tools DESTINATION ${TRIQS_PYTHON_LIB_DEST}/lattice)


