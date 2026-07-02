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

# Link directories
target_link_directories(triqs PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
target_link_directories(triqs INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_FULL_LIBDIR}>)

# C++ Standard
target_compile_features(triqs PUBLIC cxx_std_23)

# Link to the Interface target for compiler warnings
target_link_libraries(triqs PRIVATE $<BUILD_INTERFACE:triqs_warnings>)

# Our own definitions are never interposed at runtime, so let the compiler call
# them directly instead of through the PLT. ELF only, Mach-O has no preemption.
target_compile_options(triqs PRIVATE
  $<$<AND:$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:GNU,Clang,IntelLLVM>>:-fno-semantic-interposition>
)

# Compile Definitions
option(CHECK_MEMORY "Turn on memory check" OFF)
target_compile_definitions(triqs
  PUBLIC
    $<$<CONFIG:Debug>:TRIQS_DEBUG>
    $<$<CONFIG:Debug>:NDA_DEBUG>
    $<$<BOOL:${CHECK_MEMORY}>:TRIQS_ARRAYS_DEBUG_COUNT_MEMORY>
    $<$<BOOL:${CHECK_MEMORY}>:TRIQS_ARRAYS_CHECK_WEAK_REFS>
)

# This choice should be up to the user..
if(CMAKE_SYSTEM_NAME MATCHES "Darwin" AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
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
target_link_libraries(triqs PRIVATE hdf5::hdf5)

# NDA (this provides blas/lapack linkeage)
target_link_libraries(triqs PUBLIC nda::nda_c)

# FMT
target_link_libraries(triqs PUBLIC fmt::fmt-header-only)

# cppdlr (discrete Lehmann representation library)
target_link_libraries(triqs PUBLIC cppdlr::cppdlr_c)

include(FetchContent)
FetchContent_Declare(
        emhash
        GIT_REPOSITORY https://github.com/DiamonDinoia/emhash.git
        GIT_TAG master
        GIT_SHALLOW TRUE)

cmake_policy(SET CMP0169 OLD)
# Only populate emhash, don't build it
FetchContent_GetProperties(emhash)
if(NOT emhash_POPULATED)
    FetchContent_Populate(emhash)
endif()

add_library(emhash INTERFACE)
target_include_directories(emhash INTERFACE $<BUILD_INTERFACE:${emhash_SOURCE_DIR}>)
target_include_directories(emhash SYSTEM INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_features(emhash INTERFACE cxx_std_17)
install(TARGETS emhash EXPORT triqs-dependencies)
target_link_libraries(triqs PUBLIC emhash)

# ---------------------------------
# Boost
# ---------------------------------

message(STATUS "-------- Boost detection -------------")
find_package(Boost 1.70 REQUIRED)
message(STATUS "Found Boost Headers: ${Boost_INCLUDE_DIRS}")
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
# Install
# ---------------------------------

# Install the library in lib and prepare an exported cmake file to reimport it
install(TARGETS triqs DESTINATION ${CMAKE_INSTALL_LIBDIR} EXPORT triqs-targets)
install(EXPORT triqs-dependencies NAMESPACE triqs:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/triqs)
install(EXPORT triqs-targets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/triqs)

# Install headers and third-party license files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.hxx" PATTERN "LICENSE*")
