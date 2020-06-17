
SET(PYTHON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_imfreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_imtime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_refreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_retime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_legendre.py
)

# Install python sources
install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gf/backwd_compat)

