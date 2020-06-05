
SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
)


add_cpp2py_module(operators)

install(FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/operators)
install(TARGETS operators DESTINATION ${TRIQS_PYTHON_LIB_DEST}/operators)

add_subdirectory(util)
