# install Python files
set(python_sources 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/mpl_interface.py
  ${CMAKE_CURRENT_SOURCE_DIR}/protocol.py
)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/plot)
