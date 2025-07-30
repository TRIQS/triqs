#pragma once
#include <algorithm>
#include <nda/nda.hpp>
#include <cassert>
#include <string>
#include <type_traits>
#include <utility>
#include "../utility/exceptions.hpp"

// MOVE THIS
#define C2PY_IGNORE __attribute__((annotate("c2py_ignore")))

namespace triqs {

  // TODO should the R vectors be double?

  using nda::dcomplex;

  template <int coeff_dim, int kdim> class fourier_polynomial {

    protected:
    std::vector<std::array<long, kdim>> R_list; // Rs[idx_R]  Real space lattice points
    std::vector<nda::array<dcomplex, coeff_dim>> coeff_list;

    private:
    // these are for internal storage, so that we can make use of blas calls
    // when the FP is executed on a block of kpoints
    nda::matrix<double> R_mat_transpose;           // Rs[3, idx_R]  Real space lattice points transposed to use in gemm calls
    nda::array<dcomplex, coeff_dim + 1> coeff_mat; // coefficients[idx_R, a, b, ...] fourier coefficients, where the array is shape [R, coeff_dim...]

    // -------------------------
    public:
    /**
     * @brief Construct a new fourier polynomial object
     *
     * @param Rs
     * @param coefficients
     */
    fourier_polynomial(std::vector<std::array<long, kdim>> R_list, std::vector<nda::array<dcomplex, coeff_dim>> coeff_list)
       : R_list(std::move(R_list)), coeff_list(std::move(coeff_list)) {
      TRIQS_ASSERT(this->coeff_list.size() == this->R_list.size());

      // REFACTOR would be better to do this in intializer list, but this proves a bit tricky because of moves
      R_mat_transpose.resize(kdim, this->R_list.size());
      coeff_mat.resize(nda::stdutil::front_append(this->coeff_list[0].shape(), this->R_list.size()));

      // set up internally stored nda representations of coefficients and R vectors
      for (auto iR : nda::range(this->R_list.size())) {
        this->R_mat_transpose(nda::range::all, iR) = this->R_list[iR];
        this->coeff_mat(iR, nda::ellipsis{})       = this->coeff_list[iR];
      }
    }

    // put return type instead of auto TODO
    /** Access real space lattice points on which the Fourier coefficients are defined */
    [[nodiscard]] C2PY_IGNORE auto const &get_R_list() const { return this->R_list; }
    [[nodiscard]] C2PY_IGNORE auto const &get_coefficients() const { return coeff_list; }

    C2PY_IGNORE auto const &operator[](std::array<long, kdim> R) { return coeff_list[get_R_idx(R)]; }

    long get_R_idx(std::array<long, kdim> R) const {
      // find the home cell of the TB file to get H0
      auto it = std::ranges::find(R_list, R);
      if (it == R_list.end()) { TRIQS_RUNTIME_ERROR << "Could not locate R in the Wannier Hamiltonian.\n"; } // TODO add R vector in report
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
        }(is_ph);    // immediately calling the lambda. executed at compile time

        return nda::clef::make_expr_call(partial_eval<ph_position>(ks...), // auto{k...[ph_position]}); // C++26
                                         auto{std::get<ph_position>(std::tie(ks...))});
      }
      // Called with > 1 placeholders in kdim > 1 or 1 placeholder in kdim == 1
      else
        // FIXME : BUG : is this is a temporary, we MUST MOVE.
        // Use deducing this for this function
        return nda::clef::make_expr_call(*this, auto{ks}...);
    }

    private:
    /**
    * @brief Partial evaluation into a tb_hopping<1> for all arguments BUT pos
    *
    * @param ks : kx, ky, kz the pos-th argument is ignored.
    * @tparam pos Position of the argument which remains non evaluated
    * @return tb_hopping<1> with precomputed exp in all other dimensions
    */
    template <int placeholder_pos>
    fourier_polynomial<coeff_dim, 1> partial_eval(auto const &...ks) const
      requires((kdim == 3) or (kdim == 2))
    {

      // find the greatest and smallest values of R[pos] for all values of the other directions
      // we will use this to index R(pos)
      auto [it1, it2] = std::minmax_element(begin(R_list), end(R_list), [](auto &&r, auto &&l) { return r[placeholder_pos] < l[placeholder_pos]; });

      auto Rz_min      = (*it1)[placeholder_pos];
      auto Rz_max      = (*it2)[placeholder_pos];
      auto zero_coeffs = nda::zeros<dcomplex>(coeff_list[0].shape());

      // create containers for the compressed hoppings and R vectors
      auto new_coefficients = std::vector<nda::array<dcomplex, coeff_dim>>(Rz_max - Rz_min + 1, zero_coeffs);
      auto new_Rs           = std::vector<std::array<long, 1>>(Rz_max - Rz_min + 1);

      constexpr int p1 = (placeholder_pos + 1) % kdim;
      constexpr int p2 = (placeholder_pos + 2) % kdim;
      auto kst         = std::tie(ks...); // FIXME : remove in C++26
      // loop over all couples (R, hopping[R])
      for (auto idx : nda::range(R_list.size())) {
        auto Rz             = R_list[idx][placeholder_pos];
        new_Rs[Rz - Rz_min] = {Rz}; // Optimize this

        double kR = std::get<p1>(kst) * R_list[idx][p1];                    // enough for kdim ==2
        if constexpr (kdim == 3) kR += std::get<p2>(kst) * R_list[idx][p2]; // in kdim 3, there is a second term
        // FIXME : in C++26, use ks...[p1], ks...[p2] instead of get.
        new_coefficients[Rz - Rz_min] += coeff_list[idx] * std::exp(2i * M_PI * kR);
      }
      return {std::move(new_Rs), std::move(new_coefficients)};
    };

    // -----------------------------

    public:
    nda::array<dcomplex, 3> operator()(nda::array_const_view<double, 2> k_list) const {

      auto ks = nda::matrix_const_view<double>(k_list);
      long nk = ks.shape(0);
      long nR = R_mat_transpose.shape(1);

      // set up the phases
      nda::array<double, 2> kdotR(nk, nR);
      nda::blas::gemm(1., ks, R_mat_transpose, 0, kdotR); // FIXME transpose r_mat on storing it ? make internal one double
      // OP :
      //nda::array<double, 2> kdotR2 = ks * nda::transpose(Rs);
      nda::matrix<dcomplex> phases = nda::exp(2i * M_PI * kdotR);

      namespace ndas      = nda::stdutil;
      auto c_shape        = coeff_mat.shape();
      auto coeff_mat_view = nda::reshape(coeff_mat, std::array{c_shape[0], ndas::product(ndas::front_pop(c_shape))});

      auto result_shape    = c_shape;
      result_shape[0]      = nk; // coefficients has shape [nR, ...], replace nR -> nK for return
      auto result          = nda::array<dcomplex, 3>(result_shape);
      auto result_mat_view = nda::reshape(result, std::array{result_shape[0], ndas::product(ndas::front_pop(result_shape))});

      nda::blas::gemm(1, phases, coeff_mat_view, 0, result_mat_view);
      return result;
    }

    // ------ call with a generator of k points ----------------
    template <typename V>
      requires(std::ranges::contiguous_range<V>)
    nda::array<dcomplex, 3> operator()(V const &k_iterator) const {
      /// simply convert this into an nda structure and pass it to nda::array template function
      // FIXME : thus should be an nda function : vector generator -> matrix
      auto kvecs = nda::matrix<double>(k_iterator.size(), kdim);
      for (auto [ik, k] : itertools::enumerate(k_iterator)) {
        for (auto idim : nda::range(kdim)) { kvecs(ik, idim) = k[idim]; }
      }
      return operator()(kvecs);
    }

    // -----------------------------
    /**
    * @brief Function which takes a list of kpts, with varying dimension, and
    * performs sum_R t_ij exp(ik.R) for one dimension of xyz
    *
    * @param kpoints a list of kpoints, where the xyz index is of length dim
    * @return result of sum_R t_ij exp(ik.R) for one dimension of xyz
    */
    std::conditional_t<(coeff_dim == 2), nda::matrix<dcomplex>, nda::array<dcomplex, 3>> operator()(std::array<double, kdim> ks) const {
      assert(ks.size() == kdim);
      //auto vals = [&](long idx) { return std::exp(2i * M_PI * nda::blas::dot_generic(ks, Rs[idx])) * hoppings[idx]; };

      auto dot = [](auto const &x, auto const &y) {
        double r = 0;
        for (int i = 0; i < kdim; ++i) r += x[i] * y[i]; // kdim is known at COMPILE TIME, so loop will be optimized away
        return r;
      };

      using result_type = std::conditional_t<(coeff_dim == 2), nda::matrix<dcomplex>, nda::array<dcomplex, 3>>;
      auto res          = result_type::zeros(coeff_list[0].shape());
      for (auto idx : nda::range(R_list.size())) res += std::exp(2i * M_PI * dot(ks, R_list[idx])) * coeff_list[idx];
      return res;
    }
  };
} // namespace triqs

// this allows the deep partial evaluation mechanism to speed up calculations
template <int coeff_dim, int kdim> inline constexpr bool nda::clef::supports_partial_eval_of_calls<triqs::fourier_polynomial<coeff_dim, kdim>> = true;
