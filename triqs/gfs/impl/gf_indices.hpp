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
  template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar & ind; }

  //private:
  std::vector<std::string> ind;
 };

 template<int Rank> struct gf_indices_tuple;

 template<int Rank>
  void h5_write(h5::group fg, std::string subgroup_name, gf_indices_tuple<Rank> const &g) {
   if (g.is_empty()) return;
   auto gr = fg.create_group(subgroup_name);
   for(int i=0;i<g.ind_vec.size(); i++)
    h5_write(gr, "r"+std::to_string(i), g.ind_vec[i].ind);
  }
 template<int Rank>
  void h5_read(h5::group fg, std::string subgroup_name, gf_indices_tuple<Rank> &g) {
   h5::group gr = fg; // no default construction
   try {
    gr = fg.open_group(subgroup_name);
   }
   catch (...) {
    g = gf_indices_tuple<Rank>{}; // empty, no file
    return;
   }
   for(int i=0;i<g.ind_vec.size(); i++)
    h5_read(gr, "r"+std::to_string(i), g.ind_vec[i].ind);
  } 
  ///specialization for Rank=2 (backward compatibility)
  template<>
  void h5_write(h5::group fg, std::string subgroup_name, gf_indices_tuple<2> const &g) ;

  ///specialization for Rank=2 (backward compatibility)
  template<>
  void h5_read(h5::group fg, std::string subgroup_name, gf_indices_tuple<2> &g) ;


 /// A simple indice struct (tuple)
 template<int Rank>
 struct gf_indices_tuple {

  std::vector<gf_indices_one> ind_vec;

  gf_indices_tuple() : ind_vec(Rank) {}
  template <class T>
  gf_indices_tuple(T n) { ind_vec.push_back(n); }
  template <class T, class... T2>
  gf_indices_tuple(T n, T2... rest): gf_indices_tuple(rest...) {
      ind_vec.insert(ind_vec.begin(), n);
  }

  gf_indices_tuple(gf_indices_one r) : ind_vec(Rank, r) { }

  gf_indices_tuple(std::vector<std::vector<std::string>> const & _ind) : ind_vec(_ind.size()){
   for(int i=0;i<ind_vec.size();i++) ind_vec[i] = _ind[i];
  }

  /// from a shape
  gf_indices_tuple(arrays::mini_vector<int, Rank> const &shape) : ind_vec(Rank){
   for(int i=0;i<ind_vec.size();i++) ind_vec[i] = gf_indices_one(shape[i]);
  }

  /// from a size
  gf_indices_tuple(int L) : gf_indices_tuple(arrays::mini_vector<int, Rank>{std::vector<int>{Rank, L}}) {}

  bool is_empty() const { 
   for(int i=0; i<ind_vec.size();i++)
    if (ind_vec[i].size() !=0) return false;
   return true;
  }

  template <typename G> bool check_size(G *g) const {
    for(int i=0; i<ind_vec.size();i++)
     if (!(is_empty() || ((ind_vec[i].size() == get_target_shape(*g)[i])))) return false;
    return true;
    }

  arrays::range convert_index(std::string const &s, int i) const { return ind_vec[i].convert_index(s); }

  // access to one of the index list
  gf_indices_one operator[](int i) const { return ind_vec[i]; }

  friend void h5_write<>(h5::group fg, std::string subgroup_name, gf_indices_tuple<Rank> const &g) ;
  friend void h5_read<>(h5::group fg, std::string subgroup_name, gf_indices_tuple<Rank> &g) ;

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar & ind_vec; }
 };


 inline gf_indices_tuple<2> transpose_indices(gf_indices_tuple<2> const &x) {
  return {std::vector<std::vector<std::string>>{x.ind_vec[1].ind, x.ind_vec[0].ind}};
 }

 inline gf_indices_tuple<2> slice(gf_indices_tuple<2> const &ind_vec, arrays::range rr1, arrays::range rr2) {
  if (ind_vec.is_empty()) return {};
  std::vector<std::string> vr1, vr2;
  for (auto i : rr1) 
   vr1.push_back(ind_vec[0].ind[i]);
  for (auto i : rr2) 
   vr2.push_back(ind_vec[1].ind[i]);
  return {vr1, vr2};
 }

 inline gf_indices_tuple<3> slice(gf_indices_tuple<3> const &ind_vec, arrays::range rr1, arrays::range rr2, arrays::range rr3) {
  if (ind_vec.is_empty()) return {};
  std::vector<std::string> vr1, vr2, vr3;
  for (auto i : rr1) 
   vr1.push_back(ind_vec[0].ind[i]);
  for (auto i : rr2) 
   vr2.push_back(ind_vec[1].ind[i]);
  for (auto i : rr3) 
   vr3.push_back(ind_vec[2].ind[i]);
  return {vr1, vr2, vr3};
 }

 inline gf_indices_tuple<4> slice(gf_indices_tuple<4> const &ind_vec, arrays::range rr1, arrays::range rr2, arrays::range rr3, arrays::range rr4) {
  if (ind_vec.is_empty()) return {};
  std::vector<std::string> vr1, vr2, vr3, vr4;
  for (auto i : rr1) 
   vr1.push_back(ind_vec[0].ind[i]);
  for (auto i : rr2) 
   vr2.push_back(ind_vec[1].ind[i]);
  for (auto i : rr3) 
   vr3.push_back(ind_vec[2].ind[i]);
  for (auto i : rr4) 
   vr4.push_back(ind_vec[3].ind[i]);
  return {vr1, vr2, vr3, vr4};
 }

}
}

