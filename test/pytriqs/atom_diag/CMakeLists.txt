# Copy h5 files to binary dir
file(GLOB all_h5_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h5)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${all_h5_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_python_test(atom_diag)
