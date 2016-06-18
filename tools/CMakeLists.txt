
# Create a temporary copy of the python modules so that we can run before installation with the test
# same as for pytriqs
FILE(GLOB_RECURSE all_py_files RELATIVE ${CMAKE_SOURCE_DIR}/tools *.py)
SET(tools_copy_tar ${CMAKE_BINARY_DIR}/tools_copy.tar)
add_custom_command (OUTPUT ${tools_copy_tar} DEPENDS ${all_py_files}
 COMMAND cd ${CMAKE_SOURCE_DIR}/tools && ${CMAKE_COMMAND} -E tar cf ${tools_copy_tar} ${all_py_files} &&
         cd ${CMAKE_BINARY_DIR}/tools && ${CMAKE_COMMAND} -E tar xf ${tools_copy_tar})
add_custom_target(tools_copy ALL DEPENDS ${tools_copy_tar})

# list all files to be installed...

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/util.py    DESTINATION  share/triqs)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2py/__init__.py                DESTINATION  share/triqs/cpp2py)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2py/wrap_generator.py          DESTINATION  share/triqs/cpp2py)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2py/mako/py_converter.hpp      DESTINATION  share/triqs/cpp2py/mako)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2py/mako/xxx_wrap.cpp          DESTINATION  share/triqs/cpp2py/mako)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2py/mako/xxx_desc.py           DESTINATION  share/triqs/cpp2py/mako)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2py/mako/converters.hxx        DESTINATION  share/triqs/cpp2py/mako)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2py/mako/parameters.rst        DESTINATION  share/triqs/cpp2py/mako)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2doc/mako/xxx_doc_function.rst  DESTINATION  share/triqs/cpp2doc/mako)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2doc/mako/xxx_doc_class.rst     DESTINATION  share/triqs/cpp2doc/mako)

# temporary fix for brew formula before repackaging
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2doc/mako/cpp2doc_tools.py      DESTINATION  bin)
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/clang_parser.py            DESTINATION  bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp2doc/mako/cpp2doc_tools.py      DESTINATION  bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/clang_parser.py            DESTINATION  bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

# The desc_file generator from libclang ...
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cpp2py/c++2py.py.in ${CMAKE_CURRENT_BINARY_DIR}/c++2py.py @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cpp2doc/c++2doc.py.in ${CMAKE_CURRENT_BINARY_DIR}/c++2doc.py @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/c++2py.py  DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/c++2doc.py  DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )


