# ------------------------------------------------------------------------------
# Build the doxygen C++ examples.
#
# Each *.cpp in this directory is compiled as a standalone executable linked
# against the triqs library. The examples are pulled into the doxygen
# documentation via the EXAMPLE_PATH / @include mechanism (see Doxyfile.in).
# Compiling them keeps the in-docs examples from rotting when APIs change.
# ------------------------------------------------------------------------------

file(GLOB ExampleList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(example ${ExampleList})
  get_filename_component(f ${example} NAME_WE)
  set(bin doc_example_${f})
  add_executable(${bin} EXCLUDE_FROM_ALL ${example})
  set_property(TARGET ${bin} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(${bin} triqs triqs_warnings)
  add_dependencies(docs_example_output ${bin})
endforeach()
