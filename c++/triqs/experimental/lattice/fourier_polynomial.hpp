#pragma once

#include "../../utility/exceptions.hpp"

#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include <mpi/mpi.hpp>
#include <nda/h5.hpp>
#include <nda/mpi.hpp>
#include <nda/nda.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <iterator>
#include <ranges>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace triqs::experimental::lattice {

  using nda::dcomplex;

  /**
   * @addtogroup triqs-experimental-lattice
   * @{
   */

  template <int coeff_dim, int kdim> class fourier_polynomial;

  // ---------------------------------------------------------------------------
  // Free Fourier-evaluation kernels.
  //
  // These take the packed R-matrix of lattice vectors (shape [nR, kdim]) and a
  // packed coefficient array (shape [nR, coeff_shape...]) and evaluate
  //
  //     f(k) = sum_R coeff(R) * exp(2 pi i k . R)
  //
  // at a single k-point, a matrix of k-points, or any contiguous range of
  // k-like objects (anything with operator[](int)). Both fourier_polynomial's
  // member operator() and triqs::experimental::gfs::fourier_eval /
  // make_gf_from_fourier call them.
  // ---------------------------------------------------------------------------

  /**
   * @brief Build the packed matrix of lattice vectors from a sequence of R-vectors.
   *
   * @tparam kdim Dimension of the lattice vectors.
   * @param r_list Sequence of R-vectors, each supporting `operator[](int)`.
   * @return Matrix of shape `[nR, kdim]` whose row \f$ i \f$ holds the components of the \f$ i \f$-th R-vector as
   * doubles.
   */
  template <int kdim> nda::matrix<double> make_R_mat(auto const &r_list) {
    nda::matrix<double> rm(r_list.size(), kdim);
    for (long i = 0; i < static_cast<long>(r_list.size()); ++i)
      for (int d = 0; d < kdim; ++d) rm(i, d) = static_cast<double>(r_list[i][d]);
    return rm;
  }

  /**
   * @brief Evaluate a Fourier series at a single k-point.
   *
   * @details This kernel computes \f$ f(\mathbf{k}) = \sum_\mathbf{R} f(\mathbf{R}) \, e^{2 \pi i \, \mathbf{k} \cdot
   * \mathbf{R}} \f$ directly by summing the exponentials.
   *
   * @tparam coeff_dim Rank of the Fourier coefficient at each R-vector.
   * @tparam kdim Dimension of the k-vector.
   * @tparam CoeffArr Type of the packed coefficient array.
   * @param R_mat Packed matrix of lattice vectors of shape `[nR, kdim]`.
   * @param coeff_arr Packed coefficient array of shape `[nR, coeff_shape...]`.
   * @param k The k-point at which to evaluate.
   * @return Value of the Fourier series at `k`, a `dcomplex` for scalar targets or an nda array/matrix otherwise.
   */
  template <int coeff_dim, int kdim, typename CoeffArr>
  auto fourier_eval(nda::matrix_const_view<double> R_mat, CoeffArr const &coeff_arr, std::array<double, kdim> const &k) {
    auto c_shape = coeff_arr.shape();

    // coeff_dim == 0 (scalar target): accumulate into a plain dcomplex since
    // nda::array<T, 0> is not a valid container type. For matrix-valued
    // targets we preserve the nda::matrix type so matmul/linalg work directly.
    auto res = [&]() {
      if constexpr (coeff_dim == 0)
        return dcomplex{0};
      else if constexpr (coeff_dim == 2)
        return nda::matrix<dcomplex>::zeros(nda::stdutil::front_pop(c_shape));
      else
        return nda::array<dcomplex, coeff_dim>::zeros(nda::stdutil::front_pop(c_shape));
    }();

    long nR = R_mat.shape(0);
    for (long idx = 0; idx < nR; ++idx) {
      double kR = 0;
      for (int d = 0; d < kdim; ++d) kR += k[d] * R_mat(idx, d);
      res += std::exp(2i * M_PI * kR) * coeff_arr(idx, nda::ellipsis{});
    }
    return res;
  }

  /**
   * @brief Evaluate a Fourier series at a batch of k-points given as a matrix.
   *
   * @details This kernel evaluates the Fourier series at all k-points at once using BLAS matrix multiplications for the
   * phases and the coefficient contraction.
   *
   * @tparam coeff_dim Rank of the Fourier coefficient at each R-vector.
   * @tparam CoeffArr Type of the packed coefficient array.
   * @param R_mat Packed matrix of lattice vectors of shape `[nR, kdim]`.
   * @param coeff_arr Packed coefficient array of shape `[nR, coeff_shape...]`.
   * @param k_list Matrix of k-points of shape `[nk, kdim]`.
   * @return Array of shape `[nk, coeff_shape...]` holding the value of the Fourier series at each k-point.
   */
  template <int coeff_dim, typename CoeffArr>
  nda::array<dcomplex, coeff_dim + 1> fourier_eval(nda::matrix_const_view<double> R_mat, CoeffArr const &coeff_arr,
                                                   nda::array_const_view<double, 2> k_list) {
    auto ks = nda::matrix_const_view<double>(k_list);
    long nk = ks.shape(0);

    // set up the phases
    nda::array<double, 2> kdotR(nk, R_mat.shape(0));
    nda::blas::gemm(1., ks, transpose(R_mat), 0, kdotR);
    nda::matrix<dcomplex> phases = nda::exp(2i * M_PI * kdotR);

    auto c_shape = coeff_arr.shape();

    // flat product of the trailing target dims (1 if the target is scalar)
    long trailing = 1;
    for (int i = 1; i < coeff_dim + 1; ++i) trailing *= c_shape[i];

    auto coeff_arr_view = nda::reshape(coeff_arr, std::array{c_shape[0], trailing});

    auto result_shape    = c_shape;
    result_shape[0]      = nk;
    auto result          = nda::array<dcomplex, coeff_dim + 1>(result_shape);
    auto result_mat_view = nda::reshape(result, std::array{nk, trailing});

    nda::blas::gemm(1, phases, coeff_arr_view, 0, result_mat_view);
    return result;
  }

  /**
   * @brief Evaluate a Fourier series at a contiguous range of k-points.
   *
   * @details This kernel packs the k-points of the range into a matrix and forwards to the batched matrix overload.
   *
   * @tparam coeff_dim Rank of the Fourier coefficient at each R-vector.
   * @tparam kdim Dimension of the k-vectors.
   * @tparam CoeffArr Type of the packed coefficient array.
   * @tparam V Contiguous range type whose elements support `operator[](int)`.
   * @param R_mat Packed matrix of lattice vectors of shape `[nR, kdim]`.
   * @param coeff_arr Packed coefficient array of shape `[nR, coeff_shape...]`.
   * @param k_iterator Contiguous range of k-points.
   * @return Array of shape `[nk, coeff_shape...]` holding the value of the Fourier series at each k-point.
   */
  template <int coeff_dim, int kdim, typename CoeffArr, typename V>
    requires(std::ranges::contiguous_range<V>)
  nda::array<dcomplex, coeff_dim + 1> fourier_eval(nda::matrix_const_view<double> R_mat, CoeffArr const &coeff_arr, V const &k_iterator) {
    auto kvecs = nda::matrix<double>(k_iterator.size(), kdim);
    for (auto [ik, k] : itertools::enumerate(k_iterator)) {
      for (auto idim : nda::range(kdim)) { kvecs(ik, idim) = k[idim]; }
    }
    return fourier_eval<coeff_dim>(R_mat, coeff_arr, nda::array_const_view<double, 2>(kvecs));
  }

  // ---------------------------------------------------------------------------
  // fourier_polynomial: owning container for R-vectors + Fourier coefficients,
  // plus a placeholder-aware operator() for clef lazy evaluation.
  // ---------------------------------------------------------------------------

  /**
   * @brief Owning container for a Fourier-series representation of a lattice function.
   *
   * @details A fourier_polynomial stores a list of real-space lattice vectors \f$ \mathbf{R} \f$ together with their
   * Fourier coefficients and represents the function
   * \f[
   *   f(\mathbf{k}) = \sum_\mathbf{R} f(\mathbf{R}) \, e^{2 \pi i \, \mathbf{k} \cdot \mathbf{R}} \; .
   * \f]
   * It provides a placeholder-aware call operator for CLEF lazy evaluation as well as batched BLAS evaluation at a list
   * of k-points, HDF5 serialization and MPI broadcast.
   *
   * @tparam coeff_dim Rank of the Fourier coefficient at each \f$ \mathbf{R} \f$ (e.g. 0 for scalar-, 2 for
   * matrix-valued targets).
   * @tparam kdim Dimension of the k-vectors (1, 2 or 3).
   */
  template <int coeff_dim, int kdim> class fourier_polynomial {

    protected:
    std::vector<std::array<long, kdim>> R_list;    ///< Lattice vectors \f$ \mathbf{R} \f$ as integer arrays.
    nda::matrix<double> R_mat;                     ///< Lattice vectors as doubles, of shape `[nR, kdim]`, for BLAS.
    nda::array<dcomplex, coeff_dim + 1> coeff_arr; ///< Packed Fourier coefficients, of shape `[nR, coeff_shape...]`.

    /// HDF5 write helper for derived classes: writes R_list and coeff_arr under the given format tag.
    void h5_write_impl(h5::group g, std::string const &name, char const *format) const {
      auto gr = g.create_group(name);
      h5::write_hdf5_format_as_string(gr, format); // NOLINT
      h5::write(gr, "R_list", R_list);
      h5::write(gr, "coeff_arr", coeff_arr);
    }

    /// HDF5 read helper for derived classes: reads R_list and coeff_arr in place; rebuilds R_mat.
    void h5_read_impl(h5::group g, std::string const &name, char const *exp_format) {
      auto gr = g.open_group(name);
      h5::assert_hdf5_format_as_string(gr, exp_format, true); // NOLINT
      h5::read(gr, "R_list", R_list);
      h5::read(gr, "coeff_arr", coeff_arr);
      TRIQS_ASSERT(R_list.size() == coeff_arr.shape(0));
      R_mat = make_R_mat<kdim>(R_list);
    }

    public:
    /// Default constructor: construct an empty Fourier polynomial.
    fourier_polynomial() = default;
    /// Copy constructor.
    fourier_polynomial(fourier_polynomial const &) = default;
    /// Move constructor.
    fourier_polynomial(fourier_polynomial &&) noexcept = default;
    /// Copy assignment operator.
    fourier_polynomial &operator=(fourier_polynomial const &) = default;
    /// Move assignment operator.
    fourier_polynomial &operator=(fourier_polynomial &&) noexcept = default;

    /**
     * @brief Construct from a list of R-vectors and a matching list of coefficient arrays.
     *
     * @param R_list_ List of lattice vectors \f$ \mathbf{R} \f$.
     * @param coeff_list_ List of Fourier coefficients, one per R-vector and all of the same shape.
     */
    fourier_polynomial(std::vector<std::array<long, kdim>> R_list_, std::vector<nda::array<dcomplex, coeff_dim>> const &coeff_list_)
       : R_list(std::move(R_list_)), R_mat(make_R_mat<kdim>(R_list)) {
      TRIQS_ASSERT(!coeff_list_.empty());
      TRIQS_ASSERT(R_list.size() == coeff_list_.size());
      coeff_arr.resize(nda::stdutil::front_append(coeff_list_[0].shape(), coeff_list_.size()));
      for (long i = 0; i < static_cast<long>(coeff_list_.size()); ++i) coeff_arr(i, nda::ellipsis{}) = coeff_list_[i];
    }

    /**
     * @brief Construct from a list of R-vectors and a packed coefficient array.
     *
     * @param R_list_ List of lattice vectors \f$ \mathbf{R} \f$.
     * @param coeff_arr_ Packed coefficient array of shape `[nR, coeff_shape...]`.
     */
    fourier_polynomial(std::vector<std::array<long, kdim>> R_list_, nda::array<dcomplex, coeff_dim + 1> coeff_arr_)
       : R_list(std::move(R_list_)), R_mat(make_R_mat<kdim>(R_list)), coeff_arr(std::move(coeff_arr_)) {
      TRIQS_ASSERT(R_list.size() == coeff_arr.shape(0));
    }

    /**
     * @brief Broadcast a Fourier polynomial over an MPI communicator.
     *
     * @details The list of R-vectors and the coefficient array are broadcast, and the packed R-matrix is recomputed
     * locally on each rank.
     *
     * @param x Fourier polynomial to broadcast (overwritten on non-root ranks).
     * @param c MPI communicator.
     * @param root Rank of the broadcasting process.
     */
    friend void mpi_broadcast(fourier_polynomial &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(x.R_list, c, root);
      mpi::broadcast(x.coeff_arr, c, root);
      x.R_mat = make_R_mat<kdim>(x.R_list);
    }

    /**
     * @brief Get the HDF5 format tag.
     *
     * @return HDF5 format tag of a Fourier polynomial.
     */
    [[nodiscard]] static std::string hdf5_format() { return "fourier_polynomial"; }

    /**
     * @brief Write a Fourier polynomial to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param x Fourier polynomial to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, fourier_polynomial const &x) { x.h5_write_impl(g, name, "fourier_polynomial"); }

    /**
     * @brief Read a Fourier polynomial from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param x Fourier polynomial to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, fourier_polynomial &x) { x.h5_read_impl(g, name, "fourier_polynomial"); }

    /**
     * @brief Get the list of real-space lattice vectors.
     *
     * @return Const reference to the list of lattice vectors \f$ \mathbf{R} \f$.
     */
    [[nodiscard]] C2PY_IGNORE auto const &get_R_list() const { return R_list; }

    /**
     * @brief Get the packed coefficient array.
     *
     * @return View of the packed coefficient array of shape `[nR, coeff_shape...]`.
     */
    [[nodiscard]] C2PY_IGNORE auto get_coeff_arr() { return coeff_arr(); }

    /**
     * @brief Get the packed coefficient array (const overload).
     *
     * @return Const view of the packed coefficient array of shape `[nR, coeff_shape...]`.
     */
    [[nodiscard]] C2PY_IGNORE auto get_coeff_arr() const { return coeff_arr(); }

    /**
     * @brief Get the packed matrix of lattice vectors.
     *
     * @return Const reference to the matrix of lattice vectors of shape `[nR, kdim]` as doubles.
     */
    [[nodiscard]] C2PY_IGNORE auto const &get_R_mat() const { return R_mat; }

    /**
     * @brief Get the number of R-vectors.
     *
     * @return Number of lattice vectors stored in the Fourier polynomial.
     */
    [[nodiscard]] long n_R() const { return static_cast<long>(R_list.size()); }

    /**
     * @brief Access the coefficient associated with a given R-vector.
     *
     * @param R Lattice vector \f$ \mathbf{R} \f$.
     * @return View of the coefficient associated with `R`.
     */
    C2PY_IGNORE auto operator[](std::array<long, kdim> R) { return coeff_arr(get_R_idx(R), nda::ellipsis{}); }

    /**
     * @brief Access the coefficient associated with a given R-vector (const overload).
     *
     * @param R Lattice vector \f$ \mathbf{R} \f$.
     * @return Const view of the coefficient associated with `R`.
     */
    C2PY_IGNORE auto operator[](std::array<long, kdim> R) const { return coeff_arr(get_R_idx(R), nda::ellipsis{}); }

    /**
     * @brief Access the coefficient at a given storage index.
     *
     * @param i Storage index of the R-vector.
     * @return View of the coefficient at index `i`.
     */
    C2PY_IGNORE auto operator[](long i) { return coeff_arr(i, nda::ellipsis{}); }

    /**
     * @brief Access the coefficient at a given storage index (const overload).
     *
     * @param i Storage index of the R-vector.
     * @return Const view of the coefficient at index `i`.
     */
    C2PY_IGNORE auto operator[](long i) const { return coeff_arr(i, nda::ellipsis{}); }

    /**
     * @brief Get the storage index of a given R-vector.
     *
     * @details It throws an exception if the R-vector is not present.
     *
     * @param R Lattice vector \f$ \mathbf{R} \f$.
     * @return Storage index of `R` in the list of R-vectors.
     */
    long get_R_idx(std::array<long, kdim> R) const {
      auto it = std::ranges::find(R_list, R);
      if (it == R_list.end()) { TRIQS_RUNTIME_ERROR << "Could not locate R in the Wannier Hamiltonian.\n"; }
      return std::distance(R_list.begin(), it);
    }

    /**
     * @brief Evaluate the Fourier polynomial, optionally as a lazy CLEF expression.
     *
     * @details The behavior depends on how many of the arguments are CLEF placeholders (or lazy expressions):
     * - With no placeholder, the function is evaluated directly by computing the exponentials.
     * - With exactly one placeholder and `kdim > 1`, a partially evaluated Fourier polynomial of dimension 1 is built
     *   and returned as a lazy CLEF call expression.
     * - In all other cases (more than one placeholder, or a single placeholder with `kdim == 1`), a lazy CLEF call
     *   expression is returned.
     *
     * @tparam T Argument types, each either `double` or a CLEF placeholder / lazy expression.
     * @param ks The `kdim` arguments at which to evaluate, mixing doubles and placeholders.
     * @return The evaluated value if no placeholder is present, otherwise a lazy CLEF call expression.
     */
    template <typename... T>
      requires((std::is_same_v<T, double> or nda::clef::is_lazy<T>) and ...) // double OR placeholder
    auto operator()(T... ks) const {

      static_assert(sizeof...(T) == kdim, "Incorrect number of arguments"); // # arguments is same as dimension
      constexpr auto ph_number = ((nda::clef::is_lazy<T> + ...));           // How many placeholders in ks

      // Called with 0 placeholder
      if constexpr (ph_number == 0) return operator()(std::array{ks...});
      // Called with 1 placeholder, not valid for kdim = 1
      else if constexpr ((ph_number == 1) and (kdim != 1)) {
        // find the position of the placeholder
        constexpr auto is_ph      = std::array{(nda::clef::is_lazy<T> ? 1 : 0)...}; // 0 or 1 if it is a placeholder
        constexpr int ph_position = [](auto &&v) {
          for (int i = 0; i < v.size(); ++i)
            if (v[i] == 1) return i;
          return -1; // to silence compiler warning
        }(is_ph); // immediately calling the lambda. executed at compile time

        return nda::clef::make_expr_call(partial_eval<ph_position>(ks...), // auto{k...[ph_position]}); // C++26
                                         auto{std::get<ph_position>(std::tie(ks...))});
      }
      // Called with > 1 placeholders in kdim > 1 or 1 placeholder in kdim == 1
      else
        return nda::clef::make_expr_call(fourier_polynomial{*this}, auto{ks}...);
    }

    private:
    /**
     * @brief Partial evaluation into a fourier_polynomial<coeff_dim, 1> for all arguments BUT pos
     */
    template <int placeholder_pos>
    fourier_polynomial<coeff_dim, 1> partial_eval(auto const &...ks) const
      requires((kdim == 3) or (kdim == 2))
    {

      auto [it1, it2] = std::ranges::minmax_element(R_list, {}, [](auto const &r) { return r[placeholder_pos]; });

      auto Rz_min = (*it1)[placeholder_pos];
      auto Rz_max = (*it2)[placeholder_pos];

      long n_new         = Rz_max - Rz_min + 1;
      auto new_Rs        = std::vector<std::array<long, 1>>(n_new);
      auto coeff_shape   = nda::stdutil::front_pop(coeff_arr.shape());
      auto new_coeff_arr = nda::array<dcomplex, coeff_dim + 1>::zeros(nda::stdutil::front_append(coeff_shape, n_new));

      constexpr int p1 = (placeholder_pos + 1) % kdim;
      constexpr int p2 = (placeholder_pos + 2) % kdim;
      auto kst         = std::tie(ks...);

      for (long idx = 0; idx < n_R(); ++idx) {
        auto Rz         = R_list[idx][placeholder_pos];
        long new_idx    = Rz - Rz_min;
        new_Rs[new_idx] = {Rz};

        double kR = std::get<p1>(kst) * R_list[idx][p1];
        if constexpr (kdim == 3) kR += std::get<p2>(kst) * R_list[idx][p2];
        new_coeff_arr(new_idx, nda::ellipsis{}) += coeff_arr(idx, nda::ellipsis{}) * std::exp(2i * M_PI * kR);
      }

      return {std::move(new_Rs), std::move(new_coeff_arr)};
    }

    // -----------------------------

    public:
    /**
     * @brief Evaluate the Fourier polynomial at a batch of k-points given as a matrix.
     *
     * @param k_list Matrix of k-points of shape `[nk, kdim]`.
     * @return Array of shape `[nk, coeff_shape...]` holding the value at each k-point.
     */
    nda::array<dcomplex, coeff_dim + 1> operator()(nda::array_const_view<double, 2> k_list) const {
      return fourier_eval<coeff_dim>(R_mat, coeff_arr, k_list);
    }

    /**
     * @brief Evaluate the Fourier polynomial at a contiguous range of k-points.
     *
     * @param k_iterator Contiguous range of k-points, each supporting `operator[](int)`.
     * @return Array of shape `[nk, coeff_shape...]` holding the value at each k-point.
     */
    nda::array<dcomplex, coeff_dim + 1> operator()(std::ranges::contiguous_range auto const &k_iterator) const {
      return fourier_eval<coeff_dim, kdim>(R_mat, coeff_arr, k_iterator);
    }

    /**
     * @brief Evaluate the Fourier polynomial at a single k-point.
     *
     * @param ks The k-point at which to evaluate.
     * @return Value of the Fourier polynomial at `ks`.
     */
    auto operator()(std::array<double, kdim> ks) const { return fourier_eval<coeff_dim, kdim>(R_mat, coeff_arr, ks); }
  };

  /** @} */

} // namespace triqs::experimental::lattice

/// @cond
// this allows the deep partial evaluation mechanism to speed up calculations
template <int coeff_dim, int kdim>
inline constexpr bool nda::clef::supports_partial_eval_of_calls<triqs::experimental::lattice::fourier_polynomial<coeff_dim, kdim>> = true;
/// @endcond
