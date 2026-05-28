# install directory for Python modules
set(TRIQS_PYTHON_LIB_DEST ${CPP2PY_PYTHON_LIB_DEST_ROOT}/triqs)

# copy Python files into the build dir to have a complete package for the tests
file(GLOB_RECURSE all_py_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)
foreach(f ${all_py_files})
  configure_file(${f} ${f} COPYONLY)
endforeach()
configure_file(version.py.in version.py)
configure_file(utility/mpi.py.in utility/mpi.py)

# install all source Python files under triqs/ (recursive; skips generator inputs)
install(DIRECTORY . DESTINATION ${TRIQS_PYTHON_LIB_DEST}
        FILES_MATCHING PATTERN "*.py"
        PATTERN "*.inst.py" EXCLUDE)

# install generated Python files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.py DESTINATION ${TRIQS_PYTHON_LIB_DEST})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/utility/mpi.py DESTINATION ${TRIQS_PYTHON_LIB_DEST}/utility)

# install generated headers consumed under #ifdef C2PY_INCLUDED by the triqs/<mod>.hpp umbrella headers.
install(DIRECTORY . DESTINATION include/triqs
        FILES_MATCHING PATTERN "*.wrap.hxx"
        PATTERN "wrapped_aux" EXCLUDE)

# ---------------------------------------------

# fetch c2py (provides c2py_add_module)
include(FetchContent)
FetchContent_Declare(
  c2py
  GIT_REPOSITORY  https://github.com/flatironinstitute/c2py
  GIT_TAG         unstable
  EXCLUDE_FROM_ALL # exclude from all so that c2py is NOT installed.
)
FetchContent_MakeAvailable(c2py)

# add an interface target for all Python modules
add_library(${PROJECT_NAME}_python_modules INTERFACE)

# Build/install one Python module.
# Optionally declare bindings-generation dependencies between modules.
#
# Usage: add_triqs_python_module(<name> DIR <relative-dir>
#                                [DEPENDS_ON_BINDINGS m1 m2 ...])
macro(add_triqs_python_module module_name)
  cmake_parse_arguments(_ARG "" "DIR" "DEPENDS_ON_BINDINGS" ${ARGN})

  c2py_add_module(${module_name}
    DIR ${_ARG_DIR}
    LINK_LIBRARIES triqs
    DEPENDS_ON_BINDINGS ${_ARG_DEPENDS_ON_BINDINGS})
  add_library(${PROJECT_NAME}::${module_name} ALIAS ${module_name})
  add_dependencies(${PROJECT_NAME}_python_modules ${module_name})
  # Python module TUs activate C2PY_INCLUDED (via <c2py/c2py.hpp>), which in turn
  # pulls triqs/<mod>/<name>.wrap.hxx from python/ via the triqs/ umbrella headers.
  # Scope the python/ include to these modules instead of exposing it on triqs itself.
  target_include_directories(${module_name} PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/python>)
  install(TARGETS ${module_name} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/${_ARG_DIR})
endmacro()

# external dependencies for the binding generation targets
set(external_deps)
if(TARGET _h5py)
  list(APPEND external_deps _h5py)
endif()

# basic modules that can be built before all other modules
set(basic_mods meshes lattice_tools operators)
add_triqs_python_module(meshes DIR mesh DEPENDS_ON_BINDINGS ${external_deps})
add_triqs_python_module(operators DIR operators DEPENDS_ON_BINDINGS ${external_deps})
add_triqs_python_module(lattice_tools DIR lattice DEPENDS_ON_BINDINGS meshes ${external_deps})

# build/install the rest of the modules
file(GLOB_RECURSE module_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(module_src ${module_sources})
  get_filename_component(module_name ${module_src} NAME_WE)
  get_filename_component(module_dir ${module_src} DIRECTORY)
  if(module_name IN_LIST basic_mods)
    continue()
  endif()
  add_triqs_python_module(${module_name} DIR ${module_dir} DEPENDS_ON_BINDINGS ${basic_mods} ${external_deps})
endforeach()
