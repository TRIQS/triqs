// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include <triqs/arrays.hpp>
#include <string>
#include <vector>

namespace triqs::lattice {

  using r_t      = nda::vector<double>;
  using matrix_t = nda::matrix<double>;

  class bravais_lattice {

    public:
    /**
       * Construct a Bravais Lattice with given unit vectors and atomic positions
       *
       * @param units Matrix with unit vectors of the Bravais Lattice as rows.
       * @param atom_orb_pos Vector of atomic orbital positions within the unit cell.
       *  Defaults to {{0,0,0}}
       * @param atom_orb_name Vector of atomic orbitals names.
       *  Defaults to vector of empty strings.
       */
    bravais_lattice(matrix_t const &units, std::vector<r_t> atom_orb_pos = std::vector<r_t>{{0, 0, 0}}, std::vector<std::string> atom_orb_name = {});

    // Default construction, 3d cubic lattice
    bravais_lattice() : bravais_lattice(nda::eye<double>(3)) {}

    /// Check if a k_t is part of the domain
    [[nodiscard]] bool contains(r_t const &) const { return true; }

    /// Number of dimensions
    int ndim() const { return ndim_; }

    /// Matrix containing lattice basis vectors as rows
    matrix_t const &units() const { return units_; }

    // Number of orbitals in the unit cell
    long n_orbitals() const { return long(atom_orb_name.size()); }

    /// Return the vector of orbital positions
    std::vector<r_t> const &orbital_positions() { return atom_orb_pos; }

    /// Return the vector of orbital names
    std::vector<std::string> const &orbital_names() { return atom_orb_name; }

    // -------------------- Point ---------------------

    struct point_t {
      private:
      std::array<long, 3> _index       = {0, 0, 0};
      bravais_lattice const *_bl_ptr   = nullptr;
      mutable std::optional<r_t> _rval = {};

      public:
      point_t() = default;
      point_t(std::array<long, 3> const &index, bravais_lattice const *bl_ptr) : _index(index), _bl_ptr(bl_ptr) {}

      /// The index of the mesh point
      [[nodiscard]] std::array<long, 3> index() const { return _index; }

      /// The underlying bravais lattice
      [[nodiscard]] bravais_lattice const &lattice() const { return *_bl_ptr; }

      explicit operator r_t() const {
        if (_rval)
          return *_rval;
        else
          return *(_rval = _bl_ptr->lattice_to_real_coordinates(_index));
      }

      [[deprecated("() is deprecated for a cyclat::mesh_point_t. Use [] instead")]] double operator()(int d) const { return r_t(*this)[d]; }
      double operator[](int d) const { return r_t(*this)[d]; }

      point_t operator+(point_t const &y) const {
        EXPECTS(*_bl_ptr == *(y._bl_ptr));
        return {_index + y._index, _bl_ptr};
      }
      point_t operator-(point_t const &y) const {
        EXPECTS(*_bl_ptr == *(y._bl_ptr));
        return {_index - y._index, _bl_ptr};
      }
      point_t operator-() const { return {-_index, _bl_ptr}; }

      friend std::ostream &operator<<(std::ostream &out, point_t const &x) { return out << (r_t)x; }
    };

    /// Transform from lattice to real coordinates.
    template <typename R> r_t lattice_to_real_coordinates(R const &x) const {
      return transpose(units_)(range::all, range(ndim())) * nda::basic_array_view{x}(range(ndim()));
    }

    /// Transform from lattice to real coordinates.
    template <typename R> r_t real_to_lattice_coordinates(R const &x) const {
      return transpose(units_inv_)(range::all, range(ndim())) * nda::basic_array_view{x}(range(ndim()));
    }

    // ------------------- Comparison -------------------

    bool operator==(bravais_lattice const &bl) const { return units_ == bl.units() && ndim_ == bl.ndim() && n_orbitals() == bl.n_orbitals(); }

    bool operator!=(bravais_lattice const &bl) const { return !(operator==(bl)); }

    // -------------------- print -------------------

    friend std::ostream &operator<<(std::ostream &sout, bravais_lattice const &bl) {
      return sout << "Bravais Lattice with dimension " << bl.ndim() << ", units " << bl.units() << ", n_orbitals " << bl.n_orbitals();
    }

    // -------------------- hdf5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "bravais_lattice"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string subgroup_name, bravais_lattice const &bl);

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string subgroup_name, bravais_lattice &bl);

    // ---------------------------------------

    private:
    matrix_t units_     = matrix_t::zeros(3, 3);
    matrix_t units_inv_ = matrix_t::zeros(3, 3);
    std::vector<r_t> atom_orb_pos;          // atom_orb_pos[i] = position of ith atoms/orbitals in the unit cell
    std::vector<std::string> atom_orb_name; // names of these atoms/orbitals.
    int ndim_;
  };

} // namespace triqs::lattice
