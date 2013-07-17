SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
)

install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/parameters)

cython_module(Parameters parameters  parameters )

FILE(GLOB all_pxd_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.pxd )
install (FILES ${all_pxd_files} DESTINATION "include/pytriqs/parameters")
install (FILES ${CMAKE_SOURCE_DIR}/pytriqs/__init__.py.template DESTINATION "include/pytriqs/parameters" RENAME __init__.py)

