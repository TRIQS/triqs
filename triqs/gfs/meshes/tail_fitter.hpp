#include <triqs/utility/itertools.hpp>
#include <triqs/arrays/blas_lapack/gelss.hpp>

namespace triqs::gfs {

  using arrays::range;

  // construct the Vandermonde matrix // FIXME Elevate
  inline arrays::matrix<dcomplex> vander(std::vector<dcomplex> const &pts, int max_order) {
    arrays::matrix<dcomplex> V(pts.size(), max_order + 1);
    for (auto [i, p] : triqs::utility::enumerate(pts)) {
      dcomplex z = 1;
      for (int n = 0; n <= max_order; ++n) {
        V(i, n) = z;
        z *= p;
      }
    }
    return V;
  }

  template <typename M> struct tail_fitter {

    const M *m;
    int _n_tail_max       = 30;
    double _tail_fraction = 0.2;
    double _rcond         = 1e-4;
    std::array<arrays::lapack::gelss_cache<dcomplex>, 4> _lss;
    arrays::matrix<dcomplex> _vander;
    std::vector<long> _fit_idx_lst;

    // number of the points in the tail for positive omega.
    int n_pts_in_tail() const { return std::min(int(std::round(_tail_fraction * m->full_size())), _n_tail_max); }

    // returns the 2 ranges of indices of the points used for tail fitting
    std::vector<long> get_tail_fit_indices() const {

      // Total number of points in the fitting window
      int n_pts_in_fit_range = int(std::round(_tail_fraction * m->full_size()));

      // Number of points used for the fit
      int n_tail = n_pts_in_tail();

      std::vector<long> idx_vec;
      idx_vec.reserve(2 * n_tail);

      double step = double(n_pts_in_fit_range) / n_tail;

      double idx1 = m->first_index();
      double idx2 = m->last_index() - n_pts_in_fit_range;

      for (int n : range(n_tail)) {
        idx_vec.push_back(long(idx1));
        idx_vec.push_back(long(idx2));
        idx1 += step;
        idx2 += step;
      }

      return std::move(idx_vec);
    }

    // Set up the least-squares solver for a given number of known moments.
    // Requires a Lambda for the translation index -> x_value
    void setup_lss(int n_fixed_moments = 0) {

      // Calculate the indices to fit on
      if (_fit_idx_lst.empty()) _fit_idx_lst = get_tail_fit_indices();

      // Set Up full Vandermonde matrix up to order 9 if not set
      if (_vander.is_empty()) {
        std::vector<dcomplex> C;
        C.reserve(_fit_idx_lst.size());
        auto om_max = m->omega_max();
        for (long n : _fit_idx_lst) C.push_back(om_max / m->idx_to_freq(n));
        _vander = vander(C, 9);
      }

      if (n_fixed_moments + 1 > first_dim(_vander) / 2) TRIQS_RUNTIME_ERROR << "Insufficient data points for least square procedure";

      // Use biggest submatrix of Vandermonde for fitting such that condition boundary fulfilled
      int n_max        = std::min(size_t{9}, first_dim(_vander) / 2);
      bool rcond_check = false;
      for (int n = n_max; n >= n_fixed_moments and !rcond_check; --n) {
        auto _lss_n = arrays::lapack::gelss_cache<dcomplex>{_vander(range(), range(n_fixed_moments, n + 1))};
        if (_lss_n.S_vec()[_lss_n.S_vec().size() - 1] > _rcond) {
          _lss[n_fixed_moments] = _lss_n;
          rcond_check           = true;
        }
      }
      if (!rcond_check) TRIQS_RUNTIME_ERROR << "Conditioning of tail-fit violates boundary";
    }

    /**
     * @param m mesh
   * @param data 
   * @param n position of the omega in the data array
   * @param matsubara_mesh_opt tells whether the mesh is defined for all frequencies or only for positive frequencies
  */
    // FIXME : nda : use dynamic Rank here.
    template <int R, int R2 = R>
    std::pair<arrays::array<dcomplex, R>, double> fit_tail(arrays::array_const_view<dcomplex, R> g_data, int n, bool normalize = true,
                                                           arrays::array_const_view<dcomplex, R2> known_moments = {}) {

      static_assert((R == R2), "The rank of the moment array is not equal to the data to fit !!!");
      if (m->fit_tail_possible()) TRIQS_RUNTIME_ERROR << "Can not fit on an positive_only mesh";

      // If not set, build least square solver for for given number of known moments
      int n_fixed_moments = first_dim(known_moments);
      if (!_lss[n_fixed_moments].empty()) setup_lss(n_fixed_moments);

      // Total number of moments
      int n_moments = _lss[n_fixed_moments].n_var() + n_fixed_moments;

      using triqs::arrays::ellipsis;
      using triqs::utility::enumerate;

      // The values of the Green function. Swap relevant mesh to front
      auto g_data_swap_idx = rotate_index_view(g_data, n);
      auto const &imp      = g_data_swap_idx.indexmap();
      long ncols           = imp.size() / imp.lengths()[0];

      // We flatten the data in the target space and remaining meshes into the second dim
      arrays::matrix<dcomplex> g_mat(2 * n_pts_in_tail(), ncols);

      // Copy g_data into new matrix (necessary because g_data might have fancy strides/lengths)
      for (auto [i, n] : enumerate(_fit_idx_lst)) {
        if constexpr (R == 1)
          g_mat(i, 0) = g_data_swap_idx(m->index_to_linear(n));
        else
          for (auto [j, x] : enumerate(g_data_swap_idx(m->index_to_linear(n), ellipsis()))) g_mat(i, j) = x;
      }

      // If an array with known_moments was passed, flatten the array into a matrix
      // just like g_data. Then account for the proper shift in g_mat
      if (n_fixed_moments > 0) {
        auto imp_km   = known_moments.indexmap();
        long ncols_km = imp_km.size() / imp_km.lengths()[0];

        if (ncols != ncols_km) TRIQS_RUNTIME_ERROR << "known_moments shape incompatible with shape of data";
        arrays::matrix<dcomplex> km_mat(n_fixed_moments, ncols);

        // We have to scale the known_moments by 1/Omega_max^n
        dcomplex z      = 1;
        dcomplex om_max = m->omega_max();

        for (int order : range(n_fixed_moments)) {
          if constexpr (R == 1)
            km_mat(order, 0) = z * known_moments(order, ellipsis());
          else
            for (auto [n, x] : enumerate(known_moments(order, ellipsis()))) km_mat(order, n) = z * x;
          z /= om_max;
        }

        // Shift g_mat to account for known moment correction
        g_mat -= _vander(range(), range(n_fixed_moments)) * km_mat;
      }

      // Call least square solver
      auto [a_mat, epsilon] = _lss[n_fixed_moments](g_mat); // coef + error

      // === The result a_mat contains the fitted moments divided by omega_max()^n
      // Here we extract the real moments
      if (normalize) {
        dcomplex Z = 1.0, om_max = m->omega_max();
        for (int i : range(n_fixed_moments)) Z *= om_max;
        for (int i : range(first_dim(a_mat))) {
          a_mat(i, range()) *= Z;
          Z *= om_max;
        }
      }

      // === Reinterpret the result as an R-dimensional array according to initial shape and return together with the error

      using r_t = arrays::array<dcomplex, R>; // return type
      auto lg   = g_data_swap_idx.indexmap().lengths();

      // Index map for the view on the a_mat result
      lg[0]     = n_moments - n_fixed_moments;
      auto imp1 = typename r_t::indexmap_type{typename r_t::indexmap_type::domain_type{lg}};

      // Index map for the full result
      lg[0]    = n_moments;
      auto res = r_t(typename r_t::indexmap_type::domain_type{lg});

      if (n_fixed_moments) res(range(n_fixed_moments), ellipsis()) = known_moments;
      res(range(n_fixed_moments, n_moments), ellipsis()) = typename r_t::view_type{imp1, a_mat.storage()};

      return {std::move(res), epsilon};
    }

    //=========================================================================

    // Computes sum A_n / om^n
    // Return array<dcomplex, R -1 > if R>1 else dcomplex
    // FIXME : use dynamic R array when available.
    // FIXME : array of dimension 0
    template <int R> auto eval(arrays::array_const_view<dcomplex, R> A, dcomplex om) {

      // same algo for both cases below
      auto compute = [&A, om](auto res) { // copy, in fact rvalue
        dcomplex z = 1;
        long N     = first_dim(A);
        for (int n = 0; n < N; ++n, z /= om) res += A(n, arrays::ellipsis()) * z;
        return std::move(res);
      };

      if constexpr (R > 1) {
        return compute(arrays::zeros(front_pop(A.indexmap().lengths())));
      } else {
        return compute(dcomplex{0});
      }
    }
  };

} // namespace triqs::gfs