all_tests()

set(TEST_MPI_NUMPROC 2)
add_cpp_test(nda_mpi_array)
set(TEST_MPI_NUMPROC 3)
add_cpp_test(nda_mpi_array)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(nda_mpi_array)
