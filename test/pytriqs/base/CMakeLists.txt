# A sample test with text output
add_triqs_test_txt(test_example)
add_triqs_test_txt(operator_python_test)
add_triqs_test(gf_fourier)
add_triqs_test_txt(gf_slicing)

# Test the test!
add_python_test(test_example)

# Many-body operators
add_python_test(operator_python_test)

# Some basic HDF IO stuff
add_python_test(hdf5_io)

# Greens functions
add_python_test(gf_init)
add_python_test(gf_base_op)
add_python_test(gf_fourier)
add_python_test(gf_slicing)

# a simple dos on square lattice
add_python_test(dos)

# Pade approximation
add_python_test(pade)

# Bug fix #112
add_python_test(gf_inplace_112)

# Bug fix #134
add_triqs_test(gf_transpose)

# Add evaluator for g
add_triqs_test(gf_eval)

# U_matrix
add_python_test(U_mat)

#block matrix
add_python_test(block_matrix)
