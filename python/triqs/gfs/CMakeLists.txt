# install Python files
set(python_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/block2_gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/block_gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/map_block.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptor_base.py
  ${CMAKE_CURRENT_SOURCE_DIR}/descriptors.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf.py
  ${CMAKE_CURRENT_SOURCE_DIR}/lazy_expressions.py
  ${CMAKE_CURRENT_SOURCE_DIR}/tools.py
  ${CMAKE_CURRENT_SOURCE_DIR}/dlr_crm_dyson_solver.py
)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gfs)

# add other submodules
add_subdirectory(backwd_compat)
add_subdirectory(gf_fnt)
add_subdirectory(gf_factories)
add_subdirectory(plot)
add_subdirectory(wrapped_aux)
