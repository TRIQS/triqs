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
#include "./nothing.hpp"

namespace triqs {
namespace gfs {

 template <typename ReturnType> struct tail_zero {
  using const_view_type = tail_zero;
  using view_type = tail_zero;
  using regular_type = tail_zero;
 
  tail_zero() = default;
  tail_zero(tail_zero const &) = default;
  template <typename S> explicit tail_zero(S const & s) { _init(s,zero);}

  void rebind(tail_zero) {}
  template <typename RHS> void operator=(RHS &&) {}

  template <typename S> void reset(S const &s) { _init(s, zero); }

  /*friend void h5_write(h5::group fg, std::string const &subgroup_name, tail_zero const &t) {
   auto gr = fg.create_group(subgroup_name);
   h5_write(gr, "zero", t.zero);
  }

  friend void h5_read(h5::group fg, std::string const &subgroup_name, tail_zero &t) {
   auto gr = fg.open_group(subgroup_name);
   h5_read(gr, "zero", t.zero);
  }
  */
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {}
  
  template <typename RHS> friend void assign_singularity_from_function(tail_zero &, RHS) {}
  template<typename A> bool check_size(A) {return true;}
  bool is_empty() const { return false;}

  template <typename T> friend tail_zero transpose(tail_zero const & t) { return t;}
  template <typename T> friend tail_zero conj(tail_zero const & t) { return t;}
  template <typename T> friend tail_zero compose(tail_zero const & t,T&) { return t;}

  template<typename ... X> friend ReturnType evaluate(tail_zero const &t, X...) { return t.zero;} 

  friend tail_zero mpi_reduce(tail_zero t, mpi::communicator c= {}, int root=0, bool all = false) { return t; }
  friend void mpi_broadcast(tail_zero, mpi::communicator c= {}, int root=0) {}
 
  private: 
  ReturnType zero;
  template<typename S> void _init(S const & s, dcomplex & z) { z=0;} 
  template<typename S> void _init(S const & s, matrix<dcomplex> & z) { z.resize(s); z()=0;} 
  template<typename S, int R> void _init(S const & s, array<dcomplex,R> & z) { z.resize(s); z()=0;} 
 };

 template<int ... pos, typename T, typename ...X> nothing partial_eval_linear_index(tail_zero<T>, X&&...) { return {};}

  // all operations are neutral
  template <typename T, typename X> tail_zero<T> operator+(tail_zero<T>const & t, X const &) { return t; }
  template <typename T, typename X> tail_zero<T> operator-(tail_zero<T>const & t, X const &) { return t; }
  template <typename T, typename X> tail_zero<T> operator*(tail_zero<T>const & t, X const &) { return t; }
  template <typename T, typename X> tail_zero<T> operator/(tail_zero<T>const & t, X const &) { return t; }
  template <typename T, typename X>
  TYPE_DISABLE_IF(tail_zero<T>, std::is_same<X, tail_zero<T>>) operator+(X const &, tail_zero<T> const &t) { return t; }
  template <typename T, typename X>
  TYPE_DISABLE_IF(tail_zero<T>, std::is_same<X, tail_zero<T>>) operator-(X const &, tail_zero<T> const & t) { return t; }
  template <typename T, typename X>
  TYPE_DISABLE_IF(tail_zero<T>, std::is_same<X, tail_zero<T>>) operator*(X const &, tail_zero<T>const & t) { return t;}
  template <typename T, typename X>
  TYPE_DISABLE_IF(tail_zero<T>, std::is_same<X, tail_zero<T>>) operator/(X const &, tail_zero<T>const & t) { return t;}

  // the h5 write and read of singularities
  template <typename S> struct gf_h5_rw_singularity<tail_zero<S>> {
   template <typename G> static void write(h5::group gr, G const &g) {}
   template <typename G> static void read(h5::group gr, G &g) { g.singularity().reset(get_target_shape(g)); }
  };
}
}
