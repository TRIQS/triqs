# symlink svo wannier data files into the build dir
foreach(f svo_tb.dat svo_hr.dat)
  file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/${f}
       ${CMAKE_CURRENT_BINARY_DIR}/${f} SYMBOLIC)
endforeach()

# functions for experimental capabilities
add_python_test(tb_hk)
add_python_test(find_chemical_potential)
add_python_test(gloc)
add_python_test(superlattice)
add_python_test(wannier)
