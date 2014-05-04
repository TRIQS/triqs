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
#include "./string.hpp"
namespace triqs { 
 
 namespace h5 {

 void h5_write (group g, std::string const & name, std::string const & value) {
  try {
   H5::StrType strdatatype(H5::PredType::C_S1, value.size());
   H5::DataSet ds = g.create_dataset(name, strdatatype, H5::DataSpace());
   ds.write((void*)(value.c_str()), strdatatype );
  }
  TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
 }

 void h5_read (group g, std::string const & name, std::string & value) {
  try {
   H5::DataSet ds = g.open_dataset(name);
   H5::DataSpace dataspace = ds.getSpace();
   int rank = dataspace.getSimpleExtentNdims();
   if (rank != 0) TRIQS_RUNTIME_ERROR << "Reading a string and got rank !=0";
   size_t size = ds.getStorageSize();
   H5::StrType strdatatype(H5::PredType::C_S1, size);
   std::vector<char> buf(size+1, 0x00);
   ds.read( (void *)(&buf[0]), strdatatype, H5::DataSpace(), H5::DataSpace() );
   value = ""; value.append( &(buf.front()) );
  }
  TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
 }

}}

