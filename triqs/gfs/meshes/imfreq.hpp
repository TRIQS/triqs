/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include "./mesh_tools.hpp"
#include "../domains/matsubara.hpp"
#include <triqs/arrays/blas_lapack/gelss.hpp>
#include <triqs/utility/itertools.hpp>

namespace triqs::gfs {
  using arrays::range;
  using dcomplex = std::complex<double>;

  struct imfreq {};

  enum class matsubara_mesh_opt { all_frequencies, positive_frequencies_only };

  struct energy_t {
    double value = 0;
    explicit operator double() const { return value; }
  };

  // ---------------------------------------------------------------------------
  //                     The mesh point
  //  NB : the mesh point is also in this case a matsubara_freq.
  // ---------------------------------------------------------------------------
  template <> struct mesh_point<gf_mesh<imfreq>>; //forward

  /// Matsubara frequency mesh
  /**
 * The class `matsubara_freq_mesh` is the implementation of a mesh of fermionic ($\frac{(2n+1)\pi}{\beta}$) or bosonic ($\frac{2n\pi}{\beta}$) Matsubara frequencies.
 *
 * The mesh can span either only positive frequencies or both positive and negative frequencies (which is necessary for complex functions $G(\tau)$).
 * @figure matsubara_freq_mesh.png: Pictorial representation of ``gf_mesh<imfreq>({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See :ref:`constructor <gf_mesh<imfreq>_constructor>` for more details.
*/
  template <> struct gf_mesh<imfreq> {
    ///type of the domain: matsubara_domain<true>
    using domain_t = matsubara_domain<true>;
    ///type of the Matsubara index $n$ (as in $i\omega_n$)
    //using index_t = long;
    using index_t = _long;
    ///type of the linear index
    using linear_index_t          = long;
    using default_interpol_policy = interpol_t::None;
    ///type of the domain point
    using domain_pt_t = typename domain_t::point_t;
    using var_t       = imfreq;

    // -------------------- Constructors -------------------

    ///default constructor
    gf_mesh() : gf_mesh(domain_t{}, 0) {}

    ///constructor
    /**
   * Full constructor
   * @param dom domain
   * @param n_pts defined as n_pts = n_max + 1 (n_max: last matsubara index)
   * @param tail_fraction : percent of the function in the tail.
   * @param tail_order : order of computation of the tail
   * @param matsubara_mesh_opt tells whether the mesh is defined for all frequencies or only for positive frequencies
  */
    gf_mesh(domain_t dom, long n_pts = 1025, matsubara_mesh_opt opt = matsubara_mesh_opt::all_frequencies)
       : _dom(std::move(dom)), _n_pts(n_pts), _opt(opt) {
      _last_index = n_pts - 1; // total number of points
      if (opt == matsubara_mesh_opt::positive_frequencies_only) {
        _first_index = 0;
        _last_index  = n_pts - 1;
      } else {
        bool is_fermion = (_dom.statistic == Fermion);
        _last_index     = n_pts - 1;
        _first_index    = -(_last_index + (is_fermion ? 1 : 0));
      }
      _first_index_window = _first_index;
      _last_index_window  = _last_index;
    }

    /**
   * @param beta inverse temperature
   * @param S statistic (Fermion or Boson)
   * @param n_pts defined as n_pts = n_max + 1 (n_max: last matsubara index)
   * @param matsubara_mesh_opt tells whether the mesh is defined for all frequencies or only for positive frequencies
  */
    gf_mesh(double beta, statistic_enum S, long n_pts = 1025, matsubara_mesh_opt opt = matsubara_mesh_opt::all_frequencies)
       : gf_mesh({beta, S}, n_pts, opt) {}

    /**
   * constructor
   * @param beta inverse temperature
   * @param S statistic (Fermion or Boson)
   * @param n_pts defined as n_pts = n_max + 1 (n_max: last matsubara index)
   * @param matsubara_mesh_opt tells whether the mesh is defined for all frequencies or only for positive frequencies
  */
    gf_mesh(domain_t dom, energy_t omega_max, matsubara_mesh_opt opt = matsubara_mesh_opt::all_frequencies)
       : gf_mesh(std::move(dom), ((omega_max.value * dom.beta / M_PI) - 1) / 2, opt) {}

    /**
   * constructor
   * @param beta inverse temperature
   * @param S statistic (Fermion or Boson)
   * @param n_pts defined as n_pts = n_max + 1 (n_max: last matsubara index)
   * @param matsubara_mesh_opt tells whether the mesh is defined for all frequencies or only for positive frequencies
  */
    gf_mesh(double beta, statistic_enum S, energy_t omega_max, matsubara_mesh_opt opt = matsubara_mesh_opt::all_frequencies)
       : gf_mesh({beta, S}, omega_max, opt) {}

    // -------------------- Comparisons -------------------

    bool operator==(gf_mesh const &M) const { return (std::tie(_dom, _n_pts, _opt) == std::tie(M._dom, M._n_pts, M._opt)); }
    bool operator!=(gf_mesh const &M) const { return !(operator==(M)); }

    // -------------------- Accessors (from concept) -------------------

    /// The corresponding domain
    domain_t const &domain() const { return _dom; }

    /// Size (linear) of the mesh of the window
    long size() const { return _last_index_window - _first_index_window + 1; }

    /// Size (linear) of the mesh of the window
    long full_size() const { return _last_index - _first_index + 1; }

    ///
    utility::mini_vector<size_t, 1> size_of_components() const { return {size_t(size())}; }

    /// From an index of a point in the mesh, returns the corresponding point in the domain
    domain_pt_t index_to_point(index_t ind) const { return 1_j * M_PI * (2 * ind.value + (_dom.statistic == Fermion)) / _dom.beta; }

    /// Flatten the index in the positive linear index for memory storage (almost trivial here).
    long index_to_linear(index_t ind) const { return ind.value - first_index_window(); }

    /// Reverse of index_to_linear
    index_t linear_to_index(long lind) const { return {lind + first_index_window()}; }

    // -------------------- Accessors (other) -------------------

    /// first Matsubara index
    int first_index() const { return _first_index; }

    /// last Matsubara index
    int last_index() const { return _last_index; }

    /// first Matsubara index of the window
    int first_index_window() const { return _first_index_window; }

    /// last Matsubara index of the window
    int last_index_window() const { return _last_index_window; }

    /// Is the mesh only for positive omega_n (G(tau) real))
    bool positive_only() const { return _opt == matsubara_mesh_opt::positive_frequencies_only; }

    // -------------------- Get the grid for positive freq only -------------------

    gf_mesh get_positive_freq() const { return {domain(), _n_pts, matsubara_mesh_opt::positive_frequencies_only}; }

    // -------------------- tail -------------------

    void set_tail_parameters(double tail_fraction, int n_tail_max = 30, double rcond = 1e-4) const {
      _n_tail_max = n_tail_max;
      _tail_fraction = tail_fraction;
      _rcond = rcond;
      for (auto &l : _lss) l.reset();
      _vander.reset();
      _fit_idx_lst.reset();
    }

    private:
    // construct the Vandermonde matrix
    arrays::matrix<dcomplex> vander(std::vector<dcomplex> const &pts, int max_order) const {
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

    dcomplex idx_to_matsu_freq(int n) const { return 1_j * M_PI * (2 * n + (_dom.statistic == Fermion)) / _dom.beta; }

    // number of the points in the tail for positive omega.
    int n_pts_in_tail() const { return std::min(int(std::round(_tail_fraction * _n_pts)), _n_tail_max); }

    // maximum freq of the mesh
    dcomplex omega_max() const { return idx_to_matsu_freq(_last_index); }

    // returns the 2 ranges of indices of the points used for tail fitting
    std::vector<long> get_tail_fit_indices() const {

      // Total number of points in the fitting window
      int n_pts_in_fit_range = int(std::round(_tail_fraction * _n_pts));

      // Number of points used for the fit
      int n_tail      = n_pts_in_tail();

      std::vector<long> idx_vec;
      idx_vec.reserve(2 * n_tail);

      double step = double(n_pts_in_fit_range) / n_tail;

      double idx1 = _first_index;
      double idx2 = _last_index - n_pts_in_fit_range;

      for (int n : range(n_tail)) {
        idx_vec.push_back(long(idx1));
        idx_vec.push_back(long(idx2));
        idx1 += step;
        idx2 += step;
      }

      return idx_vec;
    }

    void setup_lss(int n_fixed_moments = 0) const {

      // Calculate the indices to fit on
      if (!_fit_idx_lst) _fit_idx_lst = std::make_shared<std::vector<long>>(get_tail_fit_indices());

      // Set Up full Vandermonde matrix up to order 9 if not set
      if (!_vander) {
        std::vector<dcomplex> C;
        C.reserve(_fit_idx_lst->size());
        auto om_max = omega_max();
        for (long n : *_fit_idx_lst) C.push_back(1. / (idx_to_matsu_freq(n) / om_max));
        _vander = std::make_shared<arrays::matrix<dcomplex>>(vander(C, 9));
      }

      if (n_fixed_moments + 1 > first_dim(*_vander) / 2) TRIQS_RUNTIME_ERROR << "Insufficient data points for least square procedure";

      // Use biggest submatrix of Vandermonde for fitting such that condition boundary fulfilled
      _lss[n_fixed_moments].reset();
      int n_max = std::min(size_t{9}, first_dim(*_vander) / 2);
      for (int n = n_fixed_moments + 1; n <= n_max; ++n) {
        auto ptr = std::make_unique<const arrays::lapack::gelss_cache<dcomplex>>((*_vander)(range(), range(n_fixed_moments, n + 1)));
        if (ptr->S_vec()[ptr->S_vec().size() - 1] > _rcond)
          _lss[n_fixed_moments] = std::move(ptr);
        else
          break;
      }
      if (!_lss[n_fixed_moments]) TRIQS_RUNTIME_ERROR << "Conditioning of tail-fit violates boundary";
    }

    /**
     * @param m mesh
   * @param data 
   * @param n position of the omega in the data array
   * @param matsubara_mesh_opt tells whether the mesh is defined for all frequencies or only for positive frequencies
  */
    // FIXME : nda : use dynamic Rank here.
    template <int R>
    friend std::pair<arrays::array<dcomplex, R>, double> get_tail(gf_mesh const &m, arrays::array_const_view<dcomplex, R> g_data, int n,
                                                                  bool normalize = true, arrays::array_const_view<dcomplex, R> known_moments = {}) {

      if (m._opt == matsubara_mesh_opt::positive_frequencies_only) TRIQS_RUNTIME_ERROR << "Can not fit on an positive_only mesh";

      // If not set, build least square solver for for given number of known moments
      int n_fixed_moments = first_dim(known_moments);
      if (!bool(m._lss[n_fixed_moments])) m.setup_lss(n_fixed_moments);

      // Total number of moments
      int n_moments = m._lss[n_fixed_moments]->n_var() + n_fixed_moments;

      using triqs::arrays::ellipsis;
      using triqs::utility::enumerate;

      // The values of the Green function. Swap relevant mesh to front
      auto g_data_swap_idx = swap_index_view(g_data, 0, n);
      auto const &imp      = g_data_swap_idx.indexmap();
      long ncols           = imp.size() / imp.lengths()[0];

      // We flatten the data in the target space and remaining meshes into the second dim
      arrays::matrix<dcomplex> g_mat(2 * m.n_pts_in_tail(), ncols);

      // Copy g_data into new matrix (necessary because g_data might have fancy strides/lengths)
      for (auto [i, n] : enumerate(*m._fit_idx_lst)) {
        if constexpr (R == 1)
          g_mat(i, 0) = g_data_swap_idx(m.index_to_linear(n));
        else
          for (auto [j, x] : enumerate(g_data_swap_idx(m.index_to_linear(n), ellipsis()))) g_mat(i, j) = x;
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
        dcomplex om_max = m.omega_max();

        for (int order : range(n_fixed_moments)) {
          if constexpr (R == 1)
            km_mat(order, 0) = z * known_moments(order, ellipsis());
          else
            for (auto [n, x] : enumerate(known_moments(order, ellipsis()))) km_mat(order, n) = z * x;
          z /= om_max;
        }

        // Shift g_mat to account for known moment correction
        g_mat -= (*m._vander)(range(), range(n_fixed_moments)) * km_mat;
      }

      // Call least square solver
      auto [a_mat, epsilon] = (*m._lss[n_fixed_moments])(g_mat); // coef + error

      // === The result a_mat contains the fitted moments divided by omega_max()^n
      // Here we extract the real moments
      if (normalize) {
        dcomplex Z = 1.0, om_max = m.omega_max();
        for (int i : range(n_fixed_moments)) Z *= om_max;
        for (int i : range(first_dim(a_mat))) {
          a_mat(i, range()) *= Z;
          Z *= om_max;
        }
      }

      // === Reinterpret the result as an R-dimensional array according to initial shape and return together with the error

      using r_t = arrays::array<dcomplex, R>; // return type
      auto lg   = g_data.indexmap().lengths();

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

    // Computes sum A_n / om^n
    // Return array<dcomplex, R -1 > if R>1 else dcomplex
    // FIXME : use dynamic R array when available.
    // FIXME : array of dimension 0
    template <int R> auto tail_eval(arrays::array_const_view<dcomplex, R> A, dcomplex om) {

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

    public:
    // -------------------- mesh_point -------------------

    /// Type of the mesh point
    using mesh_point_t = mesh_point<gf_mesh>;

    /// Accessing a point of the mesh from its index
    /**
    * @param i Matsubara index
    */
    inline mesh_point_t operator[](index_t i) const; //impl below

    /// Iterating on all the points...
    using const_iterator = mesh_pt_generator<gf_mesh>;
    inline const_iterator begin() const; // impl below
    inline const_iterator end() const;
    inline const_iterator cbegin() const;
    inline const_iterator cend() const;

    // -------------- Evaluation of a function on the grid --------------------------

    /// Is the point in mesh ?
    bool is_within_boundary(long n) const { return ((n >= first_index_window()) && (n <= last_index_window())); }
    bool is_within_boundary(matsubara_freq const &f) const { return is_within_boundary(f.n); }

    long get_interpolation_data(interpol_t::None, long n) const { return n; }
    long get_interpolation_data(interpol_t::None, matsubara_freq n) const { return n.n; }
    template <typename F> auto evaluate(interpol_t::None, F const &f, long n) const { return f[n]; }
    template <typename F> auto evaluate(interpol_t::None, F const &f, matsubara_freq n) const { return f[n.n]; }

    // -------------------- MPI -------------------

    /// Scatter a mesh over the communicator c
    // In practice, the same mesh, with a different window.
    // the window can only be set by these 2 operations
    friend gf_mesh mpi_scatter(gf_mesh const &m, mpi::communicator c, int root) {
      auto m2 =
         gf_mesh{m.domain(), m.size() / 2, (m.positive_only() ? matsubara_mesh_opt::positive_frequencies_only : matsubara_mesh_opt::all_frequencies)};
      std::tie(m2._first_index_window, m2._last_index_window) = mpi::slice_range(m2._first_index, m2._last_index, c.size(), c.rank());
      return m2;
    }

    /// Opposite of scatter
    friend gf_mesh mpi_gather(gf_mesh const &m, mpi::communicator c, int root) {
      return gf_mesh{m.domain(), m.full_size() / 2,
                     (m.positive_only() ? matsubara_mesh_opt::positive_frequencies_only : matsubara_mesh_opt::all_frequencies)};
    }

    friend std::ostream &operator<<(std::ostream &sout, gf_mesh const &m) {
      return sout << "Matsubara Freq Mesh of size " << m.size() <<", Domain: " << m.domain() <<", positive_only : " << m.positive_only();
    }

    // -------------------- HDF5 -------------------

    static std::string hdf5_scheme() { return "MeshImFreq"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string subgroup_name, gf_mesh const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      gr.write_hdf5_scheme(m);
      h5_write(gr, "domain", m.domain());
      h5_write(gr, "size", long(m.size()));
      h5_write(gr, "positive_freq_only", (m.positive_only() ? 1 : 0));
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string subgroup_name, gf_mesh &m) {
      h5::group gr = fg.open_group(subgroup_name);
      gr.assert_hdf5_scheme(m, true);
      typename gf_mesh::domain_t dom;
      long L;
      h5_read(gr, "domain", dom);
      h5_read(gr, "size", L);
      int pos_freq = 0;
      if (gr.has_key("positive_freq_only")) h5_read(gr, "positive_freq_only", pos_freq);
      if (gr.has_key("start_at_0")) h5_read(gr, "start_at_0", pos_freq); // backward compatibility only
      int n_pts = (pos_freq ? L : (L + 1) / 2); // positive freq, size is correct, otherwise divide by 2 (euclidian, ok for bosons).
      auto opt  = (pos_freq == 1 ? matsubara_mesh_opt::positive_frequencies_only : matsubara_mesh_opt::all_frequencies);
      m         = gf_mesh{std::move(dom), n_pts, opt};
    }

    // -------------------- boost serialization -------------------

    // not implemented: the _opt need to be serialized.
    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &_dom;
      ar &_n_pts;
      ar &_opt;
      ar &_first_index;
      ar &_last_index;
      ar &_first_index_window;
      ar &_last_index_window;
    }

    // -------------------- print  -------------------

    friend std::ostream &operator<<(std::ostream &sout, gf_mesh const &m) {
      return sout << "Matsubara Freq Mesh of size " << m.size() << ", domain " << m.domain() << ", positive_only : " << m.positive_only();
    }

    // ------------------------------------------------
    private:
    domain_t _dom;
    int _n_pts;
    matsubara_mesh_opt _opt;
    long _first_index, _last_index, _first_index_window, _last_index_window;
    mutable int _n_tail_max = 30;
    mutable double _tail_fraction = 0.2;
    mutable double _rcond = 1e-4;
    mutable std::array<std::shared_ptr<const arrays::lapack::gelss_cache<dcomplex>>, 4> _lss;
    mutable std::shared_ptr<arrays::matrix<dcomplex>> _vander;
    mutable std::shared_ptr<std::vector<long>> _fit_idx_lst;
  };

  // ---------------------------------------------------------------------------
  //                     The mesh point
  //  NB : the mesh point is also in this case a matsubara_freq.
  // ---------------------------------------------------------------------------

  template <> struct mesh_point<gf_mesh<imfreq>> : matsubara_freq {
    using index_t = typename gf_mesh<imfreq>::index_t;
    mesh_point()  = default;
    mesh_point(gf_mesh<imfreq> const &m, index_t const &index_)
       : matsubara_freq(index_, m.domain().beta, m.domain().statistic),
         first_index_window(m.first_index_window()),
         last_index_window(m.last_index_window()),
         _mesh(&m) {}
    mesh_point(gf_mesh<imfreq> const &m) : mesh_point(m, m.first_index_window()) {}
    void advance() { ++n; }
    long linear_index() const { return n - first_index_window; }
    long index() const { return n; }
    bool at_end() const { return (n == last_index_window + 1); } // at_end means " one after the last one", as in STL
    void reset() { n = first_index_window; }
    gf_mesh<imfreq> const &mesh() const { return *_mesh; }

    private:
    long first_index_window, last_index_window;
    gf_mesh<imfreq> const *_mesh;
  };

  // ------------------- implementations -----------------------------
  inline mesh_point<gf_mesh<imfreq>> gf_mesh<imfreq>::operator[](index_t i) const { return {*this, i}; }

  inline gf_mesh<imfreq>::const_iterator gf_mesh<imfreq>::begin() const { return const_iterator(this); }
  inline gf_mesh<imfreq>::const_iterator gf_mesh<imfreq>::end() const { return const_iterator(this, true); }
  inline gf_mesh<imfreq>::const_iterator gf_mesh<imfreq>::cbegin() const { return const_iterator(this); }
  inline gf_mesh<imfreq>::const_iterator gf_mesh<imfreq>::cend() const { return const_iterator(this, true); }

  //-------------------------------------------------------

  /** \brief foreach for this mesh
  *
  *  @param m : a mesh
  *  @param F : a function of synopsis auto F (matsubara_freq_mesh::mesh_point_t)
  *
  *  Calls F on each point of the mesh, in arbitrary order.
  **/
  template <typename Lambda> void foreach (gf_mesh<imfreq> const &m, Lambda F) {
    for (auto const &w : m) F(w);
  }
} // namespace triqs::gfs
