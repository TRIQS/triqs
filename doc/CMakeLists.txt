# ------------------------------------------------------------------------------------------------
# Exportation of local Sphinx extensions ...
# ------------------------------------------------------------------------------------------------

install(DIRECTORY sphinxext DESTINATION "share/triqs")

# ------------------------------------------------------------------------------------------------
# Generate the conf.py
# ------------------------------------------------------------------------------------------------
#
set(TRIQS_SOURCE_DIR "${CMAKE_SOURCE_DIR}")

# MathJax is mandatory
find_package(MathJax REQUIRED)
message(STATUS "Detected MathJax at ${MATHJAX_PATH}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)

# ------------------------------------------------------------------------------------------------
# Copy the directory substructure and link all files with relevant extensions
# cp_rs is a script in cpp2py/bin, it mimics cp -rs on Linux
# and filters the relevant extension
# ------------------------------------------------------------------------------------------------

set(EXT_FOR_DOC "rst png txt cpp css_t conf css js gif jpg py html bib sh")
execute_process(COMMAND cp_rs ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${EXT_FOR_DOC})

# ---------------------------------
# Generate C++ doc with c++2rst
# ---------------------------------
# list of namespaces selected by c++2rst
set(NS
    -N triqs::det_manip
    -N triqs::mc_tools
    -N triqs::hilbert_space
    -N triqs::atom_diag
    -N triqs::statistics)

get_property(TRIQS_INCLUDE_DIRS TARGET triqs PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

foreach(I ${TRIQS_INCLUDE_DIRS})
 set (CPP2RST_INCLUDE_DIRS  ${CPP2RST_INCLUDE_DIRS} --includes ${I})
endforeach()

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpp2rst.log DEPENDS triqs
		   COMMAND c++2rst ${NS}
		   ${CMAKE_CURRENT_SOURCE_DIR}/reference/doc_root.hpp
		   --output_directory ${CMAKE_CURRENT_BINARY_DIR}/cpp2rst_generated
		   ${CPP2RST_INCLUDE_DIRS}
		   --includes ${CMAKE_SOURCE_DIR}/
		   --includes ${CMAKE_BINARY_DIR}/Config
		   --cxxflags "-std=c++17 -DTRIQS_BUILDING_LIBRARY" 
		   2>&1 > cpp2rst.log
		   VERBATIM)
add_custom_target(docs_cpp2rst DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cpp2rst.log)
add_dependencies(docs_cpp2rst triqs)

#---------------------------------
# Top Sphinx target
# ---------------------------------
find_package(Sphinx)

# Sources
FILE(GLOB_RECURSE SOURCES *.rst)

SET(sphinx_top ${CMAKE_CURRENT_BINARY_DIR}/html/contents.html)
add_custom_command(OUTPUT ${sphinx_top} DEPENDS ${SOURCES} docs_cpp2rst
  COMMAND PYTHONPATH=${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH} ${SPHINXBUILD_EXECUTABLE} -c . -j8 -b html ${CMAKE_CURRENT_BINARY_DIR} html)
add_custom_target(docs_sphinx ALL DEPENDS ${sphinx_top} ${CMAKE_CURRENT_BINARY_DIR})

# depends on generated doc
add_dependencies(docs_sphinx docs_cpp2rst)

# doc depends on all python modules (because they  may be loaded in examples).
GET_PROPERTY(CPP2PY_MODULES_LIST GLOBAL PROPERTY CPP2PY_MODULES_LIST)
foreach (mod ${CPP2PY_MODULES_LIST})
 add_dependencies(docs_sphinx ${mod})
endforeach()

# ---------------------------------
# Build C++ examples ...
# ---------------------------------
link_libraries(triqs)
FILE(GLOB_RECURSE ExampleList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# build all the examples in their respective directory
 foreach(example ${ExampleList})
  get_filename_component (f ${example} NAME_WE)
  get_filename_component (d ${example} DIRECTORY)
  add_executable(doc_${f} ${CMAKE_CURRENT_SOURCE_DIR}/${example})
  set_target_properties(doc_${f} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${d})
  add_dependencies(docs_sphinx doc_${f}) ## so that all code examples are compiled before the doc
 endforeach()

# ---------------------------------
# Install
# ---------------------------------
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ COMPONENT documentation DESTINATION share/doc/triqs
 FILES_MATCHING
 REGEX "\\.(html|pdf|png|gif|jpg|js|xsl|css|py|txt|inv)$"
 PATTERN "_*"
 )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ COMPONENT documentation DESTINATION share/doc/triqs  FILES_MATCHING PATTERN "*.bib")

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/themes/agogo DESTINATION share/triqs/themes)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/themes/triqs DESTINATION share/triqs/themes)


