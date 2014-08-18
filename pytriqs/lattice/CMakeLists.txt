SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/bz_patch.py
  ${CMAKE_CURRENT_SOURCE_DIR}/super_lattice.py
  ${CMAKE_CURRENT_SOURCE_DIR}/tight_binding.py
)

install (FILES ${CMAKE_SOURCE_DIR}/pytriqs/__init__.py.template DESTINATION "include/pytriqs/gf/local" RENAME __init__.py)

install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/lattice)

# Build C extension module 
triqs_python_extension(lattice_tools lattice)

