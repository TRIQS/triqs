
SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/block_gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptor_base.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptors.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_imfreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_imtime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/_gf_legendre.py
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
# ?? was not needed ? triqs_set_rpath_for_target(my_module)

install (FILES ${CMAKE_SOURCE_DIR}/pytriqs/__init__.py.template DESTINATION "include/pytriqs/gf/local" RENAME __init__.py)

