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

#include "./fundamental_operator_set.hpp"

namespace triqs {
  namespace hilbert_space {

    namespace { // auxiliary functions

      // a little visitor for reduction to string
      struct variant_visitor {
        std::string operator()(long i) const { return "i" + std::to_string(i); }
        std::string operator()(std::string const &s) const { return "s" + s; }
        std::string operator()(double d) const { return "d" + std::to_string(d); }
        std::string operator()(std::array<long, 3> const &a) const {
          return "idx" + std::to_string(a[0]) + "," + std::to_string(a[1]) + "," + std::to_string(a[2]);
        }
      };

      // decode the string
      std::variant<long, std::string, double, std::array<long, 3>> string_to_variant(std::string const &s) {
        if (s.substr(0, 3) == "idx") {
          // Parse array: "idx0,1,2" -> {0, 1, 2}
          std::array<long, 3> arr{};
          auto rest = s.substr(3);
          size_t pos1 = rest.find(',');
          size_t pos2 = rest.find(',', pos1 + 1);
          arr[0] = std::stol(rest.substr(0, pos1));
          arr[1] = std::stol(rest.substr(pos1 + 1, pos2 - pos1 - 1));
          arr[2] = std::stol(rest.substr(pos2 + 1));
          return arr;
        }
        switch (s[0]) {
          case 'i': return std::stol(s.c_str() + 1); // the variant is a long. Skip the first char and recover the long
          case 's': return std::string(s.c_str() + 1); // the variant is a string. Just skip the first char
          case 'd': return std::stod(s.c_str() + 1); // the variant is a double. Skip the first char and recover the double
          default: TRIQS_RUNTIME_ERROR << "Unknown variant type prefix in h5 read: " << s[0];
        }
      }

      // fundamental_operator_set --> vec vec string
      std::vector<std::vector<std::string>> to_vec_vec_string(fundamental_operator_set const &f) {
        std::vector<std::vector<std::string>> v(f.size());
        for (auto const &p : f) { // loop over the couple (indices list, number)
          if (p.linear_index >= f.size()) TRIQS_RUNTIME_ERROR << " Internal error fundamental_operator_set to vec vec string";
          for (auto &x : p.index) v[p.linear_index].push_back(visit(variant_visitor{}, x));
          // variants x are transformed to a string, add 'i' or 's' in front of the string
        }
        return v;
      }

      fundamental_operator_set::indices_t to_indices(std::vector<std::string> const &v) {
        fundamental_operator_set::indices_t indices; // list of indices of this C, C^+ op
        for (auto &x : v)
          if (!x.empty()) indices.push_back(string_to_variant(x));
        return indices;
      }

    } // namespace

    // private constructor
    fundamental_operator_set::fundamental_operator_set(std::vector<std::vector<std::string>> const &vvs) {
      for (auto const &vs : vvs) idxs_.push_back(to_indices(vs));
    }

    // --- h5
    void h5_write_attribute(h5::object obj, std::string const &name, fundamental_operator_set const &f) {
      h5::write_attribute(obj, name, to_vec_vec_string(f));
    }

    void h5_read_attribute(h5::object obj, std::string const &name, fundamental_operator_set &f) {
      std::vector<std::vector<std::string>> fops1;
      h5::read_attribute(obj, name, fops1);
      f = fundamental_operator_set(fops1);
    }
  } // namespace hilbert_space
} // namespace triqs
