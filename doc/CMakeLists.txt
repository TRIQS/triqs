# ------------------------------------------------------------------------------------------------
# Exportation of local Sphinx extensions ...
# ------------------------------------------------------------------------------------------------

install(DIRECTORY sphinxext DESTINATION "share/triqs")

# ------------------------------------------------------------------------------------------------
# Generate the conf.py
# ------------------------------------------------------------------------------------------------
#
set(TRIQS_SOURCE_DIR "${PROJECT_SOURCE_DIR}")

# MathJax is mandatory
find_package(MathJax REQUIRED)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sphinxext DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# -----------------------------------------------------------------------------
# Create an optional target that allows us to regenerate the C++ doc with c++2rst
# -----------------------------------------------------------------------------
# list of namespaces selected by c++2rst
set(NAMESPACES
    -N triqs::gfs
    -N triqs::det_manip
    -N triqs::mc_tools
    -N triqs::hilbert_space
    -N triqs::atom_diag
    -N triqs::stat
    -N mpi
    -N itertools
)

# Caution! This target will generate documentation into the sourcetree at doc/documentation/cpp_api
add_custom_target(docs_cpp2rst)
extract_flags(triqs BUILD_INTERFACE)
separate_arguments(triqs_CXXFLAGS)
add_custom_command(
  TARGET docs_cpp2rst POST_BUILD
  COMMAND rm -rf ${CMAKE_CURRENT_SOURCE_DIR}/documentation/cpp_api
  COMMAND
    PYTHONPATH=${CPP2PY_BINARY_DIR}:$ENV{PYTHONPATH}
    ${CPP2PY_BINARY_DIR}/bin/c++2rst
    ${CMAKE_CURRENT_SOURCE_DIR}/documentation/api_root.hpp
    ${NAMESPACES}
    --output_directory ${CMAKE_CURRENT_SOURCE_DIR}/documentation/cpp_api
    -I${PROJECT_SOURCE_DIR}
    -isystem=${PROJECT_SOURCE_DIR}/cpp2py/include
    --cxxflags="${triqs_CXXFLAGS}"
)

# --------------------------------------------------------
# Build & Run the C++ doc examples and capture the output
# --------------------------------------------------------

add_custom_target(docs_example_output)
file(GLOB_RECURSE ExampleList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(example ${ExampleList})
  get_filename_component(f ${example} NAME_WE)
  get_filename_component(d ${example} DIRECTORY)
  get_filename_component(pd ${d} NAME_WE)
  set(bin doc_${pd}_${f})
  add_executable(${bin} EXCLUDE_FROM_ALL ${example})
  set_property(TARGET ${bin} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${d})
  target_link_libraries(${bin} triqs triqs_warnings)
  add_custom_command(TARGET ${bin} POST_BUILD
   COMMAND ${bin} > ${CMAKE_CURRENT_SOURCE_DIR}/${d}/${f}.output
   WORKING_DIRECTORY ${d}
  )
  add_dependencies(docs_example_output ${bin})
endforeach()

# --------------------------------------------------------
# Clone the triqs tutorials with the output included
# --------------------------------------------------------

add_custom_target(docs_tutorial_notebooks)
add_custom_command(
  TARGET docs_tutorial_notebooks POST_BUILD
  COMMAND rm -rf ${CMAKE_CURRENT_SOURCE_DIR}/userguide/python/tutorials &&
  ${GIT_EXECUTABLE} clone --depth 1 --branch unstable_executed https://github.com/triqs/tutorials ${CMAKE_CURRENT_SOURCE_DIR}/userguide/python/tutorials
)

# ---------------------------------
# Top Sphinx target
# ---------------------------------
find_package(Sphinx)

# Sphinx has internal caching, always run it
add_custom_target(docs_sphinx ALL)
add_custom_command(
  TARGET docs_sphinx POST_BUILD
  COMMAND PYTHONPATH=${PROJECT_BINARY_DIR}/python:${CPP2PY_BINARY_DIR}:${h5_BINARY_DIR}/python:$ENV{PYTHONPATH} ${SPHINXBUILD_EXECUTABLE} -c . -j auto -b html ${CMAKE_CURRENT_SOURCE_DIR} html
)

option(Sphinx_Only "When building the documentation, skip the Python Modules and the generation of C++ Api and example outputs" OFF)
if(NOT Sphinx_Only)
  # Autodoc usage requires the python modules to be built first
  get_property(CPP2PY_MODULES_LIST GLOBAL PROPERTY CPP2PY_MODULES_LIST)
  add_dependencies(docs_sphinx ${CPP2PY_MODULES_LIST})

  # Generation of C++ Api and Example Outputs
  add_dependencies(docs_sphinx docs_cpp2rst docs_example_output docs_tutorial_notebooks)
endif()

# ---------------------------------
# Install
# ---------------------------------
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ COMPONENT documentation DESTINATION share/doc/triqs
  FILES_MATCHING
  REGEX "\\.(html|pdf|png|gif|jpg|svg|ico|js|xsl|css|py|txt|inv|bib|ttf|woff2|eot|sh)$"
  PATTERN "_*"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/themes/agogo DESTINATION share/triqs/themes)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/themes/triqs DESTINATION share/triqs/themes)
