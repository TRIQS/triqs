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

 // A simple replacement of tail when there is none to maintain generic code simple...
 struct nothing {
  template <typename... Args> explicit nothing(Args &&...) {} // takes anything, do nothing..
  nothing() {}
  using const_view_type = nothing;
  using view_type = nothing;
  using regular_type = nothing;
  void rebind(nothing) {}
  template <typename RHS> void operator=(RHS &&) {}
  friend void h5_write(h5::group, std::string subgroup_name, nothing) {}
  friend void h5_read(h5::group, std::string subgroup_name, nothing) {}
  template <typename... A> friend nothing slice(nothing, A...) { return nothing(); }
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {}
  friend nothing operator+(nothing, nothing) { return nothing(); }
  template <typename RHS> friend void assign_singularity_from_function(nothing &, RHS) {}
  template<typename A> bool check_size(A) {return true;}
  bool is_empty() const { return false;}
  nothing operator()() const { return {};}
 };

 // Check if T is nothing
 template <typename T> constexpr bool is_nothing() { return std::is_same<nothing, T>::value; }

 template<int ... pos, typename ...T> nothing partial_eval_linear_index(nothing, T&&...) { return {};}
 inline nothing transpose(nothing) { return {};}
 inline nothing inverse(nothing) { return {};}
 inline nothing conj(nothing) { return {};}
 template <typename T> nothing compose(nothing,T&) { return {};}
 template <typename... T> nothing slice_target_sing(nothing, T...) { return {}; }
 template <typename... T> nothing slice_target_to_scalar_sing(nothing, T...) { return {}; }

 inline nothing reinterpret_as_matrix_valued_sing(nothing) { return {}; }

 template <typename T> nothing operator+(nothing, T const &) { return nothing(); }
 template <typename T> nothing operator-(nothing, T const &) { return nothing(); }
 template <typename T> nothing operator*(nothing, T const &) { return nothing(); }
 template <typename T> nothing operator/(nothing, T const &) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator+(T const &, nothing) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator-(T const &, nothing) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator*(T const &, nothing) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator/(T const &, nothing) { return nothing(); }

 inline nothing mpi_reduce(nothing, mpi::communicator c= {}, int root=0, bool all = false, MPI_Op op = MPI_SUM) { return {}; }
 inline void mpi_broadcast(nothing, mpi::communicator c= {}, int root=0) {}
 
 }
}
