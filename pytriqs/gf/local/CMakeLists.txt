
SET(PYTHON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/block_gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/block2_gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptor_base.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptors.py
  ${CMAKE_CURRENT_SOURCE_DIR}/select_indices.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_imfreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_imtime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_legendre.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_space.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_space_x_X.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_X_x_X.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_refreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_retime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_plot.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_common.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_imp_tools.py
  ${CMAKE_CURRENT_SOURCE_DIR}/lazy_expressions.py
  ${CMAKE_CURRENT_SOURCE_DIR}/tools.py
)

# Install python sources
install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf/local)

# Build C extension module
triqs_python_extension(gf gf/local)
add_dependencies(python_wrap_gf python_wrap_lattice_tools)

install (FILES ${CMAKE_SOURCE_DIR}/pytriqs/__init__.py.template DESTINATION "include/pytriqs/gf/local" RENAME __init__.py)

#
triqs_python_extension(multivar gf/local)
add_dependencies(python_wrap_multivar python_wrap_gf)
