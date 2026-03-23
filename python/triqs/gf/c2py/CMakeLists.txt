set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetch the c2py library
FetchContent_Declare(c2py
  GIT_REPOSITORY https://github.com/flatironinstitute/c2py
  GIT_TAG unstable
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(c2py)
include(${c2py_SOURCE_DIR}/share/cmake/clair_c2py_generate_bindings.cmake)  

# List of Python C++ extension modules
set(c2py_modules
  wrapped_aux
)

# Build the Python C++ extension modules
foreach(module ${c2py_modules})
  Python_add_library(${module}_c2py MODULE ${module}_c2py.cpp)
  target_link_libraries(${module}_c2py PRIVATE triqs c2py::c2py)
endforeach()

# -----------------------------------------------
# Optionally regenerate bindings with clair-c2py
# -----------------------------------------------

option(Update_Python_Bindings "Use clair python bindings generators" ON)

if (Update_Python_Bindings)
  foreach(module ${c2py_modules})
    clair_c2py_generate_bindings(${module}_c2py)
  endforeach()
endif()