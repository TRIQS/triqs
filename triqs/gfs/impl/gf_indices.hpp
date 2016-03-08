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
 /// one index
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


 // A simple indice struct (two indices)
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

 /// A simple indice struct (triplet)
 struct gf_indices_triplet {

  std::vector<gf_indices_one> ind_triplet;

  gf_indices_triplet() : ind_triplet(3) {}

  gf_indices_triplet(gf_indices_one r1, gf_indices_one r2, gf_indices_one r3) : ind_triplet({r1, r2, r3}) {}

  gf_indices_triplet(gf_indices_one r) : ind_triplet({r, r, r}) {}

  gf_indices_triplet(std::vector<std::vector<std::string>> _ind) : ind_triplet({_ind[0], _ind[1], _ind[2]}) {}

  /// from a shape
  gf_indices_triplet(arrays::mini_vector<int, 3> const &shape) : ind_triplet({gf_indices_one(shape[0]), gf_indices_one(shape[1]), gf_indices_one(shape[2])}) {}

  /// from a size
  gf_indices_triplet(int L) : gf_indices_triplet(arrays::mini_vector<int, 3>{L, L, L}) {}

  bool is_empty() const { return (ind_triplet[0].size() == 0) && (ind_triplet[1].size() == 0) && (ind_triplet[2].size() == 0); }

  template <typename G> bool check_size(G *g) const {
   return (is_empty() || ((ind_triplet[0].size() == get_target_shape(*g)[0]) && (ind_triplet[1].size() == get_target_shape(*g)[1]) && (ind_triplet[2].size() == get_target_shape(*g)[2])));
  }

  arrays::range convert_index(std::string const &s, int i) const { return ind_triplet[i].convert_index(s); }

  // access to one of the index list
  gf_indices_one operator[](int i) const { return ind_triplet[i]; }

  friend void h5_write(h5::group fg, std::string subgroup_name, gf_indices_triplet const &g) {
   if (g.is_empty()) return;
   auto gr = fg.create_group(subgroup_name);
   h5_write(gr, "r1", g.ind_triplet[0].ind);
   h5_write(gr, "r2", g.ind_triplet[1].ind);
   h5_write(gr, "r3", g.ind_triplet[2].ind);
  }

  friend void h5_read(h5::group fg, std::string subgroup_name, gf_indices_triplet &g) {
   h5::group gr = fg; // no default construction
   try {
    gr = fg.open_group(subgroup_name);
   }
   catch (...) {
    g = gf_indices_triplet{}; // empty, no file
    return;
   }
   h5_read(gr, "r1", g.ind_triplet[0].ind);
   h5_read(gr, "r2", g.ind_triplet[1].ind);
   h5_read(gr, "r3", g.ind_triplet[2].ind);
  }

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar &TRIQS_MAKE_NVP("ind_triplet", ind_triplet); }
 };

 inline gf_indices_triplet slice(gf_indices_triplet const &ind_triplet, arrays::range rr1, arrays::range rr2, arrays::range rr3) {
  if (ind_triplet.is_empty()) return {};
  std::vector<std::string> vr1, vr2, vr3;
  for (auto i : rr1) 
   vr1.push_back(ind_triplet[0].ind[i]);
  for (auto i : rr2) 
   vr2.push_back(ind_triplet[1].ind[i]);
  for (auto i : rr3) 
   vr3.push_back(ind_triplet[2].ind[i]);
  return {vr1, vr2, vr3};
 }

 /// A simple indice struct (tuple)
 template<int Rank>
 struct gf_indices_tuple {

  std::vector<gf_indices_one> ind_tuple;

  gf_indices_tuple() : ind_tuple(Rank) {}
  template <class T>
  gf_indices_tuple(T n) { ind_tuple.push_back(n); }
  template <class T, class... T2>
  gf_indices_tuple(T n, T2... rest): gf_indices_tuple(rest...) {
      ind_tuple.insert(ind_tuple.begin(), n);
  }

  gf_indices_tuple(gf_indices_one r) : ind_tuple(Rank, r) { }

  gf_indices_tuple(std::vector<std::vector<std::string>> const & _ind) : ind_tuple(Rank){
   for(int i=0;i<ind_tuple.size();i++) ind_tuple[i] = _ind[i];
  }

  /// from a shape
  gf_indices_tuple(arrays::mini_vector<int, Rank> const &shape) : ind_tuple(Rank){
   for(int i=0;i<ind_tuple.size();i++) ind_tuple[i] = gf_indices_one(shape[i]);
  }

  /// from a size
  gf_indices_tuple(int L) : gf_indices_tuple(arrays::mini_vector<int, Rank>{std::vector<int>{Rank, L}}) {}

  bool is_empty() const { 
   for(int i=0; i<ind_tuple.size();i++)
    if (ind_tuple[i].size() !=0) return false;
   return true;
  }

  template <typename G> bool check_size(G *g) const {
    for(int i=0; i<ind_tuple.size();i++)
     if (!(is_empty() || ((ind_tuple[i].size() == get_target_shape(*g)[i])))) return false;
    return true;
    }

  arrays::range convert_index(std::string const &s, int i) const { return ind_tuple[i].convert_index(s); }

  // access to one of the index list
  gf_indices_one operator[](int i) const { return ind_tuple[i]; }

  friend void h5_write(h5::group fg, std::string subgroup_name, gf_indices_tuple const &g) {
   if (g.is_empty()) return;
   auto gr = fg.create_group(subgroup_name);
   for(int i=0;i<g.ind_tuple.size(); i++)
    h5_write(gr, "r"+std::to_string(i), g.ind_tuple[i].ind);
  }

  friend void h5_read(h5::group fg, std::string subgroup_name, gf_indices_tuple &g) {
   h5::group gr = fg; // no default construction
   try {
    gr = fg.open_group(subgroup_name);
   }
   catch (...) {
    g = gf_indices_tuple{}; // empty, no file
    return;
   }
   for(int i=0;i<g.ind_tuple.size(); i++)
    h5_read(gr, "r"+std::to_string(i), g.ind_tuple[i].ind);
  }

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar &TRIQS_MAKE_NVP("ind_tuple", ind_tuple); }
 };
 inline gf_indices_tuple<3> slice(gf_indices_tuple<3> const &ind_tuple, arrays::range rr1, arrays::range rr2, arrays::range rr3) {
  if (ind_tuple.is_empty()) return {};
  std::vector<std::string> vr1, vr2, vr3;
  for (auto i : rr1) 
   vr1.push_back(ind_tuple[0].ind[i]);
  for (auto i : rr2) 
   vr2.push_back(ind_tuple[1].ind[i]);
  for (auto i : rr3) 
   vr3.push_back(ind_tuple[2].ind[i]);
  return {vr1, vr2, vr3};
 }
 inline gf_indices_tuple<4> slice(gf_indices_tuple<4> const &ind_tuple, arrays::range rr1, arrays::range rr2, arrays::range rr3, arrays::range rr4) {
  if (ind_tuple.is_empty()) return {};
  std::vector<std::string> vr1, vr2, vr3, vr4;
  for (auto i : rr1) 
   vr1.push_back(ind_tuple[0].ind[i]);
  for (auto i : rr2) 
   vr2.push_back(ind_tuple[1].ind[i]);
  for (auto i : rr3) 
   vr3.push_back(ind_tuple[2].ind[i]);
  for (auto i : rr4) 
   vr4.push_back(ind_tuple[3].ind[i]);
  return {vr1, vr2, vr3, vr4};
 }

}
}

