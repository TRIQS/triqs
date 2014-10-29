
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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

namespace triqs {
namespace gfs {

 //------------------------------------------------------

 // A simple replacement of tail when there is none to maintain generic code simple...
 struct tail_zero {
  template <typename... Args> explicit tail_zero(Args &&...) {} // takes anything, do tail_zero..
  tail_zero() {}
  using const_view_type = tail_zero;
  using view_type = tail_zero;
  using regular_type = tail_zero;
  void rebind(tail_zero) {}
  template <typename RHS> void operator=(RHS &&) {}
  friend void h5_write(h5::group, std::string subgroup_name, tail_zero) {}
  friend void h5_read(h5::group, std::string subgroup_name, tail_zero) {}
  template <typename... A> friend tail_zero slice(tail_zero, A...) { return tail_zero(); }
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {}
  friend tail_zero operator+(tail_zero, tail_zero) { return tail_zero(); }
  template <typename RHS> friend void assign_singularity_from_function(tail_zero &, RHS) {}
  template<typename A> bool check_size(A) {return true;}
  bool is_empty() const { return false;}
 };

 // Check if T is tail_zero
 //template <typename T> constexpr bool is_tail_zero() { return std::is_same<tail_zero, T>::value; }

 template<int ... pos, typename ...T> nothing partial_eval(tail_zero, T&&...) { return {};}
 inline tail_zero transpose(tail_zero) { return {};}
 inline tail_zero inverse(tail_zero) { return {};}
 inline tail_zero conj(tail_zero) { return {};}
 template <typename T> tail_zero compose(tail_zero,T&) { return {};}
 template <typename... T> tail_zero slice_target(tail_zero, T...) { return tail_zero(); }
 template <typename T> tail_zero operator+(tail_zero, T const &) { return tail_zero(); }
 template <typename T> tail_zero operator-(tail_zero, T const &) { return tail_zero(); }
 template <typename T> tail_zero operator*(tail_zero, T const &) { return tail_zero(); }
 template <typename T> tail_zero operator/(tail_zero, T const &) { return tail_zero(); }
 template <typename T> TYPE_DISABLE_IF(tail_zero, std::is_same<T, tail_zero>) operator+(T const &, tail_zero) { return tail_zero(); }
 template <typename T> TYPE_DISABLE_IF(tail_zero, std::is_same<T, tail_zero>) operator-(T const &, tail_zero) { return tail_zero(); }
 template <typename T> TYPE_DISABLE_IF(tail_zero, std::is_same<T, tail_zero>) operator*(T const &, tail_zero) { return tail_zero(); }
 template <typename T> TYPE_DISABLE_IF(tail_zero, std::is_same<T, tail_zero>) operator/(T const &, tail_zero) { return tail_zero(); }
}
}
