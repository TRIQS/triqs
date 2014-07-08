/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by O. Parcollet
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
  template <typename... T> gf_mesh(T &&... x) : matsubara_freq_mesh(std::forward<T>(x)...) {}
  //using matsubara_freq_mesh::matsubara_freq_mesh;
 };

 namespace gfs_implementation {

  // singularity
  template <> struct singularity<imfreq, matrix_valued, void> {
   using type = local::tail;
  };
  template <> struct singularity<imfreq, scalar_valued, void> {
   using type = local::tail;
  };

  // h5 name
  template <typename Opt> struct h5_name<imfreq, matrix_valued, Opt> {
   static std::string invoke() { return "ImFreq"; }
  };

  /// ---------------------------  evaluator ---------------------------------

  // simple evaluation : take the point on the grid...
  template <> struct evaluator_fnt_on_mesh<imfreq> {
   long n;
   double w;
   evaluator_fnt_on_mesh() = default;
   template <typename MeshType> evaluator_fnt_on_mesh(MeshType const &m, long p) { n = p; w=1; } 
   template <typename MeshType> evaluator_fnt_on_mesh(MeshType const &m, matsubara_freq const &p) { 
    if ((p.n >= m.first_index()) && (p.n < m.size()+m.first_index())) {w=1; n =p.n;}
    else {w=0; n=0;}
   }
   template <typename F> auto operator()(F const &f) const DECL_AND_RETURN(w*f(n));
  };

  // ------------- evaluator  -------------------
  // handle the case where the matsu. freq is out of grid...
  template <typename Target, typename Opt> struct evaluator<imfreq, Target, Opt> {
   static constexpr int arity = 1;

   private:
   template <typename G> int sh(G const * g) const { return (g->mesh().domain().statistic == Fermion ? 1 : 0);}

   // dispatch for 2x2 cases : matrix/scalar and tail/no_tail ( true means no_tail)
   template <typename G>
   std::complex<double> _call_impl(G const *g, matsubara_freq const &f, scalar_valued, std::false_type) const {
    if (g->mesh().positive_only()){//only positive Matsubara frequencies
     if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n];
     if ((f.n < 0) && ((-f.n-sh(g)) < g->mesh().size())) return conj((*g)[-f.n-sh(g)]);
    }
    else{
     if ((f.n >= g->mesh().first_index()) && (f.n < g->mesh().size()+g->mesh().first_index())) return (*g)[f.n];
    }
    return g->singularity().evaluate(f)(0, 0);
   }

   template <typename G>
   std::complex<double> _call_impl(G const *g, matsubara_freq const &f, scalar_valued, std::true_type) const {
    if (g->mesh().positive_only()){//only positive Matsubara frequencies
     if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n];
     if ((f.n < 0) && ((-f.n-sh(g)) < g->mesh().size())) return conj((*g)[-f.n-sh(g)]);
    }
    else{
     if ((f.n >= g->mesh().first_index()) && (f.n < g->mesh().size()+g->mesh().first_index())) return (*g)[f.n];
    }
    return 0;
   }

   template <typename G>
   arrays::matrix_const_view<std::complex<double>> _call_impl(G const *g, matsubara_freq const &f, matrix_valued,
                                                              std::false_type) const {
    if (g->mesh().positive_only()){//only positive Matsubara frequencies
     if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n]();
     if ((f.n < 0) && ((-f.n-sh(g)) < g->mesh().size()))
      return arrays::matrix<std::complex<double>>{conj((*g)[-f.n-sh(g)]())};
    }
    else{
     if ((f.n >= g->mesh().first_index()) && (f.n < g->mesh().size()+g->mesh().first_index())) return (*g)[f.n];
    }
    return g->singularity().evaluate(f);
   }

   template <typename G>
   arrays::matrix_const_view<std::complex<double>> _call_impl(G const *g, matsubara_freq const &f, matrix_valued,
                                                              std::true_type) const {
    if (g->mesh().positive_only()){//only positive Matsubara frequencies
     if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n]();
     if ((f.n < 0) && ((-f.n-sh(g)) < g->mesh().size()))
      return arrays::matrix<std::complex<double>>{conj((*g)[-f.n-sh(g)]())};
    }
    else{
     if ((f.n >= g->mesh().first_index()) && (f.n < g->mesh().size()+g->mesh().first_index())) return (*g)[f.n];
    }
    auto r = arrays::matrix<std::complex<double>>{get_target_shape(*g)};
    r() = 0;
    return r;
   }

   // does not work on gcc 4.8.1 ???
   /* template <typename G>
      auto operator()(G const *g, matsubara_freq const &f) const
      DECL_AND_RETURN(_call_impl(g, f, Target{}, std::integral_constant<bool, std::is_same<Opt, no_tail>::value>{}));
      */
   public:

   template <typename G>
   typename std::conditional<std::is_same<Target, matrix_valued>::value, arrays::matrix_const_view<std::complex<double>>,
                             std::complex<double>>::type
   operator()(G const *g, matsubara_freq const &f) const {
    return _call_impl(g, f, Target{}, std::integral_constant<bool, std::is_same<Opt, no_tail>::value>{});
   }

   // int -> replace by matsubara_freq
   template <typename G> auto operator()(G const *g, int n) const DECL_AND_RETURN((*g)(matsubara_freq(n,g->mesh().domain().beta,g->mesh().domain().statistic)));

#ifdef __clang__
   // to generate a clearer error message ? . Only ok on clang ?
   template <int n> struct error {
    static_assert(n > 0, "Green function cannot be evaluated on a complex number !");
   };

   template <typename G> error<0> operator()(G const *g, std::complex<double>) const {
    return {};
   }
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
