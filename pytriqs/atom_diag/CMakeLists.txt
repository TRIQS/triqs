# Build C++ extension module
add_cpp2py_module(atom_diag)

# gf and operators must build first
add_dependencies(atom_diag wrapped_aux gf_fnt operators)

# Install __init__.py
install(FILES __init__.py DESTINATION ${TRIQS_PYTHON_LIB_DEST}/atom_diag)
install(TARGETS atom_diag DESTINATION ${TRIQS_PYTHON_LIB_DEST}/atom_diag)

