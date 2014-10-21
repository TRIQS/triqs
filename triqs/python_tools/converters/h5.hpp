#pragma once
#include "../wrapper_tools.hpp"
#include <triqs/h5.hpp>

namespace triqs { namespace py_tools {

 // ---  h5 group of h5py into a triqs::h5 group
template <> struct py_converter<triqs::h5::group> {

 static PyObject *c2py(std::string const &x) =delete;

 static pyref group_type;

 //-------

 static triqs::h5::group py2c (PyObject * ob) {
  int id = PyInt_AsLong(borrowed(ob).attr("id").attr("id"));
  // id can be a file or a group. If it is a file, we open its root group '/'
  if (H5Iget_type(id) == H5I_FILE) {
   id = H5Gopen2(id, "/", H5P_DEFAULT);
   if (id < 0) TRIQS_RUNTIME_ERROR << "Internal error : Cannot open the root group / in the file !"; // should never happen
  }
  else { // must be a group, because check in convertible
   H5Iinc_ref(id); // we did not have ownership of the id
  }
  return triqs::h5::group (id);
 }

 //-------

#define RAISE(MESS)                                                                                                              \
 {                                                                                                                               \
  if (raise_exception) PyErr_SetString(PyExc_TypeError, MESS);                                                                   \
  return false;                                                                                                                  \
 }

 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (group_type.is_null()) {
   group_type = pyref::module("h5py").attr("highlevel").attr("Group");
   if (PyErr_Occurred()) TRIQS_RUNTIME_ERROR << "Cannot load h5py module and find the group in it";
  }
  int cmp = PyObject_RichCompareBool((PyObject *)ob->ob_type, group_type, Py_EQ);
  if (cmp < 0) RAISE("hd5 : internal : comparison to group type has failed !!");
  pyref id_py = borrowed(ob).attr("id").attr("id");
  if ((!id_py) || (!PyInt_Check((PyObject *)id_py))) RAISE("hd5 : INTERNAL : group id.id is not an int !!");
  int id = PyInt_AsLong(borrowed(ob).attr("id").attr("id"));
  if (!H5Iis_valid(id)) RAISE("Internal error : invalid id from h5py !");
  if (!((H5Iget_type(id) == H5I_FILE) || (H5Iget_type(id) == H5I_GROUP)))
   RAISE("h5py object is neither an hdf5 group or an hdf5 file");
  return true;
 }
 };
#undef RAISE

}}



