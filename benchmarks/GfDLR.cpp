// Copyright (c) 2022-2023 Simons Foundation
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

#include <benchmark/benchmark.h>
#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
#include <baobzi_template.hpp>
#include <treeweave/treeweave.hpp>

// ===== Interpolate DLR ImTime Green function

constexpr nda::clef::placeholder<2> tau_;
using namespace triqs::gfs;

auto onefermion(auto tau, double eps, double beta) { return -exp(-eps * tau) / (1 + exp(-beta * eps)); }

static void GfDLREval(benchmark::State &state) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto mesh = dlr_imtime{beta, Fermion, w_max, eps};
  auto G    = gf<dlr_imtime, scalar_valued>{mesh};
  G[tau_] << onefermion(tau_, omega, beta);
  auto G_dlr_coeff = make_gf_dlr(G);

  for (auto _ : state) {
    for (auto tau : mesh) {
      // Are there some low-hanging performance improvements here?
      benchmark::DoNotOptimize(G_dlr_coeff(double(tau)));
    }
  }
}
BENCHMARK(GfDLREval)->RangeMultiplier(2)->Range(1024, 8192)->Iterations(250000);

static void GfLegEval(benchmark::State &state) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto mesh = dlr_imtime{beta, Fermion, w_max, eps};
  auto G    = gf<dlr_imtime, scalar_valued>{mesh};
  G[tau_] << onefermion(tau_, omega, beta);

  auto const n_l = 15;
  int n_tau      = 10001;
  auto mesh_l    = legendre(beta, Fermion, n_l);
  auto Gl        = gf<legendre, scalar_valued>{mesh_l};
  legendre_matsubara_inverse(Gl, make_gf_imtime(G, n_tau));

  // Directly perform integral in GDLR ImTime
  // Q: What is the best sampling for the integral?
  // legendre_matsubara_inverse(Gl, G);
  // legendre_matsubara_inverse(Gl, Gcoeff);

  // Possible utility function API
  // make_gf_legendre(Gcoeff, n_l);
  // make_gf_legendre(G, n_l, n_tau);
  // make_gf_legendre(G, eps);
  // Q: How to choose eps / n_l?

  // Q: Do we want a paneled / piecewise version at all?

  for (auto _ : state) {
    for (auto tau : mesh) {
      // Are there some low-hanging performance improvements here?
      benchmark::DoNotOptimize(Gl(double(tau)));
    }
  }
}
BENCHMARK(GfLegEval)->RangeMultiplier(2)->Range(1024, 8192)->Iterations(250000);

static void GfDLREvalFast(benchmark::State &state) {
  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto mesh = dlr_imtime{beta, Fermion, w_max, eps};
  auto G    = gf<dlr_imtime, scalar_valued>{mesh};
  G[tau_] << onefermion(tau_, omega, beta);
  auto G_dlr_coeff = make_gf_dlr(G);

  auto input_funct = [&G_dlr_coeff](const double *x, double *res, const void *) {
    const auto eval = G_dlr_coeff(*x);
    res[0]          = real(eval);
    res[1]          = imag(eval);
  };

  baobzi_input_t input;
  input.output_dim = 2;
  input.tol        = 1e-10;
  // input.order = 12;
  input.split_multi_eval = false;

  const std::array center{0.0};
  const std::array half_length{w_max * beta};

  // use std ranges to iterate over mesh and append tau to samples
  std::vector<double> samples(mesh.size());
  std::ranges::transform(mesh, samples.begin(), [](auto tau) { return double(tau); });

  const baobzi::Function<1, 8> func(&input, center.data(), half_length.data(), input_funct, samples);

  const auto fast_dlr = [&func](const double x) {
    std::array<double, 2> res{};
    func(&x, res.data());
    return dcomplex(res[0], res[1]);
  };

  for (auto tau : mesh) {
    // check relative error
    auto eval = G_dlr_coeff(double(tau));
    auto res  = fast_dlr(double(tau));
    if (std::abs(1 - eval / res) > input.tol) { std::cerr << "Error: " << tau << " " << eval << " " << res << std::endl; }
  }

  for (auto _ : state) {
    for (auto tau : mesh) {
      // Are there some low-hanging performance improvements here?
      benchmark::DoNotOptimize(fast_dlr(double(tau)));
    }
  }
}

BENCHMARK(GfDLREvalFast)->RangeMultiplier(2)->Range(1024, 8192)->Iterations(250000);

static void GfDLREvalTreeWeave(benchmark::State &state) {
  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto mesh = dlr_imtime{beta, Fermion, w_max, eps};
  auto G    = gf<dlr_imtime, scalar_valued>{mesh};
  G[tau_] << onefermion(tau_, omega, beta);
  auto G_dlr_coeff = make_gf_dlr(G);

  // treeweave has no complex path: 1D scalar-input + array-output is a hard
  // static_assert, so spell the input as std::array<double,1> and pack the
  // complex value into a 2-vector output. tau lives in [0, beta].
  const auto fit_funct = [&G_dlr_coeff](std::array<double, 1> x) -> std::array<double, 2> {
    const auto eval = G_dlr_coeff(x[0]);
    return {real(eval), imag(eval)};
  };

  auto fn = treeweave::fit(fit_funct, std::array{0.0}, std::array{beta}, 1e-10);

  const auto fast_dlr = [&fn](const double x) {
    const auto res = fn(std::array{x});
    return dcomplex(res[0], res[1]);
  };

  for (auto tau : mesh) {
    // check relative error
    auto eval = G_dlr_coeff(double(tau));
    auto res  = fast_dlr(double(tau));
    if (std::abs(1 - eval / res) > 1e-10) { std::cerr << "Error: " << tau << " " << eval << " " << res << std::endl; }
  }

  for (auto _ : state) {
    for (auto tau : mesh) { benchmark::DoNotOptimize(fast_dlr(double(tau))); }
  }
}

BENCHMARK(GfDLREvalTreeWeave)->RangeMultiplier(2)->Range(1024, 8192)->Iterations(250000);

BENCHMARK_MAIN();
