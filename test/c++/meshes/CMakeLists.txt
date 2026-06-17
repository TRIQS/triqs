# Copy reference h5 files to the build dir so tests resolve them from their working directory
file(GLOB all_h5_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.ref.h5)
file(COPY ${all_h5_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

all_tests()
#file(GLOB TestList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
 #foreach(f ${TestList})
  #get_filename_component(n ${f} NAME_WE)
  #add_executable(${n} ${f})
  #target_link_libraries(${n} gtest)
  #add_cpp_test(${n})
 #endforeach()
