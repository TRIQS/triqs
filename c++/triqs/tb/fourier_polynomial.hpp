#pragma once
#include <algorithm>
#include <h5/h5.hpp>
#include <mpi/mpi.hpp>
#include <nda/h5.hpp>
#include <nda/mpi.hpp>
#include <nda/nda.hpp>
#include <type_traits>
#include <utility>
#include "../utility/exceptions.hpp"

namespace triqs::tb {

  using nda::dcomplex;

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
  // k-like objects (anything with operator[](int)). They are the single source
  // of truth for the non-lazy Fourier evaluation -- both fourier_polynomial's
  // member operator() and gf_evaluator<mesh::fourier_poly> call them.
  // ---------------------------------------------------------------------------

  /// Helper: build R_mat of shape [nR, kdim] from a sequence of R-vectors.
  template <int kdim> nda::matrix<double> make_R_mat(auto const &r_list) {
    nda::matrix<double> rm(r_list.size(), kdim);
    for (long i = 0; i < static_cast<long>(r_list.size()); ++i)
      for (int d = 0; d < kdim; ++d) rm(i, d) = static_cast<double>(r_list[i][d]);
    return rm;
  }

  /// Evaluate at a single k-point.
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

  /// Evaluate at a batch of k-points passed as a matrix of shape [nk, kdim].
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

  /// Evaluate at a contiguous range of k-points (each element must support operator[](int)).
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

  template <int coeff_dim, int kdim> class fourier_polynomial {

    protected:
    std::vector<std::array<long, kdim>> R_list;    // R-vectors as long arrays
    nda::matrix<double> R_mat;                     // R-vectors as doubles [nR, kdim] for BLAS
    nda::array<dcomplex, coeff_dim + 1> coeff_arr; // Fourier coefficients [nR, coeff_shape...]

    /// HDF5 write helper for derived classes: writes R_list and coeff_arr under the given format tag.
    void h5_write_impl(h5::group g, std::string const &name, char const *format) const {
      auto gr = g.create_group(name);
      h5::write_hdf5_format_as_string(gr, format);
      h5::write(gr, "R_list", R_list);
      h5::write(gr, "coeff_arr", coeff_arr);
    }

    /// HDF5 read helper for derived classes: reads R_list and coeff_arr in place; rebuilds R_mat.
    void h5_read_impl(h5::group g, std::string const &name, char const *exp_format) {
      auto gr = g.open_group(name);
      h5::assert_hdf5_format_as_string(gr, exp_format, true);
      h5::read(gr, "R_list", R_list);
      h5::read(gr, "coeff_arr", coeff_arr);
      TRIQS_ASSERT(R_list.size() == coeff_arr.shape(0));
      R_mat = make_R_mat<kdim>(R_list);
    }

    public:
    fourier_polynomial(fourier_polynomial const &)                = default;
    fourier_polynomial(fourier_polynomial &&) noexcept            = default;
    fourier_polynomial &operator=(fourier_polynomial const &)     = default;
    fourier_polynomial &operator=(fourier_polynomial &&) noexcept = default;

    /// Construct from vectors of R-vectors and coefficient arrays.
    fourier_polynomial(std::vector<std::array<long, kdim>> R_list_, std::vector<nda::array<dcomplex, coeff_dim>> const &coeff_list_)
       : R_list(std::move(R_list_)), R_mat(make_R_mat<kdim>(R_list)) {
      TRIQS_ASSERT(!coeff_list_.empty());
      TRIQS_ASSERT(R_list.size() == coeff_list_.size());
      coeff_arr.resize(nda::stdutil::front_append(coeff_list_[0].shape(), coeff_list_.size()));
      for (long i = 0; i < static_cast<long>(coeff_list_.size()); ++i) coeff_arr(i, nda::ellipsis{}) = coeff_list_[i];
    }

    /// Construct from R-vectors and a packed coefficient array directly.
    fourier_polynomial(std::vector<std::array<long, kdim>> R_list_, nda::array<dcomplex, coeff_dim + 1> coeff_arr_)
       : R_list(std::move(R_list_)), R_mat(make_R_mat<kdim>(R_list)), coeff_arr(std::move(coeff_arr_)) {
      TRIQS_ASSERT(R_list.size() == coeff_arr.shape(0));
    }

    /// MPI broadcast: send R_list and coeff_arr; recompute R_mat locally.
    friend void mpi_broadcast(fourier_polynomial &x, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(x.R_list, c, root);
      mpi::broadcast(x.coeff_arr, c, root);
      x.R_mat = make_R_mat<kdim>(x.R_list);
    }

    /// HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "fourier_polynomial"; }

    /// HDF5 write: standalone serialization of a fourier_polynomial<C, K>.
    friend void h5_write(h5::group g, std::string const &name, fourier_polynomial const &x) {
      x.h5_write_impl(g, name, "fourier_polynomial");
    }

    /// HDF5 read.
    friend void h5_read(h5::group g, std::string const &name, fourier_polynomial &x) { x.h5_read_impl(g, name, "fourier_polynomial"); }

    /** Access real space lattice points */
    [[nodiscard]] C2PY_IGNORE auto const &get_R_list() const { return R_list; }

    /** Access the packed coefficient array [nR, coeff_shape...] */
    [[nodiscard]] C2PY_IGNORE auto get_coeff_arr() { return coeff_arr(); }
    [[nodiscard]] C2PY_IGNORE auto get_coeff_arr() const { return coeff_arr(); }

    /** Access the R matrix [nR, kdim] as doubles */
    [[nodiscard]] C2PY_IGNORE auto const &get_R_mat() const { return R_mat; }

    /** Number of R-vectors */
    [[nodiscard]] long n_R() const { return static_cast<long>(R_list.size()); }

    C2PY_IGNORE auto operator[](std::array<long, kdim> R) { return coeff_arr(get_R_idx(R), nda::ellipsis{}); }
    C2PY_IGNORE auto operator[](std::array<long, kdim> R) const { return coeff_arr(get_R_idx(R), nda::ellipsis{}); }
    C2PY_IGNORE auto operator[](long i) { return coeff_arr(i, nda::ellipsis{}); }
    C2PY_IGNORE auto operator[](long i) const { return coeff_arr(i, nda::ellipsis{}); }

    long get_R_idx(std::array<long, kdim> R) const {
      auto it = std::ranges::find(R_list, R);
      if (it == R_list.end()) { TRIQS_RUNTIME_ERROR << "Could not locate R in the Wannier Hamiltonian.\n"; }
      return std::distance(R_list.begin(), it);
    }

    /**
    * @brief Call operator
    * @param ks: any combination of double and placeholder (or lazy expressions)
    * @return If ks contains
    *   * 0 placeholder: evaluation by computing the exponentials
    *   * > 1 placeholder in dim > 1 or 1 placeholder in dim ==1: a lazy call expression
    *   * exactly ONE placeholder in dim > 1: a lazy call expression of the partially evaluated tb_hopping<1>
    *
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
    /// Batch BLAS evaluation at a list of k-points of shape [nk, kdim]
    nda::array<dcomplex, coeff_dim + 1> operator()(nda::array_const_view<double, 2> k_list) const {
      return fourier_eval<coeff_dim>(R_mat, coeff_arr, k_list);
    }

    /// Evaluation at a contiguous range of k-points
    nda::array<dcomplex, coeff_dim + 1> operator()(std::ranges::contiguous_range auto const &k_iterator) const {
      return fourier_eval<coeff_dim, kdim>(R_mat, coeff_arr, k_iterator);
    }

    /// Evaluation at a single k-point
    auto operator()(std::array<double, kdim> ks) const { return fourier_eval<coeff_dim, kdim>(R_mat, coeff_arr, ks); }
  };
} // namespace triqs::tb

// this allows the deep partial evaluation mechanism to speed up calculations
template <int coeff_dim, int kdim>
inline constexpr bool nda::clef::supports_partial_eval_of_calls<triqs::tb::fourier_polynomial<coeff_dim, kdim>> = true;
