/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, H. Strand, P. Dumitrescu
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
#include <triqs/mesh.hpp>
#include <type_traits>
#include <vector>

namespace triqs::gfs {

  template <typename G> struct stacked_gf {

    using mesh_disc_t = gf_mesh<discrete<discrete_domain>>;
    using var_t       = typename decltype(mesh_disc_t{0} * std::declval<gf_mesh<typename G::variable_t>>())::var_t;
    using g_t         = gf<var_t, typename G::target_t>;
    using base_mesh_t = typename G::mesh_t;

    base_mesh_t base_mesh;
    g_t _g;

    auto _extract_base_mesh(typename g_t::mesh_t const &m) {
      if constexpr (get_n_variables<typename G::variable_t>::value == 1) // === single mesh
        return std::get<1>(m);
      else
        return triqs::tuple::pop_front(m.components());
    }

    stacked_gf() = default;

    stacked_gf(g_t &&g) : base_mesh(_extract_base_mesh(g.mesh())), _g(std::move(g)) {}

    stacked_gf(long n, G const &g) : base_mesh(g.mesh()), _g(mesh_disc_t{n} * base_mesh, g.target_shape()) {}

    typename G::view_type operator[](long i) { return _g[i, all_t{}]; }
    typename G::const_view_type operator[](long i) const { return _g[i, all_t{}]; }

    long size() const { return std::get<0>(_g.mesh()).size(); }

    // costly
    void push_back(typename G::const_view_type x) { _g = g_t{mesh_disc_t{size() + 1} * base_mesh, copy_and_push_back(_g.data(), x.data()), {}}; }

    private:
    auto _make_iter(range::const_iterator it) const {
      return utility::details::make_transform_iter(it, [this](long n) { return (*this)[n]; });
    }

    public:
    // make it iterable
    auto begin() const { return _make_iter(std::cbegin(range(0, size()))); }
    auto end() const { return _make_iter(std::cend(range(0, size()))); }
    auto cbegin() const { return begin(); }
    auto cend() const { return end(); }

    friend stacked_gf mpi_reduce(stacked_gf const &g, mpi::communicator c, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
      return {mpi_reduce(g._g, c, root, all, op)};
    }

    // FIXME redondant ...
    //friend stacked_gf mpi_all_reduce(mpi::communicator c, stacked_gf const &x) {
    //return {mpi_all_reduce(c, x._g)};
    //}
  };

  template <typename G> void h5_write(h5::group g, std::string const &name, stacked_gf<G> const &x) { h5_write(g, name, x._g); }

  template <typename G> void h5_read(h5::group g, std::string const &name, stacked_gf<G> &x) { h5_read(g, name, x._g); }

} // namespace triqs::gfs
