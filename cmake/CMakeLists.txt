configure_file(Cpp2PyConfig.cmake.in Cpp2PyConfig.cmake @ONLY)
configure_file(Cpp2PyConfigVersion.cmake.in Cpp2PyConfigVersion.cmake @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Cpp2PyConfig.cmake DESTINATION lib/cmake/cpp2py)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Cpp2PyConfigVersion.cmake DESTINATION lib/cmake/cpp2py)
