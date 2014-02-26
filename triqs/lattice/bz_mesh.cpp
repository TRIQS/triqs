/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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
#include "./bz_mesh.hpp"
#include "./grid_generator.hpp"
namespace triqs {
namespace lattice {

 bz_mesh::bz_mesh(brillouin_zone const &bz, int n_l) : bz(bz) {
  // compute the k points
  for (auto grid = grid_generator{bz.lattice().dim(), n_l}; grid; ++grid) k_pt_stack.push_back(*grid);
 }

 // locate the closest point : VERY PRIMITIVE : TO BE IMPROVED (kd-tree ?)
 long bz_mesh::locate_neighbours(k_t k) const {
  // reduction to the BZ ?
  // very slow : improve with a kd-tree ?
  double d_min = 100; // > pi
  auto sqr = [](double x) { return x * x; };
  int pos = 0, pos_min = 0;
  for (auto const &p : k_pt_stack) {
   int dim = k.size();
   double d = 0;
   for (int i = 0; i < dim; ++i) d += sqr(k(i) - p(i));
   if (d < d_min) {
    d_min = d;
    pos_min = pos;
   }
   pos++;
  }
  return pos_min;
 }

 /// Write into HDF5
 void h5_write(h5::group fg, std::string subgroup_name, bz_mesh const &m) {
  h5::group gr = fg.create_group(subgroup_name);
  h5_write(gr, "domain", m.domain());
  auto kp = matrix<double>(m.k_pt_stack.size(), 3);
  kp() = 0;
  for (int i = 0; i < m.k_pt_stack.size(); ++i) kp(i, range{}) = m.k_pt_stack[i];
  h5_write(gr, "k_pt_stack", kp);
 }

 /// Read from HDF5
 void h5_read(h5::group fg, std::string subgroup_name, bz_mesh &m) {
  h5::group gr = fg.open_group(subgroup_name);
  brillouin_zone dom;
  h5_read(gr, "domain", dom);
  matrix<double> kp;
  h5_read(gr, "k_pt_stack", kp);
  std::vector<k_t> k_pt_stack;
  int s = first_dim(kp);
  for (int i = 0; i < s; ++i) k_pt_stack[i] = kp(i, range{});
  m = bz_mesh(std::move(dom), k_pt_stack);
 }
}
} // namespaces

