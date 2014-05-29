SET(PYTHON_SOURCES 
 ${CMAKE_CURRENT_SOURCE_DIR}/wrap_generator.py
 ${CMAKE_CURRENT_SOURCE_DIR}/py_converter_wrapper.mako.hpp
 ${CMAKE_CURRENT_SOURCE_DIR}/wrapper.mako.cpp
 )

install (FILES ${PYTHON_SOURCES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/triqs/wrap_generator)

