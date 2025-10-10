#pragma once
#include "triqs/utility/integration/adaptive.hpp"
#include "triqs/utility/integration/integrator.hpp"
#include <stdexcept>
#include <triqs/gfs.hpp>

// TODO need to upgrade this for integration of the IBZ

// omp reduction operation for nda array
#pragma omp declare reduction(array_add_c_3 : nda::array<dcomplex, 3> : omp_out += omp_in)                                                           \
   initializer(omp_priv = nda::array<dcomplex, 3>(omp_orig.shape()))

namespace triqs::lattice {

  using namespace triqs::gfs;

  // The placeholders authorized in the expressions integrated by bz_integrator
  namespace placeholders {

    constexpr nda::clef::placeholder<0> kx; // global var, clang requests const(expr)
    constexpr nda::clef::placeholder<1> ky;
    constexpr nda::clef::placeholder<2> kz;
    constexpr nda::clef::placeholder<3> w;

  } // namespace placeholders

  // Options for adaptive integration
  struct adaptive_options {
    double tolerance = 1.e-3; // target error
  };

  /**
  * @brief Options for the case of integrate_bz function, with both adaptive + ptr integration
  *
  * @details The integration function we are running for Gloc currently makes an attempt to converge the integration at
  * each frequency using fixed k-grid integration with increasing grid density, starting from `k_grid`
  * and increasing in increments of `delta_k_grid` until either a given point is converged with PTR or we hit `k_grid_max`.
  * After that, the remaining unconverged frequency points are run with adaptive
  * integration until they reach a certain absolute tolerance.
  *
  */
  struct bz_int_options {
    double tolerance                 = 1.e-3;        /// absolute tolerance of the integrated quantity
    std::array<long, 3> k_grid       = {10, 10, 10}; /// default PTR number of points
    std::array<long, 3> delta_k_grid = {2, 2, 2};    /// Increase step of k in the grid refinement
    std::array<long, 3> k_grid_max   = {20, 20, 20}; /// Max of kx, ky, kz
    bool run_adaptive                = true;         /// if false, does not run adaptive integration at the end
    bool run_ptr                     = true;         /// if false, does not run PTR integration, goes directly to adaptive
    bool verbose                     = false;        /// if logging should be printed
  };

  // helper to determine the return container dimension
  int deduce_dim_from_expression(auto const &f_kw) {
    namespace ph  = triqs::lattice::placeholders;
    auto f_w_temp = eval(f_kw, ph::kx = 0., ph::ky = 0., ph::kz = 0., ph::w = 0);
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

  //---------------------------------------------------------

  /**
    * @brief Compute the integral of f_kw on k using PTR on the domain from 0,1, utilizing both MPI and OMP parallelism
    *
    * @tparam T
    * @param f_kw expression representing the function to integrate, with placeholder for kx, ky, kz and omega
    * @param omega_values list of frequency values as complex double, double, or mesh point type
    * @param k_grid the grid on which to evaluate the expression using PTR
    * @param comm MPI communicator
    * @return The value of the integral expression, fully evaluated on kx, ky, kz and omega
    */
  template <typename T>
    requires(std::convertible_to<T, dcomplex> or std::convertible_to<T, double>) // allow for real or imaginary mesh points or numbers
  nda::array<dcomplex, 3> integrate_ptr(auto const &f_kw, std::vector<T> const &omega_values, std::array<long, 3> const &k_grid,
                                        mpi::communicator comm) {

    for (auto i : {0, 1, 2}) {
      if (k_grid[i] <= 0) std::runtime_error{"Cannot use PTR integration with kgrid dim <= 0."};
    }

    namespace ph  = triqs::lattice::placeholders;
    int block_dim = deduce_dim_from_expression(f_kw);
    auto result   = nda::zeros<dcomplex>(omega_values.size(), block_dim, block_dim); // container to return

    // Determine the longest direction and apply MPI chunk to this dimension, otherwise provide a simple iterator
    auto mpi_chunk_max = [&k_grid, comm](int kdim) {
      // chunk the biggest one with MPI
      if (kdim == std::distance(k_grid.begin(), std::ranges::max_element(k_grid))) { return mpi::chunk(nda::range(k_grid[kdim]), comm); }
      return itertools::slice(nda::range(k_grid[kdim]), 0,
                              k_grid[kdim]); // otherwise complains about different return types
    };

    // REFACTOR: why does performing partial eval within different loops not help?
    // perform the PTR, integrating
#pragma omp parallel for collapse(3) reduction(array_add_c_3 : result) default(none)                                                                 \
   shared(k_grid, omega_values, f_kw, ph::kx, ph::ky, ph::kz, ph::w, mpi_chunk_max)
    for (auto ikx : mpi_chunk_max(0)) {
      //auto f_wyz = eval(f_kw, ph::kx = kx);
      for (auto iky : mpi_chunk_max(1)) {
        //auto f_wz = eval(f_wyz, ph::ky = ky);
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

    // apply normalization
    result /= double(k_grid[0] * k_grid[1] * k_grid[2]);
    return result;
  }

  // -------------------------------------------

  /**
    * @brief Compute the integral of f_kw on k using PTR on the domain from 0,1, utilizing both MPI and OMP parallelism
    *
    * @tparam T
    * @param f_kw expression representing the function to integrate, with placeholder for kx, ky, kz and omega
    * @param w_mesh mesh of frequency points on which to perform the integration
    * @param k_grid the grid on which to evaluate the expression using PTR
    * @param comm MPI communicator
    * @return The value of the integral expression, fully evaluated on kx, ky, kz and omega
    */
  template <typename Mesh> auto integrate_ptr(auto const &f_kw, Mesh const &w_mesh, std::array<long, 3> const &k_grid, mpi::communicator comm = {}) {

    int dim    = deduce_dim_from_expression(f_kw);
    auto g_out = gf{w_mesh, {dim, dim}};
    std::vector<typename Mesh::mesh_point_t> mesh_points(w_mesh.begin(), w_mesh.end());
    auto ptr_result = integrate_ptr(f_kw, mesh_points, k_grid, comm);
    // fill in the GF to return
    for (auto &&[n, w] : itertools::enumerate(mpi::chunk(w_mesh, comm))) { g_out[w] = calc(w); }
    return g_out;
  }

  // -------------------------------------------
  /**
    * @brief Compute the integral of f_kw on k adaptively on a 0,1 domain, return a lambda function
    *          which can be evaluated for a given omega (to prepare for the case of adaptive omega integration)
    *          Currently this is not parallelized (as this is not easy for adaptive integration),
    *          but can be somewhat parallelized on evaluation of frequencies on the returned lambda function.
    *
    * @param f_kw Expression to integrate on k and omega
    * @param adaptive_options An options struct giving the user's selected integration options (for now, tolerance)
    * @return f_w A lambda function which provides the final function eval for a given omega point to return the integral value
    */
  auto integrate_adaptive(auto const &f_kw, adaptive_options const &opt) {

    // use the first mesh value, evaluated, to determine the return type of the data
    namespace ph      = triqs::lattice::placeholders;
    auto f_value      = nda::make_regular(eval(f_kw, ph::kx = 0., ph::ky = 0., ph::kz = 0., ph::w = 0));
    auto int_1d_adapt = utility::integrate_1d_adapt<decltype(f_value)>{opt.tolerance};

    // OP : Beware the capture ! We need to move the expression.
    return [expr_kw = std::move(f_kw), int_1d_adapt](auto om) { //NOLINT
      std::pair<double, double> k_domain = {0, 1};
      auto expr_k                        = eval(expr_kw, ph::w = om);

      return integrate(int_1d_adapt, integrate(int_1d_adapt, integrate(int_1d_adapt, expr_k, ph::kx = k_domain), ph::ky = k_domain),
                       ph::kz = k_domain);
    };
  }

  /**
    * @brief Compute the integral of f_kw on k adaptively on a 0,1 domain, return a GF which
    *         contains the evaluation of the adaptive integration.
    *
    * @param f_kw Expression to integrate on k and omega
    * @param Mesh mesh on which the returned GF is evaluated
    * @param adaptive_options An options struct giving the user's selected integration options (for now, tolerance)
    * @return gf containing the evaluated integral
    */
  template <typename Mesh> auto integrate_adaptive(auto const &f_kw, Mesh const &w_mesh, adaptive_options const &opt) {

    int dim    = deduce_dim_from_expression(f_kw);
    auto g_out = gf{w_mesh, {dim, dim}};
    // OMP/MPI parallel evaluation over frequencies
    mpi::communicator comm = {};
    auto calc              = integrate_adaptive(f_kw, opt);
    for (auto &&[n, w] : itertools::enumerate(mpi::chunk(w_mesh, comm))) { g_out[w] = calc(w); }
  }

  // -------------------------------------------
  /**
     * @brief Compute the integral of expr_kw on k for all w, s
     *
     * The idea is to compute some w with PTR, some with Adaptive
     * If PTR fail to refine, we fall back to adaptive. This calls on integrate_ptr and integrate_adaptive to operate.
     *
     * @tparam mesh A Mesh type (Imfreq, DLR, Refreq)
     * @param f_kw CLEF expression representing the function to integrate, with placeholder for kx, ky, kz and omega
     *     from the lattice::placeholders namespace
     * @param w_mesh Frequency mesh object (Imfreq, DLR, Refreq)
     * @param bz_int_options An options struct containing options for both ptr and adaptive integration.
     * @param comm An MPI communicator (optional, defaults to world communicator)
     * @return gf Green's function containing the given expression evaluated on the mesh
     */
  template <typename Mesh>
  gf<Mesh, matrix_valued> integrate_bz(auto const &f_kw, Mesh const &w_mesh, bz_int_options const &opt, mpi::communicator comm = {}) {

    namespace ph = triqs::lattice::placeholders;

    // set up kgrid
    auto k_grid          = opt.k_grid;
    auto kgrid_above_max = [&](auto &k_grid) { // REFACTOR maybe this can be done with less
      for (auto ik : {0, 1, 2})
        if (k_grid[ik] >= opt.k_grid_max[ik]) { return true; }
      return false;
    };

    // REFACTOR this feels like something that should go into a constructor for a bz_int_opt object
    // note that the PTR will always run once as long as run_ptr = true
    if (opt.tolerance <= 0) { throw std::runtime_error("Must provide a positive tolerance."); }
    if (!opt.run_ptr and !opt.run_adaptive) {
      throw std::runtime_error("Must choose at least one of run_ptr or run_adaptive for BZ integration to work.");
    }
    if (opt.run_ptr) { // check PTR options are sound
      //if (kgrid_above_max(k_grid)) { throw std::runtime_error("Cannot perform PTR integration when initial k_grid > k_grid_max."); }
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

    int dim    = deduce_dim_from_expression(f_kw);
    auto g_out = gf{w_mesh, {dim, dim}};

    // set up containers to check if ptr has converged for different points
    std::vector<bool> ptr_converged(w_mesh.size(), false);
    auto all_converged = [&]() { return std::ranges::all_of(ptr_converged, std::identity()); };

    // set up initialize set of omega values to be run (all of them for first loop)
    std::vector<typename Mesh::mesh_point_t> mesh_points; // (w_mesh.begin(), w_mesh.end()); // TODO why doesn't this single line work?
    for (auto w : w_mesh) mesh_points.emplace_back(w);

    // ------ Do the PTR -------
    if (opt.run_ptr) {
      do { // run loop at least once if PTR is chosen

        if (opt.verbose) {
          int remaining_ptr = ptr_converged.size() - std::reduce(ptr_converged.begin(), ptr_converged.end());
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
        //mesh_points = mesh_points | std::views::filter([&](auto om) { return !ptr_converged[om.data_index()]; }) | std::ranges::to<std::vector>();
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
      auto calc = integrate_adaptive(f_kw, adaptive_opt);
      // adaptive evaluation at each frequency is MPI parallel;
      // possibly could be done better but this is ok for now
      for (auto &&[n, w] : itertools::enumerate(mpi::chunk(g_out.mesh(), comm))) {
        if (not ptr_converged[n]) g_out[w] = calc(w);
      }
    }
    return g_out;
  }
} // namespace triqs::lattice
