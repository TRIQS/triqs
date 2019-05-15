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

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/sphinxext DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# -----------------------------------------------------------------------------
# Make an optional target that allows us to regenerate the C++ doc with c++2rst
# -----------------------------------------------------------------------------
# list of namespaces selected by c++2rst
set(NAMESPACES
    -N triqs::gfs
    -N triqs::det_manip
    -N triqs::mc_tools
    -N triqs::hilbert_space
    -N triqs::atom_diag
    -N triqs::statistics
    -N mpi
    -N itertools
)

get_property(TRIQS_INCLUDE_DIRS TARGET triqs PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
get_property(TRIQS_SYSTEM_INCLUDE_DIRS TARGET triqs PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
list(REMOVE_ITEM TRIQS_INCLUDE_DIRS ${TRIQS_SYSTEM_INCLUDE_DIRS})
foreach(DIR ${TRIQS_INCLUDE_DIRS})
  set(CPP2RST_INCLUDE_FLAGS ${CPP2RST_INCLUDE_FLAGS} -I${DIR})
endforeach()
foreach(DIR ${TRIQS_SYSTEM_INCLUDE_DIRS})
  set(CPP2RST_INCLUDE_FLAGS ${CPP2RST_INCLUDE_FLAGS} -isystem=${DIR})
endforeach()
get_property(CPP2RST_CXXFLAGS TARGET triqs PROPERTY INTERFACE_COMPILE_OPTIONS)

# Caution! This will alter the files in reference/cpp/generated
# FIXME : clean after merging the nda_handle branch, we don't nee the BUILDING, or Python include
add_custom_target(docs_cpp2rst)
add_custom_command(
  TARGET docs_cpp2rst
  COMMAND rm -rf ${CMAKE_CURRENT_SOURCE_DIR}/documentation/cpp_api
  COMMAND PYTHONPATH=${CPP2PY_BINARY_DIR}:$ENV{PYTHONPATH} ${CPP2PY_BINARY_DIR}/bin/c++2rst
  ${CMAKE_CURRENT_SOURCE_DIR}/documentation/api_root.hpp
  ${NAMESPACES}
  --output_directory ${CMAKE_CURRENT_SOURCE_DIR}/documentation/cpp_api
  --cxxflags="-DTRIQS_BUILDING_LIBRARY ${CPP2RST_CXXFLAGS}"
  --parse-all-comments
  ${CPP2RST_INCLUDE_FLAGS}
  -I${CMAKE_BINARY_DIR}/Config
  -isystem=${CMAKE_SOURCE_DIR}/cpp2py/include
)

# --------------------------------------------------------
# Build & Run the C++ doc examples and capture the output
# --------------------------------------------------------

add_custom_target(docs_example_output)
file(GLOB_RECURSE ExampleList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(example ${ExampleList})
  get_filename_component(f ${example} NAME_WE)
  get_filename_component(d ${example} DIRECTORY)
  add_executable(doc_${f} EXCLUDE_FROM_ALL ${example})
  set_property(TARGET doc_${f} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${d})
  target_link_libraries(doc_${f} triqs)
  add_dependencies(docs_example_output doc_${f})
  add_custom_command(TARGET docs_example_output
   COMMAND doc_${f} > ${CMAKE_CURRENT_SOURCE_DIR}/${d}/${f}.output 2>/dev/null
   WORKING_DIRECTORY ${d}
  )
endforeach()

#---------------------------------
# Top Sphinx target
# ---------------------------------
find_package(Sphinx)

add_custom_target(docs_sphinx ALL)
add_custom_command(TARGET docs_sphinx 
                   COMMAND PYTHONPATH=${CMAKE_BINARY_DIR}:${CPP2PY_BINARY_DIR}:$ENV{PYTHONPATH} ${SPHINXBUILD_EXECUTABLE} -c . -j8 -b html ${CMAKE_CURRENT_SOURCE_DIR} html)

# Autodoc usage requires the python modules to be built first
GET_PROPERTY(CPP2PY_MODULES_LIST GLOBAL PROPERTY CPP2PY_MODULES_LIST)
add_dependencies(docs_sphinx ${CPP2PY_MODULES_LIST})

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


