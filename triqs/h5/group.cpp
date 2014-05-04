#include "./group.hpp"
#include <hdf5_hl.h>

namespace triqs {
namespace h5 {

 group::group(H5::Group g, H5::Group parent, std::string name_in_parent)
    : _g(g), _parent(parent), _name_in_parent(name_in_parent) {}

 //group::group(H5::Group g) : _g(g) {}

 group::group(H5::H5File f) : _g(f.openGroup("/")) {} // can not fail, right ?

 group::group(std::string const &filename, int flag) {
  H5::H5File f(filename, H5F_ACC_TRUNC);
  *this = group(f);
 }

 group::group(hid_t id_, bool is_group) {
  if (is_group) {
   _g.setId(id_);
  } else {
   H5::H5File f;
   f.setId(id_);
   *this = group(f);
  }
 }

 void group::_write_triqs_hdf5_data_scheme(const char *a) { h5::write_string_attribute(&_g, "TRIQS_HDF5_data_scheme", a); }

 std::string group::read_triqs_hdf5_data_scheme() const { return read_string_attribute(&_g, "TRIQS_HDF5_data_scheme"); }

 bool group::has_key(std::string const &key) const { return (H5Lexists(_g.getId(), key.c_str(), H5P_DEFAULT)); }

 void group::unlink_key_if_exists(std::string const &key) const {
  if (this->has_key(key)) _g.unlink(key.c_str());
 }

 group group::open_group(std::string const &key) const {
  if (!has_key(key)) TRIQS_RUNTIME_ERROR << "no subgroup " << key << " in the group";
  group res;
  try {
   res = group(_g.openGroup(key.c_str()), _g, key);
  } // has a parent
  catch (H5::GroupIException const &e) {
   TRIQS_RUNTIME_ERROR << "Error in opening the subgroup " << key << "\n H5 error message : \n " << e.getCDetailMsg();
  }
  return res;
 }


 /// Open an existing DataSet. Throw it if does not exists
 H5::DataSet group::open_dataset(std::string const &key) const {
  if (!has_key(key)) TRIQS_RUNTIME_ERROR << "no dataset " << key << " in the group";
  H5::DataSet res;
  try {
   res = _g.openDataSet(key.c_str());
  }
  catch (H5::GroupIException const &e) {
   TRIQS_RUNTIME_ERROR << "Error in opening the dataset " << key << "\n H5 error message : \n " << e.getCDetailMsg();
  }
  return res;
 }
 /**
  * \brief Create a subgroup.
  * \param key  The name of the subgroup
  * \param delete_if_exists  Unlink the group if it exists
  */
 group group::create_group(std::string const &key, bool delete_if_exists) const {
  unlink_key_if_exists(key);
  return group(_g.createGroup(key.c_str()), _g, key);
 }
 /**
  * \brief Create a dataset.
  * \param key The name of the subgroup
  * \param args  Other parameters are forwarded to H5::Group
  *
  * NB : It unlinks the dataset if it exists.
  */

 H5::DataSet group::create_dataset(std::string const &key, const H5::DataType &data_type, const H5::DataSpace &data_space,
                                   const H5::DSetCreatPropList &create_plist) const {
  unlink_key_if_exists(key);
  H5::DataSet res;
  try {
   res = _g.createDataSet(key.c_str(), data_type, data_space, create_plist);
  }
  catch (H5::GroupIException const &e) {
   TRIQS_RUNTIME_ERROR << "Error in creating the dataset " << key << "\n H5 error message : \n " << e.getCDetailMsg();
  }
  return res;
 }

 void group::write_string_attribute (std::string const & obj_name, std::string const & attr_name, std::string const & value){
  herr_t err =  H5LTset_attribute_string(_g.getId(),obj_name.c_str(),attr_name.c_str(), value.c_str() ) ;
  if (err<0) TRIQS_RUNTIME_ERROR << "Error in setting attribute of "<< obj_name<<" named "<< attr_name << " to " << value;
 }

 //-----------------------------------------------------------------------

 extern "C" {
 herr_t get_group_elements_name_ds(hid_t loc_id, const char *name, void *opdata) {
  H5O_info_t object_info;
  herr_t err = H5Oget_info_by_name(loc_id, name, &object_info, H5P_DEFAULT);
  if (err < 0) TRIQS_RUNTIME_ERROR << "get_group_elements_name_ds internal";
  if (object_info.type == H5O_TYPE_DATASET) static_cast<std::vector<std::string> *>(opdata)->push_back(name);
  return 0;
 }
 herr_t get_group_elements_name_grp(hid_t loc_id, const char *name, void *opdata) {
  H5O_info_t object_info;
  herr_t err = H5Oget_info_by_name(loc_id, name, &object_info, H5P_DEFAULT);
  if (err < 0) TRIQS_RUNTIME_ERROR << "get_group_elements_name_grp internal";
  if (object_info.type == H5O_TYPE_GROUP) static_cast<std::vector<std::string> *>(opdata)->push_back(name);
  return 0;
 }
 }
 //-----------------------------------------------------------------------

 std::vector<std::string> group::get_all_subgroup_names(std::string const &key) const {
  std::vector<std::string> grp_name;
  H5::Group F(_g);
  F.iterateElems(key.c_str(), NULL, get_group_elements_name_grp, static_cast<void *>(&grp_name));
  return grp_name;
 }

 std::vector<std::string> group::get_all_dataset_names(std::string const &key) const {
  std::vector<std::string> ds_name;
  H5::Group F(_g);
  F.iterateElems(key.c_str(), NULL, get_group_elements_name_ds, static_cast<void *>(&ds_name));
  return ds_name;
 }

 std::vector<std::string> group::get_all_subgroup_names() const {
  if (!has_parent()) TRIQS_RUNTIME_ERROR << "Group hdf5 : parent not found";
  return group(_parent).get_all_subgroup_names(_name_in_parent);
 }

 std::vector<std::string> group::get_all_dataset_names() const {
  if (!has_parent()) TRIQS_RUNTIME_ERROR << "Group hdf5 : parent not found";
  return group(_parent).get_all_dataset_names(_name_in_parent);
 }
}
}

