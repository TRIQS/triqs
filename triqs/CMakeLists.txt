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

add_library(triqs ${ALLSOURCES})

# first options
target_link_libraries(triqs PUBLIC cpp2py python_and_numpy)

# ---------------------------------
# C++ standard and compiler details
# ---------------------------------

if((${CMAKE_SYSTEM_NAME} MATCHES "Darwin") AND ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
 target_compile_options(triqs PUBLIC -stdlib=libc++)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_compile_options(triqs PUBLIC -fconcepts)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
 target_compile_definitions(triqs PRIVATE TRIQS_DEBUG)
endif()

# enforce color diagnostics ?
# gcc : -fdiagnostics-color
# clang :  -fcolor-diagnostics 

# ?? still necessary ?
#if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
# target_compile_options(triqs PRIVATE "-undefined dynamic_lookup")
#endif()

# ---------------------------------
# Turn the memory check
# These options must be activated for 
# the lib AND any code using it or they will give false positive
# hence PUBLIC
# ---------------------------------

option(CHECK_MEMORY "Turn on memory check" OFF)
if (CHECK_MEMORY)
target_compile_definitions(triqs PUBLIC TRIQS_ARRAYS_DEBUG_COUNT_MEMORY TRIQS_ARRAYS_CHECK_WEAK_REFS)
endif()

# ---------------------------------
# RT on linux only
# ---------------------------------

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 target_link_libraries (triqs PUBLIC -lrt)
endif()

# ---------------------------------
# MPI
# ---------------------------------

message( STATUS "-------- MPI detection -------------")
# We use MPI in C only, not the C++ bindings.
find_package(MPI)

if (NOT MPI_C_FOUND)
 message(FATAL_ERROR "TRIQS requires MPI")
endif()
message(STATUS "MPI C compiler : ${MPI_C_COMPILER}")
message(STATUS "MPI_COMPILE_FLAGS : ${MPI_C_COMPILE_FLAGS} ")
message(STATUS "MPI_C_INCLUDE_PATH : ${MPI_C_INCLUDE_PATH}")
message(STATUS "MPI_C_LIBRARIES : ${MPI_C_LIBRARIES}")
message(STATUS "MPI_CXX_LIBRARIES : ${MPI_CXX_LIBRARIES}")

# FIXME : we should not link to CXX lib, but openmpi uses them by default apparently on Linux

separate_arguments(MPI_C_COMPILE_FLAGS) # Convert to list
target_compile_options(triqs PUBLIC ${MPI_C_COMPILE_FLAGS})
target_include_directories(triqs SYSTEM PUBLIC ${MPI_C_INCLUDE_PATH})
target_link_libraries(triqs PUBLIC ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
if(NOT MPIEXEC_EXECUTABLE) # Backward compatibility for older FindMPI.cmake
  set(MPIEXEC_EXECUTABLE ${MPIEXEC} CACHE FILENAME "MPI Executable")
endif()
# Compatibility to Open-MPI 3.0.0: check whether MPI executable has option --oversubscribe and add it 
execute_process(COMMAND ${MPIEXEC_EXECUTABLE} --oversubscribe ${MPIEXEC_NUMPROC_FLAG} 4 ${MPIEXEC_PREFLAGS} ls ${MPIEXEC_POSTFLAGS} RESULT_VARIABLE HAS_NO_OVERSUBSCRIBE OUTPUT_QUIET ERROR_QUIET)
if(NOT HAS_NO_OVERSUBSCRIBE)
  list(APPEND MPIEXEC_PREFLAGS --oversubscribe)
  set(MPIEXEC_PREFLAGS ${MPIEXEC_PREFLAGS} CACHE STRING "These flags will be directly before the executable that is being run by mpiexec." FORCE)
endif()

# ---------------------------------
# Boost
# ---------------------------------

message( STATUS "-------- Boost detection -------------")

if(TRIQS_BUILD_STATIC)
 set(Boost_USE_STATIC_LIBS ON)
else()
 set(Boost_USE_STATIC_LIBS OFF)
endif()

find_package(Boost 1.53 REQUIRED COMPONENTS) # mpi serialization system
if (NOT Boost_FOUND)
 message(FATAL_ERROR "Boost not found")
endif()
mark_as_advanced(Boost_DIR)
message(STATUS  "Boost include dir: ${Boost_INCLUDE_DIRS}")
message(STATUS  "Boost libraries: ${Boost_LIBRARIES}")

target_include_directories(triqs SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

# ---------------------------------
# max_align_t bug detection
# ---------------------------------
message( STATUS "-------- Misc -------------")

# should be move to config.h ?
try_compile(_max_align_t_is_correct ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/max_align_t_test.cpp COMPILE_DEFINITIONS -std=c++11)
if (NOT _max_align_t_is_correct)
 MESSAGE(STATUS "Adding workaround for max_align_t bug (in clang normally)")
 target_compile_definitions(triqs PUBLIC TRIQS_ADD_MAX_ALIGN_T_WORKAROUND)
else ()
  MESSAGE(STATUS "Checked max_align_t. No workaround needed")
endif()

# ---------------------------------
# Lapack
# ---------------------------------

message( STATUS "-------- Lapack detection -------------")

find_package(LAPACK REQUIRED)

message(STATUS "Lapack libraries : ${LAPACK_LIBRARIES}")
message(STATUS "Lapack vendor : ${BLA_VENDOR}")

target_link_libraries (triqs PUBLIC ${LAPACK_LIBRARIES})
target_compile_options (triqs PUBLIC ${LAPACK_LINKER_FLAGS})

# ---------------------------------
# HDF5
# ---------------------------------

message( STATUS "-------- HDF5 detection -------------")

# on weiss, it is 2.8.2 and we should not put HL, on 12.04 we need to put it...
if ( ${CMAKE_VERSION} VERSION_LESS "2.8.6") # CHECK THIS BOUND, where are the cmake changelogs ??
 find_package(HDF5 REQUIRED C )
else(${CMAKE_VERSION} VERSION_LESS "2.8.6")
 find_package(HDF5 REQUIRED C HL)
endif(${CMAKE_VERSION} VERSION_LESS "2.8.6")

if(NOT HDF5_FOUND)
 message(FATAL_ERROR "Require hdf5 1.8.2 or higher. Set HDF5_HOME")
endif()

if(HDF5_IS_PARALLEL)
  message(WARNING "parallel(MPI) hdf5 is detected. The standard version is preferred.")
endif(HDF5_IS_PARALLEL)

if(HDF5_HL_LIBRARIES)    # CMake 3.6.0 and later puts libhdf5_hl into a separate variable
 list(APPEND HDF5_LIBRARIES ${HDF5_HL_LIBRARIES})
 list(REMOVE_DUPLICATES HDF5_LIBRARIES)
endif(HDF5_HL_LIBRARIES)

message(STATUS " HDF5_LIBRARIES = ${HDF5_LIBRARIES}")
mark_as_advanced(HDF5_DIR) # defined somewhere else ? what is it ?

# Get hdf5 version and add a definition HDF5_LT_189 if version < 1.8.9
execute_process(COMMAND ${HDF5_DIFF_EXECUTABLE} --version OUTPUT_VARIABLE diff_output)
string(REGEX MATCHALL "[0-9]+.[0-9]+.[0-9]+" HDF5_VERSION "${diff_output}")
message(STATUS "hdf5 version: ${HDF5_VERSION}")
if (PythonSupport)
 if (NOT "${PYTHON_H5PY_HDF5VERSION}" STREQUAL "${HDF5_VERSION}")
 message(FATAL_ERROR "The HDF5 version of the h5py (${PYTHON_H5PY_HDF5VERSION}) is different from the hdf5 library found for C++ (${HDF5_VERSION}). You MUST have the same version.")
 endif()
endif()

#
target_include_directories(triqs SYSTEM PUBLIC ${HDF5_INCLUDE_DIRS})
target_link_libraries (triqs PUBLIC ${HDF5_LIBRARIES})
target_compile_options(triqs PUBLIC ${HDF5_DEFINITIONS})

# ---------------------------------
# GMP
# ---------------------------------

message( STATUS "-------- GMP detection -------------")
find_package(GMP)

#
target_include_directories(triqs SYSTEM PUBLIC ${GMP_INCLUDE_DIR})
target_link_libraries (triqs PUBLIC ${GMP_LIBRARIES} ${GMPXX_LIBRARIES})

# ---------------------------------
# FFTW
# ---------------------------------

message( STATUS "-------- FFTW detection -------------")
find_package(FFTW)

#
target_include_directories(triqs SYSTEM PUBLIC ${FFTW_INCLUDE_DIR})
target_link_libraries (triqs PUBLIC ${FFTW_LIBRARIES})

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
# Install 
# ---------------------------------

# Add directory to installed headers to public interface of triqs triqs target
target_include_directories(triqs SYSTEM INTERFACE ${CMAKE_INSTALL_PREFIX}/include)

# Install the library in lib and prepare an exported cmake file to reimport it
install(TARGETS triqs DESTINATION lib EXPORT triqs-targets)
install(EXPORT triqs-targets DESTINATION lib/cmake/triqs)

# Install headers 
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.hxx" PATTERN "*.mako.*" EXCLUDE)
