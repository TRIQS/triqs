find_package(Test)

link_libraries(triqs triqs_py)

# Build C extension module
add_cpp2py_module(my_module)
add_cpp2py_module(my_moduleB)
add_cpp2py_module(test_g)
add_cpp2py_module(test_bl)
add_cpp2py_module(test_multivar)

add_dependencies(test_multivar meshes gf_fnt wrapped_aux)
add_dependencies(test_g meshes gf_fnt wrapped_aux)
add_dependencies(test_bl meshes gf_fnt wrapped_aux)

add_dependencies(my_moduleB my_module)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

# A sample test with text output
add_python_test(wrap_a)
add_python_test(wrap_B)
add_python_test(wrap_g)
add_python_test(wrap_multivar)
add_python_test(blockgf)
