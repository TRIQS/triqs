
# Build C extension module
triqs_python_extension(my_module)
triqs_python_extension(my_moduleB)
triqs_python_extension(test_g)
triqs_python_extension(test_bl)
triqs_python_extension(test_multivar)

# the module B must be generated AFTER my_module since it uses some wrapped types...
add_dependencies(python_wrap_my_moduleB python_wrap_my_module)
add_dependencies(python_wrap_test_multivar python_wrap_multivar)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

# A sample test with text output
add_python_test(wrap_a)
add_python_test(wrap_B)
add_python_test(wrap_g)
add_python_test(wrap_multivar)
add_python_test(blockgf)

