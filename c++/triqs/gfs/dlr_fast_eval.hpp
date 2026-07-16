#pragma once
#include <triqs/gfs.hpp>
#include <treeweave/treeweave.hpp>
#include <array>
#include <complex>

namespace triqs::gfs {
  namespace detail {
    // Self-contained fit callable: owns the DLR coefficients so the treeweave
    // Function is freely movable with no dangling references.
    // ponytail: owns the coeff gf by value (not this/&member) so the treeweave
    // Function, which stores the callable by value and may move, never dangles.
    struct dlr_scalar_packer {
      gf<mesh::dlr, scalar_valued> g; // DLR coefficient gf (from make_gf_dlr)
      std::array<double, 2> operator()(std::array<double, 1> x) const {
        auto v = g(x[0]);
        return {std::real(v), std::imag(v)};
      }
    };
  } // namespace detail

  // Fast repeated evaluation of a scalar DLR Green's function at arbitrary
  // tau in [0, beta]. Builds an adaptive treeweave tree once (fit-once,
  // query-many); safe because it owns a snapshot of the coefficients.
  class dlr_fast_eval {
    using fit_t = decltype(treeweave::fit(std::declval<detail::dlr_scalar_packer>(), std::array<double, 1>{}, std::array<double, 1>{}, 1.0));
    fit_t fn_;
    static fit_t make_fit(gf<mesh::dlr, scalar_valued> g, double tol) {
      double beta = g.mesh().beta(); // read before moving g
      return treeweave::fit(detail::dlr_scalar_packer{std::move(g)}, std::array{0.0}, std::array{beta}, tol);
    }

    public:
    explicit dlr_fast_eval(gf<mesh::dlr, scalar_valued> g, double tol = 1e-10) : fn_(make_fit(std::move(g), tol)) {}
    dcomplex operator()(double tau) const {
      auto r = fn_(std::array{tau});
      return {r[0], r[1]};
    }
  };
} // namespace triqs::gfs
