
SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/aux.py
)

# Install python sources
install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/wrap_test)

# Build C extension module 
triqs_python_extension(my_module wrap_test)
# ??triqs_set_rpath_for_target(my_module)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

install (FILES ${CMAKE_SOURCE_DIR}/pytriqs/__init__.py.template DESTINATION "include/pytriqs/gf/local" RENAME __init__.py)

