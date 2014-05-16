# all .cpp files to get the dependencies
FILE(GLOB_RECURSE ALLSOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
FOREACH(CPP ${ALLSOURCES})
  set_property (GLOBAL APPEND PROPERTY TRIQS_LIB_CPP_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/${CPP}")
ENDFOREACH(CPP ${ALLSOURCES})
get_property(ALLSOURCES GLOBAL PROPERTY TRIQS_LIB_CPP_SOURCES_LIST)

## The library also contains all the converters python <-> C++ 
## automatically generated for all wrapped modules.
## The lists are defined in subdirectory, in each module
## NB : pytriqs subdir is and must be called BEFORE triqs, or these lists are empty
get_property(all_py_converters_sources GLOBAL PROPERTY TRIQS_PY_CONVERTERS_CPP_LIST)
get_property(all_py_converters_targets GLOBAL PROPERTY TRIQS_PY_CONVERTERS_TARGETS)

# All files are generated, cmake should not worry not to see them at configuration time
foreach(CPP ${all_py_converters_sources})
 set_source_files_properties(${CPP} PROPERTIES GENERATED TRUE)
endforeach()

# compile every subdirectory
add_all_subdirectories_with_cmakelist()

# The lib will be build from the sources and the converters generated sources
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
SET( LINK_RT "")
else()
SET( LINK_RT "-lrt")
endif()
add_library(triqs ${ALLSOURCES} ${all_py_converters_sources})
target_link_libraries (triqs ${LINK_RT} ${TRIQS_LINK_LIBS}  )

# The TRIQS library depends on the generation of the .cpp wrapper and converters files
# which must be produced first.
foreach(T ${all_py_converters_targets})
 add_dependencies(triqs ${T})
endforeach()

# Install library
install(TARGETS triqs DESTINATION lib)

# Version file
configure_file(version.hpp.in version.hpp @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/version.hpp" DESTINATION "include/triqs/")

# all .hpp files...
FILE(GLOB_RECURSE ALLHEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# Install headers in good include dir
FOREACH(HEADER ${ALLHEADERS})
 get_filename_component(P ${HEADER} PATH)
 install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}" DESTINATION "include/triqs/${P}")
 #MESSAGE(STATUS "I will install ${HEADER} of ${P}")
ENDFOREACH(HEADER ${ALLHEADERS})


