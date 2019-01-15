SET(PYTHON_SOURCES
  __init__.py
  block2_gf.py
  block_gf.py
  map_block.py
  descriptor_base.py
  descriptors.py
  gf.py
  lazy_expressions.py
  mesh_product.py
  tools.py
  mesh_point.py
)

# 
add_cpp2py_module(meshes)
add_dependencies(meshes lattice_tools)

# 
add_cpp2py_module(gf_fnt)
add_dependencies(gf_fnt meshes)

# 
add_cpp2py_module(wrapped_aux)
add_dependencies(wrapped_aux meshes gf_fnt)

# 
install(FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf)
install(TARGETS meshes gf_fnt wrapped_aux DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf)

add_subdirectory(backwd_compat)
add_subdirectory(plot)

# Backward compatibility only
add_subdirectory(local)
