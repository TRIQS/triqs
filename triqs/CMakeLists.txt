# Consider ROOT env variables in find_package
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

# Version file
configure_file(version.hpp.in version.hpp @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/version.hpp" DESTINATION "include/triqs/")

# ---------------------------------
#  Get the sources
# ---------------------------------

# all .cpp files to get the dependencies
FILE(GLOB_RECURSE ALLSOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cxx)

# ---------------------------------
#  Define triqs target 
# ---------------------------------

add_library(triqs ${ALLSOURCES} ${CMAKE_CURRENT_BINARY_DIR}/git_hash.cpp)
set_property(TARGET triqs PROPERTY VERSION ${TRIQS_VERSION_MAJOR}.${TRIQS_VERSION_MINOR})

# first options
target_link_libraries(triqs PRIVATE cpp2py python_and_numpy)

# ---------------------------------
# C++ standard and compiler details
# ---------------------------------

target_compile_options(triqs PUBLIC
  -std=c++17
  $<$<CXX_COMPILER_ID:GNU>:-fconcepts>
)

if((${CMAKE_SYSTEM_NAME} MATCHES "Darwin") AND ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
 target_compile_options(triqs PUBLIC -stdlib=libc++)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  target_compile_definitions(triqs PRIVATE TRIQS_DEBUG NDA_DEBUG)
endif()

# enforce color diagnostics ?
# gcc : -fdiagnostics-color
# clang :  -fcolor-diagnostics 

# ---------------------------------
# Turn the memory check
# These options must be activated for 
# the lib AND any code using it or they will give false positive
# hence PUBLIC
# ---------------------------------

option(CHECK_MEMORY "Turn on memory check" OFF)
if(CHECK_MEMORY)
target_compile_definitions(triqs PUBLIC TRIQS_ARRAYS_DEBUG_COUNT_MEMORY TRIQS_ARRAYS_CHECK_WEAK_REFS)
endif()

# ---------------------------------
# RT on linux only
# ---------------------------------

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 target_link_libraries (triqs PUBLIC -lrt)
endif()

# ---------------------------------
# MPI
# ---------------------------------
target_link_libraries(triqs PUBLIC mpi_interface)

# ---------------------------------
# HDF5
# ---------------------------------
target_link_libraries(triqs PUBLIC h5_interface)

# ---------------------------------
# Itertools 
# ---------------------------------
target_link_libraries(triqs PUBLIC itertools_c)

# ---------------------------------
# Boost
# ---------------------------------

message(STATUS "-------- Boost detection -------------")

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.53 REQUIRED COMPONENTS)
message(STATUS "Boost include dir: ${Boost_INCLUDE_DIRS}")

add_library(boost INTERFACE)
target_include_directories(boost SYSTEM INTERFACE ${Boost_INCLUDE_DIRS})

target_link_libraries(triqs PUBLIC boost)
install(TARGETS boost EXPORT triqs-dependencies)

# ---------------------------------
# max_align_t bug detection
# ---------------------------------

message(STATUS "-------- Misc -------------")

# should be move to config.h ?
try_compile(_max_align_t_is_correct ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/cmake/max_align_t_test.cpp COMPILE_DEFINITIONS -std=c++11)
if(NOT _max_align_t_is_correct)
 MESSAGE(STATUS "Adding workaround for max_align_t bug (in clang normally)")
 target_compile_definitions(triqs PUBLIC TRIQS_ADD_MAX_ALIGN_T_WORKAROUND)
else ()
  MESSAGE(STATUS "Checked max_align_t. No workaround needed")
endif()

# ---------------------------------
# Lapack
# ---------------------------------
target_link_libraries(triqs PUBLIC blas_lapack)
#install(TARGETS blas_lapack EXPORT triqs-dependencies)

# ---------------------------------
# GMP
# ---------------------------------

message(STATUS "-------- GMP detection -------------")
find_package(GMP)

target_link_libraries(triqs PUBLIC gmp)
install(TARGETS gmp EXPORT triqs-dependencies)

# ---------------------------------
# FFTW
# ---------------------------------

message(STATUS "-------- FFTW detection -------------")
find_package(FFTW)

target_link_libraries(triqs PUBLIC fftw)
install(TARGETS fftw EXPORT triqs-dependencies)

# ---------------------------------
# pthread
# ---------------------------------

# VERY OLD.
# remove the possible horrible pthread bug on os X !!( on gcc, old, before clang... is it really needed now ???)
# check for clang compiler ?? on gcc, os X snow leopard, it MUST be set
# since _REENTRANT is mysteriously set and this leads to random stalling of the code....
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 target_compile_options(triqs PUBLIC -pthread)
endif()

# ---------------------------------
# Dynamic Analyzer Checks
# ---------------------------------

# Sanitizer Options already defined in NDA

if(ASAN)
  target_link_libraries(triqs PUBLIC asan)
endif()
if(UBSAN)
  target_link_libraries(triqs PUBLIC ubsan)
endif()

if(MSAN)
  target_compile_options(triqs PUBLIC -fsanitize=memory -fno-omit-frame-pointer -fsanitize-recover=all -fsanitize-memory-track-origins=2 -O1 -ggdb3)
  target_link_libraries(triqs PUBLIC "-fsanitize=memory -fno-omit-frame-pointer -fsanitize-recover=all -fsanitize-memory-track-origins=2")
endif()

# ---------------------------------
# Triqs header include directories
# ---------------------------------

# Add the directories where the triqs headers are located to the triqs target interface
target_include_directories(triqs INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>)
target_include_directories(triqs SYSTEM INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)


# --------------------------------------------------------
# Some helper functions for the extraction of target info
# --------------------------------------------------------

# Recursively fetch all targets that the interface of a target depends upon
macro(get_all_interface_targets name target)
  get_property(TARGET_LINK_LIBRARIES TARGET ${target} PROPERTY INTERFACE_LINK_LIBRARIES)
  foreach(lib IN LISTS TARGET_LINK_LIBRARIES)
    if(TARGET ${lib})
      # Append to list
      list(APPEND ${name}_INTERFACE_TARGETS ${lib})
      # Recure into target dependencies
      get_all_interface_targets(${name} ${lib})
    endif()
  endforeach()
endmacro()

# Extract the property from the target and recursively from all targets it depends upon
macro(get_property_recursive)
  cmake_parse_arguments(get_property_recursive "" "TARGET" "PROPERTY" ${ARGN})
  set(target ${get_property_recursive_TARGET})
  set(property ${get_property_recursive_PROPERTY})
  get_all_interface_targets(${target} ${target})
  foreach(t IN LISTS ${target}_INTERFACE_TARGETS ITEMS ${target})
    get_property(p TARGET ${t} PROPERTY ${property})
    list(APPEND ${ARGV0} ${p})
  endforeach()
  # Clean duplicates and any occurance of '/usr/include' dirs
  if(${ARGV0})
    list(REMOVE_DUPLICATES ${ARGV0})
    list(REMOVE_ITEM ${ARGV0} /usr/include)
  endif()
endmacro()

# Recursively fetch all compiler flags attached to the interface of a target
macro(extract_flags target)

  get_property_recursive(opts TARGET ${target} PROPERTY INTERFACE_COMPILE_OPTIONS)
  foreach(opt ${opts})
    set(${target}_LDFLAGS "${${target}_LDFLAGS} ${opt}")
    set(${target}_CXXFLAGS "${${target}_CXXFLAGS} ${opt}")
  endforeach()

  get_property_recursive(defs TARGET ${target} PROPERTY INTERFACE_COMPILE_DEFINITIONS)
  foreach(def ${defs})
    set(${target}_CXXFLAGS "${${target}_CXXFLAGS} -D${def}")
  endforeach()

  get_property_recursive(inc_dirs TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
  get_property_recursive(sys_inc_dirs TARGET ${target} PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
  list(REMOVE_ITEM inc_dirs ${sys_inc_dirs})
  foreach(dir ${inc_dirs})
    set(${target}_CXXFLAGS "${${target}_CXXFLAGS} -I${dir}")
  endforeach()
  foreach(dir ${sys_inc_dirs})
    set(${target}_CXXFLAGS "${${target}_CXXFLAGS} -isystem${dir}")
  endforeach()

  get_property_recursive(libs TARGET ${target} PROPERTY INTERFACE_LINK_LIBRARIES)
  foreach(lib ${libs})
    if(NOT TARGET ${lib} AND NOT IS_DIRECTORY ${lib})
      set(${target}_LDFLAGS "${${target}_LDFLAGS} ${lib}")
    endif()
  endforeach()

  # We have to replace generator expressions explicitly
  string(REGEX REPLACE "\\$<INSTALL_INTERFACE:([^ ]*)>" "\\1" ${target}_LDFLAGS "${${target}_LDFLAGS}")
  string(REGEX REPLACE "\\$<INSTALL_INTERFACE:([^ ]*)>" "\\1" ${target}_CXXFLAGS "${${target}_CXXFLAGS}")
  string(REGEX REPLACE " [^ ]*\\$<[^ ]*:[^>]*>" "" ${target}_LDFLAGS "${${target}_LDFLAGS}")
  string(REGEX REPLACE " [^ ]*\\$<[^ ]*:[^>]*>" "" ${target}_CXXFLAGS "${${target}_CXXFLAGS}")
endmacro()

# ---------------------------------
# Generate Compiler Wrapper
# ---------------------------------

# Get the compiler and linktime flags of triqs and python_and_numpy
extract_flags(triqs)
extract_flags(python_and_numpy)

# Generate compiler wrapper
configure_file(triqs++.in triqs++ @ONLY)

# ---------------------------------
# Install 
# ---------------------------------

# Install the library in lib and prepare an exported cmake file to reimport it
install(TARGETS triqs DESTINATION lib EXPORT triqs-targets)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/triqs++ PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE DESTINATION bin)
install(EXPORT triqs-dependencies NAMESPACE triqs:: DESTINATION lib/cmake/triqs)
install(EXPORT triqs-targets DESTINATION lib/cmake/triqs)

# Install headers 
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.hxx" PATTERN "*.mako.*" EXCLUDE)
