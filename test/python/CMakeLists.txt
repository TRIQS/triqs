enable_testing()
find_package(Test)

link_libraries(triqs)

add_python_test(operators)
add_python_test(lattice_utils)

add_all_subdirectories_with_cmakelist()
