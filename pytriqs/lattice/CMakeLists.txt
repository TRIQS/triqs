SET(PYTHON_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_SOURCE_DIR}/bz_patch.py
  ${CMAKE_CURRENT_SOURCE_DIR}/super_lattice.py
  ${CMAKE_CURRENT_SOURCE_DIR}/tight_binding.py
)

install (FILES ${PYTHON_SOURCES} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/lattice)

# THE C++ code
#SET(CPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/C++)
cython_module(LatticeTools lattice_tools lattice )

# ???
#target_link_libraries(_pytriqs_LatticeTools triqs ${TRIQS_LINK_LIBS})

