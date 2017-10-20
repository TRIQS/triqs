SET(PYTHON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/block2_gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/block_gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptor_base.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptors.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf.py
  #${CMAKE_CURRENT_SOURCE_DIR}/_imp_tools.py
  ${CMAKE_CURRENT_SOURCE_DIR}/lazy_expressions.py
  ${CMAKE_CURRENT_SOURCE_DIR}/mesh_product.py
  ${CMAKE_CURRENT_SOURCE_DIR}/tools.py
  ${CMAKE_CURRENT_SOURCE_DIR}/mesh_point.py
)

install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf)
install (FILES ${CMAKE_SOURCE_DIR}/pytriqs/__init__.py.template DESTINATION "include/pytriqs/gf" RENAME __init__.py)

# Build C extension module
triqs_python_extension(meshes gf)
add_dependencies(python_wrap_meshes python_wrap_lattice_tools)

# Build C extension module
triqs_python_extension(singularities gf)
add_dependencies(python_wrap_singularities python_wrap_lattice_tools)

# Build C extension module
triqs_python_extension(gf_fnt gf)
add_dependencies(python_wrap_gf_fnt python_wrap_meshes )
add_dependencies(python_wrap_gf_fnt python_wrap_singularities)

# Build C extension module
triqs_python_extension(wrapped_aux gf)

add_dependencies(python_wrap_wrapped_aux python_wrap_meshes )
add_dependencies(python_wrap_wrapped_aux python_wrap_singularities)

add_subdirectory(backwd_compat)
add_subdirectory(plot)

# Backward compatibility only
add_subdirectory(local)

