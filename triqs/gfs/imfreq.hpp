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

 // singularity
 template <> struct gf_default_singularity<imfreq, matrix_valued> {
  using type = tail;
 };
 template <> struct gf_default_singularity<imfreq, scalar_valued> {
  using type = tail;
 };

 namespace gfs_implementation {

  /// ---------------------------  hdf5 ---------------------------------
  
  template <typename S, typename Opt> struct h5_name<imfreq, matrix_valued, S, Opt> {
   static std::string invoke() { return "ImFreq"; }
  };

  /// ---------------------------  data access  ---------------------------------

  template <typename Opt> struct data_proxy<imfreq, matrix_valued, Opt> : data_proxy_array<std::complex<double>, 3> {};
  template <typename Opt> struct data_proxy<imfreq, scalar_valued, Opt> : data_proxy_array<std::complex<double>, 1> {};

  /// ---------------------------  evaluator ---------------------------------

  // simple evaluation : take the point on the grid...
  template <> struct evaluator_fnt_on_mesh<imfreq> {
   long n;
   double w;
   evaluator_fnt_on_mesh() = default;
   template <typename MeshType> void reset(MeshType const &m, long p) { n = p; w=1; } 
   template <typename MeshType> void reset(MeshType const &m, matsubara_freq const &p) { 
    if ((p.n >= m.first_index()) && (p.n < m.size()+m.first_index())) {w=1; n =p.n;}
    else {w=0; n=0;}
   }
   template <typename F> AUTO_DECL operator()(F const &f) const RETURN(w*f(n));
  };

  // ------------- evaluator  -------------------
  // handle the case where the matsu. freq is out of grid...

  struct _eval_imfreq_base_impl {
   static constexpr int arity = 1;
   template <typename G> int sh(G const * g) const { return (g->mesh().domain().statistic == Fermion ? 1 : 0);}

   // int -> replace by matsubara_freq
   template <typename G>
   AUTO_DECL operator()(G const *g, int n) const
       RETURN((*g)(matsubara_freq(n, g->mesh().domain().beta, g->mesh().domain().statistic)));

   template <typename G> typename G::singularity_t operator()(G const *g, tail_view t) const {
    return compose(g->singularity(),t);
    //return g->singularity();
   }
  };
  // --- various 4 specializations

  // scalar_valued, tail
  template <typename Opt> struct evaluator<imfreq, scalar_valued, tail, Opt> : _eval_imfreq_base_impl {
 
   using _eval_imfreq_base_impl::operator();

   template <typename G> std::complex<double> operator()(G const *g, matsubara_freq const &f) const {
    if (g->mesh().positive_only()) { // only positive Matsubara frequencies
     if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n];
     if ((f.n < 0) && ((-f.n - this->sh(g)) < g->mesh().size())) return conj((*g)[-f.n - this->sh(g)]);
    } else {
     if ((f.n >= g->mesh().first_index()) && (f.n < g->mesh().size() + g->mesh().first_index())) return (*g)[f.n];
    }
    return evaluate(g->singularity(),f)(0, 0);
   }
  };

  // scalar_valued, no tail
  template <typename Opt> struct evaluator<imfreq, scalar_valued, nothing, Opt> : _eval_imfreq_base_impl {

   using _eval_imfreq_base_impl::operator();

   template <typename G> std::complex<double> operator()(G const *g, matsubara_freq const &f) const {
    if (g->mesh().positive_only()) { // only positive Matsubara frequencies
     if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n];
     if ((f.n < 0) && ((-f.n - this->sh(g)) < g->mesh().size())) return conj((*g)[-f.n - this->sh(g)]);
    } else {
     if ((f.n >= g->mesh().first_index()) && (f.n < g->mesh().size() + g->mesh().first_index())) return (*g)[f.n];
    }
    return 0;
   }
  };

  // matrix_valued, tail
  template <typename Opt> struct evaluator<imfreq, matrix_valued, tail, Opt> : _eval_imfreq_base_impl {

   using _eval_imfreq_base_impl::operator();

   template <typename G> arrays::matrix_const_view<std::complex<double>> operator()(G const *g, matsubara_freq const &f) const {
    if (g->mesh().positive_only()) { // only positive Matsubara frequencies
     if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n]();
     if ((f.n < 0) && ((-f.n - this->sh(g)) < g->mesh().size()))
      return arrays::matrix<std::complex<double>>{conj((*g)[-f.n - this->sh(g)]())};
    } else {
     if ((f.n >= g->mesh().first_index()) && (f.n < g->mesh().size() + g->mesh().first_index())) return (*g)[f.n];
    }
    return evaluate(g->singularity(), f);
   }
  };

  // matrix_valued, no tail
  template <typename Opt> struct evaluator<imfreq, matrix_valued, nothing, Opt> : _eval_imfreq_base_impl {

   using _eval_imfreq_base_impl::operator();

   template <typename G> arrays::matrix_const_view<std::complex<double>> operator()(G const *g, matsubara_freq const &f) const {
    if (g->mesh().positive_only()) { // only positive Matsubara frequencies
     if ((f.n >= 0) && (f.n < g->mesh().size())) return (*g)[f.n]();
     if ((f.n < 0) && ((-f.n - this->sh(g)) < g->mesh().size()))
      return arrays::matrix<std::complex<double>>{conj((*g)[-f.n - this->sh(g)]())};
    } else {
     if ((f.n >= g->mesh().first_index()) && (f.n < g->mesh().size() + g->mesh().first_index())) return (*g)[f.n];
    }
    auto r = arrays::matrix<std::complex<double>>{get_target_shape(*g)};
    r() = 0;
    return r;
   }
  };

 } // gfs_implementation

 // FOR LEGACY PYTHON CODE ONLY
 // THIS MUST be kept for python operations 
 // specific operations (for legacy python code).
 // +=, -= with a matrix
 inline void operator+=(gf_view<imfreq> g, arrays::matrix<std::complex<double>> const &m) {
  for (int u = 0; u < int(first_dim(g.data())); ++u) g.data()(u, arrays::ellipsis()) += m;
  g.singularity()(0) += m;
 }

 inline void operator-=(gf_view<imfreq> g, arrays::matrix<std::complex<double>> const &m) {
  for (int u = 0; u < int(first_dim(g.data())); ++u) g.data()(u, arrays::ellipsis()) -= m;
  g.singularity()(0) -= m;
 }

 inline void operator+=(gf_view<imfreq> g, std::complex<double> a) {
  operator+=(g, arrays::make_unit_matrix(get_target_shape(g)[0], a));
 }
 inline void operator-=(gf_view<imfreq> g, std::complex<double> a) {
  operator-=(g, arrays::make_unit_matrix(get_target_shape(g)[0], a));
 }


 inline gf<imfreq> operator+(gf<imfreq> g, arrays::matrix<std::complex<double>> const &m) {
  g() += m;
  return g;
 }

 inline gf<imfreq> operator+(gf<imfreq> g, std::complex<double> const &m) {
  g() += m; // () is critical of infinite loop -> segfault
  return g;
 }

 inline gf<imfreq> operator+(std::complex<double> const &m, gf<imfreq> g) { return g + m; }
 inline gf<imfreq> operator+(arrays::matrix<std::complex<double>> const &m, gf<imfreq> g) { return g + m; }

 inline gf<imfreq> operator-(gf<imfreq> g, arrays::matrix<std::complex<double>> const &m) {
  g() -= m;
  return g;
 }

 inline gf<imfreq> operator-(gf<imfreq> g, std::complex<double> const &m) {
  g() -= m;
  return g;
 }

 inline gf<imfreq> operator-(std::complex<double> const &m, gf<imfreq> g) { 
  g *= -1;
  g+=m;
  return g;
  }

 inline gf<imfreq> operator-(arrays::matrix<std::complex<double>> const &m, gf<imfreq> g) { 
  g *= -1;
  g+=m;
  return g;
 }


}
}
