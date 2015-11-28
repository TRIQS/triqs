find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS serialization system)
link_libraries(${Boost_LIBRARIES})

if (USE_CPP14) 
all_tests()
endif()
