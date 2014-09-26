
SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/operators.py
  ${CMAKE_CURRENT_SOURCE_DIR}/hamiltonians.py
)

install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/operators)

triqs_python_extension(operators2 operators)
