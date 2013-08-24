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
#ifndef TRIQS_GF_MESH_DISCRETE_H
#define TRIQS_GF_MESH_DISCRETE_H
#include "./mesh_tools.hpp"
#include "../domains/discrete.hpp"

namespace triqs { namespace gfs { 

  template<typename Domain>
  struct discrete_mesh {

   typedef Domain domain_t;
   typedef size_t index_t; 

   discrete_mesh (domain_t && dom) : _dom(dom){}
   discrete_mesh (domain_t const & dom) : _dom(dom){} //icc has a bug 
   discrete_mesh () : _dom(){}
 
   domain_t const & domain() const { return _dom;}
   size_t size() const {return _dom.size();}

   /// Conversions point <-> index <-> discrete_index
   size_t  index_to_point (index_t ind) const {return ind;} 
   size_t  index_to_linear(index_t ind) const {return ind;}   

   /// The wrapper for the mesh point
   class mesh_point_t :  tag::mesh_point,public arith_ops_by_cast<mesh_point_t, size_t> {
    discrete_mesh const * m;  
    index_t _index;
    public:
    mesh_point_t( discrete_mesh const & mesh, index_t const & index_): m(&mesh), _index(index_) {}
    void advance() { ++_index;}
    typedef size_t cast_t;
    operator cast_t() const { return m->index_to_point(_index);} 
    size_t linear_index() const { return _index;}
    size_t index() const { return _index;}
    bool at_end() const { return (_index == m->size());}
    void reset() {_index =0;}
    };

   /// Accessing a point of the mesh
   mesh_point_t operator[](index_t i) const { return mesh_point_t (*this,i);}

   /// Iterating on all the points...
   typedef  mesh_pt_generator<discrete_mesh> const_iterator;
   const_iterator begin() const { return const_iterator (this);}
   const_iterator end()   const { return const_iterator (this, true);}
   const_iterator cbegin() const { return const_iterator (this);}
   const_iterator cend()   const { return const_iterator (this, true);}

   /// Mesh comparison
   bool operator == (discrete_mesh const & M) const { return (_dom == M._dom) ;} 

   /// Write into HDF5
   friend void h5_write (h5::group fg, std::string subgroup_name, discrete_mesh const & m) {
    h5::group gr =  fg.create_group(subgroup_name);
    h5_write(gr,"domain",m.domain());
   }

   /// Read from HDF5
   friend void h5_read  (h5::group fg, std::string subgroup_name, discrete_mesh & m){
    h5::group gr = fg.open_group(subgroup_name);
    typename discrete_mesh::domain_t dom;
    h5_read(gr,"domain",dom);
    m = discrete_mesh(std::move(dom));
   }
 
   //  BOOST Serialization
   friend class boost::serialization::access;
   template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
     ar & boost::serialization::make_nvp("domain",_dom);
    }

   private:
   domain_t _dom;
  };
}}
#endif

