enable_testing()
find_package(Test)

link_libraries(triqs)

add_python_test(operators_py)

add_all_subdirectories_with_cmakelist()
