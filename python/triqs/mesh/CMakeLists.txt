# install Python files
set(python_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/mesh_point.py
  ${CMAKE_CURRENT_SOURCE_DIR}/mesh_product.py
  ${CMAKE_CURRENT_SOURCE_DIR}/matsubara_freq.py
)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/mesh)

# install generated header
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/meshes.wrap.hxx DESTINATION include/triqs/mesh)
