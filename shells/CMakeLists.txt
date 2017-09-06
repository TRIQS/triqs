MESSAGE( STATUS "Preparing the various scripts ")
# a simple script to set the python path and have the same effect as the statically compiled code.
option(TRIQS_ADD_PYPATHS_TO_SHELLS "Add the correct python paths to shell (ON except for packaging)" ON)
mark_as_advanced( TRIQS_ADD_PYPATHS_TO_SHELLS)
if (TRIQS_ADD_PYPATHS_TO_SHELLS)
 set(EXPORT_PYTHONPATH "export PYTHONPATH=${CMAKE_INSTALL_PREFIX}/${TRIQS_PYTHON_LIB_DEST_ROOT}:${CMAKE_INSTALL_PREFIX}/libexec/venv/${TRIQS_PYTHON_LIB_DEST_ROOT}:\$PYTHONPATH")
else (TRIQS_ADD_PYPATHS_TO_SHELLS)
 set(EXPORT_PYTHONPATH "")
endif (TRIQS_ADD_PYPATHS_TO_SHELLS)

# pytriqs shell
configure_file(pytriqs.sh.in pytriqs)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pytriqs DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# ipytriqs and ipytriqs_notebook shells
if (TRIQS_IPYTHON_DETECTED)
  configure_file(ipytriqs.sh.in ipytriqs)
  configure_file(ipytriqs_notebook.sh.in ipytriqs_notebook)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ipytriqs ${CMAKE_CURRENT_BINARY_DIR}/ipytriqs_notebook DESTINATION bin
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif (TRIQS_IPYTHON_DETECTED)

# Shell for tests
set(EXPORT_PYTHONPATH "export PYTHONPATH=${CMAKE_BINARY_DIR}:\$PYTHONPATH")
configure_file(pytriqs.sh.in ${CMAKE_BINARY_DIR}/build_pytriqs)
execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/build_pytriqs)
