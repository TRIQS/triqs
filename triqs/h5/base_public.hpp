/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
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
#include <triqs/utility/mini_vector.hpp>
#include <type_traits>
#include<H5Ipublic.h>
#include<H5Fpublic.h>
#include<H5Gpublic.h>
#include<H5Ppublic.h>
#include<complex>

namespace triqs {

inline std::string get_triqs_hdf5_data_scheme(bool) { return "bool"; }
inline std::string get_triqs_hdf5_data_scheme(int) { return "int"; }
inline std::string get_triqs_hdf5_data_scheme(long) { return "long"; }
inline std::string get_triqs_hdf5_data_scheme(long long) { return "long long"; }
inline std::string get_triqs_hdf5_data_scheme(unsigned int) { return "int"; }
inline std::string get_triqs_hdf5_data_scheme(unsigned long) { return "unsigned long"; }
inline std::string get_triqs_hdf5_data_scheme(unsigned long long) { return "unsigned long long"; }
inline std::string get_triqs_hdf5_data_scheme(float) { return "float"; }
inline std::string get_triqs_hdf5_data_scheme(double) { return "double"; }
inline std::string get_triqs_hdf5_data_scheme(long double) { return "long double"; }
inline std::string get_triqs_hdf5_data_scheme(std::complex<double>) { return "complex"; }

namespace h5 {

 using utility::mini_vector;

 //------------- general hdf5 object ------------------
 // HDF5 uses a reference counting system, similar to python.
 // Same as pyref in python wrapper, its handle the ref. counting of the hdf5 object
 // using a RAII pattern.
 // We are going to store the id of the various h5 object in such a wrapper
 // to provide clean decref, and exception safety.
 class h5_object {

  // xdecref, xincref manipulate the the ref, but ignore invalid (incl. 0) id.
  //  like XINC_REF and XDEC_REF in python
  static void xdecref(hid_t id) {
   if (H5Iis_valid(id)) H5Idec_ref(id);
  }
  static void xincref(hid_t id) {
   if (H5Iis_valid(id)) H5Iinc_ref(id);
  }

  protected:
  hid_t id;

  public:

  // make an h5_object when the id is now owned (simply inc. the ref).
  static h5_object from_borrowed(hid_t id) {
   h5_object::xincref(id);
   return h5_object(id);
  }

  // constructor from an owned id (or 0). It will NOT incref, it takes ownership
  h5_object(hid_t id = 0) : id(id) {}

  h5_object(h5_object const& x) : id(x.id) { xincref(id); } // a new copy, a new ref.

  h5_object(h5_object&& x) : id(x.id) { x.id = 0; } // steal the ref.

  h5_object& operator=(h5_object const& x) { return operator=(h5_object(x)); } //rewriting with the next overload

  h5_object& operator=(h5_object&& x) { //steals the ref, after properly decref its own.
   xdecref(id);
   id = x.id;
   x.id = 0;
   return *this;
  }

  ~h5_object() {
   // debug : to check the ref counting. Ok in tests.
   //if (H5Iis_valid(id)) std::cerr << "closing h5 object id = " << id << " # ref = "<< H5Iget_ref(id) << std::endl;
   xdecref(id);
  }

  void close() { xdecref(id); id=0;} // e.g. to close a file explicitely.

  /// cast operator to use it in the C function as its id
  operator hid_t() const { return id; }

  bool is_valid() const { return H5Iis_valid(id)==1; }
 };

 //------------- dataset ------------------

 using dataset = h5_object;

 //------------- datatype  ------------------

 using datatype = h5_object;

 //------------- dataspace ------------------

 using dataspace = h5_object;

 //------------- proplist ------------------

 using proplist = h5_object;

 //------------- attribute ------------------

 using attribute = h5_object;

 /****************** Read/Write string attribute *********************************************/

 /// Write an attribute named name, of type string, of value value to the object id
 void write_string_attribute(hid_t id, std::string name, std::string value);

 /// Returns the attribute name of obj, and "" if the attribute does not exist.
 std::string read_string_attribute(hid_t id, std::string name);
}
}

