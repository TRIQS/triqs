file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/svo_tb.dat DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
all_tests()

set(TEST_MPI_NUMPROC 2)
add_cpp_test(tb_hamiltonian)
