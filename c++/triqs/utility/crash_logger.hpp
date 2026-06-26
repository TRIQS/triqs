// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief RAII container that dumps registered HDF5-serializable objects to disk if its scope unwinds abnormally.
 */

#pragma once

#include "./scope_guard.hpp"
#include "./typeid_name.hpp"

#include <h5/h5.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace triqs::utility {

  /**
   * @ingroup triqs-utility-runtime
   * @brief RAII helper that writes user-registered objects to an HDF5 crash dump on abnormal scope exit.
   *
   * @warning This is unused. It might be removed in the future.
   */
  class crash_logger {
    std::string filename_;
    std::vector<scope_guard<std::function<void()>>> guards;
    std::vector<std::string> names;

    public:
    /**
     * @brief Construct a crash logger that will write to an HDF5 file on abnormal exit.
     * @param filename HDF5 file to write to.
     */
    crash_logger(std::string filename) : filename_(std::move(filename)) {}

    /// Deleted copy constructor.
    crash_logger(const crash_logger &) = delete;

    /// Default move constructor.
    crash_logger(crash_logger &&) = default;

    /// Deleted copy-assignment.
    crash_logger &operator=(const crash_logger &) = delete;

    /**
     * @brief Move-assignment takes over guards and names from `x`.
     * 
     * @param x Other crash_logger to move from.
     * @return Reference to `*this`.
     */
    crash_logger &operator=(crash_logger &&x) noexcept {
      using std::swap;
      swap(guards, x.guards);
      swap(names, x.names);
      return *this;
    }

    /**
     * @brief Register an object to be dumped to a given HDF5 path on abnormal exit.
     * 
     * @tparam T HDF5-serializable type.
     * @param obj Object to dump on abnormal exit.
     * @param name HDF5 path / dataset name.
     * @return Reference to `*this` for chained registration.
     */
    template <typename T> crash_logger &operator()(T const &obj, std::string name) {
      names.push_back(name);
      guards.emplace_back([&obj, this, name]() {
        using h5::h5_write; // ensure proper ADL for scalar types
        try {
          h5_write(h5::group(h5::file(this->filename_.c_str(), 'a')), name, obj);
        } catch (...) {
          std::cerr << "An exception has occurred in crash_logger for an object of type " << typeid_name(obj) << " named " << name << std::endl;
        }
      });
      return *this;
    }

    /// Destructor writes all registered objects to the dump file if there has not been a call to dismiss().
    ~crash_logger() noexcept {
      if ((guards.size() > 0) && (guards.front().active())) {
        std::cerr << "crash_logger : I am destroyed without being dismissed. Dumping the objects : ";
        for (auto &x : names) std::cerr << "\"" << x << "\" ";
        std::cerr << std::endl;
        h5::file(this->filename_.c_str(), 'w'); // create the file
      }
    }

    /// Dismiss the logger by suppressing scope-exit dumps and by releasing registered objects.
    void dismiss() {
      for (auto &g : guards) g.dismiss();
    }
  };

} // namespace triqs::utility
