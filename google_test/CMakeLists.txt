add_library(gtest gtest-all)
install(TARGETS gtest DESTINATION lib EXPORT triqs-dependencies)
install(FILES gtest.h DESTINATION "include/")
