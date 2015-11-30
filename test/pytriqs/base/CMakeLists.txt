#FILE(GLOB all_h5_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h5)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${all_h5_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/gf_init.output.h5" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/pade.output.h5" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/dos.output.h5" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# A sample test with text output
add_triqs_test_txt(test_example)
add_triqs_test_txt(operator_python_test)
add_triqs_test(gf_fourier)
add_triqs_test_txt(gf_slicing)

# A sample test with hdf5 output
add_triqs_test_hdf(h5_example " -p 1.e-6")

# Some basic HDF IO stuff:
add_triqs_test_hdf(hdf5_io " -p 1.e-6")

# Greens functions:
add_triqs_test(gf_init)
add_triqs_test(gf_base_op)

# a simple dos on square lattice
add_triqs_test(dos)

# Pade approximation
add_triqs_test(pade)

# Bug fix #112
add_triqs_test_txt(gf_inplace_112)

# Bug fix #134
add_triqs_test(gf_transpose)

# Add evaluator for g
add_triqs_test(gf_eval)

# U_matrix:
add_triqs_test_hdf(U_mat " -d 1.e-6" )
