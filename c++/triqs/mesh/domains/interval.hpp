// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
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
// Authors: Philipp Dumitrescu

#pragma once

#include <concepts>
#include <algorithm>
#include <complex>
#include <string>
#include <fmt/core.h>
#include <h5/h5.hpp>
// #include "product.hpp"

namespace triqs::mesh {

  template <typename T>
  requires std::totally_ordered<T> && std::regular<T>
  struct interval_domain_base {
    using point_t = T;

    point_t min_{};
    point_t max_{};

    [[nodiscard]] point_t min() const { return min_; }
    [[nodiscard]] point_t max() const { return max_; }

    [[nodiscard]] bool is_in_domain(point_t const &pt) const { return (min_ <= pt) && (pt <= max_); }

    bool operator==(interval_domain_base const &) const = default;

    friend void h5_write(h5::group fg, std::string const &subgroup_name, interval_domain_base<T> const &d) {
      h5::group gr = fg.create_group(subgroup_name);
      h5::write(gr, "min", d.min_);
      h5::write(gr, "max", d.max_);
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, interval_domain_base<T> &d) {
      h5::group gr = fg.open_group(subgroup_name);
      h5::read(gr, "min", d.min_);
      h5::read(gr, "max", d.max_);
    }

    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive & ar, const unsigned int version) {
      ar &min_;
      ar &max_;
    }
  };

  // ---------------------------------------------

  struct real_interval_domain : interval_domain_base<double> {
    static std::string hdf5_format() { return "real_interval_domain"; }
  };

} // namespace triqs::mesh
