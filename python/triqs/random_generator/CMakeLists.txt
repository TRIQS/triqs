# All targets below link to triqs and nda_py
link_libraries(triqs nda::nda_py)

SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
)

# 
add_cpp2py_module(random_generator)

install(FILES ${PYTHON_SOURCES}  DESTINATION ${TRIQS_PYTHON_LIB_DEST}/random_generator)
install(TARGETS random_generator DESTINATION ${TRIQS_PYTHON_LIB_DEST}/random_generator)

