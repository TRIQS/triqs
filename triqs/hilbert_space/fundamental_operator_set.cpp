/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013, P. Seth, I. Krivenko, M. Ferrero and O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "./fundamental_operator_set.hpp"
#include <triqs/h5/vector.hpp>

namespace triqs {
namespace hilbert_space {

 using utility::variant_int_string;

 namespace { // auxiliary functions

  // a little visitor for reduction to string
  struct variant_visitor {
   char operator()(int) const { return 'i'; }
   char operator()(std::string const&) const { return 's'; }
  };

  // decode the string
  variant_int_string string_to_variant(std::string const& s) {
   switch (s[0]) {
    case 'i':
     return std::stoi(s.c_str() + 1); // the variant is an int. Skip the first char and recover the int
    case 's':
     return s.c_str() + 1; // the variant is a string. Just skip the first char
    default:
     TRIQS_RUNTIME_ERROR << "Variant indices absent in h5 read";
   }
  }

  // fundamental_operator_set --> vec vec string
  std::vector<std::vector<std::string>> to_vec_vec_string(fundamental_operator_set const& f) {
   std::vector<std::vector<std::string>> v(f.size());
   for (auto const& p : f) { // loop over the couple (indices list, number)
    if (p.linear_index >= f.size()) TRIQS_RUNTIME_ERROR << " Internal error fundamental_operator_set to vec vec string";
    for (auto& x : p.index) v[p.linear_index].push_back(apply_visitor(variant_visitor{}, x) + to_string(x));
    // variants x are transformed to a string, add 'i' or 's' in front of the string
   }
   return v;
  }

  std::vector<variant_int_string> to_indices(std::vector<std::string> const& v) {
   std::vector<variant_int_string> indices; // list of indices of this C, C^+ op
   for (auto& x : v)
    if (!x.empty()) indices.push_back(string_to_variant(x));
   return indices;
  }

 } // auxiliary functions


 // private constructor
 fundamental_operator_set::fundamental_operator_set(std::vector<std::vector<std::string>> const& v) {
  for (int n = 0; n < v.size(); ++n) {
   map_index_n.insert({to_indices(v[n]), n});
  }
 }

 // --- h5

 void h5_write_attribute(hid_t id, std::string const& name, fundamental_operator_set const& f) {
  h5::h5_write_attribute(id, name, to_vec_vec_string(f));
 }

 void h5_read_attribute(hid_t id, std::string const& name, fundamental_operator_set& f) {
  std::vector<std::vector<std::string>> fops1;
  h5::h5_read_attribute(id, name, fops1);
  f = fundamental_operator_set(fops1);
 }
}
}
