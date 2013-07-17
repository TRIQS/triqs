#if (MPI_FOUND)
# MESSAGE(STATUS "MPI launch command is : ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} PROCS ${MPIEXEC_PREFLAGS} EXECUTABLE ${MPIEXEC_POSTFLAGS} ARGS")
#endif (MPI_FOUND)

# A sample test with text output
add_triqs_test_txt(test_example )

# A sample test with hdf5 output
add_triqs_test_hdf(h5_example " -p 1.e-6" )
#add_triqs_test_hdf(ExampleTestH5-2 " -p 1.e-6" )

# Some basic HDF IO stuff:
add_triqs_test_hdf(hdf5_io " -p 1.e-6" )

# Greens functions:
add_triqs_test_hdf(gf_init " -d 1.e-6" )
add_triqs_test_hdf(gf_base_op " -d 1.e-6" )

# a simple dos on square lattice
add_triqs_test_hdf(dos " -d 1.e-6" )

# Pade approximation
add_triqs_test_hdf(pade " -d 1.e-6" )
