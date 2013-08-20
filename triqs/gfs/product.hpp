/*******************************************************************************
 * 
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#ifndef TRIQS_GF_PRODUCT_H
#define TRIQS_GF_PRODUCT_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./meshes/product.hpp"

namespace triqs { namespace gfs { 

 template<typename ... Ms> struct cartesian_product{ 
  typedef std::tuple<Ms...> type; 
  static constexpr size_t size = sizeof...(Ms); 
 };

 namespace gfs_implementation { 
  // the mesh is simply a cartesian product
  template<typename Opt, typename ... Ms> struct mesh<cartesian_product<Ms...>,Opt>  { 
   typedef mesh_product< typename mesh<Ms,Opt>::type ... > type;
   typedef std::tuple<Ms...> mesh_name_t;
   static type make (typename mesh<Ms,Opt>::type ... ms) { return type{std::move(ms)...};} 
  };

  // h5 name : name1_x_name2_.....
  template<typename Opt, typename ... Ms> struct h5_name<cartesian_product<Ms...>,matrix_valued,Opt>  { 
   static std::string invoke(){ 
    return  triqs::tuple::fold(
      [](std::string a, std::string b) { return a + std::string(b.empty()?"" : "_x_") + b;}, 
      std::make_tuple(h5_name<Ms,matrix_valued,Opt>::invoke()...), 
      std::string());
   }
  };

  /// ---------------------------  data access  ---------------------------------

  template<typename Opt, typename ... Ms> struct data_proxy<cartesian_product<Ms...>,matrix_valued,Opt> : data_proxy_array<std::complex<double>,3> {};
  template<typename Opt, typename ... Ms> struct data_proxy<cartesian_product<Ms...>,scalar_valued,Opt> : data_proxy_array<std::complex<double>,1> {};

  /// ---------------------------  evaluator ---------------------------------

  struct evaluator_grid_simple { 
   size_t n; 
   evaluator_grid_simple() = default;

   template<typename MeshType, typename PointType> 
    evaluator_grid_simple (MeshType const & m, PointType const & p) { n=p; }
   template<typename F> auto operator()(F const & f) const DECL_AND_RETURN(f (n));
  };

  struct evaluator_grid_linear_interpolation { 
   double w1, w2; size_t n1, n2; 

   evaluator_grid_linear_interpolation() = default;

   template<typename MeshType, typename PointType> 
    evaluator_grid_linear_interpolation (MeshType const & m, PointType const & p, double prefactor=1) { // delegate !
     bool in; double w;
     std::tie(in, n1, w) = windowing(m,p);
     //std::cout  << in << " "<< n1 << " "<< w << " " << p << std::endl;
     if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
     w1 = prefactor * w; w2 = prefactor *(1-w); n2 = n1 +1;
    }

   template<typename F> auto operator()(F const & f) const DECL_AND_RETURN(w1 * f(n1) + w2 * f (n2));
  };

  template<typename MeshType> struct evaluator_fnt_on_mesh;

  // can not use inherited constructors, too recent...
#define TRIQS_INHERIT_AND_FORWARD_CONSTRUCTOR(NEWCLASS,CLASS) : CLASS { template<typename ...T> NEWCLASS(T &&... t) : CLASS(std::forward<T>(t)...){};}; 

  template<> struct evaluator_fnt_on_mesh<imfreq> TRIQS_INHERIT_AND_FORWARD_CONSTRUCTOR(evaluator_fnt_on_mesh, evaluator_grid_simple);
  template<> struct evaluator_fnt_on_mesh<imtime> TRIQS_INHERIT_AND_FORWARD_CONSTRUCTOR(evaluator_fnt_on_mesh, evaluator_grid_linear_interpolation);
  template<> struct evaluator_fnt_on_mesh<retime> TRIQS_INHERIT_AND_FORWARD_CONSTRUCTOR(evaluator_fnt_on_mesh, evaluator_grid_linear_interpolation);
  template<> struct evaluator_fnt_on_mesh<refreq> TRIQS_INHERIT_AND_FORWARD_CONSTRUCTOR(evaluator_fnt_on_mesh, evaluator_grid_linear_interpolation);


  /** 
   * This the multi-dimensional evaluator.
   * It combine the evaluator of each components, as long as they are  a linear form 
   * eval(g, x) = \sum_i w_i g( n_i(x)) , with w some weight and n_i some points on the grid.
   * Mathematically, it is written as (example of evaluating g(x1,x2,x3,x4)).
   * Notation : eval(X)  : g -> g(X)
   * eval(x1,x2,x3,x4) (g) = eval (x1) ( binder ( g, (),  (x2,x3,x4)) )
   * binder( g, (), (x2,x3,x4)) (p1) = eval(x2)(binder (g,(p1),(x3,x4)))
   * binder( g, (p1), (x3,x4))  (p2) = eval(x3)(binder (g,(p1,p2),(x4)))
   * binder( g, (p1,p2), (x4))  (p3) = eval(x4)(binder (g,(p1,p2,p3),()))
   * binder( g, (p1,p2,p3),())  (p4) = g[p1,p2,p3,p4]
   *
   * p_i are points on the grids, x_i points in the domain.
   *
   * Unrolling the formula gives (for 2 variables, with 2 points interpolation)
   * eval(xa,xb) (g) = eval (xa) ( binder ( g, (),  (xb)) ) = w_1(xa)  binder ( g, (),  (xb))( n_1(xa)) + w_2(xa)  binder ( g, (),  (xb))( n_2(xa))
   *  =  w_1(xa) ( eval(xb)(  binder ( g, (n_1(xa) ),  ())))  + 1 <-> 2 
   *  =  w_1(xa) ( W_1(xb) *  binder ( g, (n_1(xa) ),  ())(N_1(xb)) + 1<->2 )  + 1 <-> 2 
   *  =  w_1(xa) ( W_1(xb) *  g[n_1(xa), N_1(xb)] + 1<->2 )  + 1 <-> 2 
   *  =  w_1(xa) ( W_1(xb) *  g[n_1(xa), N_1(xb)] + W_2(xb) *  g[n_1(xa), N_2(xb)] )  + 1 <-> 2 
   *  which is the expected formula
   */
  // implementation : G = gf, Tn : tuple of n points, Ev : tuple of evaluators (the evals functions), pos = counter from #args-1 =>0
  // NB : the tuple is build in reverse with respect to the previous comment.
  template<typename G, typename Tn, typename Ev, int pos> struct binder;

  template<int pos, typename G, typename Tn, typename Ev> 
   binder<G,Tn,Ev,pos> make_binder(G const * g, Tn tn, Ev const & ev) { return binder<G,Tn,Ev,pos>{g, std::move(tn), ev}; }

  template<typename G, typename Tn, typename Ev, int pos> struct binder {
   G const * g; Tn tn; Ev const & evals;
   auto operator()(size_t p) const DECL_AND_RETURN( std::get<pos>(evals) ( make_binder<pos-1>(g, triqs::tuple::push_front(tn,p), evals) ));
  };

  template<typename G, typename Tn, typename Ev> struct binder<G,Tn,Ev,-1> {
   G const * g; Tn tn; Ev const & evals;
   auto operator()(size_t p) const DECL_AND_RETURN( triqs::tuple::apply(on_mesh(*g), triqs::tuple::push_front(tn,p)));
  };

  // now the multi d evaluator itself.
  template<typename Target, typename Opt, typename ... Ms>
   struct evaluator<cartesian_product<Ms...>,Target,Opt> {
    static constexpr int arity = sizeof...(Ms);
    mutable std::tuple< evaluator_fnt_on_mesh<Ms> ... > evals;

    struct _poly_lambda {// replace by a polymorphic lambda in C++14 
     template<typename A, typename B, typename C> void operator()(A & a, B const & b, C const & c) const { a = A{b,c};}
    };

    template<typename G, typename ... Args>
     std::complex<double> operator() (G const * g, Args && ... args)  const {
      static constexpr int R = sizeof...(Args);
      // build the evaluators, as a tuple of ( evaluator<Ms> ( mesh_component, args))
      triqs::tuple::call_on_zip(_poly_lambda(), evals, g->mesh().components(), std::make_tuple(args...));
      return std::get<R-1>(evals) (make_binder<R-2> (g, std::make_tuple(), evals) );
     } 
   };

  // -------------------------------   Factories  --------------------------------------------------

  template<typename Opt, typename ... Ms>
   struct factories<cartesian_product<Ms...>, scalar_valued,Opt> {
    typedef gf<cartesian_product<Ms...>, scalar_valued,Opt> gf_t;

    template<typename ... Meshes>
     static gf_t make_gf(Meshes && ... meshes) { 
      auto m =  make_gf_mesh<cartesian_product<Ms...>,Opt>(meshes...);
      typename gf_t::data_regular_t A(m.size()); 
      A() =0;
      return gf_t (m, std::move(A), nothing(), nothing());
     }
   };

 } // gf_implementation

}}
#endif

