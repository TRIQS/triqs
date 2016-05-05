#include "./gf_indices.hpp"

namespace triqs {
namespace gfs {

 void h5_write(h5::group fg, std::string subgroup_name, gf_indices const &g) {
  if (g.empty()) return;
  auto gr = fg.create_group(subgroup_name);
  if (g.rank() == 2) { // special case for backward compat.
   h5_write(gr, "left", g._data[0]);
   h5_write(gr, "right", g._data[1]);
  } else
   for (int i = 0; i < g.rank(); i++) h5_write(gr, "r" + std::to_string(i), g._data[i]);
 }

 //------------------------------------------------
 
 void h5_read(h5::group fg, std::string subgroup_name, gf_indices &g) {
  if (!fg.has_key(subgroup_name)) {
   g = gf_indices{}; // empty, no file
   return;
  }
  auto gr = fg.open_group(subgroup_name);
  g._data.clear();
  auto read = [&](std::string const &x) {
   auto v = h5::h5_read<std::vector<std::string>>(gr, x);
   g._data.emplace_back(v);
  };
  if (gr.has_key("left")) {
   read("left");
   read("right");
  } else {
   for (int i = 0; i < 10; i++) {
    auto k = "r" + std::to_string(i);
    if (!gr.has_key(k)) break;
    read(k);
   }
  }
 }
}
}
