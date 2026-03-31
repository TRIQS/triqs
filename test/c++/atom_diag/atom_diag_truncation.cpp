// Copyright (c) 2026 Simons Foundation
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
// Authors: Nils Wentzell

// Test truncation of atom_diag to a low-energy subspace.
//
// atom_diag::truncate(energy_cutoff, max_states) returns a new atom_diag
// retaining only eigenstates below an energy cutoff and/or up to a maximum
// count. Eigenvalues are measured relative to the ground state (GS = 0).
// All derived quantities (c/cdag matrices, connections, vacuum state) are
// consistently updated; functions that require the full Hilbert space
// (get_op_mat) are guarded and will throw.

#include <triqs/test_tools/gfs.hpp>

#include <triqs/atom_diag/atom_diag.hpp>

#include <h5/h5.hpp>
#include <h5/serialization.hpp>

#include "./hamiltonian.hpp"

using namespace nda;
using namespace triqs::hilbert_space;
using namespace triqs::atom_diag;

using atom_diag_real    = triqs::atom_diag::atom_diag<false>;
using atom_diag_complex = triqs::atom_diag::atom_diag<true>;

// Helper: collect all eigenvalues across subspaces, sorted in ascending order.
const auto collect_energies = [](auto const &ad) {
  std::vector<double> energies;
  for (int sp = 0; sp < ad.n_subspaces(); ++sp)
    for (int i = 0; i < ad.get_subspace_dim(sp); ++i) energies.push_back(ad.get_eigenvalue(sp, i));
  std::ranges::sort(energies);
  return energies;
};

// 3-orbital Kanamori atom (mu=0.4, U=1, J=0.3, b=0.03, t=0.2).
// Full Hilbert space has 2^6 = 64 states split across ~20 subspaces.
class atom_diag_truncation : public ::testing::Test {
  protected:
  fundamental_operator_set fops = make_fops();
  many_body_operator_real h     = make_hamiltonian<many_body_operator_real>(0.4, 1.0, 0.3, 0.03, 0.2);
  atom_diag_real ad_full{h, fops};
};

// Same Kanamori atom with a complex hopping (t = 0.2i).
class atom_diag_truncation_complex : public ::testing::Test {
  protected:
  fundamental_operator_set fops = make_fops();
  many_body_operator_complex h  = make_hamiltonian<many_body_operator_complex>(0.4, 1.0, 0.3, 0.03, 0.2i);
  atom_diag_complex ad_full{h, fops};
};

// ============================================================
// Truncation criteria
// ============================================================

TEST_F(atom_diag_truncation, EnergyCutoff) {
  EXPECT_FALSE(ad_full.is_truncated());

  // Keep only states with eigenvalue <= 1.5 (relative to GS = 0)
  double cutoff = 1.5;
  auto ad_trunc = ad_full.truncate(cutoff);

  EXPECT_TRUE(ad_trunc.is_truncated());
  EXPECT_LT(ad_trunc.get_total_eigenstate_count(), ad_full.get_total_eigenstate_count());

  for (int sp = 0; sp < ad_trunc.n_subspaces(); ++sp)
    for (int i = 0; i < ad_trunc.get_subspace_dim(sp); ++i)
      EXPECT_LE(ad_trunc.get_eigenvalue(sp, i), cutoff + 1e-10);
}

TEST_F(atom_diag_truncation, MaxStates) {
  int max_states = 10;
  auto ad_trunc  = ad_full.truncate(std::numeric_limits<double>::infinity(), max_states);

  EXPECT_TRUE(ad_trunc.is_truncated());
  EXPECT_EQ(max_states, ad_trunc.get_total_eigenstate_count());

  // The kept states should be the 10 lowest-energy eigenstates of the full problem
  auto full_energies  = collect_energies(ad_full);
  auto trunc_energies = collect_energies(ad_trunc);
  for (int i = 0; i < max_states; ++i) EXPECT_NEAR(full_energies[i], trunc_energies[i], 1e-10);
}

TEST_F(atom_diag_truncation, CombinedCriteria) {
  // Both criteria are applied simultaneously (logical AND):
  // a state is kept only if its energy <= cutoff AND the total count < max_states.
  double energy_cutoff = 2.0;
  int max_states       = 20;
  auto ad_trunc        = ad_full.truncate(energy_cutoff, max_states);

  EXPECT_TRUE(ad_trunc.is_truncated());
  EXPECT_LE(ad_trunc.get_total_eigenstate_count(), max_states);

  for (int sp = 0; sp < ad_trunc.n_subspaces(); ++sp)
    for (int i = 0; i < ad_trunc.get_subspace_dim(sp); ++i)
      EXPECT_LE(ad_trunc.get_eigenvalue(sp, i), energy_cutoff + 1e-10);
}

// ============================================================
// Operator matrices
// ============================================================

TEST_F(atom_diag_truncation, MatrixDimensions) {
  auto ad_trunc = ad_full.truncate(1.5, 10);

  // c and cdag matrices should have dimensions matching the truncated subspace sizes:
  // c_matrix(op, sp) has shape (dim_target, dim_source), where dim = eigenstate count.
  for (int op = 0; op < static_cast<int>(fops.size()); ++op) {
    for (int sp = 0; sp < ad_trunc.n_subspaces(); ++sp) {
      if (auto c_conn = ad_trunc.c_connection(op, sp); c_conn != -1) {
        auto mat = ad_trunc.c_matrix(op, sp);
        EXPECT_EQ(ad_trunc.get_subspace_dim(c_conn), mat.extent(0));
        EXPECT_EQ(ad_trunc.get_subspace_dim(sp), mat.extent(1));
      }
      if (auto cdag_conn = ad_trunc.cdag_connection(op, sp); cdag_conn != -1) {
        auto mat = ad_trunc.cdag_matrix(op, sp);
        EXPECT_EQ(ad_trunc.get_subspace_dim(cdag_conn), mat.extent(0));
        EXPECT_EQ(ad_trunc.get_subspace_dim(sp), mat.extent(1));
      }
    }
  }
}

TEST_F(atom_diag_truncation, MatrixValues) {
  auto ad_trunc = ad_full.truncate(1.5, 10);

  // Build a mapping from truncated subspace index -> full subspace index
  // by matching their Fock state content (which is preserved during truncation).
  auto fock_full  = ad_full.get_fock_states();
  auto fock_trunc = ad_trunc.get_fock_states();

  std::vector<int> sp_map(ad_trunc.n_subspaces());
  for (int sp_t = 0; sp_t < ad_trunc.n_subspaces(); ++sp_t) {
    bool found = false;
    for (int sp_f = 0; sp_f < ad_full.n_subspaces(); ++sp_f) {
      if (fock_trunc[sp_t] == fock_full[sp_f]) {
        sp_map[sp_t] = sp_f;
        found        = true;
        break;
      }
    }
    ASSERT_TRUE(found) << "No matching full subspace for truncated sp " << sp_t;
  }

  // Within each subspace, eigenvalues are sorted by eigh, so truncation
  // keeps a contiguous prefix {0, .., k-1}. The truncated operator matrix
  // must therefore equal the top-left (k_target x k_source) block of the
  // full matrix.
  for (int op = 0; op < static_cast<int>(fops.size()); ++op) {
    for (int sp_t = 0; sp_t < ad_trunc.n_subspaces(); ++sp_t) {
      int sp_f      = sp_map[sp_t];
      int dim_src_t = ad_trunc.get_subspace_dim(sp_t);

      if (auto conn = ad_trunc.c_connection(op, sp_t); conn != -1) {
        int dim_tgt_t  = ad_trunc.get_subspace_dim(static_cast<int>(conn));
        auto const &mt = ad_trunc.c_matrix(op, sp_t);
        auto const &mf = ad_full.c_matrix(op, sp_f);
        for (int i = 0; i < dim_tgt_t; ++i)
          for (int j = 0; j < dim_src_t; ++j) EXPECT_NEAR(mt(i, j), mf(i, j), 1e-12);
      }

      if (auto conn = ad_trunc.cdag_connection(op, sp_t); conn != -1) {
        int dim_tgt_t  = ad_trunc.get_subspace_dim(static_cast<int>(conn));
        auto const &mt = ad_trunc.cdag_matrix(op, sp_t);
        auto const &mf = ad_full.cdag_matrix(op, sp_f);
        for (int i = 0; i < dim_tgt_t; ++i)
          for (int j = 0; j < dim_src_t; ++j) EXPECT_NEAR(mt(i, j), mf(i, j), 1e-12);
      }
    }
  }
}

TEST_F(atom_diag_truncation, GetOpMatThrowsOnTruncated) {
  // get_op_mat relies on the imperative_operator acting on the full Fock space,
  // which is incompatible with truncated eigensystems.
  auto ad_trunc = ad_full.truncate(1.5, 10);
  EXPECT_THROW(ad_trunc.get_op_mat(n("up", 0)), triqs::runtime_error);
}

// ============================================================
// Edge cases
// ============================================================

TEST_F(atom_diag_truncation, NoOpWithDefaultParams) {
  // Default parameters (infinite cutoff, unlimited states) produce an identical
  // copy with is_truncated() == false.
  auto ad_copy = ad_full.truncate();

  EXPECT_FALSE(ad_copy.is_truncated());
  EXPECT_EQ(ad_full.n_subspaces(), ad_copy.n_subspaces());
  EXPECT_EQ(ad_full.get_total_eigenstate_count(), ad_copy.get_total_eigenstate_count());
}

TEST_F(atom_diag_truncation, NoOpWithLargeCutoff) {
  // A finite cutoff that exceeds all eigenvalues also produces a non-truncated copy.
  auto ad_copy = ad_full.truncate(1000.0);

  EXPECT_FALSE(ad_copy.is_truncated());
  EXPECT_EQ(ad_full.n_subspaces(), ad_copy.n_subspaces());
  EXPECT_EQ(ad_full.get_total_eigenstate_count(), ad_copy.get_total_eigenstate_count());
}

TEST_F(atom_diag_truncation, Idempotence) {
  // Truncating an already-truncated object with the same parameters is a no-op.
  auto ad_trunc1 = ad_full.truncate(2.0, 20);
  auto ad_trunc2 = ad_trunc1.truncate(2.0, 20);

  EXPECT_EQ(ad_trunc1.n_subspaces(), ad_trunc2.n_subspaces());
  EXPECT_EQ(ad_trunc1.get_total_eigenstate_count(), ad_trunc2.get_total_eigenstate_count());

  for (int sp = 0; sp < ad_trunc1.n_subspaces(); ++sp) {
    EXPECT_EQ(ad_trunc1.get_subspace_dim(sp), ad_trunc2.get_subspace_dim(sp));
    for (int i = 0; i < ad_trunc1.get_subspace_dim(sp); ++i)
      EXPECT_NEAR(ad_trunc1.get_eigenvalue(sp, i), ad_trunc2.get_eigenvalue(sp, i), 1e-14);
  }
}

TEST_F(atom_diag_truncation, HDF5Roundtrip) {
  auto ad_trunc = ad_full.truncate(2.0, 15);

  auto ad_roundtrip = h5::deserialize<atom_diag_real>(h5::serialize(ad_trunc));

  EXPECT_EQ(ad_trunc.is_truncated(), ad_roundtrip.is_truncated());
  EXPECT_EQ(ad_trunc.n_subspaces(), ad_roundtrip.n_subspaces());
  EXPECT_EQ(ad_trunc.get_total_eigenstate_count(), ad_roundtrip.get_total_eigenstate_count());

  for (int sp = 0; sp < ad_trunc.n_subspaces(); ++sp) {
    EXPECT_EQ(ad_trunc.get_subspace_dim(sp), ad_roundtrip.get_subspace_dim(sp));
    for (int i = 0; i < ad_trunc.get_subspace_dim(sp); ++i)
      EXPECT_NEAR(ad_trunc.get_eigenvalue(sp, i), ad_roundtrip.get_eigenvalue(sp, i), 1e-14);
  }
}

// ============================================================
// Vacuum state
// ============================================================

TEST(atom_diag_truncation_vacuum, VacuumRemovedByTruncation) {
  // At large mu the filled states have very low (negative) bare energy,
  // while the empty state |0> sits near zero. After ground-state subtraction,
  // |0> ends up at a large positive eigenvalue and is excluded by a tight cutoff.
  auto fops = make_fops();
  auto h    = make_hamiltonian<many_body_operator_real>(/*mu=*/5.0, 1.0, 0.3, 0.03, 0.2);
  atom_diag_real ad_full(h, fops);

  EXPECT_TRUE(ad_full.has_vacuum());

  auto ad_trunc = ad_full.truncate(5.0);

  EXPECT_TRUE(ad_trunc.is_truncated());
  EXPECT_FALSE(ad_trunc.has_vacuum());
  EXPECT_EQ(ad_trunc.get_vacuum_subspace_index(), -1);
}

TEST(atom_diag_truncation_vacuum, VacuumSurvivesTruncation) {
  // At mu = 0 the empty state |0> is near the ground state and survives truncation.
  auto fops = make_fops();
  auto h    = make_hamiltonian<many_body_operator_real>(/*mu=*/0.0, 1.0, 0.3, 0.03, 0.2);
  atom_diag_real ad_full(h, fops);

  auto ad_trunc = ad_full.truncate(2.0);

  EXPECT_TRUE(ad_trunc.is_truncated());
  EXPECT_TRUE(ad_trunc.has_vacuum());
  EXPECT_GE(ad_trunc.get_vacuum_subspace_index(), 0);
  EXPECT_GT(nda::blas::dot(ad_trunc.get_vacuum_state(), ad_trunc.get_vacuum_state()), 0);
}

// ============================================================
// Complex-valued Hamiltonian
// ============================================================

// Verify that truncation works identically for the complex instantiation.
// We check the same core properties; the detailed matrix-value and edge-case
// logic is already covered by the real tests above.

TEST_F(atom_diag_truncation_complex, EnergyCutoff) {
  double cutoff = 1.5;
  auto ad_trunc = ad_full.truncate(cutoff);

  EXPECT_TRUE(ad_trunc.is_truncated());
  EXPECT_LT(ad_trunc.get_total_eigenstate_count(), ad_full.get_total_eigenstate_count());

  for (int sp = 0; sp < ad_trunc.n_subspaces(); ++sp)
    for (int i = 0; i < ad_trunc.get_subspace_dim(sp); ++i)
      EXPECT_LE(ad_trunc.get_eigenvalue(sp, i), cutoff + 1e-10);
}

TEST_F(atom_diag_truncation_complex, MaxStates) {
  int max_states = 10;
  auto ad_trunc  = ad_full.truncate(std::numeric_limits<double>::infinity(), max_states);

  EXPECT_TRUE(ad_trunc.is_truncated());
  EXPECT_EQ(max_states, ad_trunc.get_total_eigenstate_count());

  auto full_energies  = collect_energies(ad_full);
  auto trunc_energies = collect_energies(ad_trunc);
  for (int i = 0; i < max_states; ++i) EXPECT_NEAR(full_energies[i], trunc_energies[i], 1e-10);
}

TEST_F(atom_diag_truncation_complex, MatrixDimensions) {
  auto ad_trunc = ad_full.truncate(1.5, 10);

  for (int op = 0; op < static_cast<int>(fops.size()); ++op) {
    for (int sp = 0; sp < ad_trunc.n_subspaces(); ++sp) {
      if (auto c_conn = ad_trunc.c_connection(op, sp); c_conn != -1) {
        auto mat = ad_trunc.c_matrix(op, sp);
        EXPECT_EQ(ad_trunc.get_subspace_dim(c_conn), mat.extent(0));
        EXPECT_EQ(ad_trunc.get_subspace_dim(sp), mat.extent(1));
      }
      if (auto cdag_conn = ad_trunc.cdag_connection(op, sp); cdag_conn != -1) {
        auto mat = ad_trunc.cdag_matrix(op, sp);
        EXPECT_EQ(ad_trunc.get_subspace_dim(cdag_conn), mat.extent(0));
        EXPECT_EQ(ad_trunc.get_subspace_dim(sp), mat.extent(1));
      }
    }
  }
}

TEST_F(atom_diag_truncation_complex, HDF5Roundtrip) {
  auto ad_trunc = ad_full.truncate(2.0, 15);

  auto ad_roundtrip = h5::deserialize<atom_diag_complex>(h5::serialize(ad_trunc));

  EXPECT_EQ(ad_trunc.is_truncated(), ad_roundtrip.is_truncated());
  EXPECT_EQ(ad_trunc.n_subspaces(), ad_roundtrip.n_subspaces());
  EXPECT_EQ(ad_trunc.get_total_eigenstate_count(), ad_roundtrip.get_total_eigenstate_count());

  for (int sp = 0; sp < ad_trunc.n_subspaces(); ++sp) {
    EXPECT_EQ(ad_trunc.get_subspace_dim(sp), ad_roundtrip.get_subspace_dim(sp));
    for (int i = 0; i < ad_trunc.get_subspace_dim(sp); ++i)
      EXPECT_NEAR(ad_trunc.get_eigenvalue(sp, i), ad_roundtrip.get_eigenvalue(sp, i), 1e-14);
  }
}

MAKE_MAIN;
