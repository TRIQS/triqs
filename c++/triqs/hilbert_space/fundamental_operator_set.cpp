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
        std::string operator()(int i) const { return "i" + std::to_string(i); }
        std::string operator()(std::string const &s) const { return "s" + s; }
      };

      // decode the string
      std::variant<long, std::string> string_to_variant(std::string const &s) {
        switch (s[0]) {
          case 'i': return std::stoi(s.c_str() + 1); // the variant is an int. Skip the first char and recover the int
          case 's': return s.c_str() + 1;            // the variant is a string. Just skip the first char
          default: TRIQS_RUNTIME_ERROR << "Variant indices absent in h5 read";
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
      for(auto const& vs: vvs) vec.push_back(to_indices(vs));
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
