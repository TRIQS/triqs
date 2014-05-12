#include "./opaque_object_h5.hpp"

namespace triqs { namespace utility {

 std::map<size_t, std::function<_object(std::string const &)>>                     _object::code_to_deserialization_fnts;
 std::map<size_t, std::function<_object(h5::group const &, std::string const &)>>  _object::code_to_h5_read_fnts;
 std::map<hid_t, size_t >                                                          _object::h5_type_to_c_equivalence;
 std::map<std::pair<size_t,int>, size_t >                                          _object::code_element_rank_to_code_array;
 std::map<std::string, size_t >                                                    _object::h5_scheme_to_code;
 std::map<size_t, std::string >                                                    _object::type_code_to_type_name; 
 std::map<std::string, size_t >                                                    _object::type_name_to_type_code; 
 bool _object::initialized = false;

 //-----------------------------------------------------------------------

 void _object::_init() {
  if (initialized) return;
#define REGISTER_UNSERIALIZE_FUN(r, data, T) _object::register_native_type<T>();
  BOOST_PP_SEQ_FOR_EACH(REGISTER_UNSERIALIZE_FUN, nil , TRIQS_UTIL_OPAQUE_OBJECT_PREDEFINED_CAST);
#undef REGISTER_UNSERIALIZE_FUN
  initialized = true;
 }

 //-----------------------------------------------------------------------

 void h5_read ( h5::group g, std::string const & name, _object & obj){
  //std::cerr  << " read object "<< name << std::endl ;
  using namespace H5;
  if (!g.has_key(name)) TRIQS_RUNTIME_ERROR << "no such "<<name <<" in file ";
  size_t type_hash;
  // First if it is not a subgroup, it is a scalar or a an array
  // TO BE CHECKED
  try {
   H5::DataSet ds = g.open_dataset( name.c_str() );
   hid_t dtype = H5Dget_type(ds.getId());
   hid_t native_type=H5Tget_native_type(dtype, H5T_DIR_DEFAULT);
   if (H5Tget_class(dtype) == H5T_STRING ) { // it is a string
    auto triqs_data_scheme = h5::read_string_attribute(&ds,"TRIQS_HDF5_data_scheme");
    type_hash = (triqs_data_scheme == "") ?  _object::type_code<std::string>() : _object::h5_scheme_to_code[triqs_data_scheme];
   }
   else if ((H5Tget_class(dtype) == H5T_INTEGER ) || ((H5Tget_class(dtype) == H5T_FLOAT ) )) {
    int rank = ds.getSpace().getSimpleExtentNdims();
    auto it = _object::h5_type_to_c_equivalence.begin();
    for (;it != _object::h5_type_to_c_equivalence.end();++it) { if (H5Tequal(native_type, it->first))  break;}
    if (it == _object::h5_type_to_c_equivalence.end()) TRIQS_RUNTIME_ERROR << " h5_type_to_c_equivalence : type not found";
    type_hash = it->second;
    if (rank>0) {
     size_t type_hash_element = type_hash;
     auto it2= _object::code_element_rank_to_code_array.find(std::make_pair(type_hash_element,rank));
     if (it2 == _object::code_element_rank_to_code_array.end())
      TRIQS_RUNTIME_ERROR << " code_element_rank_to_code_array : type not found : " << name << "  " << type_hash_element << " "
                          << rank;
     type_hash = it2->second;
    }
   }
   //else if (H5Tget_class(dtype)==H5T_ARRAY)
   herr_t status = H5Tclose (native_type);
   status = H5Tclose (dtype);
  }
  TRIQS_ARRAYS_H5_CATCH_EXCEPTION;

  obj = _object::code_to_h5_read_fnts[type_hash](g,name);
 }
}}

