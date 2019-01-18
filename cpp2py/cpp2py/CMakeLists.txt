set(DEST ${PYTHON_LIB_DEST_ROOT})

configure_file(libclang_config.py.in libclang_config.py @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libclang_config.py DESTINATION ${DEST}/cpp2py)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${DEST} FILES_MATCHING PATTERN "*.py" PATTERN "*.cxx" PATTERN "*.rst")

# Link all files with relevant extensions into binary tree
# This allows us to get a working cpp2py Setup already after the build stage
execute_process(COMMAND ${PROJECT_BINARY_DIR}/bin/cp_rs ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} "py cxx rst")

###################################################################################
#
#            add_cpp2py_module
#
#   Set up the compilation of the cpp2py python module
#
#   Argument : ModuleName : name of the module
#   Usage    :
#              add_cpp2py_module (MyModule)
#               - expects a file MyModule_desc.py in the CURRENT_SOURCE_DIR
#               - add a target MyModule which :
#                    - DEPENDS on MyModule_desc.py
#                    - build the wrapper MyModule_wrap.cxx in CURRENT_BINARY_DIR
#                    - compiles it into the MyModule.so
#
###################################################################################
macro(add_cpp2py_module ModuleName)
 message(STATUS "Adding cpp2py Python module ${ModuleName}")

 set(desc_name  ${CMAKE_CURRENT_SOURCE_DIR}/${ModuleName}_desc.py)    # the desc file
 set(wrap_name  ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}_wrap.cxx)   # path to the wrapping code
 set(target_name ${ModuleName})

 add_custom_command(OUTPUT ${wrap_name} DEPENDS ${desc_name}
   COMMAND PYTHONPATH=${PROJECT_BINARY_DIR}:${PROJECT_BINARY_DIR}/python:${CPP2PY_BINARY_DIR}:${CPP2PY_ADD_MODULE_ADDITIONAL_PYTHONPATH}:$ENV{PYTHONPATH} ${PYTHON_INTERPRETER} ${desc_name} ${wrap_name})

 add_library(${target_name} MODULE ${wrap_name})
 target_link_libraries(${target_name} python_and_numpy cpp2py)

 set_target_properties(${target_name}
   PROPERTIES
   PREFIX ""  #eliminate the lib in front of the module name
   LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
 )

 if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   target_link_libraries(${target_name} "-undefined dynamic_lookup")
 endif()

 # Keep a list of every module target.
 # Usage : e.g.  Documentation top target depends on them being built first
 set_property(GLOBAL APPEND PROPERTY CPP2PY_MODULES_LIST ${target_name})

endmacro(add_cpp2py_module)
