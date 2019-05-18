/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, I. Krivenko
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

#include "./histograms.hpp"

namespace triqs::stat {

    void histogram::_init() {
      if (a >= b) TRIQS_RUNTIME_ERROR << "histogram construction: one must have a<b";
      _step = (n_bins - 1) / (b - a);
    }

    histogram &histogram::operator<<(double x) {
      if ((x < a) || (x > b))
        ++_n_lost_pts;
      else {
        auto n = int(std::floor(((x - a) * _step) + 0.5));
        ++_data[n];
        ++_n_data_pts;
      }
      return *this;
    }

    histogram operator+(histogram h1, histogram const &h2) {
      auto l1 = h1.limits(), l2 = h2.limits();
      if (l1 != l2 || h1.size() != h2.size()) {
        TRIQS_RUNTIME_ERROR << "Histograms with different limits in addition: [" << l1.first << ";" << l1.second << "] vs [" << l2.first << ";"
                            << l2.second << "]";
      }
      h1._data += h2._data;
      h1._n_data_pts += h2._n_data_pts;
      h1._n_lost_pts += h2._n_lost_pts;
      return h1;
    }

    void h5_write(h5::group g, std::string const &name, histogram const &h) {
      h5_write(g, name, h._data);
      auto ds = g.open_dataset(name);
      h5_write_attribute(ds, "TRIQS_HDF5_data_scheme", histogram::hdf5_scheme());
      h5_write_attribute(ds, "a", h.a);
      h5_write_attribute(ds, "b", h.b);
      h5_write_attribute(ds, "n_bins", h.n_bins);
      h5_write_attribute(ds, "n_data_pts", h._n_data_pts);
      h5_write_attribute(ds, "n_lost_pts", h._n_lost_pts);
    }

    void h5_read(h5::group g, std::string const &name, histogram &h) {
      h5_read(g, name, h._data);
      auto ds = g.open_dataset(name);
      h5_read_attribute(ds, "a", h.a);
      h5_read_attribute(ds, "b", h.b);
      h5_read_attribute(ds, "n_bins", h.n_bins);
      h5_read_attribute(ds, "n_data_pts", h._n_data_pts);
      h5_read_attribute(ds, "n_lost_pts", h._n_lost_pts);
      h._init();
    }

    std::ostream &operator<<(std::ostream &os, histogram const &h) {
      auto normed     = pdf(h);
      auto integrated = cdf(h);
      for (int i = 0; i < h.size(); ++i)
        os << h.mesh_point(i) << "  " << h.data()[i] << "  " << normed.data()[i] << "  " << integrated.data()[i] << std::endl;
      return os;
    }

} // namespace triqs::stat
