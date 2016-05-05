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

 class gf_indices {

  using v_t = std::vector<std::string>;
  using vv_t = std::vector<v_t>;

  // L -> a vector of ['0', '1', '2' ..., 'L-1'] 
  static v_t make_vt(int L) {
   v_t res;
   res.reserve(L);
   for (int i = 0; i < L; ++i) res.push_back(std::to_string(i));
   return res;
  }

  vv_t _data;

  public:

  /// Default constructor
  gf_indices() = default;

  /// from a std::vector<std::vector<std::string>>
  gf_indices(vv_t x) : _data(std::move(x)) {}

  /// from a shape, make a list of indices '0,1,2,...,r' in all dimensions
  template <int R, typename Int> gf_indices(arrays::mini_vector<Int, R> const &shape) {
   _data.reserve(R);
   for (int i = 0; i < R; i++) _data.push_back(make_vt(shape[i]));
  }

  /// Access to one of the index list
  decltype(auto) operator[](int i) const { return _data[i]; }

  /// Data access
  vv_t const & data() const  { return _data;}

  /// True iif it is empty
  bool empty() const { return _data.empty(); }

  /// Rank. 0 if empty
  int rank() const { return _data.size();}

  /// Transposition. For 2d only
  gf_indices transpose() const {
   if (rank() != 2) TRIQS_RUNTIME_ERROR << " transpose only implemented for d=2";
   return vv_t{_data[1], _data[0]};
  }

  /// True iif the gf_indices is not empty and has the shape sh
  template <int R, typename Int> bool has_shape(arrays::mini_vector<Int, R> const &sh) {
   if (empty()) return false;
   if (_data.size() != R) return false;
   for (int i = 0; i < R; i++)
    if (_data[i].size() != sh[i]) return false;
   return true;
  }

  /// Convert index string s for indices number i into the integer index.
  arrays::range convert_index(std::string const &s, int i) const { 
   auto const & v = _data[i];
   auto b = v.begin(), e = v.end();
   auto it = std::find(b, e, s);
   if (it != e) return arrays::range(it - b, it - b + 1);
   TRIQS_RUNTIME_ERROR << "Cannot find this string index for the Green's function";
  }

 
  friend void h5_write(h5::group fg, std::string subgroup_name, gf_indices const &g);
  friend void h5_read(h5::group fg, std::string subgroup_name, gf_indices &g);

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar &_data; }

  // ------------------  implement slicing -------------------------
  private:
  // slice one vector with the range r
  int slice_one_vec(vv_t &v_out, v_t const &v_in, arrays::range const &r) const {
   v_t res;
   for (auto i : r) res.push_back(v_in[i]);
   v_out.push_back(res);
   return 0;
  }
  // do nothing
  int slice_one_vec(vv_t &v_out, v_t const &v_in, long r) const { return 0; }

  template <typename Tu, size_t... Is> vv_t slice_impl(std14::index_sequence<Is...>, Tu const &tu) const {
   vv_t res;
   // rely on init order. gcc >=5.1 MANDATORY (bug in 4.9).
   std::vector<int>{slice_one_vec(res, _data[Is], std::get<Is>(tu))...};
   return res;
  }
  
  public:
  /// Slicing. R are expected to be arrays::range
  template <typename... R> friend gf_indices slice(gf_indices const &gi, R const &... r) {
   if (gi.empty()) return {};
   if (gi.rank() != sizeof...(R)) TRIQS_RUNTIME_ERROR << " Incorrect slicing of indices ";
   return {gi.slice_impl(std14::index_sequence_for<R...>{}, std::make_tuple(r...))};
  }
 };
 }
}

