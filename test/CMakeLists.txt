include_directories( ${CMAKE_SOURCE_DIR} )

SET( link_libs ${LAPACK_LIBS}  ${BOOST_LIBRARY} )
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 list (REMOVE_DUPLICATES link_libs)
ENDIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
link_libraries( ${link_libs} triqs)

if (NOT compiler_is_obsolete_gcc)
add_subdirectory(triqs)
endif()
if (PythonSupport)
 add_subdirectory(pytriqs)
endif()

