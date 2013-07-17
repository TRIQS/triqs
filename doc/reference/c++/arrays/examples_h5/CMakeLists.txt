FILE(GLOB TestList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
enable_testing()
include_directories( ${CMAKE_CURRENT_BINARY_DIR})
find_package(TestCSimple)

SET( link_libs ${LAPACK_LIBS}  ${BOOST_LIBRARY} ${ALPS_EXTRA_LIBRARIES})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 list (REMOVE_DUPLICATES link_libs)
ENDIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

link_libraries( ${link_libs} triqs ) # adding triqs for the stack_trace fun used in exceptions... 

FOREACH( TestName1  ${TestList} )
 STRING(REPLACE ".cpp" "" TestName ${TestName1})
 add_executable( ${TestName}  ${CMAKE_CURRENT_SOURCE_DIR}/${TestName}.cpp )
 add_test_C_simple( ${TestName}  )
ENDFOREACH( TestName1  ${TestList} )


