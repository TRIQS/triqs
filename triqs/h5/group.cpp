#include "./group.hpp"

namespace triqs { namespace h5 {

 extern "C" {
  herr_t get_group_elements_name_ds(hid_t loc_id, const char *name, void *opdata) {
   H5O_info_t object_info; herr_t err =H5Oget_info_by_name(loc_id,name,&object_info,H5P_DEFAULT );
   if (err<0) TRIQS_RUNTIME_ERROR << "get_group_elements_name_ds internal";
   if (object_info.type == H5O_TYPE_DATASET)  static_cast<std::vector<std::string> *>(opdata)->push_back(name);
   return 0;
  }
  herr_t get_group_elements_name_grp(hid_t loc_id, const char *name, void *opdata) {
   H5O_info_t object_info; herr_t err =H5Oget_info_by_name(loc_id,name,&object_info,H5P_DEFAULT );
   if (err<0) TRIQS_RUNTIME_ERROR << "get_group_elements_name_grp internal";
   if (object_info.type == H5O_TYPE_GROUP)  static_cast<std::vector<std::string> *>(opdata)->push_back(name);
   return 0;
  }
 }
 //-----------------------------------------------------------------------

 std::vector<std::string> group::get_all_subgroup_names(std::string const & key) const { 
  std::vector<std::string> grp_name;
  H5::Group F(_g);
  F.iterateElems(key.c_str(), NULL, get_group_elements_name_grp, static_cast<void*>(&grp_name));
  return grp_name;
 }

 std::vector<std::string> group::get_all_dataset_names(std::string const & key) const { 
  std::vector<std::string> ds_name;
  H5::Group F(_g);
  F.iterateElems(key.c_str(), NULL, get_group_elements_name_ds, static_cast<void*>(&ds_name));
  return ds_name;
 }

}}

