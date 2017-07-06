SET(PYTHON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/block2_gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/block_gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptor_base.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptors.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/lazy_expressions.py
  ${CMAKE_CURRENT_SOURCE_DIR}/mesh_product.py
  ${CMAKE_CURRENT_SOURCE_DIR}/tools.py
)

# 
add_cpp2py_module(meshes)
add_dependencies(meshes lattice_tools)

# 
add_cpp2py_module(singularities)
add_dependencies(singularities lattice_tools)

# 
add_cpp2py_module(gf_fnt)
add_dependencies(gf_fnt meshes )
add_dependencies(gf_fnt singularities)

# 
add_cpp2py_module(wrapped_aux)
add_dependencies(wrapped_aux meshes )
add_dependencies(wrapped_aux singularities)

# 
install(FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf)
install(TARGETS meshes singularities gf_fnt wrapped_aux DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf)

add_subdirectory(backwd_compat)
add_subdirectory(plot)
# Backward compatibility only
add_subdirectory(local)

