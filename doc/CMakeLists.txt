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

# -----------------------------------------------------------------------------
# Make an optional target that allows us to regenerate the C++ doc with c++2rst
# -----------------------------------------------------------------------------
# list of namespaces selected by c++2rst
set(NS
    -N triqs::det_manip
    -N triqs::mc_tools
    -N triqs::hilbert_space
    -N triqs::atom_diag
    -N triqs::statistics)

get_property(TRIQS_INCLUDE_DIRS TARGET triqs PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(TRIQS_SYSTEM_INCLUDE_DIRS TARGET triqs PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
list(REMOVE_ITEM TRIQS_INCLUDE_DIRS ${TRIQS_SYSTEM_INCLUDE_DIRS})
foreach(I ${TRIQS_INCLUDE_DIRS})
 set (CPP2RST_INCLUDE_DIRS  ${CPP2RST_INCLUDE_DIRS} --includes ${I})
endforeach()
foreach(I ${TRIQS_SYSTEM_INCLUDE_DIRS})
  set (CPP2RST_SYSTEM_INCLUDE_DIRS  ${CPP2RST_SYSTEM_INCLUDE_DIRS} --system_includes ${I})
endforeach()

# Caution! This will alter the files in doc/cpp2rst_generated
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpp2rst.log DEPENDS triqs
		   COMMAND PYTHONPATH=${CPP2PY_BINARY_DIR}:$ENV{PYTHONPATH} ${CPP2PY_BINARY_DIR}/bin/c++2rst ${NS}
		   ${CMAKE_CURRENT_SOURCE_DIR}/reference/doc_root.hpp
		   --output_directory ${CMAKE_CURRENT_SOURCE_DIR}/cpp2rst_generated
		   --cxxflags "-std=c++17 -DTRIQS_BUILDING_LIBRARY -I${CMAKE_SOURCE_DIR}"
		   ${CPP2RST_INCLUDE_DIRS}
		   ${CPP2RST_SYSTEM_INCLUDE_DIRS}
		   --includes ${CMAKE_SOURCE_DIR}/cpp2py/include
		   --includes ${CMAKE_BINARY_DIR}/Config
		   2>&1 > cpp2rst.log
		   VERBATIM)
add_custom_target(docs_cpp2rst DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cpp2rst.log)
add_dependencies(docs_cpp2rst triqs)

# ------------------------------------------------------------------------------------------------
# Copy the directory substructure and link all files with relevant extensions
# cp_rs is a script in cpp2py/bin, it mimics cp -rs on Linux
# and filters the relevant extension
# ------------------------------------------------------------------------------------------------

set(EXT_FOR_DOC "bib rst png txt cpp css_t conf css js gif jpg py html bib sh md")
execute_process(COMMAND ${CPP2PY_BINARY_DIR}/bin/cp_rs ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${EXT_FOR_DOC})

#---------------------------------
# Top Sphinx target
# ---------------------------------
find_package(Sphinx)

# Sources
FILE(GLOB_RECURSE SOURCES *.rst)

SET(sphinx_top ${CMAKE_CURRENT_BINARY_DIR}/html/contents.html)
add_custom_command(OUTPUT ${sphinx_top} DEPENDS ${SOURCES}
  COMMAND PYTHONPATH=${CMAKE_BINARY_DIR}:${CPP2PY_BINARY_DIR}:$ENV{PYTHONPATH} ${SPHINXBUILD_EXECUTABLE} -c . -j8 -b html ${CMAKE_CURRENT_BINARY_DIR} html)
add_custom_target(docs_sphinx ALL DEPENDS ${sphinx_top} ${CMAKE_CURRENT_BINARY_DIR})

# doc depends on all python modules (because they  may be loaded in examples).
GET_PROPERTY(CPP2PY_MODULES_LIST GLOBAL PROPERTY CPP2PY_MODULES_LIST)
foreach (mod ${CPP2PY_MODULES_LIST})
 add_dependencies(docs_sphinx ${mod})
endforeach()

# ---------------------------------
# Install
# ---------------------------------
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ COMPONENT documentation DESTINATION share/doc/triqs
 FILES_MATCHING
 REGEX "\\.(bib|html|pdf|png|gif|jpg|js|xsl|css|py|txt|inv)$"
 PATTERN "_*"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/themes/agogo DESTINATION share/triqs/themes)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/themes/triqs DESTINATION share/triqs/themes)


