link_libraries(gtest mpi)
add_definitions(-std=c++17)

file(GLOB test_files "mpi_*.cpp")
foreach(file ${test_files})
 get_filename_component(TestName ${file} NAME_WE)
 add_executable(${TestName} ${file})
 add_test(${TestName} ${TestName})
 add_test(${TestName}_np1 mpirun -np 1 ${TestName})
 add_test(${TestName}_np2 mpirun -np 2 ${TestName})
 add_test(${TestName}_np3 mpirun -np 3 ${TestName})
 add_test(${TestName}_np4 mpirun -np 4 ${TestName})
endforeach()

add_executable(comm_split comm_split.cpp)
add_test(comm_split mpirun -np 4 comm_split)

add_executable(vector_zero_length vector_zero_length.cpp)
add_test(vector_zero_length vector_zero_length)
