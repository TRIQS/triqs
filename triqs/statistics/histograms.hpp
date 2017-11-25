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

  double a, b;                          // start and end of mesh
  long n_bins;                          // number of points on the mesh (for double only)
  unsigned long long _n_data_pts = 0;   // number of data points
  unsigned long long _n_lost_pts = 0;   // number of discarded points
  arrays::vector<double> _data;         // histogram data
  double _step;                         // number of bins per unit length

  void _init();                         // initialize _step

  inline friend histogram pdf(histogram const& h); // probability distribution function = normalised histogram
  inline friend histogram cdf(histogram const& h); // cumulative distribution function = normalised histogram integrated

  public:

  /// Constructor with mesh of integer values
  histogram(int a, int b) : a(a), b(b), n_bins(b - a + 1), _data(n_bins, 0) { _init(); }

  /// Constructor with mesh of double values
  histogram(double a, double b, long n_bins) : a(a), b(b), n_bins(n_bins) ,_data(n_bins, 0){ _init(); }

  /// Default constructor
  histogram() = default;

  /// Bins a double into the histogram
  histogram& operator<<(double x);

  /// Point on the mesh
  double mesh_point(int n) const { return a + n / _step; }

  /// Number of bins
  size_t size() const { return _data.size(); }

  /// Returns boundaries of the histogram
  std::pair<double, double> limits() const { return {a, b}; }

  /// Read-only access to the data storage
  arrays::vector<double> const& data() const { return _data; }

  /// Norm of the stored data
  unsigned long long n_data_pts() const { return _n_data_pts; }

  /// Number of discarded points
  unsigned long long n_lost_pts() const { return _n_lost_pts; }

  /// Addition of histograms
  friend histogram operator+(histogram h1, histogram const& h2);

  /// Reset all values to 0
  void clear() {
   _n_lost_pts = 0;
   _n_data_pts = 0;
   _data() = 0.0;
  }

  /// Broadcast histogram
  friend void mpi_broadcast(histogram & h, mpi::communicator c = {}, int root = 0) {
   mpi_broadcast(h.a, c, root);
   mpi_broadcast(h.b, c, root);
   mpi_broadcast(h._data, c, root);
   mpi_broadcast(h._n_data_pts, c, root);
   mpi_broadcast(h._n_lost_pts, c, root);
   if (c.rank() != root) { h.n_bins = h._data.size(); h._init(); }
  }

  /// Reduce histogram
  friend histogram mpi_reduce(histogram const& h, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
   TRIQS_ASSERT(op == MPI_SUM);
   histogram h2(h.a, h.b, h.n_bins);
   h2._data = mpi_reduce(h._data, c, root, all, MPI_SUM);
   h2._n_data_pts = mpi_reduce(h._n_data_pts, c, root, all, MPI_SUM);
   h2._n_lost_pts = mpi_reduce(h._n_lost_pts, c, root, all, MPI_SUM);
   return h2;
  }

  /// HDF5 interface
  friend std::string get_triqs_hdf5_data_scheme(histogram const&);
  friend void h5_write(h5::group g, std::string const& name, histogram const& h);
  friend void h5_read(h5::group g, std::string const& name, histogram& h);

  /// Output stream insertion
  friend std::ostream& operator<<(std::ostream& os, histogram const& h);

  /// BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & TRIQS_MAKE_NVP("a", a);
    ar & TRIQS_MAKE_NVP("b", b);
    ar & TRIQS_MAKE_NVP("n_bins", n_bins);
    ar & TRIQS_MAKE_NVP("data", _data);
    ar & TRIQS_MAKE_NVP("n_data_pts", _n_data_pts);
    ar & TRIQS_MAKE_NVP("n_lost_pts", _n_lost_pts);
    ar & TRIQS_MAKE_NVP("step", _step);
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
