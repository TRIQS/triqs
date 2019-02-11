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

target_compile_options(triqs PUBLIC -std=c++17)

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

message(STATUS "-------- MPI detection -------------")
# We use MPI in C only, not the C++ bindings.
find_package(MPI)

if(NOT MPI_C_FOUND)
 message(FATAL_ERROR "TRIQS requires MPI")
endif()
message(STATUS "MPI C compiler : ${MPI_C_COMPILER}")
message(STATUS "MPI_COMPILE_FLAGS : ${MPI_C_COMPILE_FLAGS} ")
message(STATUS "MPI_C_INCLUDE_PATH : ${MPI_C_INCLUDE_PATH}")
message(STATUS "MPI_C_LIBRARIES : ${MPI_C_LIBRARIES}")
message(STATUS "MPI_CXX_LIBRARIES : ${MPI_CXX_LIBRARIES}")

# FIXME : we should not link to CXX lib, but openmpi uses them by default apparently on Linux

separate_arguments(MPI_C_COMPILE_FLAGS) # Convert to list

add_library(mpi INTERFACE)
target_include_directories(mpi SYSTEM INTERFACE ${MPI_C_INCLUDE_PATH})
target_link_libraries(mpi INTERFACE ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
target_compile_options(mpi INTERFACE ${MPI_C_COMPILE_FLAGS})

target_link_libraries(triqs PUBLIC mpi)
target_include_directories(triqs SYSTEM PUBLIC ${MPI_C_INCLUDE_PATH})
install(TARGETS mpi EXPORT triqs-dependencies)

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

message(STATUS "-------- Boost detection -------------")

if(TRIQS_BUILD_STATIC)
 set(Boost_USE_STATIC_LIBS ON)
else()
 set(Boost_USE_STATIC_LIBS OFF)
endif()

find_package(Boost 1.53 REQUIRED COMPONENTS) # mpi serialization system
if(NOT Boost_FOUND)
 message(FATAL_ERROR "Boost not found")
endif()
mark_as_advanced(Boost_DIR)
message(STATUS  "Boost include dir: ${Boost_INCLUDE_DIRS}")
message(STATUS  "Boost libraries: ${Boost_LIBRARIES}")

add_library(boost INTERFACE)
target_include_directories(boost SYSTEM INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})

target_link_libraries(triqs PUBLIC boost)
target_include_directories(triqs SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
install(TARGETS boost EXPORT triqs-dependencies)

# ---------------------------------
# max_align_t bug detection
# ---------------------------------

message(STATUS "-------- Misc -------------")

# should be move to config.h ?
try_compile(_max_align_t_is_correct ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/max_align_t_test.cpp COMPILE_DEFINITIONS -std=c++11)
if(NOT _max_align_t_is_correct)
 MESSAGE(STATUS "Adding workaround for max_align_t bug (in clang normally)")
 target_compile_definitions(triqs PUBLIC TRIQS_ADD_MAX_ALIGN_T_WORKAROUND)
else ()
  MESSAGE(STATUS "Checked max_align_t. No workaround needed")
endif()

# ---------------------------------
# Lapack
# ---------------------------------

message(STATUS "-------- Lapack detection -------------")

find_package(LAPACK REQUIRED)

message(STATUS "Lapack libraries : ${LAPACK_LIBRARIES}")
message(STATUS "Lapack vendor : ${BLA_VENDOR}")

list(REMOVE_DUPLICATES LAPACK_LIBRARIES)
add_library(blas_lapack INTERFACE)
target_link_libraries(blas_lapack INTERFACE ${LAPACK_LIBRARIES})
target_compile_options(blas_lapack INTERFACE ${LAPACK_LINKER_FLAGS})

target_link_libraries(triqs PUBLIC blas_lapack)
install(TARGETS blas_lapack EXPORT triqs-dependencies)

# ---------------------------------
# HDF5
# ---------------------------------

message(STATUS "-------- HDF5 detection -------------")

# on weiss, it is 2.8.2 and we should not put HL, on 12.04 we need to put it...
if(${CMAKE_VERSION} VERSION_LESS "2.8.6") # CHECK THIS BOUND, where are the cmake changelogs ??
 find_package(HDF5 REQUIRED C)
else(${CMAKE_VERSION} VERSION_LESS "2.8.6")
 find_package(HDF5 REQUIRED C HL)
endif(${CMAKE_VERSION} VERSION_LESS "2.8.6")

if(NOT HDF5_FOUND)
 message(FATAL_ERROR "Require hdf5 1.8.2 or higher. Set HDF5_HOME")
endif()

if(HDF5_IS_PARALLEL)
 message(FATAL_ERROR "parallel(MPI) hdf5 is detected. The standard version is preferred.")
endif(HDF5_IS_PARALLEL)

if(HDF5_HL_LIBRARIES)    # CMake 3.6.0 and later puts libhdf5_hl into a separate variable
 list(APPEND HDF5_LIBRARIES ${HDF5_HL_LIBRARIES})
 list(REMOVE_DUPLICATES HDF5_LIBRARIES)
endif(HDF5_HL_LIBRARIES)

message(STATUS "HDF5_LIBRARIES = ${HDF5_LIBRARIES}")

# Get hdf5 version and add a definition HDF5_LT_189 if version < 1.8.9
execute_process(COMMAND ${HDF5_DIFF_EXECUTABLE} --version OUTPUT_VARIABLE diff_output)
string(REGEX MATCHALL "[0-9]+.[0-9]+.[0-9]+" HDF5_VERSION "${diff_output}")
message(STATUS "hdf5 version: ${HDF5_VERSION}")
if(PythonSupport)
 if(NOT "${PYTHON_H5PY_HDF5VERSION}" STREQUAL "${HDF5_VERSION}")
 message(FATAL_ERROR "The HDF5 version of the h5py (${PYTHON_H5PY_HDF5VERSION}) is different from the hdf5 library found for C++ (${HDF5_VERSION}). You MUST have the same version.")
 endif()
endif()

add_library(hdf5 INTERFACE)
target_include_directories(hdf5 SYSTEM INTERFACE ${HDF5_INCLUDE_DIRS})
target_link_libraries(hdf5 INTERFACE ${HDF5_LIBRARIES})
target_compile_options(hdf5 INTERFACE ${HDF5_DEFINITIONS})

target_link_libraries(triqs PUBLIC hdf5)
target_include_directories(triqs SYSTEM PUBLIC ${HDF5_INCLUDE_DIRS})
install(TARGETS hdf5 EXPORT triqs-dependencies)

# ---------------------------------
# GMP
# ---------------------------------

message(STATUS "-------- GMP detection -------------")
find_package(GMP)

target_include_directories(triqs SYSTEM PUBLIC ${GMP_INCLUDE_DIR})
target_link_libraries (triqs PUBLIC ${GMP_LIBRARIES} ${GMPXX_LIBRARIES})

# ---------------------------------
# FFTW
# ---------------------------------

message(STATUS "-------- FFTW detection -------------")
find_package(FFTW)

set(TRIQS_FFTW_LIBRARIES ${FFTW_LIBRARIES} CACHE FILEPATH "FFTW Library used for the compilation of triqs.")
set(TRIQS_FFTW_INCLUDE_DIR ${FFTW_INCLUDE_DIR} CACHE FILEPATH "FFTW Library used for the compilation of triqs.")

target_link_libraries(triqs PUBLIC fftw)
target_include_directories(triqs SYSTEM PUBLIC ${FFTW_INCLUDE_DIR})
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

set(sanitizers "")
# Address Sanitizer
option(ASAN OFF "Compile library and executables with LLVM Address Sanitizer")
if(ASAN)
  list(APPEND sanitizers asan)
  target_compile_options(triqs PUBLIC -fsanitize=address -fno-omit-frame-pointer)
  target_link_libraries(triqs INTERFACE "-fsanitize=address -fno-omit-frame-pointer")
  if(NOT DEFINED ENV{ASAN_OPTIONS})
    message(WARNING "ASAN_OPTIONS is not set. Consider setting ASAN_OPTIONS=symbolize=1:detect_leaks=0 when running tests")
  endif()
endif()

# Undefined Behavior Sanitizer
option(UBSAN OFF "Compile library and executables with LLVM Undefined Behavior Sanitizer")
if(UBSAN)
  list(APPEND sanitizers ubsan)
  target_compile_options(triqs PUBLIC -fsanitize=undefined -fno-omit-frame-pointer -fno-sanitize=vptr)
  target_link_libraries(triqs INTERFACE "-fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-omit-frame-pointer")
  if(NOT DEFINED ENV{UBSAN_OPTIONS})
    message(WARNING "UBSAN_OPTIONS is not set. Consider setting UBSAN_OPTIONS=symbolize=1:print_stacktrace=1:halt_on_error=1 when running tests")
  endif()
endif()

if(sanitizers)
  find_package(sanitizer REQUIRED ${sanitizers})
  if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_link_libraries(triqs INTERFACE "-fuse-ld=gold")
  endif()
endif()

# ---------------------------------
# Install 
# ---------------------------------

# Add directory to installed headers to public interface of triqs triqs target
target_include_directories(triqs SYSTEM INTERFACE ${CMAKE_INSTALL_PREFIX}/include)

# Install the library in lib and prepare an exported cmake file to reimport it
install(TARGETS triqs DESTINATION lib EXPORT triqs-targets)
install(EXPORT triqs-dependencies NAMESPACE triqs:: DESTINATION lib/cmake/triqs)
install(EXPORT triqs-targets DESTINATION lib/cmake/triqs)

# Install headers 
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.hxx" PATTERN "*.mako.*" EXCLUDE)
