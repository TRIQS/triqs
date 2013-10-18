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

namespace triqs {
namespace gfs {

 // Derive from this object using CRTP to provide arithmetic operation by casting the final object to C
 // by not forwarding x, I assume the cast is a simple value, not a matrix, but this is ok
 template <typename Derived, typename C> struct arith_ops_by_cast {};

#define IMPL_OP(OP)                                                                                                              \
 template <typename D, typename C, typename Y>                                                                                   \
 auto operator OP(arith_ops_by_cast<D, C> const& x, Y&& y)->decltype(std::declval<C>() OP std::forward<Y>(y)) {                  \
  return C(static_cast<D const&>(x)) OP std::forward<Y>(y);                                                                      \
 }                                                                                                                               \
 template <typename D, typename C, typename Y>                                                                                   \
 auto operator OP(Y&& y, arith_ops_by_cast<D, C> const& x)                                                                       \
     ->TYPE_DISABLE_IF(decltype(std::forward<Y>(y) OP std::declval<C>()),                                                        \
                       std::is_same<typename std::remove_cv<typename std::remove_reference<Y>::type>::type, D>) {                \
  return std::forward<Y>(y) OP C(static_cast<D const&>(x));                                                                      \
 }

 IMPL_OP(+);
 IMPL_OP(-);
 IMPL_OP(*);
 IMPL_OP(/ );
#undef IMPL_OP


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
