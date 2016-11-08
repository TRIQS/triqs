# Build C++ extension module
triqs_python_extension(atom_diag atom_diag)
add_dependencies(python_wrap_atom_diag python_wrap_gf python_wrap_operators)

# Install __init__.py
install(FILES __init__.py DESTINATION ${TRIQS_PYTHON_LIB_DEST}/atom_diag)
