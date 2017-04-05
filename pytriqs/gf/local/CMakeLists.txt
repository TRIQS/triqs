
SET(PYTHON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/tool_gfloc.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_imfreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_imtime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_refreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_retime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_legendre.py
)

# Install python sources
install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf/local)

# Build C extension module
#triqs_python_extension(gf gf/local)
#add_dependencies(python_wrap_gf python_wrap_lattice_tools)

#install (FILES ${CMAKE_SOURCE_DIR}/pytriqs/__init__.py.template DESTINATION "include/pytriqs/gf/local" RENAME __init__.py)

#
#triqs_python_extension(multivar gf/local)
#add_dependencies(python_wrap_multivar python_wrap_gf)
