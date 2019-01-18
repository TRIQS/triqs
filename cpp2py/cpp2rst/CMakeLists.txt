set(DEST ${PYTHON_LIB_DEST_ROOT})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${DEST} FILES_MATCHING PATTERN "*.py")

# Link all files with relevant extensions into binary tree
# This allows us to get a working cpp2py Setup already after the build stage
execute_process(COMMAND ${PROJECT_BINARY_DIR}/bin/cp_rs ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} "py")
