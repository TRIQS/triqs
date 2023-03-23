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

#include <cppdlr/utils.hpp>
#include <cppdlr/dlr_basis.hpp>
#include <cppdlr/dlr_kernels.hpp>
#include <cppdlr/dlr_imtime.hpp>
#include <cppdlr/dlr_imfreq.hpp>


template<class T> struct dependent_false : std::false_type {};


namespace triqs::mesh {

  namespace tag {
    struct dlr_repr_coeffs {};
    struct dlr_repr_imtime {};
    struct dlr_repr_imfreq {};
  } // namespace tag
  
  //-----------------------------------------------------------------------

  template <typename Domain, typename Repr> struct dlr_mesh : triqs::mesh::tag::mesh {

    using domain_t       = Domain;
    using repr_t         = Repr;
    using index_t        = long;
    using linear_index_t = long;
    using domain_pt_t    = typename domain_t::point_t;

    // -------------------- Constructors -------------------

    explicit dlr_mesh(domain_t dom, double lambda, double eps) :
      _dom(std::move(dom)), _lambda(lambda), _eps(eps),
      _dlr_freq(cppdlr::dlr_freq(lambda, eps)),
      _dlr_it(lambda, _dlr_freq),
      _dlr_if(lambda, _dlr_freq, dom.statistic == Fermion ? -1 : +1) {}

    explicit dlr_mesh(domain_t dom, double lambda, double eps,
		      nda::vector<double> dlr_freq,
		      cppdlr::imtime_ops dlr_it, cppdlr::imfreq_ops dlr_if) :
      _dom(std::move(dom)), _lambda(lambda), _eps(eps),
      _dlr_freq(std::move(dlr_freq)),
      _dlr_it(std::move(dlr_it)), _dlr_if(std::move(dlr_if)) {}
    
    dlr_mesh() : _dom(), _lambda(0), _eps(0), _dlr_freq(), _dlr_it(), _dlr_if() {}

    template <typename D, typename R>
    explicit dlr_mesh(dlr_mesh<D, R> const &M) :
      _dom(M.domain()), _lambda(M.lambda()), _eps(M.eps()),
      _dlr_freq(M.dlr_freq()), _dlr_it(M.dlr_it()), _dlr_if(M.dlr_if()) {}

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

    auto dlr_freq() const { return _dlr_freq; }
    auto dlr_it() const { return _dlr_it; }
    auto dlr_if() const { return _dlr_if; }
    
    // -------------------- Accessors (from concept) -------------------

    /// The corresponding domain
    domain_t const &domain() const { return _dom; }

    /// Size (linear) of the mesh of the window
    long size() const { return _dlr_it.rank(); }

    /// Is the point in mesh ?
    static constexpr bool is_within_boundary(all_t) { return true; }
    bool is_within_boundary(double x) const { return ((x >= 0) && (x <= _dom.beta)); }
    bool is_within_boundary(index_t idx) const { return ((idx >= 0) && (idx < _dlr_it.rank())); }

    /// From an index of a point in the mesh, returns the corresponding point in the domain
    domain_pt_t index_to_point(index_t idx) const {
      EXPECTS(is_within_boundary(idx));
      if constexpr ( std::is_same_v<repr_t, tag::dlr_repr_imtime> ) {
        auto res = _dlr_it.get_itnodes()[idx]; // make selective based on domain.. ?
	if(res < 0) res = 1. + res;
	res *= _dom.beta;
	ASSERT(is_within_boundary(res));
	return res;
	
      } else if constexpr ( std::is_same_v<repr_t, tag::dlr_repr_imfreq> ) {
	auto n = _dlr_if.get_ifnodes()[idx]; // Gives integer index
	auto m = _dom.statistic == Fermion ? 2*n + 1 : 2*n;
	auto res = nda::dcomplex(0, M_PI / _dom.beta * m);
	return res; // TODO: Wrap this in a matsubara frequency mesh point

      } else if constexpr ( std::is_same_v<repr_t, tag::dlr_repr_coeffs> ) {
	auto res = _dlr_freq[idx];
	return res;
      } else {
	static_assert(dependent_false<Repr>::value,
		      "Iteration only supported for dlr_coeffs and dlr_imtime.");
      }
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

    std::array<std::pair<long, double>, 2> get_interpolation_data(double x) const {
      if constexpr ( std::is_same_v<repr_t, tag::dlr_repr_imtime> ) {
	static_assert(dependent_false<Domain>::value,
		      "Interpolation on dlr_mesh on the imaginary time domain not supported, use the dlr_mesh on the dlr_domain for interpolation.");
      } else if constexpr ( std::is_same_v<repr_t, tag::dlr_repr_coeffs> ) {
	return {std::make_pair(0, 1.0), std::make_pair(1, 1.0)};
      } else {
	static_assert(dependent_false<Repr>::value,
		      "Interploation not supported.");
      }
	
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
      h5_write(gr, "dlr_freq", m.dlr_freq());
      h5_write(gr, "dlr_it", m.dlr_it());
      h5_write(gr, "dlr_if", m.dlr_if());
    }

    /// Read from HDF5
    friend void h5_read_impl(h5::group fg, std::string const &subgroup_name, dlr_mesh &m, const char *tag_expected) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format_as_string(gr, tag_expected, true);
      typename dlr_mesh::domain_t dom;
      double lambda, eps;
      nda::vector<double> _dlr_freq;
      cppdlr::imtime_ops _dlr_it;
      cppdlr::imfreq_ops _dlr_if;
      
      h5_read(gr, "domain", dom);
      h5_read(gr, "lambda", lambda);
      h5_read(gr, "eps", eps);
      h5_read(gr, "dlr_freq", _dlr_freq);
      h5_read(gr, "dlr_it", _dlr_it);
      h5_read(gr, "dlr_if", _dlr_if);
      
      m = dlr_mesh(dom, lambda, eps, _dlr_freq, _dlr_it, _dlr_if);
    }

    // -------------------- boost serialization -------------------

    /*
    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &_dom;
      ar &_lambda;
      ar &_eps;
      ar &dlr_freq;
      ar &dlr;
    }
    */
    
    // -------------------- print  -------------------

    friend std::ostream &operator<<(std::ostream &sout, dlr_mesh const &m) { return sout << "DLR Mesh of size " << m.size(); }

    // ------------------------------------------------
    private:
    domain_t _dom;
    double _lambda, _eps;
    nda::vector<double> _dlr_freq;    
    cppdlr::imtime_ops _dlr_it; 
    cppdlr::imfreq_ops _dlr_if; 
  };

  // ---------------------------------------------------------------------------
  //                     The mesh point
  // ---------------------------------------------------------------------------

  template <typename Domain, typename Repr>
  struct mesh_point<dlr_mesh<Domain, Repr>> : public utility::arithmetic_ops_by_cast<mesh_point<dlr_mesh<Domain, Repr>>, typename Domain::point_t> {
    using mesh_t  = dlr_mesh<Domain, Repr>;
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

  inline std::ostream &operator<<(std::ostream &out, mesh_point<dlr_mesh<matsubara_domain<true>, tag::dlr_repr_imfreq> > const &y) { return out << std::complex<double>(y); }
  

} // namespace triqs::mesh
