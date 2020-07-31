// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Olivier Parcollet, Nils Wentzell

#pragma once
#include <string>
#include <typeinfo>

namespace triqs {
  namespace utility {

    std::string demangle(const char *name);
    std::string demangle(std::string const &name);
    std::string get_name(std::type_info const &info);

    template <typename T> std::string typeid_name(T const &A) { return get_name(typeid(A)); }

    template <typename T> std::string typeid_name() { return get_name(typeid(T)); }
    //std::string typeid_name() { return get_name(typeid(std::declval<T>()));}

  } // namespace utility
} // namespace triqs
