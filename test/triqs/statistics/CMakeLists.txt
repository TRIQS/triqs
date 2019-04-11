all_tests()

set(TEST_MPI_NUMPROC 2)
add_cpp_test(mpi_histogram)
set(TEST_MPI_NUMPROC 3)
add_cpp_test(mpi_histogram)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(mpi_histogram)
