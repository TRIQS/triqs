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

namespace std { // to be found by ADL

 template <typename T> std::string get_triqs_hdf5_data_scheme(std::vector<T> const&) {
  using triqs::get_triqs_hdf5_data_scheme; // for the basic types, not found by ADL
  return "std::vector<" + get_triqs_hdf5_data_scheme(T()) + ">";
 }

}

namespace triqs {

 inline std::string get_triqs_hdf5_data_scheme(std::vector<std::string> const & ) { return "vector<string>";}

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
  void h5_write(group f, std::string const& name, std::vector<unsigned long long> const& V);

  void h5_read (group f, std::string const & name, std::vector<int>  & V);
  void h5_read (group f, std::string const & name, std::vector<double>  & V);
  void h5_read (group f, std::string const & name, std::vector<std::complex<double>> & V);
  void h5_read(group f, std::string const& name, std::vector<unsigned long long>& V);

  // vector of non basic types

 /**
   * Vector of T as a subgroup with numbers
   */
 template <typename T> void h5_write(h5::group gr, std::string name, std::vector<T> const& v) {
  auto gr2 = gr.create_group(name);
  gr2.write_triqs_hdf5_data_scheme(v);
  for (int i = 0; i < v.size(); ++i) h5_write(gr2, std::to_string(i), v[i]);
 }

 /**
   * Vector of T
   */
 template <typename T> void h5_read(h5::group gr, std::string name, std::vector<T>& v) {
  auto gr2 = gr.open_group(name);
  v.resize(gr2.get_all_dataset_names().size() + gr2.get_all_subgroup_names().size());
  for (int i = 0; i < v.size(); ++i) {
   h5_read(gr2, std::to_string(i), v[i]);
  }
 }
 }
}


