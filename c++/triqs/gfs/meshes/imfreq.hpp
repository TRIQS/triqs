// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell, tayral

#pragma once
#include "./mesh_tools.hpp"
#include "../domains/matsubara.hpp"
#include "./tail_fitter.hpp"

namespace triqs::gfs {

  using itertools::range;
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
  template <> struct gf_mesh<imfreq> : tail_fitter_handle {
    ///type of the domain: matsubara_domain<true>
    using domain_t = matsubara_domain<true>;
    ///type of the Matsubara index $n$ (as in $i\omega_n$)
    //using index_t = long;
    using index_t = _long;
    ///type of the linear index
    using linear_index_t = long;
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

    /// Size (linear) of the mesh
    long size() const { return _last_index - _first_index + 1; }

    ///
    utility::mini_vector<size_t, 1> size_of_components() const { return {size_t(size())}; }

    /// Is the point in mesh ?
    static constexpr bool is_within_boundary(all_t) { return true; }
    bool is_within_boundary(long n) const { return ((n >= first_index()) && (n <= last_index())); }
    bool is_within_boundary(index_t idx) const { return is_within_boundary(idx.value); }
    bool is_within_boundary(matsubara_freq const &f) const { return is_within_boundary(f.n); }

    /// From an index of a point in the mesh, returns the corresponding point in the domain
    domain_pt_t index_to_point(index_t idx) const {
      EXPECTS(is_within_boundary(idx));
      return 1i * M_PI * (2 * idx.value + (_dom.statistic == Fermion)) / _dom.beta;
    }

    /// Flatten the index in the positive linear index for memory storage (almost trivial here).
    long index_to_linear(index_t idx) const {
      EXPECTS(is_within_boundary(idx));
      return idx.value - first_index();
    }

    /// Reverse of index_to_linear
    index_t linear_to_index(long lidx) const { return {lidx + first_index()}; }

    // -------------------- Accessors (other) -------------------

    /// first Matsubara index
    int first_index() const { return _first_index; }

    /// last Matsubara index
    int last_index() const { return _last_index; }

    /// Is the mesh only for positive omega_n (G(tau) real))
    bool positive_only() const { return _opt == matsubara_mesh_opt::positive_frequencies_only; }

    // -------------------- Get the grid for positive freq only -------------------

    gf_mesh get_positive_freq() const { return {domain(), _n_pts, matsubara_mesh_opt::positive_frequencies_only}; }

    // -------------------- tail -------------------

    // maximum freq of the mesh
    dcomplex omega_max() const { return index_to_point(_last_index); }

    dcomplex index_to_point(int n) const { return 1i * M_PI * (2 * n + (_dom.statistic == Fermion)) / _dom.beta; }

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

    // For multivar evaluation
    interpol_data_all_t get_interpolation_data(all_t) const { return {}; }
    interpol_data_0d_t<index_t> get_interpolation_data(long n) const { return {n}; }
    interpol_data_0d_t<index_t> get_interpolation_data(matsubara_freq n) const { return {n.n}; }

    // For one var evaluation
    template <typename F> auto evaluate(F const &f, long n) const { return f[n]; }
    template <typename F> auto evaluate(F const &f, matsubara_freq n) const { return f[n.n]; }

    friend std::ostream &operator<<(std::ostream &sout, gf_mesh const &m) {
      return sout << "Matsubara Freq Mesh of size " << m.size() << ", Domain: " << m.domain() << ", positive_only : " << m.positive_only();
    }
    // -------------------- HDF5 -------------------

    static std::string hdf5_format() { return "MeshImFreq"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string subgroup_name, gf_mesh const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);
      h5_write(gr, "domain", m.domain());
      h5_write(gr, "size", long(m.size()));
      h5_write(gr, "positive_freq_only", (m.positive_only() ? 1 : 0));
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string subgroup_name, gf_mesh &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);
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
    }

    // ------------------------------------------------
    private:
    domain_t _dom;
    int _n_pts;
    matsubara_mesh_opt _opt;
    long _first_index, _last_index;
  };

  // ---------------------------------------------------------------------------
  //                     The mesh point
  //  NB : the mesh point is also in this case a matsubara_freq.
  // ---------------------------------------------------------------------------

  template <> struct mesh_point<gf_mesh<imfreq>> : matsubara_freq {
    using index_t = typename gf_mesh<imfreq>::index_t;
    mesh_point()  = default;
    mesh_point(gf_mesh<imfreq> const &m, index_t const &index_)
       : matsubara_freq(index_, m.domain().beta, m.domain().statistic), first_index(m.first_index()), last_index(m.last_index()), _mesh(&m) {}
    mesh_point(gf_mesh<imfreq> const &m) : mesh_point(m, m.first_index()) {}
    void advance() { ++n; }
    long linear_index() const { return n - first_index; }
    long index() const { return n; }
    bool at_end() const { return (n == last_index + 1); } // at_end means " one after the last one", as in STL
    void reset() { n = first_index; }
    gf_mesh<imfreq> const &mesh() const { return *_mesh; }

    private:
    long first_index, last_index;
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
