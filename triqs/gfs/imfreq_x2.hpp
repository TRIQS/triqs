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
#include "./product.hpp"
#include "./meshes/matsubara_freq.hpp"
namespace triqs {
namespace gfs {

 DEAD CODE

 struct matrix_operations {};

 // in this version, we store all frequencies, to be able to write matrix operation simply.
 // memory usage is x2 too large in principle.
 
 // short cut. Here only the change compare to default multi var implementation
 using imfreq_x2 = cartesian_product<imfreq, imfreq>;

 // reimplement a simpler constructor, which enforces that the 2 meshes are equal.
 template <typename Opt> struct gf_mesh<imfreq_x2, Opt> : mesh_product<matsubara_freq_mesh, matsubara_freq_mesh> {
  using B = mesh_product<matsubara_freq_mesh, matsubara_freq_mesh>;
  gf_mesh() = default;
  gf_mesh(matsubara_freq_mesh const& m) : B{m, m} {}
  //gf_mesh(matsubara_freq_mesh const & m1, matsubara_freq_mesh const& m2) : B{m1,m2} {} //needed for curry. DO NOT document.
  template <typename... T>
  gf_mesh(T&&... x)
     : B{matsubara_freq_mesh(std::forward<T>(x)...), matsubara_freq_mesh(std::forward<T>(x)...)} {}
 };

 // The default target for this mesh
 template <> struct gf_default_target<imfreq_x2> {
  using type = tensor_valued<4>;
 };
 namespace gfs_implementation {

  /// ---------------------------  data access  ---------------------------------


   struct imfreq_x2_indices_mixer {
   template <typename MI, typename TI> static auto invoke(MI const& m, TI const& t) {
    return std::make_tuple(std::get<0>(m), std::get<0>(t), std::get<1>(t), std::get<1>(m), std::get<2>(t), std::get<3>(t));
   }
  };

  template <>
  struct data_proxy<imfreq_x2, tensor_valued<4>, void> : data_proxy_array_index_mixer<std::complex<double>, 2, 4,
                                                                                       imfreq_x2_indices_mixer> {

   //template <typename S, typename Tu> auto operator()(S& data, Tu const& tu) const {
   // return data(std::get<0>(tu), arrays::range(), std::get<1>(tu), arrays::range(), std::get<2>(tu), arrays::range());
  // }
  };
/*
  // Change the ordering of the indices in the array to allow matrix operations
  template <typename Opt> struct data_proxy<imfreq_x2, tensor_valued<4>, Opt> : data_proxy_array_common<std::complex<double>, 6> {

   template <typename S1, typename S2> static utility::mini_vector<int, 4> join_shape(S1 const& s1, S2 const& s2) {
    return {int(s1[0]), s2[0], int(s1[1]), s2[1]};
    // TODO : clean this size_t....
   }

   template <typename S, typename Tu>
   AUTO_DECL operator()(S& data, Tu const& tu) const
       RETURN(make_matrix_view(data(std::get<0>(tu), arrays::range(), std::get<1>(tu), arrays::range())));
  };

  */
 } // gfs_implementation

 /// ---------------------------  inverse  ---------------------------------

 // the generic inverse is fine. We only need to redo the invert_in_place.
 template <typename Opt> void invert_in_place(gf_view<imfreq_x2, matrix_valued, nothing, Opt> g) {

 auto arr = make_matrix_view(group_indices_view(g.data(), {0, 1, 2}, {3, 4, 5})); // a view of the array properly regrouped
 arr = inverse(arr);                                      // inverse as a big matrix (nu,n) x (nu', n')

  // no singularity
 }
}
}
