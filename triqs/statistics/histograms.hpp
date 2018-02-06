/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-15 by O. Parcollet, I. Krivenko
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
#include <triqs/h5.hpp>
#include <triqs/arrays.hpp>
#include <ostream>

namespace triqs {
namespace statistics {

 /**
  Histogram
  */
 class histogram {

  double a, b;                    // start and end of mesh
  long n_bins;                    // number of points on the mesh (for double only)
  uint64_t _n_data_pts = 0;       // number of data points
  uint64_t _n_lost_pts = 0;       // number of discarded points
  arrays::vector<double> _data;   // histogram data
  double _step;                   // number of bins per unit length

  void _init() {
   if (a >= b) TRIQS_RUNTIME_ERROR << "histogram construction: one must have a<b";
   _step = (n_bins - 1) / (b - a);
  }

  inline friend histogram pdf(histogram const& h); // probability distribution function = normalised histogram
  inline friend histogram cdf(histogram const& h); // cumulative distribution function = normalised histogram integrated

  public:
  /// Constructor with mesh of integer values
  histogram(int a, int b) : a(a), b(b), n_bins(b - a + 1), _data(n_bins, 0) { _init(); }

  /// Constructor with mesh of double values
  histogram(double a, double b, long n_bins) : a(a), b(b), n_bins(n_bins) ,_data(n_bins, 0){ _init(); }

  /// Default constructor
  histogram() = default;

  // Copy, move constructors & assignment
//  histogram(histogram const& h) = default;
//  histogram(histogram&& h) = default;
//  histogram& operator=(histogram&&) = default;
//  histogram& operator=(histogram const&) = default;

  /// Bins a double into the histogram
  histogram& operator<<(double x) {
   if ((x < a) || (x > b))
    ++_n_lost_pts;
   else {
    auto n = int(std::floor(((x - a) * _step) + 0.5));
    ++_data[n];
    ++_n_data_pts;
   }
   return *this;
  }

  /// Point on the mesh
  double mesh_point(int n) const { return a + n / _step; }

  /// Number of bins
  size_t size() const { return _data.size(); }

  /// Returns boundaries of the histogram
  std::pair<double, double> limits() const { return {a, b}; }

  /// Read-only access to the data storage
  arrays::vector<double> const& data() const { return _data; }

  /// Norm of the stored data
  uint64_t n_data_pts() const { return _n_data_pts; }

  /// Number of discarded points
  uint64_t n_lost_pts() const { return _n_lost_pts; }

  /// Addition of histograms
  friend histogram operator+(histogram h1, histogram const& h2) {
   auto l1 = h1.limits(), l2 = h2.limits();
   if (l1 != l2 || h1.size() != h2.size()) {
    TRIQS_RUNTIME_ERROR << "Histograms with different limits in addition: [" << l1.first << ";" << l1.second << "] vs ["
                        << l2.first << ";" << l2.second << "]";
   }
   h1._data += h2._data;
   h1._n_data_pts += h2._n_data_pts;
   h1._n_lost_pts += h2._n_lost_pts;
   return h1;
  }

  /// Reset all values to 0
  void clear() {
   _n_lost_pts = 0;
   _n_data_pts = 0;
   _data() = 0.0;
  }

  /// Reduce the histogram from all nodes
  friend histogram mpi_reduce(histogram const& h, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
   assert(op == MPI_SUM);
   auto h2 = h;
   h2._data = mpi_reduce(h._data, c, root, all);
   h2._n_data_pts = mpi_reduce(h._n_data_pts, c, root, all);
   h2._n_lost_pts = mpi_reduce(h._n_lost_pts, c, root, all);
   return h2;
  }

  /// HDF5 interface

  static std::string hdf5_scheme() {return  "Histogram";}

  friend void h5_write(h5::group g, std::string const& name, histogram const& h) {
   h5_write(g, name, h._data);
   auto ds = g.open_dataset(name);
   h5_write_attribute(ds, "TRIQS_HDF5_data_scheme", histogram::hdf5_scheme());
   h5_write_attribute(ds, "a", h.a);
   h5_write_attribute(ds, "b", h.b);
   h5_write_attribute(ds, "n_bins", h.n_bins);
   h5_write_attribute(ds, "n_data_pts", h._n_data_pts);
   h5_write_attribute(ds, "n_lost_pts", h._n_lost_pts);
  }

  friend void h5_read(h5::group g, std::string const& name, histogram& h) {
   h5_read(g, name, h._data);
   auto ds = g.open_dataset(name);
   h5_read_attribute(ds, "a", h.a);
   h5_read_attribute(ds, "b", h.b);
   h5_read_attribute(ds, "n_bins", h.n_bins);
   h5_read_attribute(ds, "n_data_pts", h._n_data_pts);
   h5_read_attribute(ds, "n_lost_pts", h._n_lost_pts);
   h._init();
  }

  /// Output stream insertion
  friend std::ostream& operator<<(std::ostream& os, histogram const& h) {
   auto normed = pdf(h);
   auto integrated = cdf(h);
   for (int i = 0; i < h.size(); ++i)
    os << h.mesh_point(i) << "  " << h.data()[i] << "  " << normed.data()[i] << "  " << integrated.data()[i] << std::endl;
   return os;
  }

 };

 //-------------------------------------------------------------------------------
  /// Normalise histogram to get PDF
  histogram pdf(histogram const& h) {
   auto pdf = h;
   pdf._data /= double(h.n_data_pts());
   return pdf; 
  }

  /// Integrate and normalise histogram to get CDF
  histogram cdf(histogram const& h) {
   auto cdf = h;
   for (int i = 1; i < h.size(); ++i) cdf._data[i] += cdf._data[i - 1];
   cdf._data /= double(h.n_data_pts());
   return cdf;
  }

}
}
