add_library(cpp2py signal_handler.cpp exceptions.cpp)
target_compile_options(cpp2py PRIVATE -std=c++14 -fPIC)
target_include_directories(cpp2py
  PUBLIC
  	$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  )

# Install the library in lib and prepare an exported cmake file to reimport it
install(TARGETS cpp2py DESTINATION lib EXPORT cpp2py-targets)
install(EXPORT cpp2py-targets DESTINATION lib/cmake/cpp2py)
