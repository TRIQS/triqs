// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once

#include "./concepts.hpp"
#include "./mc_measure.hpp"

#include <fmt/format.h>
#include <h5/h5.hpp>
#include <mpi/communicator.hpp>

#include <cassert>
#include <complex>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace triqs::mc_tools {

  /**
   * @brief MC measure set class.
   *
   * @details It combines multiple MC measures.
   *
   * Since it models the triqs::mc_tools::MCMeasure concept, it can be used just like any other MC measure:
   *
   * - measure_set::accumulate: Calls the measure::accumulate method for all registered MC measures.
   * - measure_set::collect_results: Calls the measure::collect_results method for all registered MC measures.
   * - measure_set::report: Concatenates the reports from all measurements by calling their measure::report method.
   *
   * @tparam MCSignType triqs::mc_tools::DoubleOrComplex type of the sign/weight of a MC configuration.
   */
  template <DoubleOrComplex MCSignType> class measure_set {
    public:
    /// Map type used for storing the measures.
    using measure_map_t = std::map<std::string, measure<MCSignType>>;

    /// Iterator type for the measure set.
    using measure_itr_t = measure_map_t::iterator;

    /// Default constructor.
    measure_set() = default;

    /// Deleted copy constructor.
    measure_set(measure_set const &) = delete;

    /// Deleted copy assignment operator.
    measure_set &operator=(measure_set const &) = delete;

    /// Default move constructor.
    measure_set(measure_set &&) = default;

    /// Default move assignment operator.
    measure_set &operator=(measure_set &&) = default;

    /**
     * @brief Add a new measure to the set with a given name.
     *
     * @details Throws a `std::runtime_error` if a measurement with the same name has already been registered.
     *
     * @tparam T triqs::mc_tools::MCMeasure type.
     * @param m MC measure to add to the set.
     * @param name Name of the measure.
     * @param enable_timer Enable the timer in the measure::accumulate and measure::collect_results methods.
     * @param enable_report Enable the measure::report method.
     */
    template <typename T>
      requires MCMeasure<T, MCSignType>
    measure_itr_t insert(T &&m, std::string name, bool enable_timer, bool enable_report) {
      if (has(name)) throw std::runtime_error(fmt::format("Error in measure_set: Measure with name {} already exists", name));
      return measures_.emplace(name, measure<MCSignType>{std::forward<T>(m), enable_timer, enable_report}).first;
    }

    /**
     * @brief Remove the measure at the given iterator from the set.
     * @param it Iterator to the measure to be removed.
     */
    void remove(measure_itr_t const &it) { measures_.erase(it); }

    /// Remove all measures from the set.
    void clear() { measures_.clear(); }

    /**
     * @brief Check if a measure with the given name is registered.
     * @param name Name of the measure.
     * @return True if the measure is registered, false otherwise.
     */
    [[nodiscard]] bool has(std::string const &name) const { return measures_.find(name) != measures_.end(); }

    /**
     * @brief Get a vector of all the measure names.
     * @return `std::vector<std::string>` containing the names of all registered measures.
     */
    [[nodiscard]] std::vector<std::string> names() const;

    /**
     * @brief Perform all measurements in the set.
     * @details It calls the measure::accumulate method for each measure.
     * @param sign Sign of the current MC configuration.
     */
    void accumulate(MCSignType sign) {
      for (auto &[name, m] : measures_) m.accumulate(sign);
    }

    /**
     * @brief Collect results from all the measures in the measure set from multiple MPI processes.
     * @details It calls the measure::collect_results method for each measure.
     * @param c MPI communicator.
     */
    void collect_results(const mpi::communicator &c);

    /**
     * @brief Report information about the measures in the set.
     * @details It calls the measure::report method for each measure and concatenates the results.
     * @return Concatenated reports from all measurements.
     */
    [[nodiscard]] std::string report() const;

    /**
     * @brief Get a formatted string with the timings of all measures.
     *
     * @details It calls the measure::get_timings method for each measure and concatenates the results.
     *
     * @param prefix Prefix string to be added to the beginning of each line (not intended for the user).
     * @return String containing the timings of all measures.
     */
    [[nodiscard]] std::string get_timings(std::string const &prefix = "") const;

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "measure_set"; }

    /**
     * @brief Write the measure set object to HDF5.
     *
     * @details It loops over all registered measures and calls the `h5_write` function for each measure.
     *
     * @param g h5::group to be written to.
     * @param key Name of the subgroup.
     * @param ms Measure set object to be written.
     */
    friend void h5_write(h5::group g, std::string const &key, measure_set const &ms) {
      auto gr = g.create_group(key);
      h5::write_hdf5_format(gr, ms);
      for (auto const &[name, m] : ms.measures_) h5::write(gr, name, m);
    }

    /**
     * @brief Read the measure set object from HDF5.
     *
     * @details It loops over all registered measures and calls the `h5_read` function for each measure.
     *
     * @param g h5::group to be read from.
     * @param ky Name of the subgroup.
     * @param ms Measure set object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &key, measure_set &ms) {
      auto gr = g.open_group(key);
      h5::assert_hdf5_format(gr, ms);
      for (auto &[name, m] : ms.measures_) h5::read(gr, name, m);
    }

    private:
    measure_map_t measures_;
  };

  // Explicit template instantiation declarations.
  extern template class measure_set<double>;
  extern template class measure_set<std::complex<double>>;

} // namespace triqs::mc_tools
