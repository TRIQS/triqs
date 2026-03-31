// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2017 Igor Krivenko
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
// Authors: Maxime Charlebois, Michel Ferrero, Igor Krivenko, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

#include "./worker.hpp"
#include "../atom_diag.hpp"
#include "../../arrays.hpp"
#include "../../hilbert_space/fundamental_operator_set.hpp"
#include "../../hilbert_space/imperative_operator.hpp"
#include "../../hilbert_space/state.hpp"
#include "../../operators/many_body_operator.hpp"

#include <itertools/itertools.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace triqs::arrays;

namespace triqs::atom_diag {

// Methods of atom_diag
#define ATOM_DIAG_CONSTRUCTOR(ARGS) template <bool Complex> atom_diag<Complex>::atom_diag ARGS
#define ATOM_DIAG_METHOD(RET, F) template <bool Complex> auto atom_diag<Complex>::F->RET

  ATOM_DIAG_CONSTRUCTOR((many_body_op_t const &h, fundamental_operator_set const &fops, std::vector<many_body_op_t> const &qn_vector))
     : h_atomic(h), fops(fops), full_hs(fops), vacuum(full_hs.size()) {
    atom_diag_worker<Complex>{this}.partition_with_qn(qn_vector);
    fill_first_eigenstate_of_subspace();
    compute_vacuum();
  }

  // -----------------------------------------------------------------

  ATOM_DIAG_CONSTRUCTOR((many_body_op_t const &h, fundamental_operator_set const &fops, many_body_op_t const &hyb))
     : h_atomic(h), fops(fops), full_hs(fops), vacuum(full_hs.size()) {
    atom_diag_worker<Complex>{this}.autopartition(hyb);
    fill_first_eigenstate_of_subspace();
    compute_vacuum();
  }

  // -----------------------------------------------------------------

  ATOM_DIAG_CONSTRUCTOR((many_body_op_t const &h, fundamental_operator_set const &fops))
     : h_atomic(h), fops(fops), full_hs(fops), vacuum(full_hs.size()) {
    atom_diag_worker<Complex>{this}.autopartition();
    fill_first_eigenstate_of_subspace();
    compute_vacuum();
  }

  ATOM_DIAG_CONSTRUCTOR((many_body_op_t const &h, fundamental_operator_set const &fops, int n_min, int n_max))
     : h_atomic(h), fops(fops), full_hs(fops), vacuum(full_hs.size()) {
    atom_diag_worker<Complex>{this, n_min, n_max}.autopartition();
    fill_first_eigenstate_of_subspace();
    compute_vacuum();
  }

  // -----------------------------------------------------------------

  ATOM_DIAG_METHOD(void, fill_first_eigenstate_of_subspace()) {
    // Calculate the index of the first eigenstate of each block
    first_eigenstate_of_subspace.resize(n_subspaces());
    if (n_subspaces() == 0) return;
    first_eigenstate_of_subspace[0] = 0;
    for (int sp = 1; sp < n_subspaces(); ++sp) first_eigenstate_of_subspace[sp] = first_eigenstate_of_subspace[sp - 1] + get_subspace_dim(sp - 1);
  }

  // -----------------------------------------------------------------

  ATOM_DIAG_METHOD(void, compute_vacuum()) {
    // Compute vacuum vector in the eigenbasis
    vacuum()              = 0;
    vacuum_subspace_index = -1;
    for (auto sp : range(sub_hilbert_spaces.size())) {
      if (sub_hilbert_spaces[sp].has_state(fock_state_t(0))) {
        vacuum_subspace_index               = sp;
        vacuum(index_range_of_subspace(sp)) = dagger(eigensystems[sp].unitary_matrix)(range::all, 0);
        break;
      }
    }
  }

  // -----------------------------------------------------------------

  ATOM_DIAG_METHOD(std::vector<std::vector<double>>, get_energies() const) {
    std::vector<std::vector<double>> R;
    for (auto const &es : eigensystems) R.emplace_back(es.eigenvalues.begin(), es.eigenvalues.end());
    return R;
  }

  // -----------------------------------------------------------------
  // Given a monomial (ccccc), and a subspace B, returns
  //   - the subspace connected by ccccc from B
  //   - the corresponding matrix (not necessarily square)

  template <bool Complex>
  auto atom_diag<Complex>::get_matrix_element_of_monomial(operators::monomial_t const &op_vec, int B) const -> std::pair<int, matrix_t> {
    if (truncated_) TRIQS_RUNTIME_ERROR << "get_matrix_element_of_monomial is not supported on a truncated atom_diag";

    imperative_operator<class hilbert_space, scalar_t, false> monomial_op(many_body_op_t(1.0, op_vec), fops);

    matrix_t m;
    int Bp = -1;

    for (auto [i_index, i] : itertools::enumerate(get_fock_states(B))) {
      state<class hilbert_space, scalar_t, true> initial_st(full_hs, i);

      auto final_st = monomial_op(initial_st);

      // The initializer for i_index is needed here because of the Core Language Defect #2313.
      // https://stackoverflow.com/a/46115028
      EXPECTS(final_st.nterms() <= 1);
      foreach (final_st, [&, i_idx = i_index](fock_state_t j, scalar_t x) {
        for (auto const &sp : sub_hilbert_spaces) {
          if (sp.has_state(j)) {
            Bp = sp.get_index();

            if (m.empty()) { m = matrix_t::zeros({get_subspace_dim(Bp), get_subspace_dim(B)}); }

            m(sp.get_state_index(j), i_idx) = x;
            break;
          }
        }
      });
    }

    if (Bp == -1)
      return {-1, std::move(m)};
    else { return {Bp, dagger(get_unitary_matrix(Bp)) * m * get_unitary_matrix(B)}; }
  }

  // -----------------------------------------------------------------

  ATOM_DIAG_METHOD(op_block_mat_t, get_op_mat(many_body_op_t const &op) const) {
    op_block_mat_t op_mat(n_subspaces());

    for (auto b : range(n_subspaces())) {
      for (auto const &term : op) {
        auto [bb, mat] = get_matrix_element_of_monomial(term.monomial, b);
        if (bb == -1) continue;

        if (op_mat.connection(b) == -1) {
          op_mat.connection(b) = bb;
          op_mat.block_mat[b]  = term.coef * mat;
        } else if (op_mat.connection(b) != bb) {
          TRIQS_RUNTIME_ERROR << "ERROR: <atom_diag::get_op_mat> Monomials in operator does not connect the same subspaces.";
        } else {
          op_mat.block_mat[b] += term.coef * mat;
        }
      }

      // Given the environment variable CHECK_ISSUE833 was set by the user
      // throw an exception if the result of this function was previously effected by issue 833
      // https://github.com/TRIQS/triqs/issues/833
      static const bool check_issue833 = std::getenv("CHECK_ISSUE833");
      if (check_issue833) {
        auto U_left  = dagger(eigensystems[op_mat.connection(b)].unitary_matrix);
        auto U_right = eigensystems[b].unitary_matrix;
        auto diff    = op_mat.block_mat[b] - U_left * op_mat.block_mat[b] * U_right;
        if (max_element(abs(make_regular(diff))) > 1e-10)
          TRIQS_RUNTIME_ERROR
             << "ERROR: The result of the get_op_mat function was previously affected by issue 833 (https://github.com/TRIQS/triqs/issues/833).\n"
                "If you used the function prior to release 3.1.0 of TRIQS the result was incorrect.";
      }
    }
    return op_mat;
  }

  // -----------------------------------------------------------------
  // Truncation

  template <bool Complex> atom_diag<Complex> atom_diag<Complex>::truncate(double energy_cutoff, int max_states) const {
    if (energy_cutoff == std::numeric_limits<double>::infinity() && max_states < 0) return *this;

    atom_diag result = *this;

    // Collect all states sorted by energy
    struct state_info_t {
      double energy;
      int sp_idx, inner_idx;
    };
    std::vector<state_info_t> all_states;
    for (int sp = 0; sp < result.n_subspaces(); ++sp)
      for (int i = 0; i < result.get_subspace_dim(sp); ++i) all_states.push_back({result.eigensystems[sp].eigenvalues[i], sp, i});
    std::ranges::sort(all_states, {}, &state_info_t::energy);

    // Determine which states to keep
    std::vector<std::vector<int>> states_to_keep(result.n_subspaces());
    int kept_count = 0;
    for (auto const &[energy, sp_idx, inner_idx] : all_states) {
      if ((max_states < 0 || kept_count < max_states) && energy <= energy_cutoff) {
        states_to_keep[sp_idx].push_back(inner_idx);
        ++kept_count;
      }
    }
    for (auto &indices : states_to_keep) std::ranges::sort(indices);

    // No states were actually removed
    if (kept_count == static_cast<int>(all_states.size())) return result;

    // Build old->new subspace index mapping
    std::map<int, int> sp_remap;
    for (int sp = 0; sp < result.n_subspaces(); ++sp)
      if (!states_to_keep[sp].empty()) sp_remap[sp] = static_cast<int>(sp_remap.size());
    if (sp_remap.empty()) TRIQS_RUNTIME_ERROR << "Truncation removed all states!";
    int n_sp_new = static_cast<int>(sp_remap.size());

    // Truncate eigensystems
    std::vector<eigensystem_t> new_eigensystems(n_sp_new);
    for (auto const &[old_sp, new_sp] : sp_remap) {
      auto const &keep = states_to_keep[old_sp];
      auto &old_eig    = result.eigensystems[old_sp];
      auto &new_eig    = new_eigensystems[new_sp];

      new_eig.eigenvalues    = vector<double>(old_eig.eigenvalues(keep));
      new_eig.unitary_matrix = old_eig.unitary_matrix(range::all, keep);
    }

    // Rebuild sub_hilbert_spaces.
    // Each sub_hilbert_space retains ALL its original Fock states even though the
    // eigensystem now has fewer eigenstates. This means sub_hilbert_spaces[sp].dimension()
    // may exceed get_subspace_dim(sp). This is intentional: the Fock states are needed by
    // compute_vacuum() to locate |0> and by get_fock_states() for basis labeling.
    // The unitary_matrix becomes rectangular (n_fock x n_kept) rather than square.
    std::vector<sub_hilbert_space> new_sub_hilbert_spaces;
    new_sub_hilbert_spaces.reserve(n_sp_new);
    for (auto const &[old_sp, new_sp] : sp_remap) {
      new_sub_hilbert_spaces.push_back(result.sub_hilbert_spaces[old_sp]);
      new_sub_hilbert_spaces.back().set_index(new_sp);
    }

    // Update connection matrices
    int n_ops = static_cast<int>(result.creation_connection.extent(0));
    matrix<long> new_creation_connection(n_ops, n_sp_new);
    matrix<long> new_annihilation_connection(n_ops, n_sp_new);
    new_creation_connection.as_array_view()     = -1;
    new_annihilation_connection.as_array_view() = -1;

    for (int op = 0; op < n_ops; ++op) {
      for (auto const &[old_sp, new_sp] : sp_remap) {
        if (long t = result.creation_connection(op, old_sp); t != -1 && sp_remap.contains(t)) new_creation_connection(op, new_sp) = sp_remap[t];
        if (long t = result.annihilation_connection(op, old_sp); t != -1 && sp_remap.contains(t)) new_annihilation_connection(op, new_sp) = sp_remap[t];
      }
    }

    // Rebuild c/cdag matrices with truncated dimensions
    std::vector<std::vector<matrix_t>> new_c_matrices(n_ops, std::vector<matrix_t>(n_sp_new));
    std::vector<std::vector<matrix_t>> new_cdag_matrices(n_ops, std::vector<matrix_t>(n_sp_new));

    auto truncate_op_matrices = [&](auto const &old_matrices, auto &new_matrices, auto const &connection) {
      for (int op = 0; op < n_ops; ++op) {
        for (auto const &[old_sp, new_sp] : sp_remap) {
          long old_target = connection(op, old_sp);
          if (old_target == -1 || !sp_remap.contains(old_target)) continue;
          auto const &old_mat = old_matrices[op][old_sp];
          if (old_mat.is_empty()) continue;

          new_matrices[op][new_sp] = old_mat(states_to_keep[old_target], states_to_keep[old_sp]);
        }
      }
    };

    truncate_op_matrices(result.c_matrices, new_c_matrices, result.annihilation_connection);
    truncate_op_matrices(result.cdag_matrices, new_cdag_matrices, result.creation_connection);

    // Apply all updates
    result.eigensystems            = std::move(new_eigensystems);
    result.sub_hilbert_spaces      = std::move(new_sub_hilbert_spaces);
    result.creation_connection     = std::move(new_creation_connection);
    result.annihilation_connection = std::move(new_annihilation_connection);
    result.c_matrices              = std::move(new_c_matrices);
    result.cdag_matrices           = std::move(new_cdag_matrices);
    result.truncated_              = true;

    result.fill_first_eigenstate_of_subspace();
    result.vacuum.resize(result.get_total_eigenstate_count());
    result.compute_vacuum();

    // Update quantum_numbers if present
    if (!result.quantum_numbers.empty()) {
      std::vector<std::vector<quantum_number_t>> new_qn;
      for (auto const &[old_sp, _] : sp_remap) new_qn.push_back(result.quantum_numbers[old_sp]);
      result.quantum_numbers = std::move(new_qn);
    }

    return result;
  }

#undef ATOM_DIAG_METHOD

  // -----------------------------------------------------------------

  // Free functions

  template <bool Complex> void h5_write(h5::group fg, std::string const &name, atom_diag<Complex> const &ad) {
    using matrix_t = typename atom_diag<Complex>::matrix_t;
    auto gr        = fg.create_group(name);
    write_hdf5_format(gr, ad); // NOLINT

    h5_write_attribute(gr, "fops", ad.fops); // NOLINT
    h5::write(gr, "h_atomic", ad.h_atomic);
    h5::write(gr, "full_hs", ad.full_hs);
    h5::write(gr, "sub_hilbert_spaces", ad.sub_hilbert_spaces);
    h5::write(gr, "eigensystems", ad.eigensystems);
    h5::write(gr, "creation_connection", ad.creation_connection);
    h5::write(gr, "annihilation_connection", ad.annihilation_connection);

    auto write_sparse = [&](std::string na, std::vector<std::vector<matrix_t>> const &Mvv) {
      auto gr2 = gr.create_group(na);
      for (int i = 0; i < Mvv.size(); ++i)
        for (int j = 0; j < Mvv[i].size(); ++j)
          if (!Mvv[i][j].is_empty()) h5::write(gr2, std::to_string(i) + ' ' + std::to_string(j), Mvv[i][j]);
    };

    write_sparse("c_matrices", ad.c_matrices);
    write_sparse("cdag_matrices", ad.cdag_matrices);

    h5::write(gr, "gs_energy", ad.gs_energy);
    h5::write(gr, "vacuum_subspace_index", ad.vacuum_subspace_index);
    h5::write(gr, "vacuum", ad.vacuum);
    h5::write(gr, "quantum_numbers", ad.quantum_numbers);
    h5::write(gr, "truncated", ad.truncated_);
  }

  // -----------------------------------------------------------------
  template <bool Complex> void h5_read(h5::group fg, std::string const &name, atom_diag<Complex> &ad) {
    using matrix_t = typename atom_diag<Complex>::matrix_t;
    auto gr        = fg.open_group(name);

    h5_read_attribute(gr, "fops", ad.fops); // NOLINT
    h5::try_read(gr, "h_atomic", ad.h_atomic);
    h5::read(gr, "full_hs", ad.full_hs);
    h5::read(gr, "sub_hilbert_spaces", ad.sub_hilbert_spaces);
    h5::read(gr, "eigensystems", ad.eigensystems);
    h5::read(gr, "creation_connection", ad.creation_connection);
    h5::read(gr, "annihilation_connection", ad.annihilation_connection);

    auto read_sparse = [&](std::string na, std::vector<std::vector<matrix_t>> &Mvv) {
      Mvv.resize(ad.creation_connection.extent(0), std::vector<matrix_t>(ad.creation_connection.extent(1)));
      auto gr2 = gr.open_group(na);
      for (auto s : gr2.get_all_dataset_names()) {
        std::stringstream ss(s);
        std::string item1, item2;
        std::getline(ss, item1, ' ');
        std::getline(ss, item2, ' ');
        int i = std::stoi(item1), j = std::stoi(item2);
        h5::read(gr2, s, Mvv[i][j]);
      }
    };

    read_sparse("c_matrices", ad.c_matrices);
    read_sparse("cdag_matrices", ad.cdag_matrices);

    h5::read(gr, "gs_energy", ad.gs_energy);
    h5::read(gr, "vacuum_subspace_index", ad.vacuum_subspace_index);
    h5::read(gr, "vacuum", ad.vacuum);
    h5::try_read(gr, "quantum_numbers", ad.quantum_numbers);
    h5::try_read(gr, "truncated", ad.truncated_); // Backward compatible: default to false
    ad.fill_first_eigenstate_of_subspace();
  }

  // -----------------------------------------------------------------

  template <bool Complex> std::ostream &operator<<(std::ostream &os, atom_diag<Complex> const &ad) {
    os << "Dimension of full Hilbert space: " << ad.get_full_hilbert_space_dim() << std::endl;
    os << "Number of invariant subspaces: " << ad.n_subspaces() << std::endl;
    for (int n_sp = 0; n_sp < ad.n_subspaces(); ++n_sp) {
      os << "Subspace " << n_sp << ", ";
      os << "lowest energy level : " << ad.eigensystems[n_sp].eigenvalues[0] << std::endl;
      os << "Subspace dimension = " << ad.eigensystems[n_sp].eigenvalues.size() << std::endl;
      //os << "-------------------------" << std::endl;
    }
    return os;
  }

  // -----------------------------------------------------------------

  // Explicit instantiations for real and complex atom_diag
  template class atom_diag<false>;
  template class atom_diag<true>;

  template void h5_write(h5::group fg, std::string const &, atom_diag<false> const &);
  template void h5_write(h5::group fg, std::string const &, atom_diag<true> const &);
  template void h5_read(h5::group fg, std::string const &, atom_diag<false> &);
  template void h5_read(h5::group fg, std::string const &, atom_diag<true> &);

  template std::ostream &operator<<(std::ostream &, atom_diag<true> const &);
  template std::ostream &operator<<(std::ostream &, atom_diag<false> const &);

} // namespace triqs::atom_diag
