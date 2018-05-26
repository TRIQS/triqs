add_library(gtest gtest-all)
install(TARGETS gtest DESTINATION lib EXPORT triqs-targets)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/gtest.h" DESTINATION "include/")

