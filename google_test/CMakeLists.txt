add_library(gtest gtest-all)
target_include_directories(gtest SYSTEM INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/google_test>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)

install(TARGETS gtest DESTINATION lib EXPORT triqs-dependencies)
install(FILES gtest.h DESTINATION "include/")
