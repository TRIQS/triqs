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

 /// The domain
 struct R_domain {
  using point_t = double;
  bool operator==(R_domain const& D) const { return true; }
  friend void h5_write(h5::group fg, std::string subgroup_name, R_domain const& d) {}
  friend void h5_read(h5::group fg, std::string subgroup_name, R_domain& d) {}
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, const unsigned int version) {}
 };
}
}


