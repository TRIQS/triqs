enable_testing()
find_package(Test)

link_libraries(triqs)

# copy wannier90 test files for parsing
set(w90_files
    wannier_TB_test_hr.dat
    wannier_TB_test.wout 
    )
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${w90_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 

add_python_test(operators)
add_python_test(lattice_utils)
add_python_test(lattice_tight_binding)

add_all_subdirectories_with_cmakelist()
