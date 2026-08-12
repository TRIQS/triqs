// Copyright (c) 2024 Simons Foundation
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

/**
 * @file
 * @brief Regression test pinning the DLR meshes (dlr_imfreq, dlr_imtime) over a Cartesian product of
 * (beta, statistic, w_max, eps, symmetrize) parameters against mesh_dlr_stability.ref.h5, guarding
 * against unintended changes in the DLR construction (e.g. an updated cppdlr).
 *
 * For each parameter set we rebuild the meshes and compare their characterizing quantities: the DLR
 * frequencies and imaginary-time nodes (floating point, compared with a tight relative tolerance, as
 * compilers/BLAS backends differ in the last ulps) and the integer Matsubara and imaginary-time
 * fine-grid indices (compared exactly).
 * The mesh hashes are intentionally not pinned: they depend on the standard-library hashing
 * implementation and so are not portable across toolchains. Failures report the parameter set via
 * SCOPED_TRACE.
 *
 * Only these quantities are stored, not the full meshes: the r-by-r transform matrices would dominate
 * the file size (~100x) without adding coverage, and the HDF5 round-trip is tested in mesh_dlr.cpp.
 * The plain triqs::mesh::dlr is omitted as it shares the pinned DLR frequencies.
 *
 * To regenerate: run the test to produce mesh_dlr_stability.out.h5, copy it onto the .ref.h5 in the
 * source and build trees, and confirm the change is expected before committing.
 */

#include <triqs/mesh/dlr_imfreq.hpp>
#include <triqs/mesh/dlr_imtime.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>
#include <nda/nda.hpp>

#include <filesystem>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace triqs::mesh;

namespace {

  // A single set of DLR mesh parameters.
  struct params_t {
    double beta;
    statistic_enum statistic;
    double w_max;
    double eps;
    bool symmetrize;
  };

  // Human-readable description of a parameter set, used in failure messages.
  std::string to_string(params_t const &p) {
    std::ostringstream s;
    s << "beta=" << p.beta << " statistic=" << (p.statistic == Fermion ? "Fermion" : "Boson") << " w_max=" << p.w_max << " eps=" << p.eps
      << " symmetrize=" << std::boolalpha << p.symmetrize;
    return s.str();
  }

  // Parameter values to pin. The meshes are built for every combination (Cartesian product).
  std::vector<params_t> const all_params = [] {
    std::vector<params_t> res;
    for (double beta : {10., 20., 40., 80., 160., 320.})
      for (auto statistic : {Fermion, Boson})
        for (double w_max : {1., 10.})
          for (double eps : {1e-6, 1e-10})
            for (bool symmetrize : {true, false})
              res.push_back({.beta = beta, .statistic = statistic, .w_max = w_max, .eps = eps, .symmetrize = symmetrize});
    return res;
  }();

  // Deterministic, filesystem-safe group name for the i-th parameter set.
  std::string group_name(int i) { return "case_" + std::to_string(i); }

  // Relative tolerance for the node comparisons: a few ulps, i.e. differences in the
  // last 1-2 significant decimal digits.
  double constexpr node_rtol = 1e-14;

  // Build the two DLR meshes for parameter set p.
  std::pair<dlr_imfreq, dlr_imtime> make_meshes(params_t const &p) {
    return {dlr_imfreq{p.beta, p.statistic, p.w_max, p.eps, p.symmetrize}, dlr_imtime{p.beta, p.statistic, p.w_max, p.eps, p.symmetrize}};
  }

  // Write the quantities that characterize the DLR meshes for parameter set p into group g.
  void write_pinned_quantities(h5::group g, params_t const &p) {
    auto [m_imfreq, m_imtime] = make_meshes(p);
    h5::write(g, "dlr_freq", m_imfreq.dlr_freq());
    h5::write(g, "ifnodes", m_imfreq.dlr_if().get_ifnodes());
    h5::write(g, "itnodes", m_imtime.dlr_it().get_itnodes());
    h5::write(g, "itnodes_idx", m_imtime.dlr_it().get_itnodes_idx());
  }

} // namespace

// Build the meshes for all parameter sets, write their characterizing quantities to the
// .out.h5 file and compare them against the stored reference in the .ref.h5 file.
TEST(MeshDLRRef, CompareAgainstReference) {
  std::string const out_file = "mesh_dlr_stability.out.h5";
  std::string const ref_file = "mesh_dlr_stability.ref.h5";

  // Write freshly built quantities to the output file.
  {
    h5::file fout(out_file, 'w');
    for (int i = 0; auto const &p : all_params) {
      write_pinned_quantities(h5::group(fout).create_group(group_name(i)), p);
      ++i;
    }
  }

  // On first creation the reference does not exist yet: skip and instruct how to seed it.
  if (!std::filesystem::exists(ref_file)) {
    GTEST_SKIP() << "Reference file '" << ref_file << "' not found. Seed it by copying '" << out_file << "' onto it (in source and build trees).";
  }

  // Compare freshly built quantities against the stored reference.
  h5::file fref(ref_file, 'r');
  for (int i = 0; auto const &p : all_params) {
    SCOPED_TRACE(group_name(i) + ": " + to_string(p));
    auto g                    = h5::group(fref).open_group(group_name(i));
    auto [m_imfreq, m_imtime] = make_meshes(p);

    // DLR frequencies (shared by both meshes), up to last-ulp rounding differences.
    EXPECT_ARRAY_REL_NEAR(m_imfreq.dlr_freq(), h5::read<nda::vector<double>>(g, "dlr_freq"), node_rtol);
    // Matsubara index nodes (dlr_imfreq, exact) and imaginary-time nodes (dlr_imtime, up to rounding).
    EXPECT_EQ_ARRAY(m_imfreq.dlr_if().get_ifnodes(), h5::read<nda::vector<int>>(g, "ifnodes"));
    EXPECT_ARRAY_REL_NEAR(m_imtime.dlr_it().get_itnodes(), h5::read<nda::vector<double>>(g, "itnodes"), node_rtol);
    // Fine-grid indices of the imaginary-time nodes (exact).
    EXPECT_EQ_ARRAY(m_imtime.dlr_it().get_itnodes_idx(), h5::read<nda::vector<int>>(g, "itnodes_idx"));
    ++i;
  }
}

MAKE_MAIN;
