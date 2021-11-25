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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell, tayral

#pragma once
#include "./details/mesh_tools.hpp"
#include "./domains/matsubara.hpp"
#include "./details/tail_fitter.hpp"

namespace triqs::mesh {

  /// A double with no implicit conversion.
  struct energy_t {
    double value = 0;
    explicit operator double() const { return value; }
  };

  class imfreq;
  template <> struct mesh_point<imfreq>; //forward

  /**
   *  Matsubara frequencies 
   *   - fermionic $\omega_n = \frac{(2n+1)\pi}{\beta}$ 
   *   - bosonic $\omega_n = \frac{2n\pi}{\beta}$
   *
   *  The mesh is a finite segment of 
   *
   * The mesh can span either only positive frequencies or both positive and negative frequencies (which is necessary for complex functions $G(\tau)$).
   *
   * @figure matsubara_freq_mesh.png: Pictorial representation of ``imfreq({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See :ref:`constructor <imfreq_constructor>` for more details.
   */
  class imfreq : tag::mesh, public tail_fitter_handle {

    public:
    ///type of the domain: matsubara_freq_domain
    using domain_t = matsubara_freq_domain;

    ///type of the Matsubara index $n$ (as in $i\omega_n$)
    using index_t = _long;

    ///type of the linear index
    using linear_index_t = long;

    ///type of the domain point
    using domain_pt_t = typename domain_t::point_t;

    /// Option for the constructor. Restrict to positive frequency or not
    enum class option { all_frequencies, positive_frequencies_only };

    // -------------------- Constructors -------------------

    ///
    imfreq() : imfreq(domain_t{}, 0) {}

    /**
     * Construct a Mesh of Matsubara frequencies on a Matsubara domain
     *
     * @param dom Matsubara domain
     * @param n_iw Number of positive Matsubara frequencies
     * @param option Wether to use all frequencies, or only the positive ones
     */
    imfreq(domain_t dom, size_t n_iw = 1025, option opt = option::all_frequencies) : _dom(std::move(dom)), _n_iw(n_iw), _opt(opt) {
      _last_index = n_iw - 1; // total number of points
      if (opt == option::positive_frequencies_only) {
        _first_index = 0;
        _last_index  = n_iw - 1;
      } else {
        bool is_fermion = (_dom.statistic == Fermion);
        _last_index     = n_iw - 1;
        _first_index    = -(_last_index + (is_fermion ? 1 : 0));
      }
    }

    /**
     * Construct a Mesh of Matsubara frequencies on a Matsubara domain
     *
     * @param beta Inverse temperature
     * @param S Statistic (Fermion or Boson)
     * @param n_iw The number of positive Matsubara frequencies
     * @param option Wether to use all frequencies, or only the positive ones
     */
    imfreq(double beta, statistic_enum S, size_t n_iw = 1025, option opt = option::all_frequencies) : imfreq({beta, S}, n_iw, opt) {}

    /**
     * @param dom
     * @param omega_max
     */
    imfreq(domain_t dom, energy_t omega_max, option opt = option::all_frequencies)
       : imfreq(std::move(dom), ((omega_max.value * dom.beta / M_PI) - 1) / 2, opt) {}

    /**
     */
    imfreq(double beta, statistic_enum S, energy_t omega_max, option opt = option::all_frequencies) : imfreq({beta, S}, omega_max, opt) {}

    // -------------------- Comparisons -------------------

    ///
    bool operator==(imfreq const &M) const { return (std::tie(_dom, _n_iw, _opt) == std::tie(M._dom, M._n_iw, M._opt)); }

    ///
    bool operator!=(imfreq const &M) const { return !(operator==(M)); }

    // -------------------- Accessors (from concept) -------------------

    /// The corresponding domain
    domain_t const &domain() const { return _dom; }

    /// Size (linear) of the mesh
    long size() const { return _last_index - _first_index + 1; }

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
    bool positive_only() const { return _opt == option::positive_frequencies_only; }

    // -------------------- Get the grid for positive freq only -------------------

    imfreq get_positive_freq() const { return {domain(), _n_iw, option::positive_frequencies_only}; }

    // -------------------- tail -------------------

    /// maximum freq of the mesh
    dcomplex omega_max() const { return index_to_point(_last_index); }

    ///
    dcomplex index_to_point(int n) const { return 1i * M_PI * (2 * n + (_dom.statistic == Fermion)) / _dom.beta; }

    // -------------------- mesh_point -------------------

    /// Type of the mesh point
    using mesh_point_t = mesh_point<imfreq>;

    /**
     * Accessing a point of the mesh from its index
     * @param i Matsubara index
     */
    inline mesh_point_t operator[](index_t i) const; //impl below

    /// Iterating on all the points...
    using const_iterator = mesh_pt_generator<imfreq>;

    ///
    inline const_iterator begin() const; // impl below

    ///
    inline const_iterator end() const;

    ///
    inline const_iterator cbegin() const;

    ///
    inline const_iterator cend() const;

    // -------------- Evaluation of a function on the grid --------------------------

    std::array<std::pair<linear_index_t, one_t>, 1> get_interpolation_data(long n) const { return {std::make_pair(index_to_linear(index_t(n)), one_t{})}; }
    std::array<std::pair<linear_index_t, one_t>, 1> get_interpolation_data(matsubara_freq n) const { return get_interpolation_data(n.n); }

    friend std::ostream &operator<<(std::ostream &sout, imfreq const &m) {
      return sout << "Matsubara Freq Mesh of size " << m.size() << ", Domain: " << m.domain() << ", positive_only : " << m.positive_only();
    }
    // -------------------- HDF5 -------------------

    static std::string hdf5_format() { return "MeshImFreq"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string subgroup_name, imfreq const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);
      h5_write(gr, "domain", m.domain());
      h5_write(gr, "size", long(m.size()));
      h5_write(gr, "positive_freq_only", (m.positive_only() ? 1 : 0));
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string subgroup_name, imfreq &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);
      typename imfreq::domain_t dom;
      long L;
      h5_read(gr, "domain", dom);
      h5_read(gr, "size", L);
      int pos_freq = 0;
      if (gr.has_key("positive_freq_only")) h5_read(gr, "positive_freq_only", pos_freq);
      if (gr.has_key("start_at_0")) h5_read(gr, "start_at_0", pos_freq); // backward compatibility only
      size_t n_iw = (pos_freq ? L : (L + 1) / 2); // positive freq, size is correct, otherwise divide by 2 (euclidian, ok for bosons).
      auto opt  = (pos_freq == 1 ? option::positive_frequencies_only : option::all_frequencies);
      m         = imfreq{std::move(dom), n_iw, opt};
    }

    // -------------------- boost serialization -------------------

    // not implemented: the _opt need to be serialized.
    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &_dom;
      ar &_n_iw;
      ar &_opt;
      ar &_first_index;
      ar &_last_index;
    }

    // ------------------------------------------------
    private:
    domain_t _dom;
    size_t _n_iw;
    option _opt;
    long _first_index, _last_index;
  };

  // ---------------------------------------------------------------------------
  //                     The mesh point
  //  NB : the mesh point is also in this case a matsubara_freq.
  // ---------------------------------------------------------------------------

  template <> struct mesh_point<imfreq> : matsubara_freq {
    using index_t = typename imfreq::index_t;
    mesh_point()  = default;
    mesh_point(imfreq const &m, index_t const &index_)
       : matsubara_freq(index_, m.domain().beta, m.domain().statistic), first_index(m.first_index()), last_index(m.last_index()), _mesh(&m) {}
    mesh_point(imfreq const &m) : mesh_point(m, m.first_index()) {}
    void advance() { ++n; }
    long linear_index() const { return n - first_index; }
    long index() const { return n; }
    bool at_end() const { return (n == last_index + 1); } // at_end means " one after the last one", as in STL
    void reset() { n = first_index; }
    imfreq const &mesh() const { return *_mesh; }

    private:
    long first_index, last_index;
    imfreq const *_mesh;
  };

  // ------------------- implementations -----------------------------
  inline mesh_point<imfreq> imfreq::operator[](index_t i) const { return {*this, i}; }

  inline imfreq::const_iterator imfreq::begin() const { return const_iterator(this); }
  inline imfreq::const_iterator imfreq::end() const { return const_iterator(this, true); }
  inline imfreq::const_iterator imfreq::cbegin() const { return const_iterator(this); }
  inline imfreq::const_iterator imfreq::cend() const { return const_iterator(this, true); }

} // namespace triqs::mesh
