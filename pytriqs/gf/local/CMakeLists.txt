
SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/block_gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptors.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_generic.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_imfreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_imtime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_legendre.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_refreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_retime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_two_real_times.py
  ${CMAKE_CURRENT_SOURCE_DIR}/impl_plot.py
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/inverse.py
  ${CMAKE_CURRENT_SOURCE_DIR}/lazy_expressions.py
  ${CMAKE_CURRENT_SOURCE_DIR}/nothing.py
  ${CMAKE_CURRENT_SOURCE_DIR}/tools.py
)

# Install python sources
install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf/local)

# Build cython module 
cython_module( GF3 gf gf/local )
cython_module( MS matrix_stack gf/local )

FILE(GLOB all_pxd_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.pxd )
install (FILES ${all_pxd_files} DESTINATION "include/pytriqs/gf/local")
install (FILES ${CMAKE_SOURCE_DIR}/pytriqs/__init__.py.template DESTINATION "include/pytriqs/gf/local" RENAME __init__.py)

