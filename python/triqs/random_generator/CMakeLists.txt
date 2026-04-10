# backward compatibility shim for triqs.random_generator -> triqs.mc_tools
# install Python files
set(python_sources ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/random_generator)
