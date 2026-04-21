# install directory for Python modules
set(TRIQS_PYTHON_LIB_DEST ${CPP2PY_PYTHON_LIB_DEST_ROOT}/triqs)

# copy Python files into the build dir to have a complete package for the tests
file(GLOB_RECURSE all_py_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)
foreach(f ${all_py_files})
  configure_file(${f} ${f} COPYONLY)
endforeach()
configure_file(version.py.in version.py)
configure_file(utility/mpi.py.in utility/mpi.py)

# install Python files
set(python_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  ${CMAKE_CURRENT_BINARY_DIR}/version.py
)
install (FILES ${python_sources} DESTINATION ${TRIQS_PYTHON_LIB_DEST})

# ---------------------------------------------

add_subdirectory(atom_diag)
add_subdirectory(dos)
add_subdirectory(fit)
add_subdirectory(gfs)
add_subdirectory(mesh)
add_subdirectory(gf)
add_subdirectory(lattice)
add_subdirectory(operators)
add_subdirectory(plot)
add_subdirectory(mc_tools)
add_subdirectory(random_generator)
add_subdirectory(stat)
add_subdirectory(sumk)
add_subdirectory(utility)

# ---------------------------------------------

# fetch c2py
include(FetchContent)
FetchContent_Declare(
  c2py
  GIT_REPOSITORY  https://github.com/flatironinstitute/c2py
  GIT_TAG         unstable
  EXCLUDE_FROM_ALL # exclude from all so that c2py is NOT installed.
)
FetchContent_MakeAvailable(c2py)
include(${c2py_SOURCE_DIR}/share/cmake/clair_c2py_generate_bindings.cmake)

# user option to regenerate the Python bindings
option(Update_Python_Bindings "Optionally update the C++ Python bindings in the source directory using clair-c2py" ON)
if(Update_Python_Bindings)
  find_program(clair-c2py clair-c2py REQUIRED)
  message(STATUS "clair-c2py found at ${clair-c2py}")
endif()

# add an interface target for all Python modules
add_library(${PROJECT_NAME}_python_modules INTERFACE)

# build and install all Python modules
file(GLOB_RECURSE module_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(module_src ${module_sources})
  get_filename_component(module_name ${module_src} NAME_WE)
  get_filename_component(module_dir ${module_src} DIRECTORY)

  # add Python module
  Python_add_library(${module_name} MODULE ${module_src})
  add_library(${PROJECT_NAME}::${module_name} ALIAS ${module_name})
  add_dependencies(${PROJECT_NAME}_python_modules ${module_name})

  # target properties
  target_link_libraries(${module_name} PRIVATE triqs c2py::c2py)
  # Python module TUs activate C2PY_INCLUDED (via <c2py/c2py.hpp>), which in turn
  # pulls triqs/<mod>/<name>.wrap.hxx from python/ via the triqs/ umbrella headers.
  # Scope the python/ include to these modules instead of exposing it on triqs itself.
  target_include_directories(${module_name} PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/python>)
  set_target_properties(${module_name} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${module_dir}
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
  )

  # install the Python module
  install(TARGETS ${module_name} DESTINATION ${TRIQS_PYTHON_LIB_DEST}/${module_dir})

  # optionally generate Python bindings with clair-c2py
  if(Update_Python_Bindings)
    set(wrap_cxx "${CMAKE_CURRENT_SOURCE_DIR}/${module_name}.wrap.cxx")
    set(depfile "${CMAKE_CURRENT_BINARY_DIR}/${module_name}.cpp.d")

    add_custom_command(
      OUTPUT ${wrap_cxx} ${depfile}
      COMMAND ${clair-c2py} -p ${PROJECT_BINARY_DIR} --generate-depfile ${depfile} ${module_src}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPFILE ${depfile}
    )

    add_custom_target(wrap_gen_${module_name} DEPENDS ${wrap_cxx})
    add_dependencies(${module_name} wrap_gen_${module_name})
  endif()
endforeach()
