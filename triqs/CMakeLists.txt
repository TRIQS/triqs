# Add C11 flag
# leave it here because boost has to be compiled without the flags because of boost python ??
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# all .cpp files to get the dependencies
FILE(GLOB_RECURSE ALLSOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
FOREACH(CPP ${ALLSOURCES})
  set_property (GLOBAL APPEND PROPERTY TRIQS_LIB_CPP_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/${CPP}")
ENDFOREACH(CPP ${ALLSOURCES})
get_property(ALLSOURCES GLOBAL PROPERTY TRIQS_LIB_CPP_SOURCES_LIST)

# compile every subdirectory
add_all_subdirectories_with_cmakelist()

# The lib will be build from the sources
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
SET( LINK_RT "")
else()
SET( LINK_RT "-lrt")
endif()
add_library(triqs ${ALLSOURCES})
target_link_libraries (triqs ${LINK_RT} ${TRIQS_LINK_LIBS}  )

# Install library
install(TARGETS triqs DESTINATION lib)

# all .hpp files...
FILE(GLOB_RECURSE ALLHEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# Install headers in good include dir
FOREACH(HEADER ${ALLHEADERS})
 get_filename_component(P ${HEADER} PATH)
 install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}" DESTINATION "include/triqs/${P}")
 #MESSAGE(STATUS "I will install ${HEADER} of ${P}")
ENDFOREACH(HEADER ${ALLHEADERS})


