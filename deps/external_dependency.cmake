# Copyright (c) 2020 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https://www.gnu.org/licenses/gpl-3.0.txt


# Consider ROOT env variables in find_package
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

# Make sure that imported targets are always global
get_property(IMPORTED_ALWAYS_GLOBAL GLOBAL PROPERTY IMPORTED_ALWAYS_GLOBAL)
if(NOT IMPORTED_ALWAYS_GLOBAL)
  function(add_library)
    set(_args ${ARGN})
    if ("${_args}" MATCHES ";IMPORTED")
        list(APPEND _args GLOBAL)
    endif()
    _add_library(${_args})
  endfunction()
  set_property(GLOBAL PROPERTY IMPORTED_ALWAYS_GLOBAL TRUE)
endif()

# Define External Dependency Function
function(external_dependency)
  cmake_parse_arguments(ARG "EXCLUDE_FROM_ALL;BUILD_ALWAYS" "VERSION;GIT_REPO;GIT_TAG" "" ${ARGN})

  # -- Was dependency already found?
  get_property(${ARGV0}_FOUND GLOBAL PROPERTY ${ARGV0}_FOUND)
  if(${ARGV0}_FOUND)
    message(STATUS "Dependency ${ARGV0} was already resolved.")
    return()
  endif()

  # -- Try to find package in system.
  if(NOT ARG_BUILD_ALWAYS AND NOT Build_Deps STREQUAL "Always")
    find_package(${ARGV0} ${ARG_VERSION} QUIET HINTS ${CMAKE_INSTALL_PREFIX})
    if(${ARGV0}_FOUND)
      message(STATUS "Found dependency ${ARGV0} in system ${${ARGV0}_ROOT}")
      return()
    elseif(Build_Deps STREQUAL "Never")
      message(FATAL_ERROR "Could not find dependency ${ARGV0} in system. Please install the dependency manually or use -DBuild_Deps=IfNotFound during cmake configuration to automatically build all dependencies that are not found.")
    endif()
  endif()

  # -- Build package from source
  message(STATUS " =============== Configuring Dependency ${ARGV0} =============== ")
  if(ARG_EXCLUDE_FROM_ALL)
    set(subdir_opts EXCLUDE_FROM_ALL)
    set(Build_Tests OFF)
    set(Build_Documentation OFF)
  endif()
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0})
    message(STATUS "Found sources for dependency ${ARGV0} at ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}")
    add_subdirectory(${ARGV0} ${subdir_opts})
  elseif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/deps/${ARGV0})
    message(STATUS "Found sources for dependency ${ARGV0} at ${CMAKE_SOURCE_DIR}/deps/${ARGV0}")
    add_subdirectory(${ARGV0} ${subdir_opts})
  elseif(ARG_GIT_REPO)
    set(bin_dir ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0})
    set(src_dir ${bin_dir}_src)
    if(NOT IS_DIRECTORY ${src_dir})
      if(ARG_GIT_TAG)
        set(clone_opts --branch ${ARG_GIT_TAG} -c advice.detachedHead=false)
      endif()
      if(NOT GIT_EXECUTABLE)
        find_package(Git REQUIRED)
      endif()
      execute_process(COMMAND ${GIT_EXECUTABLE} clone ${ARG_GIT_REPO} --depth 1 ${clone_opts} ${src_dir}
        RESULT_VARIABLE clone_failed
        ERROR_VARIABLE clone_error
      )
      if(clone_failed)
        message(FATAL_ERROR "Failed to clone sources for dependency ${ARGV0}.\n ${clone_error}")
      endif()
    endif()
    add_subdirectory(${src_dir} ${bin_dir} ${subdir_opts})
  else()
    message(FATAL_ERROR "Could not find or build dependency ${ARGV0}")
  endif()
  message(STATUS " =============== End ${ARGV0} Configuration =============== ")
  set_property(GLOBAL PROPERTY ${ARGV0}_FOUND TRUE)

endfunction()
