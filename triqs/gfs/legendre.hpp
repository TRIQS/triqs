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
#ifndef TRIQS_GF_LEGENDRE_TIME_H
#define TRIQS_GF_LEGENDRE_TIME_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./domains/legendre.hpp"
#include "./meshes/discrete.hpp"

namespace triqs { namespace gfs {

 struct legendre {};

 namespace gfs_implementation { 

  // mesh type and its factories
  template<typename Opt> struct mesh<legendre,Opt> { 
   typedef discrete_mesh<legendre_domain> type;
   typedef typename type::domain_t domain_t;
   static type make(double beta, statistic_enum S, size_t n_leg) { return type(domain_t(beta,S,n_leg)); }
  };
  
  // h5 name
  template<typename Opt> struct h5_name<legendre,matrix_valued,Opt>      { static std::string invoke(){ return  "GfLegendre";}};

  /// ---------------------------  evaluator ---------------------------------

  template<typename Opt>
   struct evaluator<legendre,matrix_valued,Opt> {
    static constexpr int arity = 1;
    //ERROR : give a double and interpolate
    template<typename G>
     arrays::matrix_view<double >  operator() (G const * g,long n)  const {return g->data()(n, arrays::range(), arrays::range()); }
    template<typename G>
     local::tail_view operator()(G const * g,freq_infty const &) const {return g->singularity();}
   };

  /// ---------------------------  data access  ---------------------------------

  template<typename Opt> struct data_proxy<legendre,matrix_valued,Opt> : data_proxy_array<double,3> {};

  // -------------------------------   Factories  --------------------------------------------------

  template<typename Opt> struct factories<legendre, matrix_valued,Opt> {
   typedef gf<legendre, matrix_valued,Opt> gf_t;
   typedef typename mesh<legendre, Opt>::type mesh_t;

   static gf_t make_gf(double beta, statistic_enum S, tqa::mini_vector<size_t,2> shape, size_t n_leg) {
    typename gf_t::data_non_view_t A(shape.front_append(n_leg)); A() = 0;
    return gf_t(mesh<legendre,Opt>::make(beta, S, n_leg), std::move(A), nothing(), nothing());
   }

  };
 } // gfs_implementation

}}
#endif

