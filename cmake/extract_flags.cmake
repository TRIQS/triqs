# Copyright (c) 2019-2020 Simons Foundation
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
# Author: Nils Wentzell

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
macro(extract_flags)

  cmake_parse_arguments(ARG "BUILD_INTERFACE" "" "" ${ARGN})

  set(target ${ARGV0})
  unset(${target}_CXXFLAGS)
  unset(${target}_LDFLAGS)

  get_property_recursive(opts TARGET ${target} PROPERTY INTERFACE_COMPILE_OPTIONS)
  foreach(opt ${opts})
    set(${target}_LDFLAGS "${${target}_LDFLAGS} ${opt}")
    set(${target}_CXXFLAGS "${${target}_CXXFLAGS} ${opt}")
  endforeach()

  get_property_recursive(cxx_features TARGET ${target} PROPERTY INTERFACE_COMPILE_FEATURES)
  if(cxx_std_23 IN_LIST cxx_features)
    set(${target}_CXXFLAGS "${${target}_CXXFLAGS} -std=c++2b")
  elseif(cxx_std_20 IN_LIST cxx_features)
    set(${target}_CXXFLAGS "${${target}_CXXFLAGS} -std=c++20")
  elseif(cxx_std_17 IN_LIST cxx_features)
    set(${target}_CXXFLAGS "${${target}_CXXFLAGS} -std=c++17")
  endif()

  get_property_recursive(defs TARGET ${target} PROPERTY INTERFACE_COMPILE_DEFINITIONS)
  foreach(def ${defs})
    set(${target}_CXXFLAGS "${${target}_CXXFLAGS} -D${def}")
  endforeach()

  get_property_recursive(inc_dirs TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
  get_property_recursive(sys_inc_dirs TARGET ${target} PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
  if(inc_dirs)
    list(REMOVE_ITEM sys_inc_dirs ${inc_dirs})
  endif()
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

  get_property_recursive(libdirs TARGET ${target} PROPERTY INTERFACE_LINK_DIRECTORIES)
  foreach(dir ${libdirs})
    set(${target}_LDFLAGS "${${target}_LDFLAGS} -L${dir}")
  endforeach()

  # ==== We have to replace generator expressions explicitly ====

  if(ARG_BUILD_INTERFACE)
    string(REGEX REPLACE "\\$<BUILD_INTERFACE:([^ ]*)>" "\\1" ${target}_LDFLAGS "${${target}_LDFLAGS}")
    string(REGEX REPLACE "\\$<BUILD_INTERFACE:([^ ]*)>" "\\1" ${target}_CXXFLAGS "${${target}_CXXFLAGS}")
  else()
    string(REGEX REPLACE "\\$<INSTALL_INTERFACE:([^ ]*)>" "\\1" ${target}_LDFLAGS "${${target}_LDFLAGS}")
    string(REGEX REPLACE "\\$<INSTALL_INTERFACE:([^ ]*)>" "\\1" ${target}_CXXFLAGS "${${target}_CXXFLAGS}")
  endif()

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    string(REGEX REPLACE "\\$<\\$<CXX_COMPILER_ID:GNU>:([^ ]*)>" "\\1" ${target}_LDFLAGS "${${target}_LDFLAGS}")
    string(REGEX REPLACE "\\$<\\$<CXX_COMPILER_ID:GNU>:([^ ]*)>" "\\1" ${target}_CXXFLAGS "${${target}_CXXFLAGS}")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    string(REGEX REPLACE "\\$<\\$<CXX_COMPILER_ID:Clang>:([^ ]*)>" "\\1" ${target}_LDFLAGS "${${target}_LDFLAGS}")
    string(REGEX REPLACE "\\$<\\$<CXX_COMPILER_ID:Clang>:([^ ]*)>" "\\1" ${target}_CXXFLAGS "${${target}_CXXFLAGS}")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    string(REGEX REPLACE "\\$<\\$<CXX_COMPILER_ID:AppleClang>:([^ ]*)>" "\\1" ${target}_LDFLAGS "${${target}_LDFLAGS}")
    string(REGEX REPLACE "\\$<\\$<CXX_COMPILER_ID:AppleClang>:([^ ]*)>" "\\1" ${target}_CXXFLAGS "${${target}_CXXFLAGS}")
  endif()

  # Remove all remaining generator expressions
  string(REGEX REPLACE " [^ ]*\\$<[^ ]*:[^ ]*>" "" ${target}_LDFLAGS "${${target}_LDFLAGS}")
  string(REGEX REPLACE " [^ ]*\\$<[^ ]*:[^ ]*>" "" ${target}_CXXFLAGS "${${target}_CXXFLAGS}")

  # Filter out ::@ expressions
  string(REGEX REPLACE "::@[^ ]* " "" ${target}_LDFLAGS "${${target}_LDFLAGS}")
  string(REGEX REPLACE "::@[^ ]* " "" ${target}_CXXFLAGS "${${target}_CXXFLAGS}")

  # Filter out system directories from LDFLAGS and CXXFLAGS
  string(REGEX REPLACE " -L/usr/lib " " " ${target}_LDFLAGS "${${target}_LDFLAGS}")
  string(REGEX REPLACE " -I/usr/include " " " ${target}_CXXFLAGS "${${target}_CXXFLAGS}")
  string(REGEX REPLACE " -isystem/usr/include " " " ${target}_CXXFLAGS "${${target}_CXXFLAGS}")

endmacro()
