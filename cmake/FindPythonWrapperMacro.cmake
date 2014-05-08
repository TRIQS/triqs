# Compile and link with python 
include_directories(${PYTHON_INCLUDE_DIRS} ${PYTHON_NUMPY_INCLUDE_DIR})

# This function add the target to build a python module
#
# ModuleName = the python name of the module 
# ModuleDest = path in the pytriqs tree [ FOR INSTALLATION ONLY] IMPROVE MAKE THIS OPTIONAL (for test) 

macro (triqs_python_extension ModuleName)
 message(STATUS "TRIQS: Preparing extension module ${ModuleName} with the interpreter ${TRIQS_PYTHON_INTERPRETER} ")

 SET(wrap_name  ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}_wrap.cpp)

 # Adjust pythonpath so that pytriqs is visible and the wrap_generator too...
 # pytriqs needed since we import modules with pure python method to extract the doc.. 
 add_custom_command(OUTPUT ${wrap_name} DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ModuleName}_desc.py 
  COMMAND PYTHONPATH=${CMAKE_BINARY_DIR}/pytriqs/wrap_generator:${CMAKE_BINARY_DIR}/ ${PYTHON_INTERPRETER} ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}_desc.py ${CMAKE_SOURCE_DIR}/pytriqs/wrap_generator/wrapper.mako.cpp ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}_wrap.cpp )

 add_custom_target(python_wrap_${ModuleName} ALL DEPENDS ${wrap_name})
 add_dependencies(python_wrap_${ModuleName} py_copy)

 add_library(${ModuleName} MODULE ${wrap_name})
 set_target_properties(${ModuleName}  PROPERTIES PREFIX "") #eliminate the lib in front of the module name 
 target_link_libraries(${ModuleName} ${TRIQS_LINK_LIBS} triqs)
 
 if (${ARGN} MATCHES "")
   install (TARGETS ${ModuleName} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/${ARGN}  )
 endif (${ARGN} MATCHES "")
 #set_property (GLOBAL APPEND PROPERTY DEPENDANCE_TO_ADD triqs_${NickName} )

endmacro (triqs_python_extension)

