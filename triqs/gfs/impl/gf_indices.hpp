/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#pragma once
#include <triqs/arrays.hpp>

namespace triqs {
namespace gfs {

 struct gf_indices_one {

  gf_indices_one() = default;

  gf_indices_one(int L) {
   for (int i = 0; i < L; ++i) ind.push_back(std::to_string(i));
  }

  gf_indices_one(std::vector<std::string> _ind) : ind(std::move(_ind)) {}

  int size() const { return ind.size(); }

  arrays::range convert_index(std::string const &s) const {
   auto b = ind.begin(), e = ind.end();
   auto it = std::find(b, e, s);
   if (it != e) return arrays::range(it - b, it - b + 1);
   TRIQS_RUNTIME_ERROR << "Cannot find this string index for the Green's function";
  }

  // the iterator on gf_indices_one just gives the vector of strings
  std::vector<std::string>::const_iterator begin() { return ind.begin(); }
  std::vector<std::string>::const_iterator end() { return ind.end(); }
  std::vector<std::string>::const_iterator cbegin() { return ind.cbegin(); }
  std::vector<std::string>::const_iterator cend() { return ind.cend(); }

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar &TRIQS_MAKE_NVP("ind", ind); }

  //private:
  std::vector<std::string> ind;
 };


 // A simple indice struct
 struct gf_indices_pair {

  std::vector<gf_indices_one> ind_pair;

  gf_indices_pair() : ind_pair(2) {}

  gf_indices_pair(gf_indices_one r, gf_indices_one l) : ind_pair({r, l}) {}

  gf_indices_pair(gf_indices_one r) : ind_pair({r, r}) {}

  gf_indices_pair(std::vector<std::vector<std::string>> _ind) : ind_pair({_ind[0], _ind[1]}) {}

  // from a shape
  gf_indices_pair(arrays::mini_vector<int, 2> const &shape) : ind_pair({gf_indices_one(shape[0]), gf_indices_one(shape[1])}) {}

  // from a size
  gf_indices_pair(int L) : gf_indices_pair(arrays::mini_vector<int, 2>{L, L}) {}

  bool is_empty() const { return (ind_pair[0].size() == 0) && (ind_pair[1].size() == 0); }

  template <typename G> bool check_size(G *g) const {
   return (is_empty() || ((ind_pair[0].size() == get_target_shape(*g)[0]) && (ind_pair[1].size() == get_target_shape(*g)[1])));
  }

  arrays::range convert_index(std::string const &s, int i) const { return ind_pair[i].convert_index(s); }

  // access to one of the index list
  gf_indices_one operator[](int i) const { return ind_pair[i]; }

  friend void h5_write(h5::group fg, std::string subgroup_name, gf_indices_pair const &g) {
   if (g.is_empty()) return;
   auto gr = fg.create_group(subgroup_name);
   h5_write(gr, "left", g.ind_pair[0].ind);
   h5_write(gr, "right", g.ind_pair[1].ind);
  }

  friend void h5_read(h5::group fg, std::string subgroup_name, gf_indices_pair &g) {
   h5::group gr = fg; // no default construction
   try {
    gr = fg.open_group(subgroup_name);
   }
   catch (...) {
    g = gf_indices_pair{}; // empty, no file
    return;
   }
   h5_read(gr, "left", g.ind_pair[0].ind);
   h5_read(gr, "right", g.ind_pair[1].ind);
  }

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar &TRIQS_MAKE_NVP("ind_pair", ind_pair); }
 };

 inline gf_indices_pair slice(gf_indices_pair const &ind_pair, arrays::range rl, arrays::range rr) {
  if (ind_pair.is_empty()) return {};
  std::vector<std::string> vl, vr;
  for (auto i : rl) {
   vl.push_back(ind_pair[0].ind[i]);
  }
  for (auto i : rr) {
   vr.push_back(ind_pair[1].ind[i]);
  }
  return {vl, vr};
 }

 inline gf_indices_pair transpose(gf_indices_pair const &x) {
  return {x.ind_pair[1], x.ind_pair[0]};
 }
}
}

