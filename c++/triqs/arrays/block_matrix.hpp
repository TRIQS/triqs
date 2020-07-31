// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2016 Igor Krivenko
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell, tayral

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <mpi/vector.hpp>
#include <mpi/string.hpp>
#include <triqs/utility/is_complex.hpp>
#include <triqs/arrays.hpp>
#include <triqs/arrays/h5.hpp>
#include <boost/serialization/access.hpp>

namespace triqs {
  namespace arrays {

    /// Block-diagonal matrix with named blocks
    /**
   @tparam T Scalar type (double/dcomplex)
  */
    template <typename T> struct block_matrix {

      using matrix_t     = triqs::arrays::matrix<T>;
      using regular_type = block_matrix<T>;
      std::vector<std::string> block_names;
      std::vector<matrix_t> matrix_vec;

      block_matrix(std::vector<std::string> const &block_names, std::vector<matrix_t> const &matrix_vec)
         : block_names(block_names), matrix_vec(matrix_vec) {}
      block_matrix() : block_names(), matrix_vec() {}

      /// Number of diagonal blocks
      int size() const { return matrix_vec.size(); }

      /// Call operator with a string (slow)
      triqs::arrays::matrix_view<T> operator()(std::string const &name) {
        auto it = std::find(block_names.begin(), block_names.end(), name);
        if (it == block_names.end()) TRIQS_RUNTIME_ERROR << "block_matrix: Block name " << name << " is incorrect";
        return matrix_vec[std::distance(block_names.begin(), it)];
      }

      /// Subscript operator (fast)
      matrix_t &operator[](int i) { return matrix_vec[i]; }

      /// Subscript operator (fast)
      triqs::arrays::matrix_const_view<T> operator[](int i) const { return matrix_vec[i]; }

      // Add block matrix
      block_matrix &operator+=(block_matrix const &b) {
        assert(b.block_names == block_names);
        for (int i = 0; i < size(); ++i) matrix_vec[i] += b[i];
        return *this;
      }
      block_matrix operator+(block_matrix const &b) {
        auto res = *this;
        res += b;
        return res;
      }

      // Subtract block matrix
      block_matrix &operator-=(block_matrix const &b) {
        assert(b.block_names == block_names);
        for (int i = 0; i < size(); ++i) matrix_vec[i] -= b[i];
        return *this;
      }
      block_matrix operator-(block_matrix const &b) {
        auto res = *this;
        res -= b;
        return res;
      }

      // Multiply by block matrix
      block_matrix &operator*=(block_matrix const &b) {
        assert(b.block_names == block_names);
        for (int i = 0; i < size(); ++i) matrix_vec[i] = matrix_vec[i] * b[i];
        return *this;
      }
      block_matrix operator*(block_matrix const &b) {
        auto res = *this;
        res *= b;
        return res;
      }

      // Multiply by scalar
      template <typename Scalar> block_matrix &operator*=(Scalar const &s) {
        for (auto &m : matrix_vec) m *= s;
        return *this;
      }
      template <typename Scalar> block_matrix operator*(Scalar const &s) {
        auto res = *this;
        res *= s;
        return res;
      }
      template <typename Scalar> friend block_matrix operator*(Scalar const &s, block_matrix const &b) {
        auto res = b;
        res *= s;
        return res;
      }

      // Divide by scalar
      template <typename Scalar> block_matrix &operator/=(Scalar const &s) {
        for (auto &m : matrix_vec) m /= s;
        return *this;
      }
      template <typename Scalar> block_matrix operator/(Scalar const &s) {
        auto res = *this;
        res /= s;
        return res;
      }

      // Unary minus
      block_matrix operator-() {
        auto res = *this;
        res *= T(-1);
        return res;
      }

      /// Stream output
      friend std::ostream &operator<<(std::ostream &out, block_matrix const &c) {
        for (int i = 0; i < c.block_names.size(); ++i) out << c.block_names[i] << ": " << c.matrix_vec[i] << std::endl;
        return out;
      }

      /// MPI
      friend block_matrix mpi_reduce(block_matrix const &m, mpi::communicator c, int root, bool all, MPI_Op op) {
        block_matrix m_tot(m);
        for (int i = 0; i < m.size(); ++i) m_tot[i] = mpi::reduce(m[i], c, root, all, op);
        return m_tot;
      }
      friend void mpi_broadcast(block_matrix &m, mpi::communicator c, int root) {
	mpi::broadcast(m.block_names, c, root);
	mpi::broadcast(m.matrix_vec, c, root);
      }

      // Boost.Serialization
      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        ar &block_names;
        ar &matrix_vec;
      }

      static std::string hdf5_format() { return is_complex<T>::value ? "BlockMatrixComplex" : "BlockMatrix"; }

      /// Write to HDF5
      friend void h5_write(h5::group fg, std::string subgroup_name, block_matrix const &c) {
        h5::group gr = fg.create_group(subgroup_name);
        write_hdf5_format(gr, c);
        h5_write(gr, "block_names", c.block_names);
        h5_write(gr, "matrix_vec", c.matrix_vec);
      }
      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string subgroup_name, block_matrix &c) {
        h5::group gr = fg.open_group(subgroup_name);
        std::vector<std::string> block_names;
        std::vector<matrix_t> matrix_vec;

        h5_read(gr, "block_names", block_names);
        h5_read(gr, "matrix_vec", matrix_vec);
        c = block_matrix<T>(block_names, matrix_vec);
      }
    };
  } // namespace arrays
} // namespace triqs
