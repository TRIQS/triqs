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
#include <map>
namespace triqs {
namespace gfs {

 class discrete_domain {
  int Nmax;
  std::vector<std::string> _names; // optional name of the points (e.g. for block)
  std::map<std::string, int> _inv_names; // table inverting the previous vector
  void init_inv() {
   int i = 0;
   for (auto const& x : _names) _inv_names[x] = i++;
  }

  public:
  using point_t = int;
  int size() const {
   return Nmax;
  };
  discrete_domain(int Nmax_ = 1) : Nmax(Nmax_) {
   for (int i = 0; i < Nmax; ++i) {
    std::stringstream fs;
    fs << i;
    _names.push_back(fs.str());
   }
   init_inv();
  }

  discrete_domain(std::vector<std::string> Names) : Nmax(Names.size()), _names(std::move(Names)) { init_inv(); }
  discrete_domain(std::initializer_list<std::string> const& Names) : Nmax(Names.size()), _names(Names) { init_inv(); }

  std::vector<std::string> const& names() const { return _names; }
  int index_from_name(std::string const& s) const { return _inv_names.at(s); }

  bool operator==(discrete_domain const& D) const { return (Nmax == D.Nmax); }

  /*
  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, discrete_domain const& d) {
   h5::group gr = fg.create_group(subgroup_name);
   h5_write(gr, "n_max", d.Nmax);
   // Do not write the names
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, discrete_domain& d) {
   h5::group gr = fg.open_group(subgroup_name);
   int n;
   h5_read(gr, "n_max", n);
   d = discrete_domain(n);
   // NAME ARE MISSING
  }
  */

  //  BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, const unsigned int version) {
   ar& TRIQS_MAKE_NVP("n_max", Nmax);
   ar& TRIQS_MAKE_NVP("names", _names);
   ar& TRIQS_MAKE_NVP("names_inv", _inv_names);
  }
 };
}
}

