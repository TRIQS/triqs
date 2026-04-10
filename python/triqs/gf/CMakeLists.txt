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

add_cpp2py_module(gf_fnt)
add_cpp2py_module(gf_factories)
add_cpp2py_module(wrapped_aux)

install(FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf)
install(TARGETS gf_fnt gf_factories wrapped_aux DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf)

add_subdirectory(backwd_compat)
add_subdirectory(plot)
