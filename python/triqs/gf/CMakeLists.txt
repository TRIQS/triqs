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

add_cpp2py_module(meshes)
add_cpp2py_module(gf_fnt)
add_cpp2py_module(gf_factories)
add_cpp2py_module(wrapped_aux)

# One dep is sufficient to trigger the build of the _h5py.so
# dep only at runtime
add_dependencies(meshes _h5py)


install(FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf)
install(TARGETS meshes gf_fnt gf_factories wrapped_aux DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf)

add_subdirectory(backwd_compat)
add_subdirectory(plot)

# Backward compatibility only
add_subdirectory(local)
