add_subdirectory(test_tools)

# all .cpp files to get the dependencies
FILE(GLOB_RECURSE ALLSOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cxx)
FILE(GLOB_RECURSE ALLHEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h *.hxx)

# remove the .mako.?pp from the build
# they are not preprocessed by cmake
foreach (TMP_PATH ${ALLHEADERS})
    string (FIND ${TMP_PATH} ".mako." EXCLUDE_DIR_FOUND)
    if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
        list (REMOVE_ITEM ALLHEADERS ${TMP_PATH})
    endif ()
endforeach()
foreach (TMP_PATH ${ALLSOURCES})
    string (FIND ${TMP_PATH} ".mako." EXCLUDE_DIR_FOUND)
    if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
        list (REMOVE_ITEM ALLSOURCES ${TMP_PATH})
    endif ()
endforeach()

# The lib will be build from the sources and the converters generated sources
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
SET(LINK_RT "")
else()
SET(LINK_RT "-lrt")
endif()
add_library(triqs ${ALLSOURCES} ${all_py_converters_sources})
target_link_libraries (triqs ${LINK_RT} ${TRIQS_LINK_LIBS})

# The TRIQS library depends on the generation of the .cpp wrapper and converters files
# which are produced first.
get_property(all_py_converters_targets GLOBAL PROPERTY TRIQS_PY_CONVERTERS_TARGETS)
foreach(T ${all_py_converters_targets})
 add_dependencies(triqs ${T})
endforeach()

# Install library
install(TARGETS triqs DESTINATION lib)
set(TRIQS_LIB_LOCATION
    ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}triqs${CMAKE_SHARED_LIBRARY_SUFFIX}
    PARENT_SCOPE)

# Version file
configure_file(version.hpp.in version.hpp @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/version.hpp" DESTINATION "include/triqs/")

# Install headers in good include dir
FOREACH(HEADER ${ALLHEADERS})
 get_filename_component(P ${HEADER} PATH)
 install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}" DESTINATION "include/triqs/${P}")
ENDFOREACH()


