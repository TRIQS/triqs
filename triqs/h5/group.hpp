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
#include "./base.hpp"

namespace triqs {
namespace h5 {

 // using hid_t = int;

 /**
  *  \brief A local derivative of Group.
  *  Rational : use ADL for h5_read/h5_write, catch and rethrow exception, add some policy for opening/creating
  */
 class group {
  // hid_t _g_id, _parent_id;
  H5::Group _g, _parent;
  std::string _name_in_parent;
  group(H5::Group g, H5::Group parent, std::string name_in_parent);
  void _write_triqs_hdf5_data_scheme(const char *a);

  public:
  group() = default;
  group(group const &) = default;
  group(H5::Group g) : _g(g) {}
  group(H5::H5File f); /// Takes the "/" group at the top of the file
  group(std::string const &filename, int flag);
  group(hid_t id_, bool is_group);

  bool has_parent() const { return _name_in_parent != ""; }

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

  /// Open a subgroup. Throw it if does not exists
  group open_group(std::string const &key) const;

  /// Open an existing DataSet. Throw it if does not exists
  H5::DataSet open_dataset(std::string const &key) const;

  /**
   * \brief Create a subgroup.
   * \param key  The name of the subgroup
   * \param delete_if_exists  Unlink the group if it exists
   */
  group create_group(std::string const &key, bool delete_if_exists = true) const;
  
  /**
   * \brief Create a dataset.
   * \param key The name of the subgroup
   *
   * NB : It unlinks the dataset if it exists.
   */
  H5::DataSet create_dataset(std::string const &key, const H5::DataType &data_type, const H5::DataSpace &data_space,
                             const H5::DSetCreatPropList &create_plist= H5::DSetCreatPropList::DEFAULT) const;

  /// Returns all names of subgroup of key in G
  std::vector<std::string> get_all_subgroup_names(std::string const &key) const;

  /// Returns all names of subgroup of  G
  std::vector<std::string> get_all_subgroup_names() const;

  /// Returns all names of dataset of key in G
  std::vector<std::string> get_all_dataset_names(std::string const &key) const;

  /// Returns all names of dataset of G
  std::vector<std::string> get_all_dataset_names() const;

  void write_string_attribute (std::string const & obj_name, std::string const & attr_name, std::string const & value);
 };
}
}
