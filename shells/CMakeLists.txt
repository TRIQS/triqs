
MESSAGE( STATUS "Preparing the various scripts ")
# a simple script to set the python path and have the same effect as the statically compiled code.
option(TRIQS_ADD_PYPATHS_TO_SHELLS "Add the correct python paths to shell (ON except for packaging)" ON)
mark_as_advanced( TRIQS_ADD_PYPATHS_TO_SHELLS)
if (TRIQS_ADD_PYPATHS_TO_SHELLS)
 configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/pytriqs_shell.bash.in ${CMAKE_CURRENT_BINARY_DIR}/pytriqs)
 if (TRIQS_IPYTHON_DETECTED)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ipytriqs_shell.bash.in ${CMAKE_CURRENT_BINARY_DIR}/ipytriqs)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ipytriqs_shell.notebook.bash.in ${CMAKE_CURRENT_BINARY_DIR}/ipytriqs_notebook)
 endif (TRIQS_IPYTHON_DETECTED)
else (TRIQS_ADD_PYPATHS_TO_SHELLS)
 configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/pytriqs_shell.nopypath.bash.in ${CMAKE_CURRENT_BINARY_DIR}/pytriqs)
 if (TRIQS_IPYTHON_DETECTED)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ipytriqs_shell.nopypath.bash.in ${CMAKE_CURRENT_BINARY_DIR}/ipytriqs)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ipytriqs_shell.notebook.nopypath.bash.in ${CMAKE_CURRENT_BINARY_DIR}/ipytriqs_notebook)
 endif (TRIQS_IPYTHON_DETECTED)
endif (TRIQS_ADD_PYPATHS_TO_SHELLS)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/pytriqs_for_test_shell.bash.in ${CMAKE_BINARY_DIR}/run_pytriqs_for_test.sh)
execute_process( COMMAND chmod u+x  ${CMAKE_BINARY_DIR}/run_pytriqs_for_test.sh )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pytriqs DESTINATION bin  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

 if (TRIQS_IPYTHON_DETECTED) 
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ipytriqs ${CMAKE_CURRENT_BINARY_DIR}/ipytriqs_notebook DESTINATION bin  
 PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
 endif (TRIQS_IPYTHON_DETECTED) 


