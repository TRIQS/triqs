/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2017, H. U.R. Strand, O. Parcollet
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

#include <triqs/gfs.hpp>
#include <triqs/arrays.hpp>
#include <triqs/gfs/gf/defs.hpp> // neded for get_n_variables template def
#include <triqs/gfs/meshes/product.hpp>  // neded for cartesian_product (contain specializ of get_n_variables)

namespace triqs {
  namespace gfs {

    // -------------------------------------------------------------------------

    /// Simple discrete domain without any naming features, c.f. triqs::gfs::discrete_domain
    class discrete_domain_simple {
      int n_max;

      public:
      using point_t = int;
      int size() const { return n_max; };
      discrete_domain_simple(int n_max = 1) : n_max(n_max) {}
      bool operator==(discrete_domain_simple const &x) const { return (n_max == x.n_max); }

      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string subgroup_name, discrete_domain_simple const & d) {
	h5::group gr = fg.create_group(subgroup_name);
	h5_write(gr, "n_max", d.n_max);
      }
      
      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string subgroup_name, discrete_domain_simple & d) {
	h5::group gr = fg.open_group(subgroup_name);
	int n_max;
	h5_read(gr, "n_max", n_max);
	d = discrete_domain_simple(n_max);
      }

      //  BOOST Serialization
      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive& ar, const unsigned int version) {
	ar& TRIQS_MAKE_NVP("n_max", n_max);
      }
      
    };
    
    // -------------------------------------------------------------------------

    struct discrete {};

    /// MeshDiscrete: gf_mesh<discrete> for usage in cartesian products with legendre meshes
    template <> struct gf_mesh<discrete> : discrete_mesh<discrete_domain_simple> {
      using B = discrete_mesh<discrete_domain_simple>;
      using var_t = discrete;

      gf_mesh() = default;
      gf_mesh(int n_max) : B{n_max} {}

      friend std::string get_triqs_hdf5_data_scheme(gf_mesh const &) { return "MeshDiscrete";}
  
      friend void h5_write(h5::group fg, std::string const &subgroup_name, gf_mesh const &m) {
	h5_write_impl(fg, subgroup_name, m, "MeshDiscrete");
      }
  
      friend void h5_read(h5::group fg, std::string const & subgroup_name, gf_mesh &m) {
	h5_read_impl(fg, subgroup_name, m, "MeshDiscrete");
      }   
    };

    // -------------------------------------------------------------------------

    struct poly_one {};

    template <> struct gf_mesh<poly_one> : discrete_mesh<legendre_domain> {
      using B = discrete_mesh<legendre_domain>;
      using var_t = poly_one;
      
      gf_mesh() = default;
      gf_mesh(double beta, statistic_enum S, long n) : B(B::domain_t{beta, S, static_cast<size_t>(n)}) {}

      friend std::string get_triqs_hdf5_data_scheme(gf_mesh const &) { return "MeshPolyOne";}
  
      friend void h5_write(h5::group fg, std::string const &subgroup_name, gf_mesh const &m) {
	h5_write_impl(fg, subgroup_name, m, "MeshPolyOne");
      }
  
      friend void h5_read(h5::group fg, std::string const & subgroup_name, gf_mesh &m) {
	h5_read_impl(fg, subgroup_name, m, "MeshPolyOne");
      }   
    };

    // -------------------------------------------------------------------------

    struct poly2 {};

    template <> struct gf_mesh<poly2> : gf_mesh<cartesian_product<discrete, poly_one>> {
      using B = gf_mesh<cartesian_product<discrete, poly_one>>;

      gf_mesh() = default;
      gf_mesh(double beta, statistic_enum S, long n) : B{{2}, {beta, S, n}} {}
    };

    // GET RID OF THIS TRAIT ?
    template <> struct get_n_variables<poly2> { static const int value = 2; };
    
    // -------------------------------------------------------------------------

    struct poly3 {};

    template <> struct gf_mesh<poly3> : gf_mesh<cartesian_product<discrete, poly_one>> {
      using B = gf_mesh<cartesian_product<discrete, poly_one>>;
      
      gf_mesh() = default;
      gf_mesh(double beta, statistic_enum S, long order) : order(order), B{{n_tetras}, {beta, S, n_coeff(order)}} {}

      size_t size() const { return B::size(); }
      
      friend std::string get_triqs_hdf5_data_scheme(gf_mesh const &) { return "MeshPoly3";}

      friend void h5_write(h5::group fg, std::string const &subgroup_name, gf_mesh const &m) {
	h5::group gr = fg.create_group(subgroup_name);
	gr.write_triqs_hdf5_data_scheme(m);
	h5_write(gr, "order", m.order);
	auto l = [gr](int N, auto const &m) { h5_write(gr, "MeshComponent" + std::to_string(N), m); };
	triqs::tuple::for_each_enumerate(m.components(), l);
      }

      friend void h5_read(h5::group fg, std::string const & subgroup_name, gf_mesh &m) {
	h5::group gr = fg.open_group(subgroup_name);
	gr.assert_triqs_hdf5_data_scheme(m, true);
	int order;
	h5_read(gr, "order", m.order);
	auto l = [gr](int N, auto &m) { h5_read(gr, "MeshComponent" + std::to_string(N), m); };
	triqs::tuple::for_each_enumerate(m.components(), l);	
      }   
      
      int order = 1;
      constexpr static int n_tetras = 6;
      constexpr int n_coeff(int order) { return (order * (order * order + 3 * order + 2)) / 6; }

      public:
      int get_order() { return order; }
    };

    // GET RID OF THIS TRAIT ?
    template <> struct get_n_variables<poly3> { static const int value = 2; };
    
  } // namespace gfs
} // namespace triqs
