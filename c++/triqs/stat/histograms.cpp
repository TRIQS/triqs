// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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
// Authors: Philipp D, Igor Krivenko, Nils Wentzell

#include "./histograms.hpp"
#include "../utility/exceptions.hpp"

#include <h5/h5.hpp>
#include <nda/h5.hpp>

#include <cmath>
#include <ostream>
#include <string>

namespace triqs::stat {

  void histogram::initialize() {
    if (a_ >= b_) TRIQS_RUNTIME_ERROR << "Error in histogram::initialize: Incorrect interval: a >= b";
    if (size() < 2) TRIQS_RUNTIME_ERROR << "Error in histogram::initialize: Number of bins has to be at least 2";
    binsize_ = (b_ - a_) / static_cast<double>(size() - 1);
  }

  histogram::histogram(int a, int b) : a_(a), b_(b), data_(nda::vector<double>::zeros(b - a + 1)) { initialize(); }

  histogram::histogram(double a, double b, std::size_t nbins) : a_(a), b_(b), data_(nbins) { initialize(); }

  histogram &histogram::operator<<(double x) {
    if (x < a_ || x > b_)
      ++n_lost_pts_;
    else {
      auto n = static_cast<int>(std::floor(((x - a_) / binsize_) + 0.5));
      ++data_[n];
      ++n_data_pts_;
    }
    return *this;
  }

  histogram operator+(histogram h1, histogram const &h2) {
    if (h1.limits() != h2.limits() || h1.size() != h2.size())
      TRIQS_RUNTIME_ERROR << "Error when adding histograms: Histograms have different domains or number of bins";
    h1.data_ += h2.data_;
    h1.n_data_pts_ += h2.n_data_pts_;
    h1.n_lost_pts_ += h2.n_lost_pts_;
    return h1;
  }

  bool histogram::operator==(histogram const &h) const {
    return a_ == h.a_ && b_ == h.b_ && data_ == h.data_ && n_data_pts_ == h.n_data_pts_ && n_lost_pts_ == h.n_lost_pts_ && binsize_ == h.binsize_;
  }

  void h5_write(h5::group g, std::string const &name, histogram const &h) {
    h5::write(g, name, h.data_);
    auto ds = g.open_dataset(name);
    write_hdf5_format(ds, h);
    h5::write_attribute(ds, "a", h.a_);
    h5::write_attribute(ds, "b", h.b_);
    h5::write_attribute(ds, "n_data_pts", h.n_data_pts_);
    h5::write_attribute(ds, "n_lost_pts", h.n_lost_pts_);
  }

  void h5_read(h5::group g, std::string const &name, histogram &h) {
    h5::read(g, name, h.data_);
    auto ds = g.open_dataset(name);
    h5::read_attribute(ds, "a", h.a_);
    h5::read_attribute(ds, "b", h.b_);
    h5::read_attribute(ds, "n_data_pts", h.n_data_pts_);
    h5::read_attribute(ds, "n_lost_pts", h.n_lost_pts_);
    h.initialize();
  }

  std::ostream &operator<<(std::ostream &os, histogram const &h) {
    auto normed     = pdf(h);
    auto integrated = cdf(h);
    for (int i = 0; i < h.size(); ++i)
      os << h.mesh_point(i) << "  " << h.data()[i] << "  " << normed.data()[i] << "  " << integrated.data()[i] << std::endl;
    return os;
  }

} // namespace triqs::stat
