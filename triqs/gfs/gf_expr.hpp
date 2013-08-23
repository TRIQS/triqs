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
#ifndef TRIQS_GF_EXPR_H
#define TRIQS_GF_EXPR_H
#include <triqs/utility/expression_template_tools.hpp>
namespace triqs { namespace gfs { 
 using utility::is_in_ZRC;
 namespace gfs_expr_tools { 
  template<typename S> struct scalar_wrap {
   typedef S value_type; 
   S s; scalar_wrap(S const &s_):s(s_){} 
   S singularity() const { return s;}
   S data() const { return s;} 
   template<typename KeyType> value_type operator[](KeyType && key) const { return s;}
   template<typename ... Args> inline value_type operator()(Args && ... args) const { return s;}
   friend std::ostream &operator <<(std::ostream &sout, scalar_wrap const &expr){return sout << expr.s; }
  };

  // Combine the two meshes of LHS and RHS : need to specialize where there is a scalar
  struct combine_mesh {
   template<typename L, typename R> 
    inline auto operator() (L const & l, R const & r) const -> decltype(l.mesh()) { 
     if (!(l.mesh() == r.mesh())) TRIQS_RUNTIME_ERROR << "Mesh mismatch : ";//<< l.mesh()<<" vs" <<r.mesh();
     return l.mesh();
    }
   template<typename S, typename R> auto operator() (scalar_wrap<S> const & w, R const & r) const -> decltype(r.mesh()) { return r.mesh();}
   template<typename S, typename L> auto operator() (L const & l, scalar_wrap<S> const & w) const -> decltype(l.mesh()) { return l.mesh();}
  };

  template<typename T> struct keeper_type : std::conditional<utility::is_in_ZRC<T>::value, scalar_wrap<T>, typename view_type_if_exists_else_type<T>::type> {};
 }// gfs_expr_tools

 template<typename Descriptor, typename Tag, typename L, typename R>  struct gf_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction),gf_tag<Descriptor> {
  typedef typename gfs_expr_tools::keeper_type<L>::type L_t;
  typedef typename gfs_expr_tools::keeper_type<R>::type R_t;
  typedef Descriptor  descriptor_t;
  //typedef typename std::result_of<utility::operation<Tag>(typename L_t::value_type,typename R_t::value_type)>::type  value_t;
  typedef typename std::remove_reference<typename std::result_of<gfs_expr_tools::combine_mesh(L_t,R_t)>::type>::type        mesh_t;
  //typedef typename Descriptor::singularity_t::view_type    singularity_view_t;
  //typedef value_t value_type;
  L_t l; R_t r;
  template<typename LL, typename RR> gf_expr(LL && l_, RR && r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}
  mesh_t mesh() const  { return gfs_expr_tools::combine_mesh()(l,r); } 
  auto data() const ->decltype( utility::operation<Tag>()(l.data(), r.data())) {  return utility::operation<Tag>()(l.data(), r.data());}
  auto singularity() const DECL_AND_RETURN (utility::operation<Tag>()(l.singularity() , r.singularity()));
  //const singularity_view_t singularity() const { return utility::operation<Tag>()(l.singularity() , r.singularity());}
  //symmetry_t const & symmetry() const { return _symmetry;}
  template<typename KeyType> auto operator[](KeyType && key) const DECL_AND_RETURN(utility::operation<Tag>()(l[std::forward<KeyType>(key)] , r[std::forward<KeyType>(key)]));
  template<typename ... Args> auto operator()(Args && ... args) const DECL_AND_RETURN(utility::operation<Tag>()(l(std::forward<Args>(args)...) , r(std::forward<Args>(args)...)));
  friend std::ostream &operator <<(std::ostream &sout, gf_expr const &expr){return sout << "("<<expr.l << " "<<utility::operation<Tag>::name << " "<<expr.r<<")" ; }
 };

 // -------------------------------------------------------------------
 //a special case : the unary operator !
 template<typename Descriptor, typename L>   struct gf_unary_m_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction),gf_tag<Descriptor>{
  typedef typename gfs_expr_tools::keeper_type<L>::type L_t;
  typedef Descriptor  descriptor_t;
  //typedef typename L_t::value_type value_type;
  typedef typename L_t::mesh_t mesh_t;
  //typedef typename Descriptor::singularity_t::view_type    singularity_view_t;
  L_t l; 
  template<typename LL> gf_unary_m_expr(LL && l_) : l(std::forward<LL>(l_)) {}
  mesh_t mesh() const  { return l.mesh(); } 
  auto data() const ->decltype( - l.data()) {  return - l.data();}
  auto singularity() const DECL_AND_RETURN(l.singularity());
  //const singularity_view_t singularity() const { return l.singularity();}
  //symmetry_t const & symmetry() const { return _symmetry;}
  template<typename KeyType> auto operator[](KeyType&& key) const DECL_AND_RETURN( -l[key]); 
  template<typename ... Args> auto operator()(Args && ... args) const DECL_AND_RETURN( -l(std::forward<Args>(args)...));
  friend std::ostream &operator <<(std::ostream &sout, gf_unary_m_expr const &expr){return sout << '-'<<expr.l; }
 };

 // -------------------------------------------------------------------
 // get the descriptor ...
 template<typename A1, typename A2, typename Enable=void> struct get_desc;
 template<typename A1, typename A2>
  struct get_desc <A1,A2, typename std::enable_if<((!ImmutableGreenFunction<A1>::value) && (ImmutableGreenFunction<A2>::value))>::type > { 
   typedef typename A2::descriptor_t type;
  };
 template<typename A1, typename A2>
  struct get_desc <A1,A2,typename std::enable_if<(!ImmutableGreenFunction<A2>::value) && (ImmutableGreenFunction<A1>::value)>::type >  { 
   typedef typename A1::descriptor_t type;
  };
 template<typename A1, typename A2>
  struct get_desc <A1,A2, typename std::enable_if< (ImmutableGreenFunction<A1>::value) && (ImmutableGreenFunction<A2>::value) && std::is_same<typename A1::descriptor_t,typename A2::descriptor_t>::value >::type >  { typedef typename A2::descriptor_t type; };

 // -------------------------------------------------------------------
 // Now we can define all the C++ operators ...
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2) \
 template<typename A1, typename A2>\
 typename std::enable_if<TRAIT1<A1>::value && TRAIT2 <A2>::value, gf_expr<typename get_desc<A1,A2>::type, utility::tags::TAG, A1,A2>>::type\
 operator OP (A1 const & a1, A2 const & a2) { return gf_expr<typename get_desc<A1,A2>::type,utility::tags::TAG, A1,A2>(a1,a2);} 

 DEFINE_OPERATOR(plus,       +, ImmutableGreenFunction,ImmutableGreenFunction);
 DEFINE_OPERATOR(minus,      -, ImmutableGreenFunction,ImmutableGreenFunction);
 DEFINE_OPERATOR(multiplies, *, ImmutableGreenFunction,ImmutableGreenFunction);
 DEFINE_OPERATOR(multiplies, *, is_in_ZRC,ImmutableGreenFunction);
 DEFINE_OPERATOR(multiplies, *, ImmutableGreenFunction,is_in_ZRC);
 DEFINE_OPERATOR(divides,    /, ImmutableGreenFunction,ImmutableGreenFunction);
 DEFINE_OPERATOR(divides,    /, is_in_ZRC,ImmutableGreenFunction);
 DEFINE_OPERATOR(divides,    /, ImmutableGreenFunction,is_in_ZRC);
#undef DEFINE_OPERATOR

 // the unary is special
 template<typename A1> typename std::enable_if<ImmutableGreenFunction<A1>::value, gf_unary_m_expr<typename A1::descriptor_t, A1>>::type
  operator - (A1 const & a1) { return  gf_unary_m_expr<typename A1::descriptor_t, A1>(a1);} 

}}//namespace triqs::gf
#endif


