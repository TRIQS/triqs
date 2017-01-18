/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2013 by O. Parcollet
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
#include "./file.hpp"

namespace triqs {
namespace h5 {

 /**
  *  \brief A local derivative of Group.
  *  Rationale : use ADL for h5_read/h5_write, catch and rethrow exception, add some policy for opening/creating
  */
 class group : public h5_object {
  void _write_triqs_hdf5_data_scheme(const char *a); // impl.

  public:
  group() = default; // for python converter only

  ///
  group(group const &) = default;

  /// Takes the "/" group at the top of the file
  group(h5::file f);

  /**
   * Takes ownership of the id [expert only]
   * id can be :
   *  - a file : in this case, make a group on /
   *  - a group : in this case, take the id of the group. DOES NOT take ownership of the ref
   */
  group(hid_t id_);

  // [expert only]. If not present, the obj is casted to hid_t and there is a ref. leak
  group(h5_object obj);

  /// Name of the group
  std::string name() const;

  ///  Write the triqs tag of the group if it is an object.
  template <typename T> void write_triqs_hdf5_data_scheme(T const &obj) {
   _write_triqs_hdf5_data_scheme(get_triqs_hdf5_data_scheme(obj).c_str());
  }

  /// Read the triqs tag of the group if it is an object. Returns"" if attribute is not present
  std::string read_triqs_hdf5_data_scheme() const;

  ///
  bool has_key(std::string const &key) const;

  ///
  void unlink_key_if_exists(std::string const &key) const;

 /**
   * \brief Open a subgroup.
   * \param key  The name of the subgroup. If empty, return this group.
   * 
   * Throws if it does not exist.
   */
  group open_group(std::string const &key) const;

  /// Open an existing DataSet. Throw if it does not exist.
  dataset open_dataset(std::string const &key) const;

  /**
   * \brief Create a subgroup.
   * \param key  The name of the subgroup. If empty, return this group.
   * \param delete_if_exists  Unlink the group if it exists
   */
  group create_group(std::string const &key, bool delete_if_exists = true) const;

  /**
   * \brief Create a dataset.
   * \param key The name of the subgroup
   *
   * NB : It unlinks the dataset if it exists.
   */
  dataset create_dataset(std::string const &key, datatype ty, dataspace sp, hid_t pl = H5P_DEFAULT) const;

  /// Returns all names of subgroup of  G
  std::vector<std::string> get_all_subgroup_names() const;

  /// Returns all names of dataset of G
  std::vector<std::string> get_all_dataset_names() const;
 };
}
}
