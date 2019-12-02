set(scripts c++2py c++2rst c++2cxx cp_rs)

foreach(script ${scripts})
  configure_file(${script}.in ${script} @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${script} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endforeach()
