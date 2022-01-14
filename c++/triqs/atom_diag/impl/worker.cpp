// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2017 Igor Krivenko
// Copyright (c) 2018 Simons Foundation
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
// Authors: Michel Ferrero, Igor Krivenko, Hugo U. R. Strand, Nils Wentzell

#include "./worker.hpp"

#include <vector>
#include <bitset>
#include <map>
#include <triqs/hilbert_space/state.hpp>
#include <triqs/hilbert_space/imperative_operator.hpp>
#include <triqs/hilbert_space/space_partition.hpp>
#include <nda/linalg/eigenelements.hpp>

using namespace triqs::hilbert_space;

namespace triqs {
  namespace atom_diag {

// Methods of atom_diag_worker
#define ATOM_DIAG_WORKER_METHOD(RET, F) template <bool Complex> auto atom_diag_worker<Complex>::F->RET

    //------------------------------------------------------------------------------------

    // Filter the Fock states with a number of particles within [n_min;n_max]
    ATOM_DIAG_WORKER_METHOD(bool, fock_state_filter(fock_state_t s)) {
      auto c = std::bitset<64>(s).count();
      return ((c >= n_min) && (c <= n_max));
    }

    //------------------------------------------------------------------------------------

    ATOM_DIAG_WORKER_METHOD(void, autopartition(many_body_op_t const &hyb)) {
    //ATOM_DIAG_WORKER_METHOD(void, autopartition()) {

      fundamental_operator_set const &fops = hdiag->get_fops();
      many_body_op_t const &h              = hdiag->get_h_atomic();

      using imperative_operator_t = imperative_operator<class hilbert_space, scalar_t, false>;
      imperative_operator_t hamiltonian(h, fops);
      imperative_operator_t hybridization(hyb, fops);
      state<class hilbert_space, scalar_t, true> st(hdiag->full_hs);

      using space_partition_t = space_partition<state<class hilbert_space, scalar_t, true>, imperative_operator_t>;
      // Split the Hilbert space
      space_partition_t SP(st, hamiltonian, false, hybridization);

      std::vector<typename space_partition_t::matrix_element_map_t> creation_melem(fops.size());
      std::vector<typename space_partition_t::matrix_element_map_t> annihilation_melem(fops.size());
      // Merge subspaces
      for (auto const &o : fops) {
        auto create  = many_body_op_t::make_canonical(true, o.index);
        auto destroy = many_body_op_t::make_canonical(false, o.index);

        imperative_operator_t op_c_dag(create, fops), op_c(destroy, fops);

        int n                                              = o.linear_index;
        std::tie(creation_melem[n], annihilation_melem[n]) = SP.merge_subspaces(op_c_dag, op_c, true);
      }
 
      // Fill subspaces
      auto & h_spaces = hdiag->sub_hilbert_spaces;

      h_spaces.reserve(SP.n_subspaces());
      for (int sp = 0; sp < SP.n_subspaces(); ++sp) h_spaces.emplace_back(sp);

      foreach (SP, [&](fock_state_t s, int spn) {
        if (fock_state_filter(s)) h_spaces[spn].add_fock_state(s);
      })
        ;

      // Discard empty subspaces
      h_spaces.erase(
        std::remove_if(h_spaces.begin(), h_spaces.end(),
                       [](sub_hilbert_space const& sp) { return sp.size() == 0; }
        ),
        h_spaces.end()
      );

      // Correspondence between subspace indices before and after filtering
      std::map<int, int> remap;
      for(auto const& sp : h_spaces) remap.emplace(sp.get_index(), remap.size());

      // Fill connections
      hdiag->creation_connection.resize(fops.size(), h_spaces.size());
      hdiag->annihilation_connection.resize(fops.size(), h_spaces.size());
      hdiag->creation_connection.as_array_view()     = -1;
      hdiag->annihilation_connection.as_array_view() = -1;

      for (auto const &o : fops) {
        int n = o.linear_index;
        for (auto const &e : creation_melem[n]) {
          fock_state_t i, f;
          std::tie(i, f)                                                  = e.first;
          auto i_remap_it = remap.find(SP.lookup_basis_state(i));
          if(i_remap_it == remap.end()) continue;
          auto f_remap_it = remap.find(SP.lookup_basis_state(f));
          if(f_remap_it == remap.end()) continue;
          hdiag->creation_connection(n, i_remap_it->second) = f_remap_it->second;
        }
        for (auto const &e : annihilation_melem[n]) {
          fock_state_t i, f;
          std::tie(i, f)                                                      = e.first;
          auto i_remap_it = remap.find(SP.lookup_basis_state(i));
          if(i_remap_it == remap.end()) continue;
          auto f_remap_it = remap.find(SP.lookup_basis_state(f));
          if(f_remap_it == remap.end()) continue;
          hdiag->annihilation_connection(n, i_remap_it->second) = f_remap_it->second;
        }
      }

      // Reindex subspaces
      for(int i = 0; i < h_spaces.size(); ++i) h_spaces[i].set_index(i);

      complete();
    }


    // -----------------------------------------------------------------

    ATOM_DIAG_WORKER_METHOD(void, partition_with_qn(std::vector<many_body_op_t> const &qn_vector)) {

      fundamental_operator_set const &fops = hdiag->get_fops();
      many_body_op_t const &h              = hdiag->get_h_atomic();
      class hilbert_space const &full_hs   = hdiag->full_hs;

      // Define a more tolerant comparison between vectors for the quantum numbers
      auto lt_dbl = [](std::vector<double> const &v1, std::vector<double> const &v2) {
        for (int i = 0; i < v1.size(); ++i) {
          if (v1[i] < (v2[i] - 1e-8))
            return true;
          else if (v2[i] < (v1[i] - 1e-8))
            return false;
        }
        return false;
      };

      // Quantum numbers -> Hilbert subspace mapping
      std::map<std::vector<double>, int, decltype(lt_dbl)> map_qn_n(lt_dbl);

      // The QN as operators: a vector of imperative operators for the quantum numbers
      std::vector<imperative_operator<class hilbert_space, scalar_t>> qsize;
      for (auto &qn : qn_vector) qsize.emplace_back(qn, fops);

      // Helper function to get quantum numbers
      auto get_quantum_numbers = [&qsize](state<class hilbert_space, scalar_t, false> const &s) {
        std::vector<quantum_number_t> qn;
        for (auto const &op : qsize) {
          auto y = dot_product(s, op(s));
          if (std::abs(std::imag(y)) > 1.e-10) TRIQS_RUNTIME_ERROR << "Quantum number is complex !";
          qn.push_back(std::real(y));
        }
        return qn;
      };

      // The first part consists in dividing the full Hilbert space
      // into smaller subspaces using the quantum numbers
      for (int r = 0; r < full_hs.size(); ++r) {

        // fock_state corresponding to r
        fock_state_t fs = full_hs.get_fock_state(r);

        // The state we'll act on
        state<class hilbert_space, scalar_t, false> s(full_hs);
        s(r) = 1.0;

        // Create the vector with the quantum numbers
        std::vector<quantum_number_t> qn = get_quantum_numbers(s);

        // If first time we meet these quantum numbers create partial Hilbert space
        if (map_qn_n.count(qn) == 0) {
          auto n_blocks = hdiag->sub_hilbert_spaces.size();
          hdiag->sub_hilbert_spaces.emplace_back(n_blocks); // a new sub_hilbert_space
          hdiag->quantum_numbers.push_back(qn);
          map_qn_n[qn] = n_blocks;
        }

        // Add fock state to partial Hilbert space
        hdiag->sub_hilbert_spaces[map_qn_n[qn]].add_fock_state(fs);
      }

      // ---- Now make the creation/annihilation maps -----

      auto creation_map     = std::vector<std::vector<int>>(fops.size(), std::vector<int>(hdiag->sub_hilbert_spaces.size(), -1));
      auto annihilation_map = creation_map;

      // init the mapping tables
      hdiag->creation_connection.resize(fops.size(), hdiag->sub_hilbert_spaces.size());
      hdiag->annihilation_connection.resize(fops.size(), hdiag->sub_hilbert_spaces.size());
      hdiag->creation_connection.as_array_view()     = -1;
      hdiag->annihilation_connection.as_array_view() = -1;

      for (auto const &x : fops) {

        // Get the operators and their index
        int n        = x.linear_index;
        auto create  = many_body_op_t::make_canonical(true, x.index);
        auto destroy = many_body_op_t::make_canonical(false, x.index);

        // Construct their imperative counterpart
        imperative_operator<class hilbert_space, scalar_t> op_c_dag(create, fops), op_c(destroy, fops);

        // to avoid declaring every time in the loop below
        std::vector<quantum_number_t> qn_before, qn_after;

        // Now act on the state with the c, c_dag to see how quantum numbers change
        for (int r = 0; r < full_hs.size(); ++r) {

          // The state we'll act on and its quantum numbers
          state<class hilbert_space, scalar_t, false> s(full_hs);
          s(r)      = 1.0;
          qn_before = get_quantum_numbers(s);

          // Apply creation on state to figure quantum numbers
          qn_after = get_quantum_numbers(op_c_dag(s));

          // Insert in creation map checking whether it was already there
          if (!triqs::utility::is_zero(dot_product(op_c_dag(s), op_c_dag(s)))) {
            auto origin = hdiag->sub_hilbert_spaces[map_qn_n[qn_before]].get_index();
            auto target = hdiag->sub_hilbert_spaces[map_qn_n[qn_after]].get_index();
            if (creation_map[n][origin] == -1)
              creation_map[n][origin] = target;
            else if (creation_map[n][origin] != target)
              TRIQS_RUNTIME_ERROR << "partition_with_qn(): internal error while filling creation_connection";
            hdiag->creation_connection(n, map_qn_n[qn_before]) = map_qn_n[qn_after];
          }

          // apply annihilation on state to figure quantum numbers
          qn_after = get_quantum_numbers(op_c(s));

          // insert in annihilation map checking whether it was already there
          if (!triqs::utility::is_zero(dot_product(op_c(s), op_c(s)))) {
            auto origin = hdiag->sub_hilbert_spaces[map_qn_n[qn_before]].get_index();
            auto target = hdiag->sub_hilbert_spaces[map_qn_n[qn_after]].get_index();
            if (annihilation_map[n][origin] == -1)
              annihilation_map[n][origin] = target;
            else if (annihilation_map[n][origin] != target)
              TRIQS_RUNTIME_ERROR << "partition_with_qn(): internal error while filling annihilation_connection";
            hdiag->annihilation_connection(n, map_qn_n[qn_before]) = map_qn_n[qn_after];
          }
        }
      }
      complete();
    }

    // -----------------------------------------------------------------

    ATOM_DIAG_WORKER_METHOD(matrix_t, make_op_matrix(many_body_op_t const &op, int from_spn, int to_spn) const) {

      fundamental_operator_set const &fops = hdiag->get_fops();
      class hilbert_space const &full_hs   = hdiag->full_hs;
      auto const &from_sp                  = hdiag->sub_hilbert_spaces[from_spn];
      auto const &to_sp                    = hdiag->sub_hilbert_spaces[to_spn];

      imperative_operator<class hilbert_space, scalar_t> imp_op(op, fops);

      auto M = matrix_t(to_sp.size(), from_sp.size());
      M()    = 0;

      for (int i = 0; i < from_sp.size(); ++i) { // loop on all fock states of the blocks
        state<class hilbert_space, scalar_t, true> from_s(full_hs);
        from_s(from_sp.get_fock_state(i)) = 1.0;
        auto to_s                         = imp_op(from_s);
        auto proj_s                       = project<state<sub_hilbert_space, scalar_t, true>>(to_s, to_sp);
        foreach (proj_s, [&](int j, scalar_t ampl) { M(j, i) = ampl; })
          ;
      }

      return dagger(hdiag->eigensystems[to_spn].unitary_matrix) * M * hdiag->eigensystems[from_spn].unitary_matrix;
    }

    // -----------------------------------------------------------------

    ATOM_DIAG_WORKER_METHOD(void, complete()) {

      fundamental_operator_set const &fops = hdiag->get_fops();
      many_body_op_t const &h              = hdiag->get_h_atomic();

      imperative_operator<class hilbert_space, scalar_t, false> hamiltonian(h, fops);

      //  Compute energy levels and eigenvectors of the local Hamiltonian
      int n_subspaces = hdiag->sub_hilbert_spaces.size();
      hdiag->eigensystems.resize(n_subspaces);
      hdiag->gs_energy = std::numeric_limits<double>::infinity();

      // Prepare the eigensystem in a temporary map to sort them by energy !
      std::map<std::pair<double, int>, typename atom_diag<Complex>::eigensystem_t> eign_map;
      double energy_split = 1.e-10; // to split the eigenvalues, which are numerically very close
      for (int spn = 0; spn < n_subspaces; ++spn) {
        auto const &sp = hdiag->sub_hilbert_spaces[spn];
        typename atom_diag<Complex>::eigensystem_t eigensystem;

        state<sub_hilbert_space, scalar_t, false> i_state(sp);
        matrix_t h_matrix(sp.size(), sp.size());

        for (int i = 0; i < sp.size(); ++i) {
          i_state.amplitudes()() = 0;
          i_state(i)             = 1;
          auto f_state           = hamiltonian(i_state);
          h_matrix(range(), i)   = f_state.amplitudes();
        }

        auto eig                   = linalg::eigenelements(h_matrix);
        eigensystem.eigenvalues    = eig.first;
        eigensystem.unitary_matrix = eig.second;
        hdiag->gs_energy           = std::min(hdiag->gs_energy, eigensystem.eigenvalues[0]);

        eign_map.insert({{eigensystem.eigenvalues(0) + energy_split * spn, spn}, eigensystem});
      }

      // Reorder the block along their minimal energy
      {
        auto tmp = hdiag->sub_hilbert_spaces;
        std::map<int, int> remap;
        int i = 0;
        for (auto const &x : eign_map) { // in order of min energy !
          hdiag->eigensystems[i] = x.second;
          tmp[i]                 = hdiag->sub_hilbert_spaces[x.first.second];
          tmp[i].set_index(i);
          remap[x.first.second] = i;
          ++i;
        }
        std::swap(tmp, hdiag->sub_hilbert_spaces);
        auto remap_connection = [&](matrix<long> &connection) {
          auto c2 = connection;
          for (int n = 0; n < first_dim(connection); ++n)
            for (int i = 0; i < second_dim(connection); ++i) connection(n, remap[i]) = (c2(n, i) == -1 ? -1 : remap[c2(n, i)]);
        };
        remap_connection(hdiag->creation_connection);
        remap_connection(hdiag->annihilation_connection);
      } // end reordering

      // Shift the ground state energy of the local Hamiltonian to zero.
      for (auto &eigensystem : hdiag->eigensystems) eigensystem.eigenvalues() -= hdiag->get_gs_energy();

      for (auto const &x : fops) {
        // get the operators and their index
        int n = x.linear_index;
        // n is guaranteed to be 0, 1, 2, 3, ... by the fundamental_operator_set class ... but it is very weird...
        // otherwise the push_back below is false.
        auto create  = many_body_op_t::make_canonical(true, x.index);
        auto destroy = many_body_op_t::make_canonical(false, x.index);

        // Compute the matrices of c, c dagger in the diagonalization base of H_loc
        // first a lambda, since it is almost the same code for c and cdag
        auto make_c_mat = [&](int n, matrix<long> const &connection, many_body_op_t const &op) {
          std::vector<matrix_t> cmat(second_dim(connection));
          for (int B = 0; B < second_dim(connection); ++B) {
            auto Bp = connection(n, B);
            if (Bp == -1) continue;
            cmat[B] = make_op_matrix(op, B, Bp);
          }
          return cmat;
        };

        // now execute code...
        hdiag->c_matrices.push_back(make_c_mat(n, hdiag->annihilation_connection, destroy));
        hdiag->cdag_matrices.push_back(make_c_mat(n, hdiag->creation_connection, create));

      } // end of loop on operators
    }

    // -----------------------------------------------------------------

    // Explicit instantiations for real and complex atom_diag_worker
    template struct atom_diag_worker<false>;
    template struct atom_diag_worker<true>;

  } // namespace atom_diag
} // namespace triqs
