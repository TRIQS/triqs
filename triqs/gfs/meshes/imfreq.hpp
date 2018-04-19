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

namespace triqs {
namespace gfs {
 using arrays::range;
 using dcomplex = std::complex<double>;

 struct imfreq {};

 enum class matsubara_mesh_opt { all_frequencies, positive_frequencies_only };

 struct energy_t { 
   double value = 0;
   explicit operator double() const { return value;}
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
  using linear_index_t = long;
  using default_interpol_policy = interpol_t::None;
  ///type of the domain point
  using domain_pt_t = typename domain_t::point_t;
  using var_t = imfreq;

  // -------------------- Constructors -------------------
  
  ///default constructor
  gf_mesh() : gf_mesh(domain_t{}, 0){}

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
     : _dom(std::move(dom)), _n_pts(n_pts), _opt(opt){
   _last_index = n_pts - 1; // total number of points
   if (opt == matsubara_mesh_opt::positive_frequencies_only) {
    _first_index = 0;
    _last_index = n_pts - 1;
   } else {
    bool is_fermion = (_dom.statistic == Fermion);
    _last_index = n_pts - 1;
    _first_index = -(_last_index + (is_fermion ? 1 : 0));
   }
   _first_index_window = _first_index;
   _last_index_window = _last_index;
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
     : gf_mesh(std::move(dom), ((omega_max.value*dom.beta/M_PI)-1)/2, opt) {}

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

  bool operator==(gf_mesh const &M) const {
   return (std::tie(_dom, _n_pts, _opt) == std::tie(M._dom, M._n_pts, M._opt));
  }
  bool operator!=(gf_mesh const &M) const { return !(operator==(M)); }

  // -------------------- Accessors (from concept) -------------------

  /// The corresponding domain
  domain_t const &domain() const { return _dom; }

  /// Size (linear) of the mesh of the window
  long size() const { return _last_index_window - _first_index_window + 1; }

  /// Size (linear) of the mesh of the window
  long full_size() const { return _last_index - _first_index + 1; }

  ///
  utility::mini_vector<size_t, 1> size_of_components() const {
   return {size_t(size())};
  }

  /// From an index of a point in the mesh, returns the corresponding point in the domain
  domain_pt_t index_to_point(index_t ind) const { return 1_j * M_PI * (2 * ind.value + (_dom.statistic == Fermion)) / _dom.beta; }

  /// Flatten the index in the positive linear index for memory storage (almost trivial here).
  long index_to_linear(index_t ind) const { return ind.value - first_index_window(); }

  /// Reverse of index_to_linear
  index_t linear_to_index(long lind) const { return {lind + first_index_window()}; }

  // -------------------- Accessors (other) -------------------

  /// first Matsubara index
  int first_index() const { return _first_index;}

  /// last Matsubara index
  int last_index() const { return _last_index;}

  /// first Matsubara index of the window
  int first_index_window() const { return _first_index_window;}

  /// last Matsubara index of the window
  int last_index_window() const { return _last_index_window;}

  /// Is the mesh only for positive omega_n (G(tau) real))
  bool positive_only() const { return _opt == matsubara_mesh_opt::positive_frequencies_only;}

  // -------------------- Get the grid for positive freq only -------------------

  gf_mesh get_positive_freq() const { return {domain(), _n_pts, matsubara_mesh_opt::positive_frequencies_only}; }

  // -------------------- tail -------------------

  private:
  
  // construct the Vandermonde matrix
  arrays::matrix<dcomplex> vander(std::vector<dcomplex> const & pts, int order) const { 
    arrays::matrix<dcomplex> V(pts.size(), order);
   for (auto [i,p] : triqs::utility::enumerate(pts)) { 
     dcomplex z=1;
     for (int n =0; n < order; ++n) {
	V(i,n) = z;
	z *= p;
      }
   }
   return V;
  }

  dcomplex idx_to_matsu_freq(int n) const { 
   return 1_j * M_PI * (2 * n + (_dom.statistic == Fermion)) / _dom.beta;
  }

  void set_tail_parameters(double tail_fraction, int tail_order = -1) { 
   _tail_fraction = tail_fraction;
   _tail_order = tail_order;
   _lss.reset(); 	
  }

  // number of the points in the tail for positive omega.
  int n_pts_in_tail() const { return std::round(_tail_fraction * _n_pts);}
 
  // maximum freq of the mesh
  dcomplex omega_max () const { return idx_to_matsu_freq(_last_index);}

  // returns the 2 ranges of indices of the points used for tail fitting 
  std::array<range, 2> tail_fit_point_indices() const { 
   int n_tail = n_pts_in_tail();
   int n_step = 1; //FIXME : Cap the number of points in tail fitting.
   // two ranges of size n_tail exactly, at the start and at the end of the mesh
   auto R_p  = range{ _last_index - n_tail, _last_index, n_step};
   auto R_m  = range{ _first_index, _first_index + n_tail, n_step};
   return {R_m, R_p};
  }

  void setup_lss() const { 
    auto N = n_pts_in_tail();
    std::vector<dcomplex> C; 
    C.reserve(2*N);
    auto om_max = omega_max();
    for (auto const & r: tail_fit_point_indices())
      for (auto x : r) C.push_back( 1/ (idx_to_matsu_freq(x)/om_max));
    _lss = std::make_shared<const arrays::lapack::gelss_cache<dcomplex>>(vander(C, _tail_order));
  }

  /**
   * @param m mesh
   * @param data 
   * @param n position of the omega in the data array
   * @param matsubara_mesh_opt tells whether the mesh is defined for all frequencies or only for positive frequencies
  */
  template<int R>
  friend std::pair<arrays::array<dcomplex, R>, double> get_tail(gf_mesh const & m, arrays::array_const_view<dcomplex,R> g_data, int n) {
   
   if (m._opt == matsubara_mesh_opt::positive_frequencies_only) TRIQS_RUNTIME_ERROR << "Can not fit on an positive_only mesh";
   
   if (!bool(m._lss)) m.setup_lss();

   // The points of G
   auto g_data_swap_idx = swap_index_view(g_data, 0, n); // put index n as the first index
   auto imp = g_data_swap_idx.indexmap();
   auto lg = imp.lengths(); // lengths of the data array of g

   arrays::matrix<dcomplex> g_mat(2*m.n_pts_in_tail(), imp.size()/lg[0]);
   
   // 
   std::array<range, R-1> rs{};
   for (int i =1; i <R; ++i) rs[i-1] = range(0, lg[i]); 
   auto prod_ranges = triqs::utility::make_product(rs);

   long i=0;
   for (auto const & ra : m.tail_fit_point_indices()){
     for (auto p : ra){
      int count = 0;
      //for (auto [j, tu] : triqs::utility::enumerate(prod_ranges)) {
      for (auto const & tu : prod_ranges) {
	// FIXME Use enumerate -> BUG
	// FIXME : looks like a clang BUG ?
	auto la = [&](auto && ...x) { g_mat(i, count) = g_data_swap_idx(p, x...);};
	++count;
        std::apply(la, tu);
      }
     }
     ++i;
    }
   
   // Call SVD 
   auto [a_mat, epsilon] = (*m._lss)(g_mat); // coef + error
  
   // Reinterpret the result as an R dim array and return 
   using r_t = arrays::array<dcomplex, R>; // return type
   lg[0] = m._tail_order; // change the length corresponding to omega, now it is the order index
   //FIXME : Ugly, Avoid copy, use std::move
   //FIXME : Assert Memory layout C
   auto arr = r_t{ typename r_t::indexmap_type::domain_type{lg}, std::move(a_mat).storage()};
   return { std::move(arr), epsilon };

  }
 
  // Computes sum A_n / om^n 
  // Return array<dcomplex, R -1 > if R>1 else dcomplex
  // FIXME : use dynamic R array when available.
  // FIXME : array of dimension 0
  template<int R> 
    auto tail_eval(arrays::array_const_view<dcomplex,R> A, dcomplex om) {
   
    // same algo for both cases below 
    auto compute = [&A, om](auto res) { // copy, in fact rvalue 
       dcomplex z = 1;
       long N = first_dim(A);       
       for (int n=0; n<N; ++n, z /=om)
	  res += A(n, arrays::ellipsis()) * z;
       return std::move(res);
    };

    if constexpr(R>1) {
      return compute(arrays::zeros(front_pop(A.indexmap().lengths())));
    }
    else {
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
  inline const_iterator begin() const;  // impl below
  inline const_iterator end() const;
  inline const_iterator cbegin() const;
  inline const_iterator cend() const;

 // -------------- Evaluation of a function on the grid --------------------------

  /// Is the point in mesh ?
  bool is_within_boundary(long n) const { return ((n >= first_index_window()) && (n <= last_index_window())); }
  bool is_within_boundary(matsubara_freq const &f) const { return is_within_boundary(f.n);}

  long get_interpolation_data(interpol_t::None, long n) const { return n;}
  long get_interpolation_data(interpol_t::None, matsubara_freq n) const { return n.n;}
  template <typename F> auto evaluate(interpol_t::None, F const &f, long n) const { return f[n]; }
  template <typename F> auto evaluate(interpol_t::None, F const &f, matsubara_freq n) const { return f[n.n]; }

  // -------------------- MPI -------------------

  /// Scatter a mesh over the communicator c
  // In practice, the same mesh, with a different window.
  // the window can only be set by these 2 operations
  friend gf_mesh mpi_scatter(gf_mesh const &m, mpi::communicator c, int root) {
   auto m2 = gf_mesh{m.domain(), m.size()/2,
                     (m.positive_only() ? matsubara_mesh_opt::positive_frequencies_only : matsubara_mesh_opt::all_frequencies)};
   std::tie(m2._first_index_window, m2._last_index_window) = mpi::slice_range(m2._first_index, m2._last_index, c.size(), c.rank());
   return m2;
  }

  /// Opposite of scatter
  friend gf_mesh mpi_gather(gf_mesh const &m, mpi::communicator c, int root) {
   return gf_mesh{m.domain(), m.full_size()/2,
                  (m.positive_only() ? matsubara_mesh_opt::positive_frequencies_only : matsubara_mesh_opt::all_frequencies)};
  }

  // -------------------- HDF5 -------------------

  static std::string hdf5_scheme() {return  "MeshImFreq";}

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
   int n_pts = (pos_freq ? L : (L+1) / 2); // positive freq, size is correct, otherwise divide by 2 (euclidian, ok for bosons).
   auto opt = (pos_freq == 1 ? matsubara_mesh_opt::positive_frequencies_only : matsubara_mesh_opt::all_frequencies);
   m = gf_mesh{std::move(dom), n_pts, opt};
  }

  // -------------------- boost serialization -------------------

  // not implemented: the _opt need to be serialized.
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   ar & _dom;
   ar & _n_pts;
   ar & _opt;
   ar & _first_index;
   ar & _last_index;
   ar & _first_index_window;
   ar & _last_index_window;
  }

  // -------------------- print  -------------------

  friend std::ostream &operator<<(std::ostream &sout, gf_mesh const &m) {
   return sout << "Matsubara Freq Mesh of size " << m.size() <<", Domain: " << m.domain() <<", positive_only : " << m.positive_only();
  }

  // ------------------------------------------------
  private:
  domain_t _dom;
  int _n_pts;
  matsubara_mesh_opt _opt;
  long _first_index, _last_index, _first_index_window, _last_index_window;
  double _tail_fraction = 0.2;
  int _tail_order = -1;
  mutable std::shared_ptr<const arrays::lapack::gelss_cache<dcomplex>> _lss;
 };

 // ---------------------------------------------------------------------------
 //                     The mesh point
 //  NB : the mesh point is also in this case a matsubara_freq.
 // ---------------------------------------------------------------------------

 template <> struct mesh_point<gf_mesh<imfreq>> : matsubara_freq {
  using index_t = typename gf_mesh<imfreq>::index_t;
  mesh_point() = default;
  mesh_point(gf_mesh<imfreq> const &m, index_t const &index_)
     : matsubara_freq(index_, m.domain().beta, m.domain().statistic)
     , first_index_window(m.first_index_window())
     , last_index_window(m.last_index_window())
     , _mesh(&m) {}
  mesh_point(gf_mesh<imfreq> const &m) : mesh_point(m, m.first_index_window()) {}
  void advance() { ++n; }
  long linear_index() const { return n - first_index_window; }
  long index() const { return n; }
  bool at_end() const { return (n == last_index_window + 1); } // at_end means " one after the last one", as in STL
  void reset() { n = first_index_window; }
  gf_mesh<imfreq> const & mesh() const { return *_mesh;}

  private:
  long first_index_window, last_index_window;
  gf_mesh<imfreq> const * _mesh;
 };

 // ------------------- implementations -----------------------------
 inline mesh_point<gf_mesh<imfreq>> gf_mesh<imfreq>::operator[](index_t i) const {
  return {*this, i};
 }

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
 template <typename Lambda> void foreach(gf_mesh<imfreq> const &m, Lambda F) {
  for (auto const &w : m) F(w);
 }
}
}

