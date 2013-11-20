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
#ifndef TRIQS_GF_MATSUBARA_FREQ_H
#define TRIQS_GF_MATSUBARA_FREQ_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./local/no_tail.hpp"
#include "./meshes/matsubara_freq.hpp"
#include "./evaluators.hpp"
namespace triqs {
namespace gfs {

 struct imfreq {};

 template <typename Opt> struct gf_mesh<imfreq, Opt> : matsubara_freq_mesh {
  using B = matsubara_freq_mesh;
  static double m1(double beta) { return std::acos(-1) / beta; }
  gf_mesh() = default;
  gf_mesh(B const &x) : B(x) {} // enables also construction from another Opt
  gf_mesh(typename B::domain_t const &d, int Nmax = 1025) : B(d, Nmax, true) {}
  gf_mesh(double beta, statistic_enum S, int Nmax = 1025) : gf_mesh({beta, S}, Nmax) {}
 };

 namespace gfs_implementation {

  // singularity
  template <> struct singularity<imfreq, matrix_valued, void> {
   typedef local::tail type;
  };
  template <> struct singularity<imfreq, scalar_valued, void> {
   typedef local::tail type;
  };

  // h5 name
  template <typename Opt> struct h5_name<imfreq, matrix_valued, Opt> {
   static std::string invoke() { return "ImFreq"; }
  };

  /// ---------------------------  evaluator ---------------------------------

  // simple evaluation : take the point on the grid...
  template <> struct evaluator_fnt_on_mesh<imfreq> {
   long n;
   evaluator_fnt_on_mesh() = default;
   template <typename MeshType> evaluator_fnt_on_mesh(MeshType const &m, long p) { n = p; }
   template <typename MeshType> evaluator_fnt_on_mesh(MeshType const &m, matsubara_freq_mesh::mesh_point_t const &p) { n = p.n; }
   template <typename F> auto operator()(F const &f) const DECL_AND_RETURN(f(n));
  };

  // ------------- evaluator  -------------------
  // handle the case where the matsu. freq is out of grid...
  template <typename Target, typename Opt> struct evaluator<imfreq, Target, Opt> {
   static constexpr int arity = 1;
   template <typename G> auto operator()(G const *g, int n) const DECL_AND_RETURN((*g)[n]);
   template <typename G>
   auto operator()(G const *g, matsubara_freq_mesh::mesh_point_t const &p) const DECL_AND_RETURN((*g)[p.index()]);

   // dispatch for 2x2 cases : matrix/scalar and tail/no_tail ( true means no_tail)
   template <typename G>
   std::complex<double> _call_impl(G const *g, matsubara_freq const &f, scalar_valued, std::false_type) const {
    if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n];
    if ((f.n < 0) && (-f.n < g->mesh().size())) return conj((*g)[-f.n]);
    return g->singularity().evaluate(f)(0, 0);
   }

   template <typename G>
   std::complex<double> _call_impl(G const *g, matsubara_freq const &f, scalar_valued, std::true_type) const {
    if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n];
    if ((f.n < 0) && (-f.n < g->mesh().size())) return conj((*g)[-f.n]);
    return 0;
   }

   template <typename G>
   arrays::matrix_const_view<std::complex<double>> _call_impl(G const *g, matsubara_freq const &f, matrix_valued,
                                                              std::false_type) const {
    if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n]();
    if ((f.n < 0) && (-f.n < g->mesh().size()))
     return arrays::matrix<std::complex<double>>{conj((*g)[-f.n]())};
    else
     return g->singularity().evaluate(f);
   }

   template <typename G>
   arrays::matrix_const_view<std::complex<double>> _call_impl(G const *g, matsubara_freq const &f, matrix_valued,
                                                              std::true_type) const {
    if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n]();
    if ((f.n < 0) && (-f.n < g->mesh().size())) return arrays::matrix<std::complex<double>>{conj((*g)[-f.n]())};
    auto r = arrays::matrix<std::complex<double>>{get_target_shape(*g)};
    r() = 0;
    return r;
   }

   // does not work on gcc 4.8.1 ???
   /* template <typename G>
      auto operator()(G const *g, matsubara_freq const &f) const
      DECL_AND_RETURN(_call_impl(g, f, Target{}, std::integral_constant<bool, std::is_same<Opt, no_tail>::value>{}));
      */

   template <typename G>
   typename std::conditional<std::is_same<Target, matrix_valued>::value, arrays::matrix_const_view<std::complex<double>>,
                             std::complex<double>>::type
   operator()(G const *g, matsubara_freq const &f) const {
    return _call_impl(g, f, Target{}, std::integral_constant<bool, std::is_same<Opt, no_tail>::value>{});
   }

#ifdef __clang__
   // to generate a clearer error message ? . Only ok on clang ?
   template<int n> struct error { static_assert(n>0, "Green function can not be evaluated on a complex number !");};

   template <typename G>
    error<0> operator()(G const *g, std::complex<double>) const { return {};}
#endif

   template <typename G> typename G::singularity_t const &operator()(G const *g, freq_infty const &) const {
    return g->singularity();
   }
  };

  /// ---------------------------  data access  ---------------------------------
  template <typename Opt> struct data_proxy<imfreq, matrix_valued, Opt> : data_proxy_array<std::complex<double>, 3> {};
  template <typename Opt> struct data_proxy<imfreq, scalar_valued, Opt> : data_proxy_array<std::complex<double>, 1> {};

 } // gfs_implementation
}
}
#endif
