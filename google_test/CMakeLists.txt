add_library(gtest gtest-all)
install(TARGETS gtest DESTINATION lib EXPORT triqs-targets)
install(FILES gtest.h DESTINATION "include/")
