# All targets below link to triqs and nda_py
link_libraries(triqs nda::nda_py)

SET(PYTHON_SOURCES
  __init__.py
  block2_gf.py
  block_gf.py
  map_block.py
  descriptor_base.py
  descriptors.py
  gf.py
  lazy_expressions.py
  tools.py
  dlr_crm_dyson_solver.py
)

install(FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf)

add_subdirectory(backwd_compat)
add_subdirectory(plot)
