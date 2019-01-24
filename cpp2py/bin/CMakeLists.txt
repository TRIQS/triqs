set(scripts c++2py c++2rst c++2cxx cp_rs)

foreach(script ${scripts})
 install(FILES ${script} DESTINATION  bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
 execute_process(COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/${script} ${CMAKE_CURRENT_BINARY_DIR}/${script})
endforeach()
