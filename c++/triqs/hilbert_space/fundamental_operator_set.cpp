// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
// Copyright (c) 2015 Igor Krivenko
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
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Implementation details for triqs/hilbert_space/fundamental_operator_set.hpp.
 */

#include "./fundamental_operator_set.hpp"
#include "../utility/exceptions.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <h5/h5.hpp>
#include <itertools/itertools.hpp>

#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace triqs::hilbert_space {

  namespace {

    // A little visitor for reduction to string.
    struct variant_visitor {
      std::string operator()(long i) const { return "i" + std::to_string(i); }
      std::string operator()(std::string const &s) const { return "s" + s; }
      std::string operator()(double d) const { return "d" + std::to_string(d); }
      std::string operator()(std::array<long, 3> const &a) const {
        return "idx" + std::to_string(a[0]) + "," + std::to_string(a[1]) + "," + std::to_string(a[2]);
      }
    };

    // Decode the string.
    std::variant<long, std::string, double, std::array<long, 3>> string_to_variant(std::string const &s) {
      if (s.substr(0, 3) == "idx") {
        // Parse array: "idx0,1,2" -> {0, 1, 2}
        std::array<long, 3> arr{};
        auto rest   = s.substr(3);
        size_t pos1 = rest.find(',');
        size_t pos2 = rest.find(',', pos1 + 1);
        arr[0]      = std::stol(rest.substr(0, pos1));
        arr[1]      = std::stol(rest.substr(pos1 + 1, pos2 - pos1 - 1));
        arr[2]      = std::stol(rest.substr(pos2 + 1));
        return arr;
      }
      switch (s[0]) {
        case 'i': return std::stol(s.c_str() + 1);   // the variant is a long. Skip the first char and recover the long
        case 's': return std::string(s.c_str() + 1); // the variant is a string. Just skip the first char
        case 'd': return std::stod(s.c_str() + 1);   // the variant is a double. Skip the first char and recover the double
        default: TRIQS_RUNTIME_ERROR << "Unknown variant type prefix in h5 read: " << s[0];
      }
    }

    // Turn a fundamental_operator_set into a vector of vector of strings.
    auto to_vec_vec_string(fundamental_operator_set const &fops) {
      std::vector<std::vector<std::string>> vvs(fops.size());
      for (auto const &[n, alpha] : itertools::enumerate(fops.data())) {
        for (auto const &beta : alpha) vvs[n].push_back(visit(variant_visitor{}, beta));
      }
      return vvs;
    }

    // Turn a vector of strings into an operator index.
    fundamental_operator_set::indices_t to_indices(std::vector<std::string> const &v) {
      fundamental_operator_set::indices_t alpha;
      for (auto &beta : v)
        if (!beta.empty()) alpha.push_back(string_to_variant(beta));
      return alpha;
    }

  } // anonymous namespace

  std::string format_indices(indices_t const &alpha, std::string_view sep, std::string_view prefix, std::string_view suffix) {
    // early return for empty indices
    if (alpha.empty()) return fmt::format("{}{}", prefix, suffix);

    // converts a single variant element to its string representation (strings get single quotes)
    auto idx_to_str = [](auto const &x) {
      return std::visit(
         [](auto const &v) -> std::string {
           if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>)
             return fmt::format("'{}'", v);
           else
             return fmt::format("{}", v);
         },
         x);
    };

    // build the final string
    std::string str = fmt::format("{}{}", prefix, idx_to_str(alpha.front()));
    for (int i = 1; i < alpha.size(); ++i) str += fmt::format("{}{}", sep, idx_to_str(alpha[i]));
    str += fmt::format("{}", suffix);
    return str;
  }

  fundamental_operator_set::fundamental_operator_set(std::vector<std::vector<std::string>> const &vvs) {
    for (auto const &vs : vvs) idxs_.push_back(to_indices(vs));
  }

  void h5_write_attribute(h5::object obj, std::string const &name, fundamental_operator_set const &fops) {
    h5::write_attribute(obj, name, to_vec_vec_string(fops));
  }

  void h5_read_attribute(h5::object obj, std::string const &name, fundamental_operator_set &fops) {
    std::vector<std::vector<std::string>> vvs;
    h5::read_attribute(obj, name, vvs);
    fops = fundamental_operator_set(vvs);
  }

} // namespace triqs::hilbert_space
