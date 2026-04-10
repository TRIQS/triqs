# install Python files
set(python_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_imfreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_imtime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_refreq.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_retime.py
  ${CMAKE_CURRENT_SOURCE_DIR}/gf_legendre.py
)
install(FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/gfs/backwd_compat)
