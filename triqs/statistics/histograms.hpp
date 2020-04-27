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

    /// Statistical histogram
    /**
   This class serves to sample a continuous random variable, and to 'bin' it.
   It divides a given range of real values into a series of equal intervals,
   and counts amounts of samples falling into each interval.
   The histogram keeps track of the total number of the sampled values, as well
   as of the lost samples that lie outside the chosen range.

   @include triqs/statistics/histograms.hpp
  */
    class histogram {

      double a, b;                        // start and end of mesh
      long n_bins;                        // number of points on the mesh (for double only)
      unsigned long long _n_data_pts = 0; // number of data points
      unsigned long long _n_lost_pts = 0; // number of discarded points
      arrays::vector<double> _data;       // histogram data
      double _step;                       // number of bins per unit length

      void _init(); // initialize _step

      inline friend histogram pdf(histogram const &h); // probability distribution function = normalised histogram
      inline friend histogram cdf(histogram const &h); // cumulative distribution function = normalised histogram integrated

      public:
      /// Constructor
      /**
    Constructs a histogram over :math:`[a; b]` range with bin length equal to 1.

    @param a Left end of the sampling range
    @param b Right end of the sampling range
   */
      histogram(int a, int b) : a(a), b(b), n_bins(b - a + 1), _data(n_bins, 0) { _init(); }

      /// Constructor
      /**
    Constructs a histogram over :math:`[a; b]` range with a given number of bins.

    @param a Left end of the sampling range
    @param b Right end of the sampling range
    @param n_bins Number of bins
   */
      histogram(double a, double b, long n_bins) : a(a), b(b), n_bins(n_bins), _data(n_bins, 0) { _init(); }

      /// Default constructor
      histogram() = default;

      /// Bins a real value into the histogram
      /**
    A sampled value :math:`x` falls into the :math:`n`-th bin if :math:`x\in[nh-h/2;nh+h/2[`,
    where :math:`h` is the bin length, :math:`h = (b - a) / (n_\mathrm{bins} - 1)`.
    This convention implies that the first (:math:`n = 0`) and the last (:math:`n = n_\mathrm{bins} - 1`)
    bins are effectively two times shorter that others. Sample is discarded if :math:`x\notin[a; b]`.

    @param x Sampled value
    @return Reference to `*this`, so that it is possible to chain `operator<<` calls
   */
      histogram &operator<<(double x);

      /// Get position of bin's center
      /**
    @param n Bin index
    @return Position of the center, :math:`n (b - a) / (n_\mathrm{bins} - 1)`
   */
      double mesh_point(int n) const { return a + n / _step; }

      /// Get number of histogram bins
      /**
    @return Number of bins
   */
      size_t size() const { return _data.size(); }

      /// Return boundaries of the histogram
      /**
    @return Pair of histogram boundaries, `(a,b)`
   */
      std::pair<double, double> limits() const { return {a, b}; }

      /// Read-only access to the data storage
      /**
    @return Constant reference to the histogram data array
   */
      arrays::vector<double> const &data() const { return _data; }

      /// Get number of accumulated samples
      /**
    @return Number of accumulated data points
   */
      unsigned long long n_data_pts() const { return _n_data_pts; }

      /// Get number of discarded samples
      /**
    @return Number of discarded data points
   */
      unsigned long long n_lost_pts() const { return _n_lost_pts; }

      /// Addition of histograms
      /**
    Compute the sum of two histograms over the same range, and with equal numbers of bins.
    This operator will throw if histograms to be added up are incompatible.
    Summation is also performed on the numbers of accumulated and discarded points.

    @param h1 First histogram to add up
    @param h2 Second histogram to add up
    @return Sum of histograms
   */
      friend histogram operator+(histogram h1, histogram const &h2);

      /// Reset all histogram values to 0
      /**
    Resets all data values and the total counts of accumulated and discarded points.

   */
      void clear() {
        _n_lost_pts = 0;
        _n_data_pts = 0;
        _data()     = 0.0;
      }

      /// MPI-broadcast histogram
      /**
    @param h Histogram to be broadcast
    @param c MPI communicator object
    @param root MPI root rank for broadcast operation
   */
      friend void mpi_broadcast(histogram &h, mpi::communicator c = {}, int root = 0) {
	 mpi::broadcast(h.a, c, root);
        mpi::broadcast(h.b, c, root);
        mpi::broadcast(h._data, c, root);
        mpi::broadcast(h._n_data_pts, c, root);
        mpi::broadcast(h._n_lost_pts, c, root);
        if (c.rank() != root) {
          h.n_bins = h._data.size();
          h._init();
        }
      }

      /// MPI-reduce histogram
      /**
    The only supported reduction operation is MPI_SUM, which is equivalent to `operator+()`.

    @param h Histogram subject to reduction
    @param c MPI communicator object
    @param root MPI root rank for MPI reduction
    @param all Send reduction result to all ranks in `c`?
    @param op Reduction operation, must be MPI_SUM
    @return Reduction result; valid only on MPI rank 0 if `all = false`
   */
      friend histogram mpi_reduce(histogram const &h, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
        TRIQS_ASSERT(op == MPI_SUM);
        histogram h2(h.a, h.b, h.n_bins);
        h2._data       = mpi::reduce(h._data, c, root, all, MPI_SUM);
        h2._n_data_pts = mpi::reduce(h._n_data_pts, c, root, all, MPI_SUM);
        h2._n_lost_pts = mpi::reduce(h._n_lost_pts, c, root, all, MPI_SUM);
        return h2;
      }

      /// HDF5 interface

      /// Get HDF5 scheme name
      /**
    @return HDF5 scheme name
   */
      static std::string hdf5_scheme() { return "Histogram"; }
      /// Write histogram to HDF5
      /**
    @param g Enclosing HDF5 group
    @param name Dataset name for histogram data
    @param h Histogram to be written
   */
      friend void h5_write(h5::group g, std::string const &name, histogram const &h);
      /// Read histogram from HDF5
      /**
    @param g Enclosing HDF5 group
    @param name Dataset name with histogram data
    @param h Histogram to read data into
   */
      friend void h5_read(h5::group g, std::string const &name, histogram &h);

      /// Comparison operator
      /**
    @param h Histogram to be compared
    @return True iff both Histograms are equal, else False
   */
      bool operator==(histogram const &h) const;
      inline bool operator!=(histogram const &h) const { return not (*this == h); }

      /// Output stream insertion
      /**
    @param os Reference to an output stream
    @param h Histogram to be inserted
    @return Reference to the same output stream
   */
      friend std::ostream &operator<<(std::ostream &os, histogram const &h);

      /// BOOST Serialization
      friend class boost::serialization::access;
      /// Serialization
      /**
    @tparam Archive Archive type for serialization
    @param ar Archive object to serialize to/deserialize from
    @param version Archive format version tag
   */
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        ar &a;
        ar &b;
        ar &n_bins;
        ar &_data;
        ar &_n_data_pts;
        ar &_n_lost_pts;
        ar &_step;
      }
    };

    //-------------------------------------------------------------------------------

    /// Normalise histogram to get probability density function (PDF)
    /**
   @param h Histogram to be normalised
   @return Probability density function
  */
    inline histogram pdf(histogram const &h) {
      auto pdf = h;
      pdf._data /= double(h.n_data_pts());
      return pdf;
    }

    /// Integrate and normalise histogram to get cumulative distribution function (CDF)
    /**
   @param h Histogram to be integrated and normalised
   @return Cumulative distribution function
  */
    inline histogram cdf(histogram const &h) {
      auto cdf = h;
      for (int i = 1; i < h.size(); ++i) cdf._data[i] += cdf._data[i - 1];
      cdf._data /= double(h.n_data_pts());
      return cdf;
    }

  } // namespace statistics
} // namespace triqs
