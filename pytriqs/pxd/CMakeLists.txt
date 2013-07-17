FILE(GLOB all_pxd_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.pxd )
install (FILES ${all_pxd_files} DESTINATION "include/pytriqs")
install (FILES ${CMAKE_SOURCE_DIR}/pytriqs/__init__.py.template DESTINATION "include/pytriqs" RENAME __init__.py)
