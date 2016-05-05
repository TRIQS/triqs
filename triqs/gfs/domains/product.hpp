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
namespace triqs {
namespace gfs {

 template <typename... Domains> struct domain_product {
  using point_t = std::tuple<typename Domains::point_t...>;
  std::tuple<Domains...> domains;
  domain_product() = default;
  domain_product(Domains const&... doms) : domains(doms...) {}
  friend bool operator==(domain_product const& D1, domain_product const& D2) { return D1.domains == D2.domains; }
  // implement boost serializable, hdf5 if needed... (done at the mesh level).
 };
}
}
