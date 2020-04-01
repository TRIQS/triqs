# Build gtest from git subtree
set(PYTHON_EXECUTABLE ${PYTHON_INTERPRETER})
add_subdirectory(gtest EXCLUDE_FROM_ALL)

# FIXME : TO BE REMOVED
# add_all_subdirectories_with_cmakelist : include all subdirectory to current path
# which contains a CMakeLists.txt file...
# rationale : automatize inclusion when projects are splitted & merged ....
macro(add_all_subdirectories_with_cmakelist)
 set(print_decal_sub_dir "${print_decal_sub_dir}  ")
 file(GLOB ALLSUBS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */CMakeLists.txt)
  foreach(f ${ALLSUBS})
   get_filename_component(d ${f} PATH)
   add_subdirectory(${d})
 endforeach(f ${ALLSUBS}) 
endmacro(add_all_subdirectories_with_cmakelist)

add_subdirectory(triqs)

if(PythonSupport)
 add_subdirectory(pytriqs)
 add_subdirectory(cpp2py)
endif()
