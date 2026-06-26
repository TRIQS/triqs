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

#include "./first_include.hpp"
#include "./typeid_name.hpp"

#include <boost/algorithm/string/erase.hpp>

#include <sstream>
#include <string>
#include <typeinfo>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

namespace triqs::utility {

  std::string demangle(const char *name) {
    std::stringstream fs;
#ifdef __GNUC__
    int status{};
    char *demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    if (!status) {
      std::string res(demangled);
      boost::erase_all(res, ", boost::tuples::null_type");
      boost::erase_all(res, ", -1");
      boost::erase_all(res, ", void");
      fs << res;
      free(demangled); // NOLINT
    } else
      fs << name;
#else
    fs << name;
#endif
    return fs.str();
  }

  std::string demangle(std::string const &name) { return demangle(name.c_str()); }

  std::string get_name(std::type_info const &info) { return demangle(info.name()); }

} // namespace triqs::utility
