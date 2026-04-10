# install Python files
set(python_sources 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/histogram.py
)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/stat)

# install generated header
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/histograms.wrap.hxx DESTINATION include/triqs/stat)
