
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/wrap_generator.py      DESTINATION  share/triqs/c++2py)      
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/clang_parser.py        DESTINATION  bin)                      
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/mako/py_converter.hpp  DESTINATION  share/triqs/c++2py/mako)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/mako/xxx_wrap.cpp      DESTINATION  share/triqs/c++2py/mako)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/mako/xxx_desc.py       DESTINATION  share/triqs/c++2py/mako)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/mako/converters.hxx    DESTINATION  share/triqs/c++2py/mako)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/mako/parameters.rst    DESTINATION  share/triqs/c++2py/mako)

# The desc_file generator from libclang ...
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/c++2py.py.in ${CMAKE_CURRENT_BINARY_DIR}/c++2py.py @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/c++2py.py  DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

# The param generator
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/param_generator.py.in ${CMAKE_CURRENT_BINARY_DIR}/param_generator.py @ONLY)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/param_generator.py DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/param.mako.cxx DESTINATION share/triqs/c++2py/)

