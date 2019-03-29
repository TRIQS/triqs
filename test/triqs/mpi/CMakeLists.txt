link_libraries(triqs gtest)

file(GLOB TestList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} mpi_*.cpp)
foreach(TestName1 ${TestList})
 string(REPLACE ".cpp" "" TestName ${TestName1})
 add_executable(${TestName}  ${CMAKE_CURRENT_SOURCE_DIR}/${TestName}.cpp)
 add_test(${TestName} ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
 if (TESTS_C_WITH_VALGRIND)
  add_test(${TestName}_valgrind valgrind --error-exitcode=1 --suppressions=${CMAKE_SOURCE_DIR}/test/triqs/valgrind.supp ${CMAKE_CURRENT_BINARY_DIR}/${TestName})
 endif()

 set(TEST_MPI_NUMPROC 2)
 add_cpp_test(${TestName})
 set(TEST_MPI_NUMPROC 3)
 add_cpp_test(${TestName})
 set(TEST_MPI_NUMPROC 4)
 add_cpp_test(${TestName})
endforeach()

# Communicator split test
add_executable(comm_split comm_split.cpp)
set(TEST_MPI_NUMPROC 4)
add_cpp_test(comm_split)

add_executable(vector_zero_length vector_zero_length.cpp)
add_cpp_test(vector_zero_length)
