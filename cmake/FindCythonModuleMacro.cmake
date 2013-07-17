
# Compile and link with python 
#link_libraries(${PYTHON_LIBRARY} ${PYTHON_EXTRA_LIBS} )
include_directories(${PYTHON_INCLUDE_DIRS} ${PYTHON_NUMPY_INCLUDE_DIR})

function (python_register_dynamic_module NickName)
  set_property (GLOBAL APPEND PROPERTY DEPENDANCE_TO_ADD triqs_${NickName} )
  set_property (GLOBAL APPEND PROPERTY Modules_actually_compiled ${NickName} )
endfunction (python_register_dynamic_module NickName)

# This function add the target to build a python module
#
# NickName =  
# ModuleName = the python name of the module 
# ModuleDest = path in the pytriqs tree [ FOR INSTALLATION ONLY] IMPROVE MAKE THIS OPTIONAL (for test) 
macro (cython_module NickName ModuleName)
 MESSAGE(STATUS "Preparing cython module  ${NickName} ")
 get_filename_component(CYTHON_EXECUTABLE_PATH ${PYTHON_INTERPRETER} PATH)
 SET(CYTHON_EXECUTABLE ${CYTHON_EXECUTABLE_PATH}/cython CACHE STRING "Path to the cython executable")
 SET(cython_src ${CMAKE_CURRENT_SOURCE_DIR}/${ModuleName}.pyx )
 FILE(GLOB all_pyx_src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.pyx *.pxd )
 SET(cython_wrap  ${CMAKE_CURRENT_BINARY_DIR}/wrap_${NickName}_by_cython.cpp)
 add_custom_command (OUTPUT ${cython_wrap} DEPENDS ${all_pyx_src} COMMAND ${CYTHON_EXECUTABLE} ${cython_src} -I ${CMAKE_CURRENT_SOURCE_DIR}/ -I${CMAKE_SOURCE_DIR}/pytriqs/pxd -X embedsignature=True --cplus -o ${cython_wrap}  )
 add_custom_target(cython_${NickName} ALL DEPENDS ${cython_wrap})

 add_library(${ModuleName} MODULE ${cython_wrap}  )
 set_target_properties(${ModuleName}  PROPERTIES PREFIX "") #eliminate the lib in front of the module name 
 target_link_libraries(${ModuleName} ${TRIQS_LINK_LIBS} triqs)
 if (${ARGN} MATCHES "")
   install (TARGETS ${ModuleName} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/${ARGN}  )
 endif (${ARGN} MATCHES "")
 set_property (GLOBAL APPEND PROPERTY DEPENDANCE_TO_ADD triqs_${NickName} )

endmacro (cython_module)

