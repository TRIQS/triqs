SET(PYTHON_SOURCES 
 ${CMAKE_CURRENT_SOURCE_DIR}/wrap_generator.py
 ${CMAKE_CURRENT_SOURCE_DIR}/py_converter_wrapper.mako.hpp
 ${CMAKE_CURRENT_SOURCE_DIR}/wrapper.mako.cpp
 )

install(FILES ${PYTHON_SOURCES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/triqs/wrap_generator)

# The desc_file generator from libclang ...
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wrapper_desc_generator.py.in ${CMAKE_CURRENT_BINARY_DIR}/wrapper_desc_generator.py @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/wrapper_desc_generator.py DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/clang_parser.py DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/wrap_desc.mako.py DESTINATION share/triqs/wrap_generator/)

