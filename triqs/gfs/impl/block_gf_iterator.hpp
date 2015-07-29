/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2015 by M. Ferrero, O. Parcollet
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

 // -------------------------------   an iterator over the blocks --------------------------------------------------

 template <typename G>
 class block_gf_iterator
     : std::iterator<std::forward_iterator_tag,
                     std14::remove_reference_t<decltype(std::declval<G>()[*std::declval<G>().mesh().begin()])>> {
  G *bgf = NULL;
  typename G::mesh_t::const_iterator n;

  public:
  block_gf_iterator() = default;
  block_gf_iterator(G &bgf, bool at_end = false) : bgf(&bgf), n(at_end ? bgf.mesh().end() : bgf.mesh().begin()) {}

  using value_type = std14::remove_reference_t<decltype(std::declval<G>()[*std::declval<G>().mesh().begin()])>;

  value_type &operator*() { return (*bgf)[*n]; }
  value_type &operator->() { return (*bgf)[*n]; }

  block_gf_iterator &operator++() {
   ++n;
   return *this;
  }

  block_gf_iterator operator++(int) {
   auto it = *this;
   ++n;
   return it;
  }

  bool operator==(block_gf_iterator const &other) const { return ((bgf == other.bgf) && (n == other.n)); }
  bool operator!=(block_gf_iterator const &other) const { return (!operator==(other)); }
 };

 //------------

 template <typename G> std14::enable_if_t<is_block_gf_or_view<G>::value, block_gf_iterator<G>> begin(G &bgf) {
  return {bgf, false};
 }

 template <typename G> std14::enable_if_t<is_block_gf_or_view<G>::value, block_gf_iterator<G const>> cbegin(G const &bgf) {
  return {bgf, false};
 }

 template <typename G> std14::enable_if_t<is_block_gf_or_view<G>::value, block_gf_iterator<G>> end(G &bgf) {
  return {bgf, true};
 }

 template <typename G> std14::enable_if_t<is_block_gf_or_view<G>::value, block_gf_iterator<G const>> cend(G const &bgf) {
  return {bgf, true};
 }
}
}

