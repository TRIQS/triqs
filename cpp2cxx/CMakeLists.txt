set(DEST ${PYTHON_LIB_DEST_ROOT}/cpp2cxx)
install(FILES __init__.py DESTINATION ${DEST})
install(FILES cpp2cxx.py DESTINATION ${DEST})

# Link all files with relevant extensions into binary tree
# This allows us to get a working cpp2py Setup already after the build stage
execute_process(COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py ${CMAKE_CURRENT_BINARY_DIR}/__init__.py)
execute_process(COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/cpp2cxx.py ${CMAKE_CURRENT_BINARY_DIR}/cpp2cxx.py)
