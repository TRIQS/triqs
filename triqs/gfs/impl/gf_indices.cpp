#include "./gf_indices.hpp"


namespace triqs {
namespace gfs {
  template<>
  void h5_write(h5::group fg, std::string subgroup_name, gf_indices_tuple<2> const &g) {
   if (g.is_empty()) return;
   auto gr = fg.create_group(subgroup_name);
   h5_write(gr, "left", g.ind_vec[0].ind);
   h5_write(gr, "right", g.ind_vec[1].ind);
  }

  template<>
  void h5_read(h5::group fg, std::string subgroup_name, gf_indices_tuple<2> &g) {
   h5::group gr = fg; // no default construction
   try {
    gr = fg.open_group(subgroup_name);
   }
   catch (...) {
    g = gf_indices_tuple<2>{}; // empty, no file
    return;
   }
   h5_read(gr, "left", g.ind_vec[0].ind);
   h5_read(gr, "right", g.ind_vec[1].ind);
  }
}}
