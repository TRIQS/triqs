add_executable(itertools itertools.cpp)
target_link_libraries(itertools PRIVATE gtest)
target_compile_options(itertools PRIVATE -std=c++17)
add_test(itertools itertools)
