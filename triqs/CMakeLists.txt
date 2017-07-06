# Version file
configure_file(version.hpp.in version.hpp @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/version.hpp" DESTINATION "include/triqs/")

# ---------------------------------
#  Get the sources
# ---------------------------------

# all .cpp files to get the dependencies
FILE(GLOB_RECURSE ALLHEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h *.hxx)
FILE(GLOB_RECURSE ALLSOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cxx)

# remove the .mako.?pp from the build
# they are not preprocessed by cmake
foreach (TMP_PATH ${ALLHEADERS})
    string (FIND ${TMP_PATH} ".mako." EXCLUDE_DIR_FOUND)
    if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
        list (REMOVE_ITEM ALLHEADERS ${TMP_PATH})
    endif ()
endforeach()
foreach (TMP_PATH ${ALLSOURCES})
    string (FIND ${TMP_PATH} ".mako." EXCLUDE_DIR_FOUND)
    if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
        list (REMOVE_ITEM ALLSOURCES ${TMP_PATH})
    endif ()
endforeach()

# ---------------------------------
#  Define triqs target 
# ---------------------------------

add_library(triqs ${ALLSOURCES})

# first options
target_link_libraries (triqs python_and_numpy)

# ---------------------------------
# C++ standard and compiler details
# ---------------------------------

if((${CMAKE_SYSTEM_NAME} MATCHES "Darwin") AND ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
 target_compile_options(triqs PUBLIC -stdlib=libc++)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
 target_compile_definitions(triqs PUBLIC TRIQS_WORKAROUND_INTEL_COMPILER_BUGS BOOST_MATH_DISABLE_STD_FPCLASSIFY)
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
target_compile_options(triqs PUBLIC  " -DTRIQS_ARRAYS_DEBUG_COUNT_MEMORY -DTRIQS_ARRAYS_CHECK_WEAK_REFS")
endif()

# ---------------------------------
# RT on linux only
# ---------------------------------

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 target_link_libraries (triqs -lrt)
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

target_compile_options(triqs PUBLIC ${MPI_C_COMPILE_FLAGS})
target_include_directories(triqs SYSTEM PUBLIC ${MPI_C_INCLUDE_PATH})
target_link_libraries(triqs ${MPI_C_LIBRARIES}  ${MPI_CXX_LIBRARIES})

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
try_compile(_max_align_t_is_correct ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/max_align_t_test.cpp COMPILE_DEFINITIONS  -std=c++11 )
if (NOT _max_align_t_is_correct)
 MESSAGE(STATUS "Adding workaround for max_align_t bug (in clang normally)")
 target_compile_options(triqs PUBLIC TRIQS_ADD_MAX_ALIGN_T_WORKAROUND)
else ()
  MESSAGE(STATUS "Checked max_align_t. No workaround needed")
endif()

# ---------------------------------
# Check availability of <optional>
# ---------------------------------

# should be move to config.h ?

try_compile(has_optional_header ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/has_optional.cpp COMPILE_DEFINITIONS  -std=c++14 )
if (NOT has_optional_header)
 message(STATUS "Using <experimental/optional> header instead of <optional>")
else ()
 message(STATUS "<optional> header is available. No workaround needed")
 target_compile_options(triqs PUBLIC -DHAS_OPTIONAL_HEADER)
endif()

# ---------------------------------
# Lapack
# ---------------------------------

message( STATUS "-------- Lapack detection -------------")

if (NOT MKLROOT)
 if (NOT LAPACK_FOUND)
  find_package(Lapack)
 endif (NOT LAPACK_FOUND)
 if (REQUIRE_PTHREAD)
  set(LAPACK_LIBRARY "${LAPACK_LIBRARY};${PTHREAD_LIBRARY}")
  set(LAPACK_LIBRARIES "${LAPACK_LIBRARIES};${PTHREAD_LIBRARY}")
 endif (REQUIRE_PTHREAD)

 set(LAPACK_LIBS ${LAPACK_LIBRARY} ${BLAS_LIBRARY} ${LAPACK_LINKER_FLAGS} CACHE STRING "Flags to link Lapack and Blas")

 set(TRIQS_LIBRARY_LAPACK ${LAPACK_LIBS})
 if(MAC_VECLIB)
 target_link_libraries(triqs "-framework Accelerate") # ${TRIQS_LIBRARY_LAPACK}")
 endif()

else(NOT MKLROOT)
 # MKL
 # we use the LP64 mode, not IL64 mode which probably require porting the blas wrappers for MKL_INT and co
 message (STATUS "Using MKL with root : ${MKLROOT}")
 set(TRIQS_LIBRARY_LAPACK "-Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_core.a ${MKLROOT}/lib/intel64/libmkl_sequential.a -Wl,--end-group -lpthread -lm")
 if(CMAKE_COMPILER_IS_ICC)
  target_compile_options(triqs  " -I${MKLROOT}/include")
 else()
  # must be GNU
  target_compile_options(triqs " -m64 -I${MKLROOT}/include")
 endif()
endif(NOT MKLROOT)

message(STATUS "Lapack libraries : ${TRIQS_LIBRARY_LAPACK}")

#
target_link_libraries (triqs ${TRIQS_LIBRARY_LAPACK})

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
 message(FATAL_ERROR "parallel(MPI) hdf5 is detected. The standard version is preferred.")
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
 if (NOT "${CPP2PY_PYTHON_H5PY_HDF5VERSION}" STREQUAL "${HDF5_VERSION}")
 message(FATAL_ERROR "The HDF5 version of the h5py (${CPP2PY_PYTHON_H5PY_HDF5VERSION}) is different from the hdf5 library found for C++ (${HDF5_VERSION}). You MUST have the same version.")
 endif()
endif()

#
target_include_directories(triqs SYSTEM PUBLIC ${HDF5_INCLUDE_DIRS})
target_link_libraries (triqs ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES})
target_compile_options(triqs PUBLIC ${HDF5_DEFINITIONS})

# ---------------------------------
# GMP
# ---------------------------------

message( STATUS "-------- GMP detection -------------")
find_package(GMP)

#
target_include_directories(triqs SYSTEM PUBLIC ${GMP_INCLUDE_DIR})
target_link_libraries (triqs ${GMP_LIBRARIES} ${GMPXX_LIBRARIES})

# ---------------------------------
# FFTW
# ---------------------------------

message( STATUS "-------- FFTW detection -------------")
find_package(FFTW)

#
target_include_directories(triqs SYSTEM PUBLIC ${FFTW_INCLUDE_DIR})
target_link_libraries (triqs ${FFTW_LIBRARIES})

# ---------------------------------
# pthread
# ---------------------------------

# VERY OLD.
# remove the possible horrible pthread bug on os X !!( on gcc, old, before clang... is it really needed now ???)
# check for clang compiler ?? on gcc, os X snow leopard, it MUST be set
# since _REENTRANT is mysteriously set and this leads to random stalling of the code....
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 target_compile_options(triqs PUBLIC -pthread )
endif()

# ---------------------------------
# Install 
# ---------------------------------

# Install the library in lib and prepare an exported cmake file to reimport it
install(TARGETS triqs DESTINATION lib EXPORT triqs-targets)
install(EXPORT triqs-targets DESTINATION share/cmake)

# Install headers 
FOREACH(HEADER ${ALLHEADERS})
 get_filename_component(P ${HEADER} PATH)
 install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}" DESTINATION "include/triqs/${P}")
ENDFOREACH()

