#pragma once

#include "../utility/adaptive.hpp"
#include "../utility/integrator.hpp"
#include "../../gfs.hpp"

#include <itertools/itertools.hpp>
#include <mpi/mpi.hpp>
#include <nda/nda.hpp>

#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

namespace triqs::experimental::lattice {

  using namespace triqs::gfs;

  /**
   * @addtogroup triqs-experimental-lattice
   * @{
   */

  /**
   * @brief CLEF placeholders for the momentum and frequency arguments of integrable expressions.
   *
   * @details These placeholders are the ones recognized by the Brillouin-zone integrators in this module: `kx`, `ky`
   * and `kz` for the three momentum components and `w` for the frequency.
   */
  namespace placeholders {

    /// Placeholder for the first momentum component \f$ k_x \f$.
    constexpr nda::clef::placeholder<0> kx; // global var, clang requests const(expr)
    /// Placeholder for the second momentum component \f$ k_y \f$.
    constexpr nda::clef::placeholder<1> ky;
    /// Placeholder for the third momentum component \f$ k_z \f$.
    constexpr nda::clef::placeholder<2> kz;
    /// Placeholder for the frequency \f$ \omega \f$.
    constexpr nda::clef::placeholder<3> w;

  } // namespace placeholders

  /**
   * @brief Options controlling the adaptive Brillouin-zone integration.
   *
   * @details This struct is intended to be extended as improved adaptive integrators are added.
   */
  struct adaptive_options {
    /// Target absolute error of the adaptive integration.
    double tolerance = 1.e-3;
  };

  /**
   * @brief Options controlling the combined PTR and adaptive Brillouin-zone integration.
   *
   * @details The integration first attempts to converge each frequency with fixed k-grid (PTR) integration of
   * increasing grid density, starting from `k_grid` and increasing in steps of `delta_k_grid` until a point converges
   * or the grid reaches `k_grid_max`. The remaining unconverged frequency points are then integrated adaptively until
   * they reach the requested absolute tolerance.
   */
  struct bz_int_options {
    double tolerance                 = 1.e-3;        ///< Absolute tolerance of the integrated quantity.
    std::array<long, 3> k_grid       = {10, 10, 10}; ///< Initial number of PTR k-points along each direction.
    std::array<long, 3> delta_k_grid = {2, 2, 2};    ///< Increment of the k-grid size at each refinement step.
    std::array<long, 3> k_grid_max   = {20, 20, 20}; ///< Maximum number of k-points along each direction.
    bool run_adaptive                = true;         ///< Whether to run adaptive integration on the remaining points.
    bool run_ptr                     = true;         ///< Whether to run PTR integration before the adaptive step.
    bool verbose                     = false;        ///< Whether to print the convergence progress.
  };

  namespace detail {
    // helper to determine the return container dimension
    template <typename T>
    int deduce_dim_from_expression(auto const &f_kw, T const &w) {
      namespace ph  = placeholders;
      // need to use w here and not 0, as this can result in the expression becoming 1/0
      auto f_w_temp = eval(f_kw, ph::kx = 0., ph::ky = 0., ph::kz = 0., ph::w = w);
      static_assert(not nda::clef::is_lazy<decltype(f_w_temp)>, "Integration expects a proper expression with placeholders.");
      // check if this is a matrix or scalar type -- if scalar, return 1 for dim
      return [&]() {
        if constexpr (requires { f_w_temp.shape(); }) { // i.e. if this compiles ...
          return f_w_temp.shape()[0];                   // this is a matrix type with block dim
        } else {
          return 1; // this is a scalar type, will be handled as a 1D matrix
        }
      }();
    }
  } // namespace detail

  //---------------------------------------------------------

  /**
   * @brief Integrate an expression over the Brillouin zone on a fixed k-grid (PTR) for a list of frequencies, using
   * both MPI and OpenMP parallelism.
   *
   * @details The expression is evaluated on a regular grid of \f$ k_x \times k_y \times k_z \f$ points and averaged.
   * The work is distributed over MPI ranks along the longest grid direction and over OpenMP threads.
   *
   * @tparam T Type of the frequency values, convertible to `double` or `dcomplex` (e.g. a number or a mesh point).
   * @param f_kw CLEF expression to integrate, using the placeholders for \f$ k_x, k_y, k_z \f$ and \f$ \omega \f$.
   * @param omega_values List of frequency values at which the integral is evaluated.
   * @param k_grid Number of grid points along each direction; e.g. `{2, 2, 2}` samples a total of 8 k-points.
   * @param comm MPI communicator over which the k-grid is distributed.
   * @return Array of shape `[n_omega, dim, dim]` holding the integral, fully integrated over \f$ k_x, k_y, k_z \f$ for
   * each frequency.
   */
  template <typename T>
    requires(std::convertible_to<T, dcomplex> or std::convertible_to<T, double>) // allow for real or imaginary mesh points or numbers
  nda::array<dcomplex, 3> integrate_ptr(auto const &f_kw, std::vector<T> const &omega_values, std::array<long, 3> const &k_grid,
                                        mpi::communicator comm) {

    for (auto i : {0, 1, 2}) {
      if (k_grid[i] <= 0) throw std::runtime_error{"Cannot use PTR integration with kgrid dim <= 0."};
    }

    namespace ph  = placeholders;
    int block_dim = detail::deduce_dim_from_expression(f_kw, omega_values[0]);
    auto result   = nda::zeros<dcomplex>(omega_values.size(), block_dim, block_dim); // container to return

    // Determine the longest direction and apply MPI chunk to this dimension, otherwise provide a simple iterator
    auto mpi_chunk_max = [&k_grid, comm](int kdim) {
      // chunk the biggest one with MPI
      if (kdim == std::distance(k_grid.begin(), std::ranges::max_element(k_grid))) { return mpi::chunk(nda::range(k_grid[kdim]), comm); }
      return itertools::slice(nda::range(k_grid[kdim]), 0,
                              k_grid[kdim]); // otherwise complains about different return types
    };

// omp reduction operation for nda array
#pragma omp declare reduction(array_add_c_3 : nda::array<dcomplex, 3> : omp_out += omp_in)                                                           \
   initializer(omp_priv = nda::array<dcomplex, 3>(omp_orig.shape()))

#pragma omp parallel for collapse(3) reduction(array_add_c_3 : result) default(none)                                                                 \
   shared(k_grid, omega_values, f_kw, ph::kx, ph::ky, ph::kz, ph::w, mpi_chunk_max)
    for (auto ikx : mpi_chunk_max(0)) {
      for (auto iky : mpi_chunk_max(1)) {
        for (auto ikz : mpi_chunk_max(2)) {
          double kx = ikx / double(k_grid[0]);
          double ky = iky / double(k_grid[1]);
          double kz = ikz / double(k_grid[2]);
          auto f_w  = eval(f_kw, ph::kx = kx, ph::ky = ky, ph::kz = kz);
          for (auto &&[n, omega] : itertools::enumerate(omega_values)) result(n, nda::range::all, nda::range::all) += eval(f_w, ph::w = omega);
        }
      }
    }
    result = mpi::all_reduce(result, comm);

    // apply normalization, divide by number of k-points on integrated grid
    result /= double(k_grid[0] * k_grid[1] * k_grid[2]);
    return result;
  }

  // -------------------------------------------

  /**
   * @brief Integrate an expression over the Brillouin zone on a fixed k-grid (PTR) for all points of a frequency mesh,
   * using both MPI and OpenMP parallelism.
   *
   * @details This overload evaluates the integral at every point of the given frequency mesh and stores the result in a
   * Green's function defined on that mesh.
   *
   * @tparam Mesh Frequency mesh type.
   * @param f_kw CLEF expression to integrate, using the placeholders for \f$ k_x, k_y, k_z \f$ and \f$ \omega \f$.
   * @param w_mesh Frequency mesh on which the integration is performed.
   * @param k_grid Number of grid points along each direction; e.g. `{2, 2, 2}` samples a total of 8 k-points.
   * @param comm MPI communicator over which the k-grid is distributed.
   * @return Green's function on `w_mesh` holding the integral, fully integrated over \f$ k_x, k_y, k_z \f$.
   */
  template <typename Mesh> auto integrate_ptr(auto const &f_kw, Mesh const &w_mesh, std::array<long, 3> const &k_grid, mpi::communicator comm = {}) {

    int dim    = detail::deduce_dim_from_expression(f_kw);
    auto g_out = gf{w_mesh, {dim, dim}};
    std::vector<typename Mesh::mesh_point_t> mesh_points(w_mesh.begin(), w_mesh.end());
    auto ptr_result = integrate_ptr(f_kw, mesh_points, k_grid, comm);
    // fill in the GF to return
    for (auto &&[n, w] : itertools::enumerate(mpi::chunk(w_mesh, comm))) { g_out[w] = calc(w); }
    return g_out;
  }

  // -------------------------------------------
  /**
   * @brief Build a callable that adaptively integrates an expression over the Brillouin zone for a given frequency.
   *
   * @details This overload returns a lambda that, given a frequency \f$ \omega \f$, performs the adaptive
   * three-dimensional Brillouin-zone integration of the expression. The k-integration itself is not parallelized, but
   * the returned callable can be evaluated in parallel over different frequencies.
   *
   * @param f_kw CLEF expression to integrate, using the placeholders for \f$ k_x, k_y, k_z \f$ and \f$ \omega \f$.
   * @param opt Adaptive integration options (currently only the absolute tolerance).
   * @param w A frequency from the mesh which will be integrated, needed for type eval
   * @return Callable that maps a frequency \f$ \omega \f$ to the value of the Brillouin-zone integral.
   */
   template <typename T>
  auto integrate_adaptive(auto const &f_kw, adaptive_options const &opt, T const &w) {

    // use the first mesh value, evaluated, to determine the return type of the data
    namespace ph      = placeholders;
    // need to use w here and not 0, as this can result in the expression becoming 1/0
    auto f_value      = nda::make_regular(eval(f_kw, ph::kx = 0., ph::ky = 0., ph::kz = 0., ph::w = w));
    auto int_1d_adapt = utility::integrate_1d_adapt<decltype(f_value)>{opt.tolerance};

    // OP : Beware the capture ! We need to move the expression.
    return [expr_kw = std::move(f_kw), int_1d_adapt](auto om) { // NOLINT
      std::pair<double, double> k_domain = {0, 1};
      auto expr_k                        = eval(expr_kw, ph::w = om);

      return utility::integrate(int_1d_adapt,
                                utility::integrate(int_1d_adapt, utility::integrate(int_1d_adapt, expr_k, ph::kx = k_domain), ph::ky = k_domain),
                                ph::kz = k_domain);
    };
  }

  /**
   * @brief Adaptively integrate an expression over the Brillouin zone for all points of a frequency mesh.
   *
   * @details This overload evaluates the adaptive Brillouin-zone integral at every point of the given frequency mesh,
   * in parallel over frequencies, and stores the result in a Green's function defined on that mesh.
   *
   * @tparam Mesh Frequency mesh type.
   * @param f_kw CLEF expression to integrate, using the placeholders for \f$ k_x, k_y, k_z \f$ and \f$ \omega \f$.
   * @param w_mesh Frequency mesh on which the integral is evaluated and which defines the returned Green's function.
   * @param opt Adaptive integration options (currently only the absolute tolerance).
   * @return Green's function on `w_mesh` holding the adaptively integrated expression.
   */
  template <typename Mesh> auto integrate_adaptive(auto const &f_kw, Mesh const &w_mesh, adaptive_options const &opt) {

    int dim    = detail::deduce_dim_from_expression(f_kw, w_mesh[0]);
    auto g_out = gf{w_mesh, {dim, dim}};
    // OMP/MPI parallel evaluation over frequencies
    mpi::communicator comm = {};
    auto calc              = integrate_adaptive(f_kw, opt, w_mesh[0]);
    for (auto &&[n, w] : itertools::enumerate(mpi::chunk(w_mesh, comm))) { g_out[w] = calc(w); }
    return g_out;
  }

  // -------------------------------------------
  /**
   * @brief Integrate an expression over the Brillouin zone for all frequencies, combining PTR and adaptive integration.
   *
   * @details This is the main entry point of the module. It first integrates the frequencies with fixed k-grid (PTR)
   * integration of increasing grid density and then falls back to adaptive integration for the frequency points that
   * did not converge before the maximum grid size was reached. It dispatches to
   * triqs::experimental::lattice::integrate_ptr and triqs::experimental::lattice::integrate_adaptive according to the
   * given options.
   *
   * @tparam Mesh Frequency mesh type (e.g. imfreq, DLR or refreq).
   * @param f_kw CLEF expression to integrate, using the placeholders for \f$ k_x, k_y, k_z \f$ and \f$ \omega \f$ from
   * the triqs::experimental::lattice::placeholders namespace.
   * @param w_mesh Frequency mesh on which the integral is evaluated.
   * @param opt Options controlling both the PTR and the adaptive integration.
   * @param comm MPI communicator (defaults to the world communicator).
   * @return Matrix-valued Green's function on `w_mesh` holding the integrated expression.
   */
  template <typename Mesh>
  gf<Mesh, matrix_valued> integrate_bz(auto const &f_kw, Mesh const &w_mesh, bz_int_options const &opt, mpi::communicator comm = {}) {

    namespace ph = placeholders;

    // set up kgrid
    auto k_grid          = opt.k_grid;
    auto kgrid_above_max = [&](auto &k_grid) {
      for (auto ik : {0, 1, 2})
        if (k_grid[ik] >= opt.k_grid_max[ik]) { return true; }
      return false;
    };

    // REFACTOR this seems like it should go into a constructor for a bz_int_opt object
    // note that the PTR will always run once as long as run_ptr = true
    if (opt.tolerance <= 0) { throw std::runtime_error("Must provide a positive tolerance."); }
    if (!opt.run_ptr and !opt.run_adaptive) {
      throw std::runtime_error("Must choose at least one of run_ptr or run_adaptive for BZ integration to work.");
    }
    if (opt.run_ptr) {                                                                // check PTR options are sound
      if (!std::all_of(k_grid.begin(), k_grid.end(), [&](int k) { return k > 0; })) { // check if values are positive
        throw std::runtime_error("Cannot run integraton with k_grid <= 0. ");
      }
      // check that kgrid increment is meaningful
      if (!std::all_of(opt.delta_k_grid.begin(), opt.delta_k_grid.end(), [&](int k) { return k >= 0; })) {
        throw std::runtime_error("delta_k_grid cannot be negative.");
      }
      // delta_k_grid = 0 is reasonable only if k_grid >= k_grid_max, otherwise this will run doing nothing
      if (std::all_of(opt.delta_k_grid.begin(), opt.delta_k_grid.end(), [&](int k) { return k == 0; }) and !kgrid_above_max(k_grid)) {
        throw std::runtime_error("delta_k_grid can only be zero if k_grid >= k_grid_max.");
      }
    }

    int dim    = detail::deduce_dim_from_expression(f_kw, w_mesh[0]);
    auto g_out = gf{w_mesh, {dim, dim}};

    // set up containers to check if ptr has converged for different points
    std::vector<bool> ptr_converged(w_mesh.size(), false);
    auto all_converged = [&]() { return std::ranges::all_of(ptr_converged, std::identity()); };

    // set up initialize set of omega values to be run (all of them for first loop)
    std::vector<typename Mesh::mesh_point_t> mesh_points(w_mesh.begin(), w_mesh.end());

    // ------ Do the PTR -------
    if (opt.run_ptr) {
      do { // NOLINT (run loop at least once if PTR is chosen )

        if (opt.verbose) {
          int remaining_ptr = static_cast<int>(ptr_converged.size()) - std::reduce(ptr_converged.begin(), ptr_converged.end());
          std::cout << "Points remaining unconverged: " << remaining_ptr << ", now running with k-grid " << k_grid[0] << " " << k_grid[1] << " "
                    << k_grid[2] << std::endl;
        }

        // update the list of omega values we need to cover
        auto ptr_result = integrate_ptr(f_kw, mesh_points, k_grid, comm);

        // check which ones are converged after this run
        for (auto &&[n, w] : itertools::enumerate(mesh_points)) {
          ptr_converged[w.data_index()] = (max_element(abs(ptr_result(n, nda::range::all, nda::range::all) - g_out[w])) < opt.tolerance);
          g_out[w]                      = ptr_result(n, nda::range::all, nda::range::all);
        }
        // update list of unconverged frequencies to work on
        // REFACTOR it's much nicer to use the below line if we later can
        // mesh_points = mesh_points | std::views::filter([&](auto om) { return !ptr_converged[om.data_index()]; }) | std::ranges::to<std::vector>();
        std::vector<typename Mesh::mesh_point_t> unconv_mesh_points;
        for (auto w : mesh_points) {
          if (!ptr_converged[w.data_index()]) unconv_mesh_points.emplace_back(w);
        }
        mesh_points = unconv_mesh_points;

        // increment the grid for the next iteration
        for (auto ik : {0, 1, 2}) k_grid[ik] += opt.delta_k_grid[ik];

      } while (!all_converged() and !kgrid_above_max(k_grid));
    }

    // ------- Execute adaptive algo for the frequencies PTR could not do ----------
    if (opt.run_adaptive) {
      adaptive_options adaptive_opt = {.tolerance = opt.tolerance};
      if (opt.verbose) std::cout << "Running adaptive integration on remaining points." << std::endl;
      auto calc = integrate_adaptive(f_kw, adaptive_opt, w_mesh[0]);
      // adaptive evaluation at each frequency is MPI parallel;
      // possibly could be done better but this is ok for now
      for (auto &&[n, w] : itertools::enumerate(mpi::chunk(g_out.mesh(), comm))) {
        if (not ptr_converged[n]) g_out[w] = calc(w);
      }
    }
    return g_out;
  }

  /** @} */

} // namespace triqs::experimental::lattice
