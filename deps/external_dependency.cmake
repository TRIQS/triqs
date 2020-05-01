###################################################################################
#
# APP4TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2020 Simons Foundation
#    authors: N. Wentzell
#
# APP4TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# APP4TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# APP4TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
###################################################################################

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
    find_package(${ARGV0} ${${ARGV0}_VERSION} QUIET HINTS ${CMAKE_INSTALL_PREFIX})
    if(${ARGV0}_FOUND)
      message(STATUS "Found dependency ${ARGV0} in system")
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
  endif()
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0})
    message(STATUS "Found sources for dependency ${ARGV0} at ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}")
    add_subdirectory(${ARGV0} ${subdir_opts})
  elseif(ARG_GIT_REPO)
    set(bin_dir ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0})
    set(src_dir ${bin_dir}_src)
    if(NOT IS_DIRECTORY ${src_dir})
      if(ARG_GIT_TAG)
	set(clone_opts --branch ${ARG_GIT_TAG} -c advice.detachedHead=false)
      endif()
      execute_process(COMMAND git clone ${ARG_GIT_REPO} --depth 1 ${clone_opts} ${src_dir})
    endif()
    add_subdirectory(${src_dir} ${bin_dir} ${subdir_opts})
  else()
    message(FATAL_ERROR "Could not find or build dependency ${ARGV0}")
  endif()
  message(STATUS " =============== End ${ARGV0} Configuration =============== ")
  set_property(GLOBAL PROPERTY ${ARGV0}_FOUND TRUE)

endfunction()
