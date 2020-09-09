// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include "./many_body_operator.hpp"
#include "./../utility/mini_vector.hpp"
#include <h5/h5.hpp>
#include <hdf5.h>

namespace triqs {
  namespace operators {

    using h5::object;
    using h5::dataset;
    using hilbert_space::fundamental_operator_set;

// maximum order of the monomial (here quartic operators)
#define MAX_MONOMIAL_SIZE 4

    struct print_visitor { // TODO C17 use generic lambda + if constexpr
      std::ostream &os;
      void operator()(std::string const &x) { os << '\'' << x << '\''; }
      void operator()(int i) { os << i; }
    };

    std::ostream &operator<<(std::ostream &os, canonical_ops_t const &op) {
      print_visitor pr{os};
      os << 'c' << (op.dagger ? "_dag" : "") << '(';
      int u = 0;
      for (auto const &i : op.indices) {
        if (u++) os << ",";
        visit(pr, i);
      }
      return os << ')';
    }

    bool operator<(monomial_t const &m1, monomial_t const &m2) {
      return m1.size() != m2.size() ? m1.size() < m2.size() : std::lexicographical_compare(m1.begin(), m1.end(), m2.begin(), m2.end());
    }

    std::ostream &operator<<(std::ostream &os, monomial_t const &m) {
      for (auto const &c : m) { os << '*' << c; }
      return os;
    }

    /// ----- h5 support

    namespace {

      // the type of the monomial to be stored in the h5 file
      // the complicated indices of the C, C^ will be transformed into an int
      // via a fundamental_operator_set
      struct h5_monomial {
        bool is_real;
        double re, im;
        long op_indices[MAX_MONOMIAL_SIZE];
      };

      // create the h5 type corresponding to h5_monomial
      object h5_monomial_dtype() {
        object mono_obj = H5Tcreate(H5T_COMPOUND, sizeof(h5_monomial));
        H5Tinsert(mono_obj, "is_real", HOFFSET(h5_monomial, is_real), H5T_NATIVE_INT);
        H5Tinsert(mono_obj, "re", HOFFSET(h5_monomial, re), H5T_NATIVE_DOUBLE);
        H5Tinsert(mono_obj, "im", HOFFSET(h5_monomial, im), H5T_NATIVE_DOUBLE);
        hsize_t array_dim[] = {MAX_MONOMIAL_SIZE};
        object array_tid = H5Tarray_create(H5T_NATIVE_LONG, 1, array_dim);
        H5Tinsert(mono_obj, "op_indices", HOFFSET(h5_monomial, op_indices), array_tid);
        return mono_obj;
      }
    } // namespace

    // ---------------------------  WRITE -----------------------------------------

    void h5_write(h5::group g, std::string const &name, many_body_operator const &op, fundamental_operator_set const &fops) {

      // first prepare the data
      // datavec stores all monomials.
      // In each monomial, the C, C^+ operator is labeled by an int,
      // >0 for C^+, <0 for C. 0 means : no operator.
      // Its abs is the unique int associated to the series of indices of the C, from the fundamental_operator_set
      std::vector<h5_monomial> datavec;

      for (auto const &m : op.monomials) { // for all monomials of the operator
        if (m.first.size() > MAX_MONOMIAL_SIZE)
          TRIQS_RUNTIME_ERROR << " h5 writing many_body_operator : unexpected monomial with more than " << MAX_MONOMIAL_SIZE << "operators !";
        h5_monomial y = {m.second.is_real(), real(m.second), imag(m.second), {0, 0, 0, 0}}; // we want to transform it to an h5_monomial
        int i         = 0;
        for (auto const &c_cdag_op : m.first) {            // loop over the C C^+ operators of the monomial
          long c_number     = fops[c_cdag_op.indices] + 1; // the number of the C C^+ op. 0 means "no operators" here, so we shift by 1
          y.op_indices[i++] = (c_cdag_op.dagger ? c_number : -c_number);
        }
        datavec.push_back(y);
      }
      // now datavec and correspondance_table are ready.

      // ----- Store datavec into the h5 file.

      // datatype
      object dt = h5_monomial_dtype();

      // dataspace
      hsize_t dim[] = {datavec.size()};
      object dspace = H5Screate_simple(1, dim, NULL);

      // dataset
      object ds = g.create_dataset(name.c_str(), dt, dspace);

      // write
      herr_t status = H5Dwrite(ds, dt, H5S_ALL, H5S_ALL, H5P_DEFAULT, datavec.data());
      if (status < 0) TRIQS_RUNTIME_ERROR << "Error writing the many_body_operator " << op << " in the group" << g.name();

      // Store fundamental_operator_set as an attribute of the dataset
      h5_write_attribute(ds, "fundamental_operator_set", fops);
      write_hdf5_format(ds, op);
    }

    // ---------------------------  READ -----------------------------------------

    void h5_read(h5::group g, std::string const &name, many_body_operator &op, fundamental_operator_set &fops) {

      // --- Read the datavec

      // open the dataset
      dataset ds = g.open_dataset(name);

      // dataspace
      h5::dataspace dspace = H5Dget_space(ds);

      // recover the dimension: must be of rank 1
      utility::mini_vector<hsize_t, 1> dims_out;
      int ndims = H5Sget_simple_extent_dims(dspace, dims_out.ptr(), NULL);
      if (ndims != 1)
        TRIQS_RUNTIME_ERROR << "h5 : Trying to read many_body_operator. Rank mismatch : the array stored in the hdf5 file has rank = "
                            << ndims;

      // datatype
      object dt = h5_monomial_dtype();

      // reading
      std::vector<h5_monomial> datavec(dims_out[0]);

      herr_t status = H5Dread(ds, h5_monomial_dtype(), dspace, H5S_ALL, H5P_DEFAULT, datavec.data());
      if (status < 0) TRIQS_RUNTIME_ERROR << "Error reading the many_body_operator " << op << " from the group" << g.name();

      // --- Read correspondance_table as an attribute
      h5_read_attribute(ds, "fundamental_operator_set", fops);

      // ---- Now we must reverse the operations of write

      auto r_fops = fops.data(); // the data vector v[int] -> indices inverting fops[indices] -> n

      for (auto const &mon : datavec) {
        monomial_t monomial;                                      // vector of canonical_ops_t
        for (int i : mon.op_indices) {                            // loop over the index of the C, C^ ops of the monomial
          if (i == 0) break;                                      // means we have reach the end of the C,  C^+ list
          monomial.push_back({(i > 0), r_fops[std::abs(i) - 1]}); // add one C, C^+ op to the monomial
        }
        real_or_complex s = (mon.is_real ? real_or_complex(mon.re) : real_or_complex(std::complex<double>(mon.re, mon.im)));
        op.monomials.insert({monomial, s}); // add the monomial to the operator
      }
    }
  } // namespace operators
} // namespace triqs
