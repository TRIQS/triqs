# Version file
configure_file(version.hpp.in version.hpp @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/version.hpp" DESTINATION "include/triqs/")

# ---------------------------------
#  Get the sources
# ---------------------------------

# all .cpp files to get the dependencies
#DEBUG
file(GLOB_RECURSE ALLSOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cxx)

# ---------------------------------
#  Define and configure the triqs target 
# ---------------------------------

add_library(triqs ${ALLSOURCES} ${CMAKE_CURRENT_BINARY_DIR}/git_hash.cpp)
set_property(TARGET triqs PROPERTY VERSION ${TRIQS_VERSION_MAJOR}.${TRIQS_VERSION_MINOR})

# Include dirs
target_include_directories(triqs PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/c++>)
target_include_directories(triqs SYSTEM INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_FULL_INCLUDEDIR}>)

# C++ Standard
target_compile_features(triqs PUBLIC cxx_std_23)

# Link to the Interface target for compiler warnings
target_link_libraries(triqs PRIVATE $<BUILD_INTERFACE:triqs_warnings>)

# Compile Definitions
option(CHECK_MEMORY "Turn on memory check" OFF)
target_compile_definitions(triqs
  PUBLIC
    $<$<CONFIG:Debug,RelWithDebInfo>:TRIQS_DEBUG>
    $<$<CONFIG:Debug,RelWithDebInfo>:NDA_DEBUG>
    $<$<BOOL:${CHECK_MEMORY}>:TRIQS_ARRAYS_DEBUG_COUNT_MEMORY>
    $<$<BOOL:${CHECK_MEMORY}>:TRIQS_ARRAYS_CHECK_WEAK_REFS>
)

# This choice should be up to the user..
if((${CMAKE_SYSTEM_NAME} MATCHES "Darwin") AND ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
 target_compile_options(triqs PUBLIC -stdlib=libc++)
endif()

# enforce color diagnostics ?
# gcc : -fdiagnostics-color
# clang :  -fcolor-diagnostics 

# ---------------------------------
# Link Dependencies
# ---------------------------------

# RT on linux only
target_link_libraries(triqs PUBLIC $<$<PLATFORM_ID:Linux>:-lrt>)

# Itertools
target_link_libraries(triqs PUBLIC itertools::itertools_c)

# MPI Interface
target_link_libraries(triqs PUBLIC mpi::mpi_c)

# H5 Interface
target_link_libraries(triqs PUBLIC h5::h5_c)

# HDF5 (still used explicitly in many_body_operator)
target_link_libraries(triqs PRIVATE h5::hdf5)

# NDA (this provides blas/lapack linkeage)
target_link_libraries(triqs PUBLIC nda::nda_c)

# FMT
target_link_libraries(triqs PUBLIC fmt::fmt-header-only)

# cppdlr (discrete Lehmann representation library)
target_link_libraries(triqs PUBLIC cppdlr::cppdlr_c)

# ---------------------------------
# Boost
# ---------------------------------

message(STATUS "-------- Boost detection -------------")
find_package(Boost 1.70 REQUIRED)
target_link_libraries(triqs PUBLIC Boost::headers)

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
target_compile_options(triqs PUBLIC $<$<PLATFORM_ID:Darwin>:-pthread>)

# ---------------------------------
# max_align_t bug detection
# ---------------------------------

message(STATUS "-------- Misc -------------")

# should be move to config.h ?
try_compile(_max_align_t_is_correct ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/cmake/max_align_t_test.cpp COMPILE_DEFINITIONS -std=c++11)
if(NOT _max_align_t_is_correct)
  message(STATUS "Adding workaround for max_align_t bug (in clang normally)")
  target_compile_definitions(triqs PUBLIC TRIQS_ADD_MAX_ALIGN_T_WORKAROUND)
else()
  message(STATUS "Checked max_align_t. No workaround needed")
endif()

# ---------------------------------
# Dynamic Analyzer Checks
# ---------------------------------

# Sanitizer Options already defined in NDA

if(ASAN)
  target_link_libraries(triqs PUBLIC $<BUILD_INTERFACE:asan>)
endif()
if(UBSAN)
  target_link_libraries(triqs PUBLIC $<BUILD_INTERFACE:ubsan>)
endif()
if(MSAN)
  target_link_libraries(triqs PUBLIC $<BUILD_INTERFACE:msan>)
endif()
if(TSAN)
  target_link_libraries(triqs PUBLIC $<BUILD_INTERFACE:tsan>)
endif()

# ---------------------------------
# Generate Compiler Wrapper
# ---------------------------------

# Get the compiler and linktime flags of triqs and python_and_numpy
include(${PROJECT_SOURCE_DIR}/cmake/extract_flags.cmake)
extract_flags(triqs)
if(PythonSupport)
 extract_flags(cpp2py::python_and_numpy)
endif()

# Generate compiler wrapper
configure_file(triqs++.in triqs++ @ONLY)

# ---------------------------------
# Install 
# ---------------------------------

# Install the library in lib and prepare an exported cmake file to reimport it
install(TARGETS triqs DESTINATION ${CMAKE_INSTALL_LIBDIR} EXPORT triqs-targets)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/triqs++ PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE DESTINATION ${CMAKE_INSTALL_BINDIR})
install(EXPORT triqs-dependencies NAMESPACE triqs:: DESTINATION lib/cmake/triqs)
install(EXPORT triqs-targets DESTINATION lib/cmake/triqs)

# Install headers 
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.hxx" PATTERN "*.mako.*" EXCLUDE)
