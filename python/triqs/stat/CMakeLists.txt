# All targets below link to triqs and nda_py
link_libraries(triqs nda::nda_py)

SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/histogram.py
)

#
add_cpp2py_module(histograms)

install(FILES ${PYTHON_SOURCES}  DESTINATION ${TRIQS_PYTHON_LIB_DEST}/stat)
install(TARGETS histograms       DESTINATION ${TRIQS_PYTHON_LIB_DEST}/stat)

