# install Python files
set(python_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/one_var.py
  ${CMAKE_CURRENT_SOURCE_DIR}/plot_base.py
  ${CMAKE_CURRENT_SOURCE_DIR}/select_indices.py
)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gfs/plot)
