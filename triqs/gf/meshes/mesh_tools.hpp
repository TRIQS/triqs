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
#ifndef TRIQS_GF_MESHTOOLS_H
#define TRIQS_GF_MESHTOOLS_H
#include "../tools.hpp"

namespace triqs { namespace gf {

 // Derive from this object using CRTP to provide arithmetic operation by casting the final object to C 
 template<typename Derived, typename C> struct arith_ops_by_cast {};
#define IMPL_OP(OP)\
 template<typename D, typename C, typename T> \
 auto operator OP(arith_ops_by_cast<D,C> const & x, T const & y) -> decltype( std::declval<C>() OP y) {return C(static_cast<D const& >(x)) OP y;}\
 template<typename D, typename C, typename T> \
 auto operator OP( T const & y, arith_ops_by_cast<D,C> const & x) -> TYPE_DISABLE_IF(decltype (y OP std::declval<C>()), std::is_same<T,D>)  {return y OP C(static_cast<D const& >(x));}
 IMPL_OP(+); IMPL_OP(-); IMPL_OP(*); IMPL_OP(/);
#undef IMPL_OP
 
 //TYPE_DISABLE_IF(decltype (std::declval<T>() OP std::declval<C>()), std::is_same<T,D>)\
 //operator OP( T const & y, arith_ops_by_cast<D,C> const & x)  {return y OP C(static_cast<D const& >(x));}

 //------------------------------------------------------

 template<typename MeshType>
  class mesh_pt_generator : 
   public boost::iterator_facade< mesh_pt_generator<MeshType>, typename MeshType::mesh_point_t , boost::forward_traversal_tag, 
   typename MeshType::mesh_point_t const & > {
    friend class boost::iterator_core_access;
    MeshType const * mesh;
    size_t u;
    typename MeshType::mesh_point_t pt;
    typename MeshType::mesh_point_t const & dereference() const { return pt;}
    bool equal(mesh_pt_generator const & other) const { return ((other.u==u) );}
    // do NOT check = of mesh, otherwise e.g. block iterator does not work (infinite loop...)
    //bool equal(mesh_pt_generator const & other) const { return ((mesh == other.mesh) && (other.u==u) );}
    public:
    mesh_pt_generator( MeshType const * m=NULL, bool atEnd = false): mesh(m), u(atEnd ? m->size(): 0), pt((*m)[typename MeshType::index_t()]) {}
    void increment() { ++u; pt.advance(); }
    bool at_end() const { return (u>=mesh->size());}
    typename MeshType::domain_t::point_t to_point() const { return pt;}    
   };


}}
#endif
