# install Python files
set(python_sources ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/operators)

# install generated header
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/operators.wrap.hxx DESTINATION include/triqs/operators)

# add submodule util
add_subdirectory(util)
