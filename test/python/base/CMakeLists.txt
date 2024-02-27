# Copy h5 files to binary dir
FILE(GLOB all_h5_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h5)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${all_h5_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# test if MPI detection works on system
set(testenv PYTHONPATH=${CMAKE_BINARY_DIR}/python:${CPP2PY_MODULE_DIR}:${CPP2PY_BINARY_DIR}:${h5_MODULE_DIR}:${h5_BINARY_DIR}/python:./:$ENV{PYTHONPATH})
add_test(NAME py_check_mpi_env COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} 1 ${MPIEXEC_PREFLAGS} ${CPP2PY_PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/check_mpi_env.py)
set_property(TEST py_check_mpi_env PROPERTY ENVIRONMENT ${testenv})


# Greens functions
add_python_test(gf_imfreq)
add_python_test(gf_block)
add_python_test(gf_init)
add_python_test(gf_density)
add_python_test(gf_base_op)
add_python_test(gf_fourier)
add_python_test(gf_fourier_real)
add_python_test(g_tau_mul)
add_python_test(gf_dlr)
add_python_test(block_gf_constr)

# a simple dos on square lattice
add_python_test(dos)

# Pade approximation
add_python_test(pade)

# Bug fix #112
add_python_test(gf_inplace_112)

# Bug fix #134
add_python_test(gf_transpose)

# Issue #517
add_python_test(hdf5_meshBZ)

add_python_test(issue460)
add_python_test(issue437)
add_python_test(issue649)
add_python_test(issue913)
add_python_test(bug_tight_binding)
add_python_test(bug_bcast_mesh)

# Issues with the high frequency moments
add_python_test(tail_issues)

# issue with mult of Gf with different dim
add_python_test(issue_gf_mult_dim)

# Add evaluator for g
add_python_test(gf_eval)

# U_matrix
add_python_test(U_mat)

#block matrix
#add_python_test(block_matrix)

add_python_test(gf_tensor_valued)

# Histograms
add_python_test(histograms)
add_python_test(histogram_bcast)
add_python_test(brillouin_zone)
add_python_test(cyclic_lattice)

# discretize bath
add_python_test(discretize_bath)
