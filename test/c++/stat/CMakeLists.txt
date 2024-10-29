all_tests()

set(TEST_MPI_NUMPROC 2)
add_cpp_test(stat_mean_error)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(stat_mean_error)

set(TEST_MPI_NUMPROC 2)
add_cpp_test(stat_lin_binning_mpi)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(stat_lin_binning_mpi)

set(TEST_MPI_NUMPROC 2)
add_cpp_test(stat_log_binning_mpi)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(stat_log_binning_mpi)

set(TEST_MPI_NUMPROC 2)
add_cpp_test(stat_jackknife)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(stat_jackknife)

set(TEST_MPI_NUMPROC 2)
add_cpp_test(stat_histograms_mpi)
set(TEST_MPI_NUMPROC 3)
add_cpp_test(stat_histograms_mpi)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(stat_histograms_mpi)

set(TEST_MPI_NUMPROC 2)
add_cpp_test(stat_ar_model_mpi)
set(TEST_MPI_NUMPROC 3)
add_cpp_test(stat_ar_model_mpi)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(stat_ar_model_mpi)


set(TEST_MPI_NUMPROC 2)
add_cpp_test(accumulator_mpi)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(accumulator_mpi)

set(TEST_MPI_NUMPROC 2)
add_cpp_test(mpi_MQ_reduce)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(mpi_MQ_reduce)
