// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Nils Wentzell

#include <variant>

namespace std {

  // == ostream operator<< for variant and vector of variant

  template <typename T1, typename... T> std::ostream &operator<<(std::ostream &os, std::variant<T1, T...> const &v) {
    visit([&os](auto const &x) { os << x; }, v);
    return os;
  }

  inline std::ostream &operator<<(std::ostream &os, std::vector<std::variant<long, std::string>> const &fs) {
    int u = 0;
    for (auto const &i : fs) {
      if (u++) os << ",";
      os << i;
    }
    return os;
  }

  // == Make std::to_string available for both string and variant

  inline string to_string(string const &str) { return str; }
  template <typename T1, typename T2> inline string to_string(variant<T1, T2> const &var) {
    stringstream ss;
    ss << var;
    return ss.str();
  }

} // namespace std
