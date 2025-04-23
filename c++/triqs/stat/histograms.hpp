// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
// Copyright (c) 2016 Igor Krivenko
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Philipp Dumitrescu, Igor Krivenko, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

#pragma once

#include "../arrays.hpp"
#include "../utility/exceptions.hpp"

#include <h5/h5.hpp>
#include <nda/mpi.hpp>
#include <nda/nda.hpp>

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <utility>

namespace triqs::stat {

  /**
   * @brief Class representing a histogram on a given interval.
   *
   * @details The histogram is defined on the interval \f$ [a, b] \f$ with the following values for the center of its
   * bins:
   * \f[
   *   g_n = a + n h = a + n \frac{b - a}{N - 1} \; .
   * \f]
   * Here, \f$ N \f$ is the number of bins in the histogram, \f$ h \f$ is the bin size and \f$ n = 0, 1, \ldots, N - 1
   * \f$ is the index of the bin.
   *
   * That means that each bin is of the same size \f$ h \f$, except for the first and last bin, which have a size of
   * \f$ h / 2 \f$.
   *
   * When a value is added to the histogram, it first determines into which bin the value falls and then increases the
   * count of that bin. If the value is outside of the interval, it is discarded. Additionally, the histogram keeps
   * track of the total number of data points as well as the number of lost points that fall outside of the interval.
   */
  class histogram {
    public:
    /// Default constructor.
    histogram() = default;

    /**
     * @brief Construct a histogram on the interval \f$ [a, b] \f$ with a bin size of 1, except for the first and last
     * bins, which have a size of 0.5.
     *
     * @details The histogram will have a total of \f$ N = b - a + 1 \f$ bins.
     *
     * If \f$ a \geq b \f$, an exception is thrown.
     *
     * @param a Lower bound of the interval.
     * @param b Upper bound of the interval.
     */
    histogram(int a, int b);

    /**
     * @brief Construct a histogram on the interval \f$ [a, b] \f$ with the given number \f$ N \f$ of bins.
     *
     * @details The bin size is set to \f$ h = (b - a) / (N - 1) \f$. The first and last bins have a size of \f$ h / 2
     * \f$.
     *
     * If \f$ a \geq b \f$ or if the number of bins is smaller than 2, an exception is thrown.
     *
     * @param a Lower bound of the interval.
     * @param b Upper bound of the interval.
     * @param nbins Number of bins.
     */
    histogram(double a, double b, std::size_t nbins);

    /**
     * @brief Add a data point to the histogram.
     *
     * @details The data point \f$ x \f$ falls into the bin \f$ n = \lfloor \frac{x - a}{h} + 0.5 \rfloor.
     *
     * If \f$ x \notin [a, b] \f$, the data point is not added to the histogram but instead the number of lost points is
     * increased.
     *
     * @param x Data point to be added to the histogram.
     * @return Reference to `this` object.
     */
    histogram &operator<<(double x);

    /// Reset the histogram to its initial state, i.e. with no data points added to it.
    void clear() {
      n_lost_pts_ = 0;
      n_data_pts_ = 0;
      data_()     = 0.0;
    }

    /**
     * @brief Get the position of the center of the n<sup>th</sup> bin.
     *
     * @param n Index of the bin.
     * @return Position of the n<sup>th</sup> bin center, i.e. \f$ a + n h \f$.
     */
    auto mesh_point(int n) const { return a_ + n * binsize_; }

    /**
     * @brief Get number of bins in the histogram.
     * @return Size of the data vector.
     */
    auto size() const { return data_.size(); }

    /**
     * @brief Get the domain on which the histogram is defined.
     * @return `std::pair` containing the lower and upper bounds of the histogram.
     */
    C2PY_PROPERTY_GET(limits) auto limits() const { return std::pair{a_, b_}; }

    /**
     * @brief Get the data stored in the histogram.
     * @return `nda::vector<double>` containing the count of data points in each bin.
     */
    C2PY_PROPERTY_GET(data) auto const &data() const { return data_; }

    /**
     * @brief Get the number of data points that have been added to the histogram.
     * @return Number of accumulated data points.
     */
    C2PY_PROPERTY_GET(n_data_pts) auto n_data_pts() const { return n_data_pts_; }

    /**
     * @brief Get the number of data point that fell outside of the interval and were discarded.
     * @return Number of discarded data points.
     */
    C2PY_PROPERTY_GET(n_lost_pts) auto n_lost_pts() const { return n_lost_pts_; }

    /**
     * @brief Add two histograms together.
     *
     * @details It simply adds the data vector, the number of accumulated data points and the number of discarded data
     * points together.
     *
     * It throws an expception, if the domains or the number of bins of the two histograms are not equal.
     *
     * @param h1 Left-hand side histogram operand.
     * @param h2 Right-hand side histogram operand.
     * @return Sum of the two histograms.
     */
    friend histogram operator+(histogram h1, histogram const &h2);

    /**
     * @brief Equal-to operator for histograms.
     * @return True, if their domains, data vectors, number of accumulated and discarded data points and bin sizes are
     * equal. False otherwise.
     */
    bool operator==(histogram const &h) const;

    /// Not-equal-to operator for histograms (see operator==(histogram const &)).
    inline bool operator!=(histogram const &h) const { return not(*this == h); }

    /**
     * @brief Implementation of an MPI broadcast for triqs::stat::histogram.
     *
     * @param h Histogram to be broadcasted.
     * @param c MPI communicator object.
     * @param root Rank of the root process.
     */
    C2PY_IGNORE friend void mpi_broadcast(histogram &h, mpi::communicator c = {}, int root = 0) {
      mpi::broadcast(h.a_, c, root);
      mpi::broadcast(h.b_, c, root);
      mpi::broadcast(h.data_, c, root);
      mpi::broadcast(h.n_data_pts_, c, root);
      mpi::broadcast(h.n_lost_pts_, c, root);
      if (c.rank() != root) h.initialize();
    }

    /**
     * @brief Implementation of an MPI reduce for triqs::stat::histogram.
     *
     * @details The reduction does the same as the operator+(histogram, histogram const &).
     *
     * @param h Histogram to be reduced.
     * @param c MPI communicator object.
     * @param root Rank of the root process.
     * @param all Should all processes receive the result of the reduction.
     * @param op MPI reduction operation (only `MPI_SUM` is allowed).
     * @return Reduced histogram.
     */
    C2PY_IGNORE friend histogram mpi_reduce(histogram const &h, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
      TRIQS_ASSERT(op == MPI_SUM);
      histogram h2(h.a_, h.b_, h.size());
      mpi::reduce_into(h.data_, h2.data_, c, root, all, MPI_SUM);
      h2.n_data_pts_ = mpi::reduce(h.n_data_pts_, c, root, all, MPI_SUM);
      h2.n_lost_pts_ = mpi::reduce(h.n_lost_pts_, c, root, all, MPI_SUM);
      return h2;
    }

    /**
     * @brief Get the HDF5 format tag for the histogram type.
     * @return `std::string` containing the format tag.
     */
    [[nodiscard]] static std::string hdf5_format() { return "Histogram"; }

    /**
     * @brief Write a triqs::stat::histogram to HDF5.
     *
     * @param g h5::group in which the dataset is created.
     * @param name Name of the dataset to which the histogram will be written.
     * @param h Histogram to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, histogram const &h);

    /**
     * @brief Read a triqs::stat::histogram from HDF5.
     *
     * @param g h5::group containing the dataset.
     * @param name Name of the dataset from which the histogram will be read.
     * @param h Histogram to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, histogram &h);

    /**
     * @brief Write a histogram to a `std::ostream`.
     *
     * @param os `std::ostream` to which the histogram will be written.
     * @param h Histogram to be written.
     * @return Reference to the same `std::ostream`.
     */
    friend std::ostream &operator<<(std::ostream &os, histogram const &h);

    /**
     * @brief Serialize the histogram to an archive.
     * @param ar Archive to which the histogram is serialized.
     */
    void serialize(auto &ar) const { ar & a_ & b_ & n_data_pts_ & n_lost_pts_ & data_ & binsize_; }

    /**
     * @brief Deserialize the histogram from an archive.
     * @param ar Archive from which the histogram is deserialized.
     */
    void deserialize(auto &ar) { ar & a_ & b_ & n_data_pts_ & n_lost_pts_ & data_ & binsize_; }

    // Friend declarations.
    inline friend histogram pdf(histogram const &h);
    inline friend histogram cdf(histogram const &h);

    private:
    // Initialize the histogram by checking the interval and setting the bin size.
    void initialize();

    private:
    double a_{0.0};
    double b_{0.0};
    double binsize_{0.0};
    std::uint64_t n_data_pts_{0};
    std::uint64_t n_lost_pts_{0};
    nda::vector<double> data_{};
  };

  /**
   * @brief Normalize a histogram.
   *
   * @details It simply divides each bin count by the total number of data points (including the lost points).
   *
   * @note This does not return the PDF of the underlying continuous distribution but rather the discrete probabilities
   * that a data point falls into a certain bin.
   *
   * @param h Histogram to be normalized.
   * @return Normalized histogram.
   */
  inline histogram pdf(histogram const &h) {
    auto pdf = h;
    pdf.data_ /= double(h.n_data_pts());
    return pdf;
  }

  /**
   * @brief Normalize and integrate a histogram.
   *
   * @details It simply performs partial summation of the bin counts and then divides by the total number of data
   * points (including the lost points).
   *
   * @details This does not return the CDF of the underlying continuous distribution but rather the CDF of the discrete
   * probabilities from triqs::stat::pdf.
   *
   * @param h Histogram to be normalized and integrated.
   * @return Normalized and integrated histogram.
   */
  inline histogram cdf(histogram const &h) {
    auto cdf = h;
    for (int i = 1; i < h.size(); ++i) cdf.data_[i] += cdf.data_[i - 1];
    cdf.data_ /= static_cast<double>(h.n_data_pts());
    return cdf;
  }

} // namespace triqs::stat
