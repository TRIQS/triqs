
# Build C extension module 
triqs_python_extension(my_module)
triqs_python_extension(my_moduleB)
triqs_python_extension(test_g)

# the module B must be generated AFTER my_module since it uses some wrapped types...
add_dependencies(python_wrap_my_moduleB python_wrap_my_module)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

# A sample test with text output
add_triqs_test_txt(wrap_a)

