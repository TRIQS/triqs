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
 

 WRONGE CODE 
 // short cut. Here only the change compare to default multi var implementation
 using imfreq_bff = cartesian_product<imfreq, imfreq, imfreq>;
 //struct imfreq_bff {}; // = cartesian_product<imfreq, imfreq, imfreq>;
 using imfreq_mesh_3 = mesh_product<matsubara_freq_mesh, matsubara_freq_mesh, matsubara_freq_mesh>;

 // The default target for this mesh
 template <> struct gf_default_target<imfreq_bff> {
  using type = tensor_valued<4>;
 };

 // reimplement a simpler constructor, which enforces that the 2 meshes are equal.
 template <typename Opt>
 struct gf_mesh<imfreq_bff, Opt> : imfreq_mesh_3 {
  gf_mesh() = default;
  gf_mesh(matsubara_freq_mesh const& m1, matsubara_freq_mesh const& m2) : imfreq_mesh_3{m1, m2, m2} {}
 };

 namespace gfs_implementation {

  /// ---------------------------  data access  ---------------------------------

  struct imfreq_bff_indices_mixer {
   template <typename MI, typename TI> static auto invoke(MI const& m, TI const& t) {
    return std::make_tuple(std::get<0>(m), std::get<1>(m), std::get<0>(t), std::get<1>(t), std::get<2>(m), std::get<2>(t),
                           std::get<3>(t));
   }
  };

  template <>
  struct data_proxy<imfreq_bff, tensor_valued<4>, void> : data_proxy_array_index_mixer<std::complex<double>, 3, 4,
                                                                                       imfreq_bff_indices_mixer> {

   //template <typename S, typename Tu> auto operator()(S& data, Tu const& tu) const {
   // return data(std::get<0>(tu), arrays::range(), std::get<1>(tu), arrays::range(), std::get<2>(tu), arrays::range());
  // }
  };


  // -------------------------------  evaluator  --------------------------------------------------

  //template <typename Target, typename Opt>
  //struct evaluator<imfreq_bff, Target, nothing, Opt> : evaluator<cartesian_product<imfreq, imfreq, imfreq>, Target, nothing, Opt> {};

  /// ---------------------------  hdf5  ---------------------------------

  template <typename Opt> struct h5_name<imfreq_bff, tensor_valued<4>, nothing, Opt> {
   static std::string invoke() { return "imfreq_bff"; }
  };

 /// ---------------------------  partial eval ---------------------------------

  template <bool IsConst> struct partial_eval_impl<imfreq_x2, matrix_valued, nothing, void, IsConst> {

   template <int pos, typename Omega>
   gf_view<imfreq_x2, matrix_valued, nothing, void, IsConst>
   partial_eval(gf_view<imfreq_bff, tensor_valued<4>, nothing, void, IsConst> g, Omega const& omega) {
    static_assert(pos == 0, "EE");
    auto& m = g.mesh().components();
    auto av = g.data()(std::get<0>(m).index_to_linear(omega), arrays::ellipsis{});
    return {{std::get<1>(m)}, av, {}, {}, {}};
   }
  };

 } // gfs_implementation

 gf_mesh<imfreq> get_bosonic_mesh(gf_view<imfreq_bff, tensor_valued<4>> g) { return std::get<0>(g.mesh().components()); }


/* template <typename Omega>
 auto partial_eval(gf<imfreq_bff, tensor_valued<4>>& g, Omega const& omega) RETURN(partial_eval(g(), omega));

 template <typename Omega>
 auto partial_eval(gf<imfreq_bff, tensor_valued<4>> const& g, Omega const& omega) RETURN(partial_eval(g(), omega));

 /// ---------------------------  curry ---------------------------------

 template <bool IsConst> auto curry_on_bosonic_freq(gf_view<imfreq_bff, tensor_valued<4>, nothing, void, IsConst> g) {
  return make_gf_view_lambda_valued<imfreq>(std::get<0>(g.mesh().components()),
                                            [g](auto&& x) { return partial_eval(g, x); });
 }

 auto curry_on_bosonic_freq(gf<imfreq_bff, tensor_valued<4>>& g) RETURN(curry_on_bosonic_freq(g()));
 auto curry_on_bosonic_freq(gf<imfreq_bff, tensor_valued<4>> const& g) RETURN(curry_on_bosonic_freq(g()));
*/
 
 }
}


/* template<typename G>
 struct curry_polymorphic_lambda2 {
  G g;
  template <typename... I> AUTO_DECL operator()(I &&... i) const RETURN(partial_eval(g, i...));
 };
*/

