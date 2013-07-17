
SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf_archive_basic_layer_h5py.py
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf_archive.py
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf_archive_schemes.py
)

install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/archive)

