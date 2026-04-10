# install Python files
set(python_sources 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/dos.py
  ${CMAKE_CURRENT_SOURCE_DIR}/hilbert_transform.py
)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/dos)
