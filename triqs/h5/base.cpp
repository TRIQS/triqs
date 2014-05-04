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
#include "./base.hpp"
#include <hdf5_hl.h>

namespace triqs {

namespace h5 {

 // dataspace from lengths and strides. Correct for the complex. strides must be >0
 H5::DataSpace dataspace_from_LS(int R, bool is_complex, hsize_t const *Ltot, hsize_t const *L, hsize_t const *S,
                                 hsize_t const *offset) {
  int rank = R + (is_complex ? 1 : 0);
  hsize_t totdimsf[rank], dimsf[rank], stridesf[rank], offsetf[rank]; // dataset dimensions
  for (size_t u = 0; u < R; ++u) {
   offsetf[u] = (offset ? offset[u] : 0);
   dimsf[u] = L[u];
   totdimsf[u] = Ltot[u];
   stridesf[u] = S[u];
  }
  if (is_complex) {
   offsetf[rank - 1] = 0;
   dimsf[rank - 1] = 2;
   totdimsf[rank - 1] = 2;
   stridesf[rank - 1] = 1;
  }
  H5::DataSpace ds(rank, totdimsf);
  ds.selectHyperslab(H5S_SELECT_SET, dimsf, offsetf, stridesf);
  return ds;
 }

 /****************** Write string attribute *********************************************/

 void write_string_attribute(H5::H5Object const *obj, std::string name, std::string value) {
  H5::DataSpace attr_dataspace = H5::DataSpace(H5S_SCALAR);
  // Create new string datatype for attribute
  H5::StrType strdatatype(H5::PredType::C_S1, value.size());
  // Set up write buffer for attribute
  // const H5std_string strwritebuf (value);
  // Create attribute and write to it
  H5::Attribute myatt_in = obj->createAttribute(name.c_str(), strdatatype, attr_dataspace);
  // myatt_in.write(strdatatype, strwritebuf);
  myatt_in.write(strdatatype, (void *)(value.c_str()));
 }


 /****************** Read string attribute *********************************************/

 /// Return the attribute name of obj, and "" if the attribute does not exist.
 std::string read_string_attribute(H5::H5Object const *obj, std::string name) {
  std::string value = "";
  H5::Attribute attr;
  if (H5LTfind_attribute(obj->getId(), name.c_str()) == 0) return value; // not present
  // can not find how to get the size with hl. Using full interface
  // herr_t err2 =  H5LTget_attribute_string(gr.getId(), x.c_str(), name.c_str() , &(buf.front())  ) ;
  // value.append( &(buf.front()) );
  try {
   attr = obj->openAttribute(name.c_str());
  }
  catch (H5::AttributeIException) {
   return value;
  }
  try {
   H5::DataSpace dataspace = attr.getSpace();
   int rank = dataspace.getSimpleExtentNdims();
   if (rank != 0) TRIQS_RUNTIME_ERROR << "Reading a string attribute and got rank !=0";
   size_t size = attr.getStorageSize();
   H5::StrType strdatatype(H5::PredType::C_S1, size + 1);
   std::vector<char> buf(size + 1, 0x00);
   attr.read(strdatatype, (void *)(&buf[0]));
   value.append(&(buf.front()));
  }
  TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
  return value;
 }
}
}

