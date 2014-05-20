/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
//#include <hdf5.h>
#include <triqs/h5.hpp>
#include <triqs/utility/view_tools.hpp>

//#define CHECK_OR_THROW(Cond, Mess)
#define CHECK_OR_THROW(Cond, Mess)                                                                                               \
 if (!Cond) TRIQS_RUNTIME_ERROR << "Error in h5 (de)serialization " << Mess;

namespace triqs {
namespace h5 {

 template <typename T> std::string serialize(T const &x) {

  hid_t fapl_id = H5Pcreate(H5P_FILE_ACCESS);
  CHECK_OR_THROW((fapl_id >= 0), "creating fapl");

  auto err = H5Pset_fapl_core(fapl_id, (size_t)(64 * 1024), false);
  CHECK_OR_THROW((err >= 0), "setting core file driver in fapl.");

  hid_t file_id = H5Fcreate("serialization", 0, H5P_DEFAULT, fapl_id);
  CHECK_OR_THROW((file_id >= 0), "created core file");

  auto gr = triqs::h5::group(file_id, false);
  h5_write(gr, "object", x);

  err = H5Fflush(file_id, H5F_SCOPE_GLOBAL);
  CHECK_OR_THROW((err >= 0), "flushed core file.");

  ssize_t image_len = H5Fget_file_image(file_id, NULL, (size_t)0);
  CHECK_OR_THROW((image_len > 0), "got image file size");

  std::string buf(image_len, 0);

  ssize_t bytes_read = H5Fget_file_image(file_id, (void *)&buf[0], (size_t)image_len);
  CHECK_OR_THROW(bytes_read == image_len, "wrote file into image buffer");

  err = H5Fclose(file_id);
  CHECK_OR_THROW((err >= 0), "closed core file(1).");

  err = H5Pclose(fapl_id);
  CHECK_OR_THROW((err >= 0), "closed fapl(1).");

  return buf;
 }

 template <typename T> T deserialize(std::string const &buf) {
  T res;

  hid_t fapl_id = H5Pcreate(H5P_FILE_ACCESS);
  CHECK_OR_THROW((fapl_id >= 0), "creating fapl");

  auto err = H5Pset_fapl_core(fapl_id, (size_t)(64 * 1024), false);
  CHECK_OR_THROW((err >= 0), "setting core file driver in fapl.");

  err = H5Pset_file_image(fapl_id, (void *)&buf[0], buf.size());
  CHECK_OR_THROW((err >= 0), "set file image in fapl.");

  hid_t file_id = H5Fopen("serialization", H5F_ACC_RDONLY, fapl_id);
  CHECK_OR_THROW((file_id >= 0), "opened received file image file");

  auto gr = triqs::h5::group(file_id, false);
  h5_read(gr, "object", res);

  err = H5Fclose(file_id);
  CHECK_OR_THROW((err >= 0), "closed core file(1).");

  err = H5Pclose(fapl_id);
  CHECK_OR_THROW((err >= 0), "closed fapl(1).");

  return res;
 }
}

#undef CHECK_OR_THROW
}

