# Copyright Nils Wentzell 2018
# Distributed under the GNU GENERAL PUBLIC LICENSE Version 3.0.
# See accompanying file LICENSE or https://www.gnu.org/licenses/gpl-3.0.txt
#
# This cmake find module looks for the LLVM Sanitizer Runtime Librariies
# It sets up SANITIZER_RT_PRELOAD and ${COMPONENT}_RT_LIBRARY for each
# requested component.
#
# Use this module by invoking find_package with the form::
#
#   find_package(sanitizer [REQUIRED] [asan] [ubsan])
#
# Results are reported in::
#
#    SANITIZER_RT_PRELOAD 		Environment setting to load all Sanitizer Runtime Libraries
#    ${COMPONENT}_RT_LIBRARY 		Individual Sanitizer Runtime Library

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-resource-dir
    OUTPUT_VARIABLE clang_resource_dir OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(prefix clang_rt.)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(search_path ${clang_resource_dir}/lib/darwin)
    set(suffix _osx_dynamic)
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(search_path ${clang_resource_dir}/lib/linux)
    set(suffix -${CMAKE_SYSTEM_PROCESSOR})
  else()
    message(FATAL_ERROR "Unknown platform")
  endif()
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-libgcc-file-name
    OUTPUT_VARIABLE libgcc_file OUTPUT_STRIP_TRAILING_WHITESPACE)
  get_filename_component(search_path ${libgcc_file} DIRECTORY)
else()
  message(FATAL_ERROR "Sanitizer is not available for your compiler")
endif()

set(required_vars "")
foreach(component ${${CMAKE_FIND_PACKAGE_NAME}_FIND_COMPONENTS})
  string(TOUPPER ${component} COMPONENT)
  if((${component} STREQUAL "ubsan") AND (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang") AND ASAN_RT_LIBRARY)
    set(component ubsan_minimal)
  endif()

  find_library(${COMPONENT}_RT_LIBRARY
    NAMES ${prefix}${component}${suffix} ${prefix}${component}_standalone${suffix}
    PATHS ${search_path} ${search_path}/../../../)
  mark_as_advanced(${COMPONENT}_RT_LIBRARY)

  # Imported target
  add_library(lib${component}_rt SHARED IMPORTED)
  set_property(TARGET lib${component}_rt PROPERTY IMPORTED_LOCATION ${${COMPONENT}_RT_LIBRARY})

  if(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED_${component})
    list(APPEND required_vars ${COMPONENT}_RT_LIBRARY)
  endif()
  if(DEFINED sanitizer_rt_libraries)
    string(APPEND sanitizer_rt_libraries :${${COMPONENT}_RT_LIBRARY})
  else()
    set(sanitizer_rt_libraries ${${COMPONENT}_RT_LIBRARY})
  endif()
endforeach()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(preload_var DYLD_INSERT_LIBRARIES)
else()
  set(preload_var LD_PRELOAD)
endif()

set(SANITIZER_RT_PRELOAD ${preload_var}=${sanitizer_rt_libraries} CACHE INTERNAL "Runtime shared libraries needed to load the sanitizer")
mark_as_advanced(SANITIZER_RT_PRELOAD)

# ----- Create Interface Targets -----
if(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED_asan)
  add_library(asan INTERFACE)
  target_compile_options(asan INTERFACE -fsanitize=address -fno-omit-frame-pointer -ggdb3)
  target_link_libraries(asan INTERFACE "-fsanitize=address -fno-omit-frame-pointer$<$<CXX_COMPILER_ID:GNU>: -fuse-ld=gold>")
  if(NOT DEFINED ENV{ASAN_OPTIONS})
    message(WARNING "ASAN_OPTIONS is not set. Consider setting ASAN_OPTIONS=symbolize=1:detect_leaks=0 when running tests")
  endif()
endif()
if(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED_ubsan)
  add_library(ubsan INTERFACE)
  target_compile_options(ubsan INTERFACE -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-omit-frame-pointer -ggdb3)
  target_link_libraries(ubsan INTERFACE "-fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-omit-frame-pointer$<$<CXX_COMPILER_ID:GNU>: -fuse-ld=gold>")
  if(NOT DEFINED ENV{UBSAN_OPTIONS})
    message(WARNING "UBSAN_OPTIONS is not set. Consider setting UBSAN_OPTIONS=symbolize=1:print_stacktrace=1:halt_on_error=1 when running tests")
  endif()
endif()
# ------------------------------------

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("Sanitizer Runtime Libraries"
  REQUIRED_VARS SANITIZER_RT_PRELOAD ${required_vars}
  FAIL_MESSAGE "Sanitizer Runtime Libraries not found! Consider installing for additional checks!"
)
