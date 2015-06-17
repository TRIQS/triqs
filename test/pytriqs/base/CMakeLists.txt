# A sample test with text output
add_triqs_test_txt(test_example)
add_triqs_test_txt(operator_python_test)
add_triqs_test_txt(gf_fourier)
add_triqs_test_txt(gf_slicing)

# A sample test with hdf5 output
add_triqs_test_hdf(h5_example " -p 1.e-6")

# Some basic HDF IO stuff:
add_triqs_test_hdf(hdf5_io " -p 1.e-6")

# Greens functions:
add_triqs_test_hdf(gf_init " -d 1.e-6")
add_triqs_test(gf_base_op " -d 1.e-6")

# a simple dos on square lattice
add_triqs_test_hdf(dos " -d 1.e-6")

# Pade approximation
add_triqs_test_hdf(pade " -d 1.e-6")

# Bug fix #112
add_triqs_test_txt(gf_inplace_112)

# Bug fix #134
add_triqs_test_txt(gf_transpose)

# Add evaluator for g
add_triqs_test_txt(gf_eval)

# U_matrix:
add_triqs_test_hdf(U_mat " -d 1.e-6" )
