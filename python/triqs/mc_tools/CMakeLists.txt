# install Python files
set(python_sources ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/mc_tools)

# install generated header
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/random_generator.wrap.hxx DESTINATION include/triqs/mc_tools)
