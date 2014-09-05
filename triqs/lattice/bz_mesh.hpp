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
#pragma once
#include "./brillouin_zone.hpp"
#include "../gfs/tools.hpp"
#include "../gfs/meshes/mesh_tools.hpp"

namespace triqs {
namespace lattice {

 struct bz_mesh {

  using domain_t = brillouin_zone;
  using index_t = long;
  using linear_index_t = long;
  using domain_pt_t = typename domain_t::point_t;

  bz_mesh() = default;
  bz_mesh(brillouin_zone const &bz, int n_l);
  bz_mesh(brillouin_zone const &bz, std::vector<k_t> k_pt_stack) : bz(bz), k_pt_stack(std::move(k_pt_stack)) {}

  domain_t const &domain() const { return bz; }
  size_t size() const { return k_pt_stack.size(); }

  ///
  utility::mini_vector<size_t, 1> size_of_components() const {
   return {size()};
  }

  /// Conversions point <-> index <-> linear_index
  domain_pt_t const &index_to_point(index_t i) const { return k_pt_stack[i]; }

  long index_to_linear(index_t ind) const { return ind; }

  // f (k) -> void where k is a k_t, a point in the BZ
  template <typename F> friend void foreach(bz_mesh const &m, F f) {
   for (auto const &k : m.k_pt_stack) f(k);
  }

  // locate the closest point : VERY PRIMITIVE : TO BE IMPROVED (kd-tree ?) 
  long locate_neighbours(k_t k) const;

  /// The wrapper for the mesh point
  class mesh_point_t : gfs::tag::mesh_point, public utility::arithmetic_ops_by_cast<mesh_point_t, domain_pt_t> {
   bz_mesh const *m;
   index_t _index;

   public:
   mesh_point_t() : m(nullptr) {}
   mesh_point_t(bz_mesh const &mesh, index_t const &index_) : m(&mesh), _index(index_) {}
   mesh_point_t(bz_mesh const &mesh) : mesh_point_t(mesh, 0) {}
   void advance() { ++_index; }
   using cast_t = domain_pt_t;
   operator cast_t() const { return m->index_to_point(_index); }
   long linear_index() const { return _index; }
   long index() const { return _index; }
   bool at_end() const { return (_index == m->size()); }
   void reset() { _index = 0; }
   double operator()(int i) const { return m->index_to_point(_index)(i);}
   double operator[](int i) const { return m->index_to_point(_index)(i);}
  };

  /// Accessing a point of the mesh
  mesh_point_t operator[](index_t i) const { return mesh_point_t(*this, i); }

  /// Iterating on all the points...
  using const_iterator = gfs::mesh_pt_generator<bz_mesh>;
  const_iterator begin() const { return const_iterator(this); }
  const_iterator end() const { return const_iterator(this, true); }
  const_iterator cbegin() const { return const_iterator(this); }
  const_iterator cend() const { return const_iterator(this, true); }

  /// Mesh comparison
  bool operator==(bz_mesh const &M) const { return ( (size() == M.size())); }
  //bool operator==(bz_mesh const &M) const { return ((bz == M.bz) && (size() == M.size())); }
  bool operator!=(bz_mesh const &M) const { return !(operator==(M)); }

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, bz_mesh const &m);

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, bz_mesh &m);

  //  BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   ar &TRIQS_MAKE_NVP("domain", bz);
   ar &TRIQS_MAKE_NVP("k_pt_stack", k_pt_stack);
  }

  friend std::ostream &operator<<(std::ostream &sout, bz_mesh const &m) { return sout << "Mesh over BZ "; }

  private:
  domain_t bz;
  std::vector<k_t> k_pt_stack;
 };
}
}

