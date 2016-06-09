
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
add_dependencies(python_wrap_gf python_wrap_lattice_tools)

install (FILES ${CMAKE_SOURCE_DIR}/pytriqs/__init__.py.template DESTINATION "include/pytriqs/gf/local" RENAME __init__.py)

# 
triqs_python_extension(multivar gf/local)
add_dependencies(python_wrap_multivar python_wrap_gf)
#triqs_python_extension_and_install(multivar)
#target_link_libraries(multivar ${TRIQS_LIBRARY_ALL})
#include_directories(${TRIQS_INCLUDE_ALL} ${CMAKE_CURRENT_SOURCE_DIR})
#triqs_set_rpath_for_target(multivar)

