SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
)

add_cpp2py_module(block_matrix)

# 
install(FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/arrays)
install(TARGETS block_matrix DESTINATION ${TRIQS_PYTHON_LIB_DEST}/arrays)


