// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include <triqs/mc_tools/random_generator.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <fmt/ranges.h>

#include <cmath>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

TEST(TRIQSMCTools, RandomGeneratorNames) {
  fmt::print("{}\n", triqs::mc_tools::random_generator_names());
  auto names = triqs::mc_tools::random_generator_names_list();
  fmt::print("{}\n", names);
  EXPECT_EQ(names.size(), 6);
  EXPECT_EQ(names[0], "mt19937_64");
}

// Verify that the default (empty string) and "mt19937_64" produce the same sequence.
TEST(TRIQSMCTools, DefaultIsMT19937_64) {
  using namespace triqs::mc_tools;
  int const seed   = 42;
  auto rng_default = random_generator("", seed, mpi::communicator{});
  auto rng_named   = random_generator("mt19937_64", seed, mpi::communicator{});
  for (int i = 0; i < 100; ++i) EXPECT_DOUBLE_EQ(rng_default(), rng_named());
}

// Verify that the wrapped mt19937_64 produces the same double sequence as direct usage
// of an engine seeded with splitmix_seed_seq and our 53-bit conversion formula.
TEST(TRIQSMCTools, MT19937_64MatchesDirectUsage) {
  using namespace triqs::mc_tools;
  mpi::communicator c;
  std::uint64_t const seed = 0x18a2b3c4;
  auto rng                 = random_generator("mt19937_64", seed, c);
  // the communicator ctor folds in the rank as spawn key {rank}
  auto sseq = splitmix_seed_seq{seed, {static_cast<std::uint64_t>(c.rank())}};
  std::mt19937_64 direct_engine{sseq};

  for (int i = 0; i < 100; ++i) {
    double const from_rng    = rng();
    auto raw                 = direct_engine();
    double const from_direct = (raw >> 11) * 0x1.0p-53;
    EXPECT_DOUBLE_EQ(from_rng, from_direct);
  }
}

// Spawn keys identify parallel streams in splitmix_seed_seq (the internal seed sequence; the public
// random_generator API no longer exposes spawn keys). Equal keys reproduce the stream, distinct keys
// -- including different hierarchy levels and different order -- give decorrelated streams.
TEST(TRIQSMCTools, SpawnKeyStreams) {
  using namespace triqs::mc_tools;
  std::uint64_t const seed = 198;

  // generate n 32-bit words from a splitmix_seed_seq seeded with (seed, key)
  auto words = [&](std::initializer_list<std::uint64_t> key, int n) {
    splitmix_seed_seq s{seed, key};
    std::vector<std::uint32_t> w(n);
    s.generate(w.begin(), w.end());
    return w;
  };
  int const n = 16;

  EXPECT_EQ(words({0, 0}, n), words({0, 0}, n)); // equal keys -> identical
  EXPECT_NE(words({0, 0}, n), words({0, 1}, n)); // different thread level
  EXPECT_NE(words({0, 0}, n), words({1, 0}, n)); // different rank level
  EXPECT_NE(words({0}, n), words({0, 0}, n));    // different key depth
  EXPECT_NE(words({}, n), words({0}, n));        // keyed vs unkeyed
  // fold sanity check: the key chain is order-sensitive, so {0,1} and {1,0} differ (a plain
  // sequence of independent mixes that ignored the running state could collide here).
  EXPECT_NE(words({0, 1}, n), words({1, 0}, n));
}

// The communicator constructor folds in the rank as spawn key {rank}, matching a direct
// splitmix_seed_seq{seed, {rank}}.
TEST(TRIQSMCTools, CommunicatorSeeding) {
  using namespace triqs::mc_tools;
  mpi::communicator c;
  std::uint64_t const seed = 198;
  auto r1                  = random_generator("", seed, c);
  auto sseq                = splitmix_seed_seq{seed, {static_cast<std::uint64_t>(c.rank())}};
  std::mt19937_64 direct{sseq};
  for (int i = 0; i < 100; ++i) EXPECT_DOUBLE_EQ(r1(), (direct() >> 11) * 0x1.0p-53);
}

// Across MPI ranks, passing different seeds to the communicator constructor is a programming error:
// the collective same-seed check must throw on every rank (only exercised under np > 1).
TEST(TRIQSMCTools, CommunicatorSeedMismatchThrows) {
  using namespace triqs::mc_tools;
  mpi::communicator c;
  if (c.size() < 2) GTEST_SKIP() << "needs np > 1";
  EXPECT_THROW(random_generator("mt19937_64", 100 + c.rank(), c), std::runtime_error);
  // equal seeds must not throw
  EXPECT_NO_THROW(random_generator("mt19937_64", 42, c));
}

// Across MPI ranks, the communicator constructor yields pairwise different streams (run with np > 1).
TEST(TRIQSMCTools, CommunicatorRankStreams) {
  using namespace triqs::mc_tools;
  mpi::communicator c;
  int const n = 100;

  for (auto const &name : random_generator_names_list()) {
    auto rng = random_generator(name, 198, c);
    auto v   = std::vector<double>(n);
    for (auto &x : v) x = rng();

    auto all = mpi::all_gather(v, c);
    ASSERT_EQ(all.size(), static_cast<std::size_t>(n) * c.size());
    for (int r1 = 0; r1 < c.size(); ++r1)
      for (int r2 = r1 + 1; r2 < c.size(); ++r2) {
        bool diff = false;
        for (int i = 0; i < n; ++i) diff |= (all[r1 * n + i] != all[r2 * n + i]);
        EXPECT_TRUE(diff) << "Engine: " << name << ", identical streams on ranks " << r1 << " and " << r2;
      }
  }
}

// Across MPI ranks, the integer sequences from the communicator constructor are statistically
// independent (run with np > 1). For each pair of ranks we check two things: the Pearson
// cross-correlation at lag 0 and at several lags in both directions (catches linear/shifted
// correlation), and a chi-squared test of independence on the joint 2-D histogram (catches general,
// including nonlinear, dependence). Identical, duplicated, or lag-shifted streams would be caught.
TEST(TRIQSMCTools, CommunicatorRankCorrelation) {
  using namespace triqs::mc_tools;
  mpi::communicator c;
  int const n       = 100000;
  int const range   = 1000000;
  int const max_lag = 10;
  // Under independence the sample correlation has standard error 1/sqrt(n) ~ 3.2e-3, irrespective of
  // the (uniform) marginals. The test is deterministic (fixed seed, standard engines), and the
  // decorrelated streams here peak at 0.0103 (~3.3 sigma, the expected max over the 756 comparisons at np=4).
  // The bound below is 6 sigma: comfortably above that, yet far tighter than a round 0.05 (~16 sigma),
  // so it still flags any genuine correlation down to ~0.02.
  double const max_corr = 6.0 / std::sqrt(double(n));

  // Pearson correlation of the first m aligned samples of a and b.
  auto pearson = [](double const *a, double const *b, int m) {
    double sa = 0, sb = 0, saa = 0, sbb = 0, sab = 0;
    for (int i = 0; i < m; ++i) {
      sa += a[i];
      sb += b[i];
      saa += a[i] * a[i];
      sbb += b[i] * b[i];
      sab += a[i] * b[i];
    }
    double const cov = sab - sa * sb / m;
    double const va  = saa - sa * sa / m;
    double const vb  = sbb - sb * sb / m;
    return cov / std::sqrt(va * vb);
  };

  // Pearson chi-squared statistic for independence of a and b, each binned into `bins` bins over
  // [0, range), against the product of the observed marginals. Catches general (incl. nonlinear)
  // dependence that the linear correlation above would miss.
  int const bins = 20;
  auto chi2      = [](double const *a, double const *b, int m) {
    std::vector<long> joint(static_cast<std::size_t>(bins) * bins, 0), ra(bins, 0), cb(bins, 0);
    auto whichbin = [](double x) { return std::min(bins - 1, static_cast<int>(x / range * bins)); };
    for (int i = 0; i < m; ++i) {
      int const ja = whichbin(a[i]), jb = whichbin(b[i]);
      ++joint[static_cast<std::size_t>(ja) * bins + jb];
      ++ra[ja];
      ++cb[jb];
    }
    double s = 0;
    for (int j = 0; j < bins; ++j)
      for (int k = 0; k < bins; ++k) {
        double const e = static_cast<double>(ra[j]) * static_cast<double>(cb[k]) / m;
        double const d = static_cast<double>(joint[static_cast<std::size_t>(j) * bins + k]) - e;
        if (e > 0) s += d * d / e;
      }
    return s;
  };
  // chi-squared with df = (bins-1)^2; mean df, sd sqrt(2 df). The decorrelated streams here peak at
  // chi2 ~ 417 (~2.1 sigma, at np=4). The bound df + 6 sqrt(2 df) is 6 sd in the normal approximation;
  // the chi-squared right skew makes this a less extreme upper-tail quantile than a true 6 sigma, but
  // it is still ~p=1e-7 -- it never false-fails here yet flags genuine dependence.
  double const df       = (bins - 1) * (bins - 1);
  double const max_chi2 = df + 6.0 * std::sqrt(2.0 * df);

  for (auto const &name : random_generator_names_list()) {
    auto rng = random_generator(name, 198, c);
    auto v   = std::vector<double>(n);
    for (auto &x : v) x = rng(range); // each integer is exact in double (range < 2^53); the running
                                      // sums of products in pearson()/chi2() exceed 2^53, but the
                                      // resulting rounding (~1e-13 in the correlation) is far below the thresholds

    auto all = mpi::all_gather(v, c);
    ASSERT_EQ(all.size(), static_cast<std::size_t>(n) * c.size());

    for (int r1 = 0; r1 < c.size(); ++r1)
      for (int r2 = r1 + 1; r2 < c.size(); ++r2) {
        double const *a = all.data() + static_cast<long>(r1) * n;
        double const *b = all.data() + static_cast<long>(r2) * n;
        EXPECT_LT(std::abs(pearson(a, b, n)), max_corr) << "Engine: " << name << ", ranks " << r1 << "/" << r2 << " at lag 0";
        for (int lag = 1; lag <= max_lag; ++lag) {
          double const c_ab = pearson(a, b + lag, n - lag); // a[i] vs b[i+lag]
          double const c_ba = pearson(a + lag, b, n - lag); // a[i+lag] vs b[i]
          EXPECT_LT(std::abs(c_ab), max_corr) << "Engine: " << name << ", corr " << c_ab << " ranks " << r1 << "/" << r2 << " at lag +" << lag;
          EXPECT_LT(std::abs(c_ba), max_corr) << "Engine: " << name << ", corr " << c_ba << " ranks " << r1 << "/" << r2 << " at lag -" << lag;
        }
        double const chi = chi2(a, b, n);
        EXPECT_LT(chi, max_chi2) << "Engine: " << name << ", chi2 " << chi << " ranks " << r1 << "/" << r2;
      }
  }
}

// Verify seed reproducibility: same engine + same seed = same sequence.
TEST(TRIQSMCTools, SeedReproducibility) {
  using namespace triqs::mc_tools;
  for (auto const &name : random_generator_names_list()) {
    auto rng1 = random_generator(name, 12345, mpi::communicator{});
    auto rng2 = random_generator(name, 12345, mpi::communicator{});
    for (int i = 0; i < 50; ++i) EXPECT_DOUBLE_EQ(rng1(), rng2()) << "Engine: " << name;
  }
}

// Verify all engines produce valid doubles in [0, 1).
TEST(TRIQSMCTools, AllEnginesDoubleRange) {
  using namespace triqs::mc_tools;
  for (auto const &name : random_generator_names_list()) {
    auto rng = random_generator(name, 54321, mpi::communicator{});
    for (int i = 0; i < 10000; ++i) {
      double val = rng();
      EXPECT_GE(val, 0.0) << "Engine: " << name;
      EXPECT_LT(val, 1.0) << "Engine: " << name;
    }
  }
}

// Verify all engines produce valid integers in the requested range.
TEST(TRIQSMCTools, AllEnginesIntegerRange) {
  using namespace triqs::mc_tools;
  for (auto const &name : random_generator_names_list()) {
    auto rng = random_generator(name, 99999, mpi::communicator{});
    for (int i = 0; i < 10000; ++i) {
      auto val = rng(100);
      EXPECT_GE(val, 0);
      EXPECT_LT(val, 100);
    }
  }
}

// Integer generation at large ranges: verify results span the full 64-bit range.
TEST(TRIQSMCTools, LargeRangeIntegerGeneration) {
  using namespace triqs::mc_tools;
  auto rng                  = random_generator("mt19937_64", 42, mpi::communicator{});
  constexpr auto range      = std::numeric_limits<std::uint64_t>::max();
  bool has_high_bits        = false;
  bool has_low_bits         = false;
  constexpr auto half_range = range / 2;

  for (int i = 0; i < 10000; ++i) {
    auto val = rng(range);
    if (val > half_range) has_high_bits = true;
    if (val < half_range) has_low_bits = true;
    EXPECT_LT(val, range);
  }
  EXPECT_TRUE(has_high_bits) << "No values in upper half of 64-bit range";
  EXPECT_TRUE(has_low_bits) << "No values in lower half of 64-bit range";
}

// Integer uniformity: chi-squared test for moderate range.
TEST(TRIQSMCTools, IntegerUniformity) {
  using namespace triqs::mc_tools;
  auto rng           = random_generator("mt19937_64", 42, mpi::communicator{});
  constexpr int bins = 100;
  constexpr int N    = 1000000;
  std::vector<int> counts(bins, 0);

  for (int i = 0; i < N; ++i) { ++counts[rng(bins)]; }

  double expected = static_cast<double>(N) / bins;
  double chi2     = 0.0;
  for (int c : counts) { chi2 += (c - expected) * (c - expected) / expected; }

  // chi-squared with 99 df: p=0.001 critical value is ~148.2
  EXPECT_LT(chi2, 150.0) << "Chi-squared test failed: distribution is not uniform";
}

// Double generation quality: verify 53 bits of mantissa are used.
TEST(TRIQSMCTools, DoublePrecision53Bits) {
  using namespace triqs::mc_tools;
  auto rng = random_generator("mt19937_64", 42, mpi::communicator{});

  // Count distinct values in a small interval. With 53-bit precision,
  // values near 0.5 should have spacing ~2^-53 ≈ 1.1e-16.
  // With only 32-bit precision, spacing would be ~2^-32 ≈ 2.3e-10.
  int distinct_low_bits = 0;
  for (int i = 0; i < 100000; ++i) {
    double val = rng();
    // Check if the value has non-trivial bits below the 32-bit precision threshold.
    // Multiply by 2^53 and check if the lower 21 bits (53-32) are non-zero.
    auto scaled   = static_cast<std::uint64_t>(val * (1ULL << 53));
    auto low_bits = scaled & ((1ULL << 21) - 1);
    if (low_bits != 0) ++distinct_low_bits;
  }

  // With true 53-bit precision, roughly half the values should have non-zero lower 21 bits.
  EXPECT_GT(distinct_low_bits, 40000) << "Doubles do not appear to use full 53-bit precision";
}

// Preview returns the same value as the next call to operator().
TEST(TRIQSMCTools, PreviewConsistency) {
  using namespace triqs::mc_tools;
  auto rng = random_generator("mt19937_64", 42, mpi::communicator{});
  for (int i = 0; i < 100; ++i) {
    double preview_val = rng.preview();
    double actual_val  = rng();
    EXPECT_DOUBLE_EQ(preview_val, actual_val);
  }
}

// HDF5 round-trip: save/restore and verify continued sequence.
TEST(TRIQSMCTools, RandomGeneratorHDF5) {
  using namespace triqs::mc_tools;
  // rank-specific file names to avoid lock contention when run under MPI
  auto rank       = mpi::communicator{}.rank();
  // random_generator is not default-constructible, so we do the round-trip manually (rw_h5 requires
  // a default ctor); h5 reconstructs it via random_generator::h5_read_construct.
  auto check_hdf5 = [rank](std::string const &name) {
    auto label = name.empty() ? std::string{"default"} : name;
    auto fname = fmt::format("mctools_rng_{}_r{}.h5", label, rank);
    auto rng   = random_generator(name, 0x18a2b3c4, mpi::communicator{});
    for (int i = 0; i < 10; ++i) rng();
    { h5::file f{fname, 'w'}; h5::write(f, label, rng); }
    auto rng2 = [&] {
      h5::file f{fname, 'r'};
      return h5::read<random_generator>(f, label);
    }();
    for (int i = 0; i < 10; ++i) EXPECT_DOUBLE_EQ(rng(), rng2()) << "HDF5 round-trip failed for engine: " << name;
  };

  for (auto const &name : random_generator_names_list()) check_hdf5(name);
  check_hdf5(""); // default
}

// Move semantics: verify move constructor and assignment produce identical sequences.
TEST(TRIQSMCTools, RandomGeneratorMoveOperation) {
  using namespace triqs::mc_tools;
  auto rng  = random_generator("mt19937_64", 198, mpi::communicator{});
  auto rng2 = random_generator("mt19937_64", 198, mpi::communicator{});
  for (int i = 0; i < 10; ++i) {
    rng();
    rng2();
  }

  // move constructor
  auto rng3 = std::move(rng);
  for (int i = 0; i < 10; ++i) EXPECT_DOUBLE_EQ(rng2(), rng3());

  // move assignment
  auto rng4 = random_generator("mt19937_64", 198, mpi::communicator{});
  rng4      = std::move(rng2);
  for (int i = 0; i < 10; ++i) EXPECT_DOUBLE_EQ(rng3(), rng4());
}

MAKE_MAIN;
