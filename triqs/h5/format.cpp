#include "./format.hpp"
#include "./stl/string.hpp"
#include <string>

namespace triqs::h5 {

  // Code left
  std::string read_hdf5_format(group g) {
    std::string s;
    h5_read_attribute(g, "TRIQS_HDF5_data_scheme", s);
    return s;
  }

  void assert_hdf5_format_as_string(group g, const char *tag_expected, bool ignore_if_absent) {
    auto tag_file = read_hdf5_format(g);
    if (ignore_if_absent and tag_file.empty()) return;
    if (tag_file != tag_expected)
      throw std::runtime_error("h5_read : mismatch of the tag TRIQS_HDF5_data_format tag in the h5 group : found " + tag_file + " while I expected "
                               + tag_expected);
  }

} // namespace triqs::h5
