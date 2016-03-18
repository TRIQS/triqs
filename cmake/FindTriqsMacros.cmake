
# add_all_subdirectories_with_cmakelist : include all subdirectory to current path
# which contains a CMakeLists.txt file...
# rationale : automatize inclusion when projects are splitted & merged ....

macro (add_all_subdirectories_with_cmakelist)
 SET( print_decal_sub_dir "${print_decal_sub_dir}  ")
 FILE(GLOB ALLSUBS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */CMakeLists.txt)
 foreach ( f ${ALLSUBS})
  get_filename_component(d ${f} PATH)
  #MESSAGE(STATUS "${print_decal_sub_dir}## Including subdirectory :  ${d}")    
  add_subdirectory(${d})
 endforeach ( f ${ALLSUBS}) 
endmacro (add_all_subdirectories_with_cmakelist)

