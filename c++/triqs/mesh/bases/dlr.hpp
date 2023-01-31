/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#include "../details/mesh_tools.hpp"

#include <cppdlr/dlr_imtime.hpp>
#include <cppdlr/dlr_basis.hpp>
#include <cppdlr/utils.hpp>
#include <cppdlr/dlr_kernels.hpp>

namespace triqs::mesh {

  //-----------------------------------------------------------------------

  template <typename Domain> struct dlr_mesh : triqs::mesh::tag::mesh {

    using domain_t       = Domain;
    using index_t        = long;
    using linear_index_t = long;
    using domain_pt_t    = typename domain_t::point_t;

    static_assert(!std::is_base_of<std::complex<double>, domain_pt_t>::value, "Internal error : cannot use Linear Mesh in this case");

    // -------------------- Constructors -------------------

    explicit dlr_mesh(domain_t dom, double lambda, double eps) :
      _dom(std::move(dom)), _lambda(lambda), _eps(eps),
      dlr_freq(cppdlr::dlr_freq(lambda, eps)),
      dlr(lambda, dlr_freq) {}

    explicit dlr_mesh(domain_t dom, double lambda, double eps,
		      nda::vector<double> dlr_freq,  cppdlr::imtime_ops dlr) :
      _dom(std::move(dom)), _lambda(lambda), _eps(eps),
      dlr_freq(std::move(dlr_freq)),
      dlr(std::move(dlr)) {}
    
    dlr_mesh() : dlr_mesh(domain_t{}, 0, 0) {}

    /// Mesh comparison
    bool operator==(dlr_mesh const &M) const {
      return ((_dom == M.domain()) && (size() == M.size()) && (std::abs(_lambda - M.lambda()) < 1.e-15) && (std::abs(_eps - M.eps()) < 1.e-15));
    }
    bool operator!=(dlr_mesh const &M) const { return !(operator==(M)); }

    // -------------------- Accessors (other) -------------------

    /// Lambda energy over beta parameter
    double lambda() const { return _lambda; }

    /// Representation accuracy
    double eps() const { return _eps; }

    // -------------------- Accessors (from concept) -------------------

    /// The corresponding domain
    domain_t const &domain() const { return _dom; }

    /// Size (linear) of the mesh of the window
    long size() const { return dlr.rank(); }

    /// Is the point in mesh ?
    static constexpr bool is_within_boundary(all_t) { return true; }
    bool is_within_boundary(double x) const { return ((x >= 0) && (x <= _dom.beta)); }
    bool is_within_boundary(index_t idx) const { return ((idx >= 0) && (idx < dlr.rank())); }

    /// From an index of a point in the mesh, returns the corresponding point in the domain
    domain_pt_t index_to_point(index_t idx) const {
      EXPECTS(is_within_boundary(idx));
      auto res = dlr.get_itnodes()[idx]; // make selective based on domain.. ?
      if(res < 0) res = 1. + res;
      res *= _dom.beta;
      ASSERT(is_within_boundary(res));
      return res;
    }

    /// Flatten the index in the positive linear index for memory storage (almost trivial here).
    long index_to_linear(index_t idx) const {
      EXPECTS(is_within_boundary(idx));
      return idx;
    }

    // -------------------- mesh_point -------------------

    /// Type of the mesh point
    using mesh_point_t = mesh_point<dlr_mesh>;

    /// Accessing a point of the mesh
    mesh_point_t operator[](index_t i) const { return {*this, i}; }

    /// Iterating on all the points...
    using const_iterator = mesh_pt_generator<dlr_mesh>;
    const_iterator begin() const { return const_iterator(this); }
    const_iterator end() const { return const_iterator(this, true); }
    const_iterator cbegin() const { return const_iterator(this); }
    const_iterator cend() const { return const_iterator(this, true); }

    // -------------- Evaluation of a function on the grid --------------------------

    template<class T> struct dependent_false : std::false_type {};

    std::array<std::pair<long, double>, 2> get_interpolation_data(double x) const {
      static_assert(dependent_false<Domain>::value,
		    "Interpolation on dlr_mesh on the imaginary time domain not supported, use the dlr_mesh on the dlr_domain for interpolation.");
      return std::array<std::pair<long, double>, 2>{};
    }

    // -------------- HDF5  --------------------------
    /// Write into HDF5
    friend void h5_write_impl(h5::group fg, std::string const &subgroup_name, dlr_mesh const &m, const char *_type) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format_as_string(gr, _type);
      h5_write(gr, "domain", m.domain());
      h5_write(gr, "lambda", m.lambda());
      h5_write(gr, "eps", m.eps());
      //h5_write(gr, "dlr_freq", m.dlr_freq);
      //h5_write(gr, "dlr", m.dlr);
    }

    /// Read from HDF5
    friend void h5_read_impl(h5::group fg, std::string const &subgroup_name, dlr_mesh &m, const char *tag_expected) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format_as_string(gr, tag_expected, true);
      typename dlr_mesh::domain_t dom;
      double lambda, eps;
      h5_read(gr, "domain", dom);
      h5_read(gr, "lambda", lambda);
      h5_read(gr, "eps", eps);
      //h5_read(gr, "dlf_freq", dlr_freq);
      //h5_read(gr, "dlf", dlr);
      m = dlr_mesh(std::move(dom), lambda, eps);
      //m = dlr_mesh(std::move(dom), lambda, eps,
      //	   std::move(dlr_freq), std::move(dlr));
    }

    // -------------------- boost serialization -------------------

    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &_dom;
      ar &_lambda;
      ar &_eps;
      ar &dlr_freq;
      ar &dlr;
    }

    // -------------------- print  -------------------

    friend std::ostream &operator<<(std::ostream &sout, dlr_mesh const &m) { return sout << "DLR Mesh of size " << m.size(); }

    // ------------------------------------------------
    private:
    domain_t _dom;
    double _lambda, _eps;
    nda::vector<double> dlr_freq;    
    cppdlr::imtime_ops dlr; 
  };

  // ---------------------------------------------------------------------------
  //                     The mesh point
  // ---------------------------------------------------------------------------

  template <typename Domain>
  struct mesh_point<dlr_mesh<Domain>> : public utility::arithmetic_ops_by_cast<mesh_point<dlr_mesh<Domain>>, typename Domain::point_t> {
    using mesh_t  = dlr_mesh<Domain>;
    using index_t = typename mesh_t::index_t;
    mesh_t const *m;
    index_t _index;

    public:
    mesh_point() : m(nullptr) {}
    mesh_point(mesh_t const &mesh, index_t const &index_) : m(&mesh), _index(index_) {}
    mesh_point(mesh_t const &mesh) : mesh_point(mesh, 0) {}
    void advance() { ++_index; }
    using cast_t = typename Domain::point_t;
    operator cast_t() const { return m->index_to_point(_index); }
    long linear_index() const { return _index; }
    long index() const { return _index; }
    bool at_end() const { return (_index == m->size()); }
    void reset() { _index = 0; }
    mesh_t const &mesh() const { return *m; }
  };

} // namespace triqs::mesh
