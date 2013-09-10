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
#ifndef TRIQS_GF_MESH_PRODUCT_H
#define TRIQS_GF_MESH_PRODUCT_H
#include "./mesh_tools.hpp"
#include "../domains/product.hpp"
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/utility/mini_vector.hpp>
namespace triqs { namespace gfs {

 template<typename... Meshes> struct mesh_product : tag::composite {
   typedef domain_product<typename Meshes::domain_t ... >  domain_t;
   typedef std::tuple<typename Meshes::index_t ... >       index_t;
   typedef std::tuple<Meshes...>                           m_tuple_t;
   typedef std::tuple<typename Meshes::mesh_point_t ...>   m_pt_tuple_t;
   typedef typename domain_t::point_t                      domain_pt_t;

   static constexpr int dim = sizeof...(Meshes);

   mesh_product () {}
   mesh_product (Meshes const & ... meshes) : m_tuple(meshes...), _dom(meshes.domain()...) {}

   domain_t  const & domain()     const { return _dom;}
   m_tuple_t const & components() const { return m_tuple;}
   m_tuple_t & components() { return m_tuple;}

   /// size of the mesh is the product of size
   struct _aux0 { template<typename M> size_t operator()(M const & m, size_t R) { return R*m.size();}};
   size_t size() const { return triqs::tuple::fold(_aux0(), m_tuple, 1);}

   /// Conversions point <-> index <-> linear_index
   struct _aux1 { template<typename P, typename M, typename I> void operator()(P & p, M const & m, I const& i) {p = m.index_to_point(i);}};
   typename domain_t::point_t index_to_point(index_t const & ind) const { domain_pt_t res; triqs::tuple::apply_on_zip(_aux1(), res,m_tuple,ind); return res;}

   // index[0] + component[0].size * (index[1] + component[1].size* (index[2] + ....))
   struct _aux2 { template<typename I, typename M> size_t operator()(M const & m, I const & i,size_t R) {return m.index_to_linear(i) + R * m.size();}};
   size_t index_to_linear(index_t const & ii) const { return triqs::tuple::fold_on_zip(_aux2(), m_tuple, ii, size_t(0)); }

   // Same but a tuple of mesh_point_t
   struct _aux3 { template<typename P, typename M> size_t operator()(M const & m, P const & p,size_t R) {return p.linear_index() + R * m.size();}};
   size_t mp_to_linear(m_pt_tuple_t const & mp) const { return triqs::tuple::fold_on_zip(_aux3(), m_tuple, mp, size_t(0)); }

   //
   struct _aux4 { template< typename M, typename V> V * operator()(M const & m, V * v) {*v = m.size(); return ++v;}};
   utility::mini_vector<size_t,dim> all_size_as_mini_vector () const { 
    utility::mini_vector<size_t,dim> res;
    triqs::tuple::fold(_aux4(), m_tuple, &res[0] ); 
    return res;
   }

   // Same but a variadic list of mesh_point_t
   template<typename ... MP> size_t mesh_pt_components_to_linear(MP const &  ... mp) const {
    static_assert(std::is_same< std::tuple<MP...>, m_pt_tuple_t>::value, "Call incorrect ");
    //static_assert(std::is_same< std::tuple<typename std::remove_cv<typename std::remove_reference<MP>::type>::type...>, m_pt_tuple_t>::value, "Call incorrect ");
    return mp_to_linear(std::forward_as_tuple(mp...));
   } // speed test ? or make a variadic fold...

   /// The wrapper for the mesh point
   class mesh_point_t : tag::mesh_point{
    const mesh_product * m;
    m_pt_tuple_t _c; bool _atend;
    struct F2 { template<typename M> typename M::mesh_point_t operator()(M const & m, typename M::index_t const & i) const { return m[i];}};
    struct F1 { template<typename M> typename M::mesh_point_t operator()(M const & m) const { return m[typename M::index_t()];}};
    public :
    mesh_point_t(mesh_product const & m_, index_t index_ ) : m(&m_), _c (triqs::tuple::apply_on_zip(F2(), m_.m_tuple, index_)), _atend(false) {}
    mesh_point_t(mesh_product const & m_)                  : m(&m_), _c (triqs::tuple::apply(F1(), m_.m_tuple)), _atend(false)    {}
    m_pt_tuple_t const & components_tuple() const { return _c;}
    size_t linear_index() const { return m->mp_to_linear(_c);}
    const mesh_product * mesh() const { return m;}

    typedef domain_pt_t cast_t;
    operator cast_t() const { return m->index_to_point(index);}

    // index[0] +=1; if index[0]==m.component[0].size() { index[0]=0; index[1] +=1; if  ....}  and so on until dim
    struct _aux1 { template<typename P> bool operator()(P & p, bool done)
     {if (done) return true; p.advance(); if (p.at_end()) {p.reset(); return false;} return true;}
    };
    void advance() { triqs::tuple::fold(_aux1(), _c, false);}

    //index_t index() const { return _index;} // not implemented yet
    bool at_end() const { return _atend;}

    struct _aux{ template<typename M> size_t operator()(M & m,size_t ) { m.reset(); return 0;}};
    void reset() { _atend = false; triqs::tuple::fold(_aux(), _c,0);}
   };// end mesh_point_t

   /// Accessing a point of the mesh
   mesh_point_t operator[](index_t i) const { return mesh_point_t(*this, i);}
   mesh_point_t operator()(typename Meshes::index_t ... i) const { return (*this)[std::make_tuple(i...)];}

   /// Iterating on all the points...
   typedef  mesh_pt_generator<mesh_product> const_iterator;
   const_iterator begin() const { return const_iterator (this);}
   const_iterator end()   const { return const_iterator (this, true);}
   const_iterator cbegin() const { return const_iterator (this);}
   const_iterator cend()   const { return const_iterator (this, true);}

   /// Mesh comparison
   friend bool operator == (mesh_product const & M1, mesh_product const & M2) { return M1.m_tuple==M2.m_tuple; }

   /// Write into HDF5
   struct _auxh5w {
    h5::group gr; _auxh5w( h5::group gr_) : gr(gr_) {} //icc has yet another bug on new initialization form with {}...
    template<typename M> size_t operator()(M const & m, size_t N) { std::stringstream fs;fs <<"MeshComponent"<< N; h5_write(gr,fs.str(), m); return N+1; }
   };
   friend void h5_write (h5::group fg, std::string subgroup_name, mesh_product const & m) {
    h5::group gr =  fg.create_group(subgroup_name);
    //h5_write(gr,"domain",m.domain());
    triqs::tuple::fold(_auxh5w(gr), m.components(), size_t(0));
   }

   /// Read from HDF5
   struct _auxh5r {
    h5::group gr;_auxh5r( h5::group gr_) : gr(gr_) {}
    template<typename M> size_t operator()(M & m, size_t N) { std::stringstream fs;fs <<"MeshComponent"<< N; h5_read(gr,fs.str(), m); return N+1; }
   };
   friend void h5_read  (h5::group fg, std::string subgroup_name, mesh_product & m){
    h5::group gr = fg.open_group(subgroup_name);
    //h5_read(gr,"domain",m._dom);
    triqs::tuple::fold(_auxh5r(gr), m.components(), size_t(0));
   }

   //  BOOST Serialization
   friend class boost::serialization::access;
   template<typename Archive> struct _aux_ser {
    Archive & ar;_aux_ser( Archive & ar_) : ar(ar_) {}
    template<typename M> size_t operator()(M & m, size_t N) {
     std::stringstream fs;fs <<"MeshComponent"<< N;
     ar & boost::serialization::make_nvp(fs.str().c_str(),m);
     return N+1;
    }
   };
   template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
     triqs::tuple::fold(_aux_ser<Archive>(ar), m_tuple, size_t(0));
    }

  private:
   m_tuple_t  m_tuple;
   domain_t _dom;
 };

 //template<int pos, typename ... M> 
 //typename std::tuple_element<pos,typename mesh_product<M...>::index_t>::type get_index1(typename mesh_product<M...>::mesh_point_t const & p) { return std::get<pos>(p.components_tuple());}

 template<int pos, typename P> 
  auto get_index(P const & p) DECL_AND_RETURN( std::get<pos>(p.components_tuple()).index());

 template<int pos, typename P> 
  auto get_point(P const & p) DECL_AND_RETURN( std::get<pos>( p.mesh()->components() ).index_to_point( std::get<pos>(p.components_tuple()).index() ) );
 
 template<int pos, typename P> 
  auto get_component(P const & p) DECL_AND_RETURN( std::get<pos>(p.components_tuple()));
 
 // C++14
 //auto get_point(P const & p) { return std::get<pos> (p.mesh()->components()).index_to_point( std::get<pos>(p.components_tuple()));}

 // Given a composite mesh m , and a linear array of storage A
 // reinterpret_linear_array(m,A) returns a d-dimensionnal view of the array
 // with indices egal to the indices of the components of the mesh.
 // Very useful for slicing, currying functions.
 template<typename ... Meshes, typename T, ull_t OptionsFlags >
  arrays::array_view<T, sizeof...(Meshes),OptionsFlags, arrays::indexmaps::mem_layout::fortran_order(sizeof...(Meshes)) >
  reinterpret_linear_array(mesh_product<Meshes...> const & m, arrays::array_view<T,1,OptionsFlags> const & A) {
   return { {m.all_size_as_mini_vector()}, A.storage()};
  }
  /* static int constexpr rank = sizeof...(Meshes);
   typedef arrays::array_view<T, sizeof...(Meshes),OptionsFlags, arrays::indexmaps::mem_layout::fortran_order(rank)> return_t;
   typedef typename return_t::indexmap_type im_t;
   auto l = m.all_size_as_mini_vector();
   typename im_t::strides_type sv;
   std::ptrdiff_t s= 1;
   for (int u=0; u<rank; ++u) { sv[u] = s; s *= l[u];} // fortran type folding 
   return return_t (im_t (l,sv,0) , A.storage());

   // why fortran ??
   // why compute the stride ?? just use return { {l}, A.storage()} ;
*/



}}
#endif
