list(APPEND CMAKE_MODULE_PATH ${TRIQS_PATH}/share/triqs/cmake)
cmake_minimum_required(VERSION 2.8)
project(myproj CXX)
set(CMAKE_BUILD_TYPE Release)

find_package(TRIQS)

add_executable(hdf5 hdf5.cpp)
include_directories(${TRIQS_INCLUDE_ALL})
target_link_libraries(hdf5 ${TRIQS_LIBRARY_ALL})
