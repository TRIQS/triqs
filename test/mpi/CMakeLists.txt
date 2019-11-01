link_libraries(gtest mpi)
add_definitions(-std=c++17 -fopenmp)

macro(add_mpi_test TestName NumProc)
  add_test(NAME ${TestName}_np${NumProc}
	   COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${NumProc} ${MPIEXEC_PREFLAGS} ./${TestName} ${MPIEXEC_POSTFLAGS})
endmacro()

file(GLOB test_files "mpi_*.cpp")
foreach(file ${test_files})
 get_filename_component(TestName ${file} NAME_WE)
 add_executable(${TestName} ${file})
 target_link_libraries(${TestName} -fopenmp)
 add_test(${TestName} ${TestName})
 add_mpi_test(${TestName} 1)
 add_mpi_test(${TestName} 2)
 add_mpi_test(${TestName} 3)
 add_mpi_test(${TestName} 4)
endforeach()

add_executable(comm_split comm_split.cpp)
add_mpi_test(comm_split 4)

add_executable(vector_zero_length vector_zero_length.cpp)
add_test(vector_zero_length vector_zero_length)
