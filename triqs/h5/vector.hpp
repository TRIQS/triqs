/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#include "./group.hpp"
#include "./string.hpp"
#include <vector>
#include <complex>

namespace triqs {

 inline std::string get_triqs_hdf5_data_scheme(std::vector<std::string> const & ) { return "vector<string>";}

 template <typename T> std::string get_triqs_hdf5_data_scheme(std::vector<T> const&) {
  using triqs::get_triqs_hdf5_data_scheme; // for the basic types, not found by ADL
  return "std::vector<" + get_triqs_hdf5_data_scheme(T()) + ">";
 }

 namespace h5 {

  void h5_write (group f, std::string const & name, std::vector<std::string> const & V);
  void h5_read (group f, std::string const & name, std::vector<std::string> & V);

  void h5_write_attribute (hid_t ob, std::string const & name, std::vector<std::vector<std::string>> const & V);
  void h5_read_attribute (hid_t ob, std::string const & name, std::vector<std::vector<std::string>> & V);

  //void h5_write_attribute (hid_t ob, std::string const & name, std::vector<std::string> const & V);
  //void h5_read_attribute (hid_t ob, std::string const & name, std::vector<std::string> & V);

  void h5_write (group f, std::string const & name, std::vector<int> const & V);
  void h5_write (group f, std::string const & name, std::vector<double> const & V);
  void h5_write (group f, std::string const & name, std::vector<std::complex<double>> const & V);

  void h5_read (group f, std::string const & name, std::vector<int>  & V);
  void h5_read (group f, std::string const & name, std::vector<double>  & V);
  void h5_read (group f, std::string const & name, std::vector<std::complex<double>> & V);

 }
}


