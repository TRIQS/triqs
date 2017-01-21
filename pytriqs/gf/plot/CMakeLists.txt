
SET(PYTHON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/dispatch.py
  ${CMAKE_CURRENT_SOURCE_DIR}/one_var.py
  ${CMAKE_CURRENT_SOURCE_DIR}/plot_base.py
  ${CMAKE_CURRENT_SOURCE_DIR}/select_indices.py
)

# Install python sources
install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf/plot)

