# Compile and link with python
include_directories(${PYTHON_INCLUDE_DIRS} ${PYTHON_NUMPY_INCLUDE_DIR})

# This function add the target to build a python module
#
# ModuleName = the python name of the module
# ModuleDest = path in the pytriqs tree [ FOR INSTALLATION ONLY] IMPROVE MAKE THIS OPTIONAL (for test)

EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/include/pytriqs/converters/)
include_directories( ${CMAKE_BINARY_DIR})

macro (triqs_python_extension ModuleName)
 message(STATUS "Preparing extension module ${ModuleName}")

 SET(wrap_name  ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}_wrap.cpp)
 SET(converter_name  ${CMAKE_BINARY_DIR}/include/pytriqs/converters/${ModuleName}.hpp)

 # Adjust pythonpath so that pytriqs is visible and the wrap_generator too...
 # pytriqs needed since we import modules with pure python method to extract the doc..
 add_custom_command(OUTPUT ${wrap_name} ${converter_name} DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ModuleName}_desc.py
  COMMAND PYTHONPATH=${CMAKE_BINARY_DIR}/tools/cpp2py:${CMAKE_BINARY_DIR}/:$ENV{PYTHONPATH} ${PYTHON_INTERPRETER} ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}_desc.py
   ${CMAKE_SOURCE_DIR}/tools/cpp2py/mako/xxx_wrap.cpp
   ${wrap_name}
   ${CMAKE_SOURCE_DIR}/tools/cpp2py/mako/py_converter.hpp
   ${converter_name} 
   ${CMAKE_BINARY_DIR}/include/pytriqs/converters/
   )

 set_property (GLOBAL APPEND PROPERTY TRIQS_PY_CONVERTERS_TARGETS "python_wrap_${ModuleName}")

 add_custom_target(python_wrap_${ModuleName} ALL DEPENDS ${wrap_name} ${converter_name})
 add_dependencies(python_wrap_${ModuleName} py_copy tools_copy)
 #add_dependencies(python_wrap_${ModuleName} ${CMAKE_CURRENT_SOURCE_DIR}/${ModuleName}_desc.py )

 add_library(${ModuleName} MODULE ${wrap_name})
 set_target_properties(${ModuleName} PROPERTIES PREFIX "") #eliminate the lib in front of the module name
 target_link_libraries(${ModuleName} ${TRIQS_LINK_LIBS} triqs)

 if (${ARGN} MATCHES "")
   install (FILES ${converter_name} DESTINATION "include/pytriqs/converters")
   install (TARGETS ${ModuleName} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/${ARGN}  )
 endif (${ARGN} MATCHES "")

endmacro (triqs_python_extension)

