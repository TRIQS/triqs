/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2015 by O. Parcollet
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
#include <triqs/utility/first_include.hpp>
#include <triqs/utility/factory.hpp>
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/utility/c14.hpp>
#include <triqs/utility/macros.hpp>
#include <triqs/arrays/h5.hpp>
#include "./gf_indices.hpp"
#include "./tools.hpp"
#include "../singularity/tail.hpp"
#include "../singularity/nothing.hpp"
#include "../singularity/tail_zero.hpp"
#include "./data_proxies.hpp"
#include <triqs/mpi/vector.hpp>
#include <vector>

namespace triqs {
namespace gfs {
 using utility::factory;
 using arrays::make_shape;
 using arrays::range;
 using arrays::array;
 using arrays::array_view;
 using arrays::matrix;
 using arrays::matrix_view;
 using arrays::matrix_const_view;
 using triqs::make_clone;

 /*----------------------------------------------------------
  *   Declarations related to the mesh
  *--------------------------------------------------------*/   
 
// Interpolation policies
 namespace interpol_t {
  struct None{};
  struct Product{};
  struct Linear1d{};
  struct Linear2d{};
 }

 // The mesh for each Mesh
 template <typename Mesh> struct gf_mesh;

 // The mesh point for each mesh
 template<typename MeshType> struct mesh_point;
 
 /*----------------------------------------------------------
  *   Default Target, Singularity
  *--------------------------------------------------------*/   
 
 template <typename Mesh> struct gf_default_target {
  using type = matrix_valued;
 };
 template <typename M> using gf_default_target_t = typename gf_default_target<M>::type;

 template <typename Mesh, typename Target> struct gf_default_singularity {
  using type = nothing;
 };
 template <typename M, typename T>
 using gf_default_singularity_t = typename gf_default_singularity<M, T>::type;

 /*----------------------------------------------------------
  *   Declaration of main types : gf, gf_view, gf_const_view
  *--------------------------------------------------------*/   
  // The regular type
 template <typename Mesh, typename Target = gf_default_target_t<Mesh>,
           typename Singularity = gf_default_singularity_t<Mesh, Target>, typename Evaluator = void>
 class gf;

 // The view type
 template <typename Mesh, typename Target = gf_default_target_t<Mesh>,
           typename Singularity = gf_default_singularity_t<Mesh, Target>, typename Evaluator = void>
 class gf_view;

 // The const view type
 template <typename Mesh, typename Target = gf_default_target_t<Mesh>,
           typename Singularity = gf_default_singularity_t<Mesh, Target>, typename Evaluator = void>
 class gf_const_view;

 /*----------------------------------------------------------
  *   Useful metafunctions, traits
  *--------------------------------------------------------*/   

 // Get the number of variables
 template<typename Mesh> struct get_n_variables { static const int value = 1;};

 /*----------------------------------------------------------
  *   Traits 
  *--------------------------------------------------------*/   

 // The trait that "marks" the Green function
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(ImmutableGreenFunction);

 // Is G a gf, gf_view, gf_const_view
 // is_gf_or_view<G> is true iif G is a gf or a view
 // is_gf_or_view<G,M0> is true iif  G is a gf or a view and its mesh is M0
 template <typename G, typename M0 = void> struct is_gf_or_view : std::false_type {};

 template <typename M, typename T, typename S, typename E> struct is_gf_or_view<gf<M, T, S, E>, void> : std::true_type {};
 template <typename M, typename T, typename S, typename E> struct is_gf_or_view<gf<M, T, S, E>, M> : std::true_type {};
 
 template <typename M, typename T, typename S, typename E>
 struct is_gf_or_view<gf_view<M, T, S, E>, void> : std::true_type {};

 template <typename M, typename T, typename S, typename E>
 struct is_gf_or_view<gf_view<M, T, S, E>, M> : std::true_type {};

 template <typename M, typename T, typename S, typename E>
 struct is_gf_or_view<gf_const_view<M, T, S, E>, void> : std::true_type {};

 template <typename M, typename T, typename S, typename E>
 struct is_gf_or_view<gf_const_view<M, T, S, E>, M> : std::true_type {};

 template <typename G, typename M> struct is_gf_or_view<G&,M> : is_gf_or_view<G,M>{};
 template <typename G, typename M> struct is_gf_or_view<G const &,M> : is_gf_or_view<G,M>{};
 template <typename G, typename M> struct is_gf_or_view<G &&,M> : is_gf_or_view<G,M>{};
 
 /*----------------------------------------------------------
  *  Evaluator
  *--------------------------------------------------------*/   

 // gf_evaluator regroup functions to evaluate the function.
 // default : one variable. Will be specialized in more complex cases.
 template <typename Mesh, typename Target, typename Singularity> struct gf_evaluator {
  static constexpr int arity = 1;
  template <typename G> gf_evaluator(G *) {};

  template <typename G, typename X>
  auto operator()(G const &g, X x) const RETURN((g.mesh().evaluate(typename G::mesh_t::default_interpol_policy{}, g, x)));

  template <typename G> typename G::singularity_t operator()(G const &g, tail_view t) const {
   return compose(g.singularity(), t);
  }
 };

 /*----------------------------------------------------------
  *  closest_point mechanism
  *  This trait will contain the specialisation to get 
  *  the closest point ...
  *--------------------------------------------------------*/   

 template <typename Mesh, typename Target, typename Singularity, typename Evaluator> struct gf_closest_point;

 /*----------------------------------------------------------
  *  symmetry
  *--------------------------------------------------------*/   

 template <typename Mesh, typename Target, typename Singularity> struct gf_symmetry {
  using type = nothing;
 };

 /*----------------------------------------------------------
  *  indices
  *--------------------------------------------------------*/   

 template <typename Target> struct gf_indices {
  using type = nothing;
 };

 template <> struct gf_indices<matrix_valued> {
  using type = gf_indices_pair;
 };

 /*----------------------------------------------------------
  *  data proxy
  *--------------------------------------------------------*/   

 // gf_data_proxy contains function to manipulate the data array, but no data itself.
 // this is used to specialize this part of the code to array of dim 3 (matrix gf), 
 // dim 1 (scalar gf) and vector (e.g. block gf, ...)
 template <typename Mesh, typename Target, typename Enable = void> struct gf_data_proxy;

 template <typename M> struct gf_data_proxy<M, scalar_valued> : data_proxy_array<dcomplex, 1> {};
 template <typename M> struct gf_data_proxy<M, matrix_valued> : data_proxy_array<dcomplex, 3> {};
 template <typename M, int R> struct gf_data_proxy<M, tensor_valued<R>> : data_proxy_array<dcomplex, 1+R> {};

 // should only be used exceptionnal, for real g(tau) e.g. 
 template <typename M> struct gf_data_proxy<M, scalar_real_valued> : data_proxy_array<double, 1> {};
 template <typename M> struct gf_data_proxy<M, matrix_real_valued> : data_proxy_array<double, 3> {};
 template <typename M, int R> struct gf_data_proxy<M, tensor_real_valued<R>> : data_proxy_array<double, 1+R> {};

 /*----------------------------------------------------------
  *  HDF5
  *  Traits to read/write in hdf5 files. 
  *  Can be specialized for some case (Cf block). Defined below
  *--------------------------------------------------------*/   

 template <typename Mesh, typename Target, typename Singularity> struct gf_h5_name;
 template <typename Mesh, typename Target, typename Singularity, typename Evaluator> struct gf_h5_rw;
 template <typename Mesh, typename Target, typename Singularity, typename Evaluator> struct gf_h5_before_write;
 template <typename Mesh, typename Target, typename Singularity, typename Evaluator> struct gf_h5_after_read;

 /*----------------------------------------------------------
  *  Factories for data and singularity
  *  Defaults implemented below.
  *--------------------------------------------------------*/   
 
 template <typename Mesh, typename Target, typename Singularity> struct gf_data_factory;
 template <typename Mesh, typename Target, typename Singularity> struct gf_singularity_factory;

 /*----------------------------------------------------------
  *  Get shape of the data or of the target
  *--------------------------------------------------------*/   

 template <typename M, typename T, typename S, typename E>
 auto get_gf_data_shape(gf<M,T,S,E> const &g) RETURN(get_shape(g.data()));
 
 template <typename M, typename T, typename S, typename E>
 auto get_gf_data_shape(gf_view<M,T,S,E> const &g) RETURN(get_shape(g.data()));
 
 template <typename M, typename T, typename S, typename E>
 auto get_gf_data_shape(gf_const_view<M,T,S,E> const &g) RETURN(get_shape(g.data()));

 template <typename M, typename T, typename S, typename E>
 auto get_target_shape(gf<M,T,S,E> const &g)
     DECL_AND_RETURN(g.data().shape().front_pop());

 template <typename M, typename T, typename S, typename E>
 auto get_target_shape(gf_view<M,T,S,E> const &g)
     DECL_AND_RETURN(g.data().shape().front_pop());

 template <typename M, typename T, typename S, typename E>
 auto get_target_shape(gf_const_view<M,T,S,E> const &g)
     DECL_AND_RETURN(g.data().shape().front_pop());

 // overload get_shape for a vector to simplify code below in gf block case.
 template <typename T> long get_shape(std::vector<T> const &x) { return x.size(); }


 /*------------------------------------------------------------------------------------------------------
  *                                  For mpi lazy
  *-----------------------------------------------------------------------------------------------------*/

 // A small lazy tagged class
 template <typename Tag, typename T> struct mpi_lazy {
  T rhs;
  mpi::communicator c;
  int root;
  bool all;
 };

}
}
