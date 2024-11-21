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

#include <h5/h5.hpp>
#include <mpi/communicator.hpp>

#include <complex>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace triqs::mc_tools {

  // Forward declaration.
  template <DoubleOrComplex MCSignType> class move_set;

  /**
   * @brief Type erasure class for MC moves.
   *
   * @details It takes any type that models the triqs::mc_tools::MCMove concept and erases its type.
   *
   * Any MC move must define the following methods:
   * - `MCSignType attempt()`: Proposes a new MC configuration and returns the acceptance ratio of the proposed move.
   * - `MCSignType accept()`: Accepts the previously proposed move and returns a possible sign correction.
   *
   * The sign of the new configuration, if accepted, is calculated as `new_sign = old_sign * sgn(attempt()) *
   * accept()`. If `attempt()` returns the full MC acceptance ratio (including all signs), then `accept()` should
   * always return 1. Otherwise, the calculation of the sign can be partially or fully done in the `accept()` method.
   *
   * Optionally, the following methods can be defined:
   * - `void reject()`: Callback function if the previously proposed move is rejected.
   * - `void calibrate(mpi::communicator const &)`: Calibrates the move. Usually done during the warm-up phase.
   * - `void collect_statistics(mpi::communicator const &)`: Collects statistics from multiple MPI processes.
   *
   * Optionally, the following free functions can be defined:
   * - `void h5_write(h5::group, std::string const &, T const &) const`: Writes the move object of type `T` to HDF5.
   * - `void h5_read(h5::group, std::string const &, T &)`: Reads the move object of type `T` from HDF5.
   *
   * @tparam MCSignType triqs::mc_tools::DoubleOrComplex type of the sign/weight of a MC configuration.
   */
  template <DoubleOrComplex MCSignType> class move {
    private:
    // MC move concept defines the interface for MC moves.
    struct move_concept {
      virtual ~move_concept()                                    = default;
      virtual MCSignType attempt()                               = 0;
      virtual MCSignType accept()                                = 0;
      virtual void reject()                                      = 0;
      virtual void calibrate(mpi::communicator const &)          = 0;
      virtual void collect_statistics(mpi::communicator const &) = 0;
      virtual void h5write(h5::group, std::string const &) const = 0;
      virtual void h5read(h5::group, std::string const &)        = 0;

      // Methods only implemented in move_set.
      virtual void ms_clear_statistics()                                                  = 0;
      [[nodiscard]] virtual std::string ms_get_statistics(std::string const &) const      = 0;
      [[nodiscard]] virtual std::map<std::string, double> ms_get_acceptance_rates() const = 0;
    };

    // MC move model implements the MC move concept by calling the appropriate methods of the type erased object.
    template <typename T>
      requires MCMove<T, MCSignType>
    struct move_model : public move_concept {
      static constexpr bool is_move_set = std::is_same_v<T, move_set<MCSignType>>;
      T move_;
      move_model(T m) : move_{std::move(m)} {}
      MCSignType attempt() override { return move_.attempt(); }
      MCSignType accept() override { return move_.accept(); }
      void reject() override {
        if constexpr (requires { move_.reject(); }) move_.reject();
      }
      void calibrate(mpi::communicator const &c) override {
        if constexpr (requires { move_.calibrate(c); }) move_.calibrate(c);
      }
      void collect_statistics(mpi::communicator const &c) override {
        if constexpr (requires { move_.collect_statistics(c); }) move_.collect_statistics(c);
      }
      void h5write(h5::group g, std::string const &name) const override {
        if constexpr (h5::Storable<T>) h5::write(g, name, move_);
      }
      void h5read(h5::group g, std::string const &name) override {
        if constexpr (h5::Storable<T>) h5::read(g, name, move_);
      }

      // Methods only implemented in move_set.
      void ms_clear_statistics() override {
        if constexpr (is_move_set) move_.clear_statistics();
      }
      [[nodiscard]] std::string ms_get_statistics(std::string const &prefix) const override {
        if constexpr (is_move_set) return move_.get_statistics(prefix);
        return {};
      }
      [[nodiscard]] std::map<std::string, double> ms_get_acceptance_rates() const override {
        if constexpr (is_move_set) return move_.get_acceptance_rates();
        return {};
      }
    };

    public:
    /**
     * @brief Constructor takes an object that models the triqs::mc_tools::MCMove concept and erases its type.
     *
     * @tparam T Original type of the MC move object.
     * @param m MC move object to have its type erased.
     */
    template <typename T>
      requires(MCMove<T, MCSignType> && !std::is_same_v<T, move>)
    move(T m) : ptr_{std::make_unique<move_model<T>>(std::move(m))}, is_move_set_{std::is_same_v<T, move_set<MCSignType>>} {}

    /// Deleted copy constructor.
    move(move const &) = delete;

    /// Deleted copy assignment operator.
    move &operator=(move const &) = delete;

    /// Default move constructor leaves the moved from object in an empty state.
    move(move &&) = default;

    /// Default move assignment operator leaves the moved from object in an empty state.
    move &operator=(move &&) = default;

    /**
     * @brief Propose a new MC configuration.
     * @return The acceptance ratio of the proposed move.
     */
    MCSignType attempt() {
      ++nprop_;
      return ptr_->attempt();
    }

    /**
     * @brief Accept the new MC configuration.
     * @return A possible sign correction to the acceptance ratio returned by attempt().
     */
    MCSignType accept() {
      ++nacc_;
      return ptr_->accept();
    }

    /**
     * @brief Optional callback function if the proposed move is rejected.
     * @details Does nothing if the original type does not implement a `reject()` method.
     */
    void reject() { ptr_->reject(); }

    /**
     * @brief Optional callback function to calibrate the move.
     * @details Does nothing if the original type does not implement a `calibrate(mpi::communicator const &)` method.
     * @param c MPI communicator.
     */
    void calibrate(mpi::communicator const &c) { ptr_->calibrate(c); }

    /**
     * @brief Collect statistics from multiple MPI processes.
     * @details It sets the total (over all MPI processes) acceptance rate and optionally calls the
     * `collect_statistics(mpi::communicator const &)` method of the original type.
     * @param c MPI communicator.
     */
    void collect_statistics(mpi::communicator const &c);

    /// Get the acceptance rate of the move. You need to call collect_statistics() first.
    [[nodiscard]] auto acceptance_rate() const { return acc_rate_; }

    /// Get the number of proposed configurations, i.e. the number of times attempt() has been called.
    [[nodiscard]] auto n_proposed_config() const { return nprop_; }

    /// Get the number of accepted configurations, i.e. the number of times accept() has been called.
    [[nodiscard]] auto n_accepted_config() const { return nacc_; }

    /**
     * @brief Reset the gathered statistics to their initial states.
     * @details If the move is a move set, it calls the `clear_statistics()` method for all moves in the set.
     */
    void clear_statistics();

    /**
     * @brief Get a formatted string showing the statistics of the move (and other moves if it is a move set).
     *
     * @param name Name of the move.
     * @param prefix Prefix string to be added to the beginning of each line.
     * @return String containing the number of proposed and accepted configurations and the acceptance rate.
     */
    [[nodiscard]] std::string get_statistics(std::string const &name, std::string const &prefix = "") const;

    /**
     * @brief Get the acceptance rates of all moves in case it is a move set.
     * @return `std::map` containing the names of the moves and their acceptance rates if the move is a move set,
     * otherwise an empty map.
     */
    [[nodiscard]] std::map<std::string, double> get_acceptance_rates() const { return ptr_->ms_get_acceptance_rates(); }

    /// Is the move object a move set?
    [[nodiscard]] auto is_set() const { return is_move_set_; }

    /**
     * @brief Write the move object to HDF5.
     *
     * @details Does nothing if there is no specialized `h5_write` function for the original move object.
     *
     * @param g h5::group to be written to.
     * @param name Name of the dataset/subgroup.
     * @param m Move object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, move const &m) { m.ptr_->h5write(g, name); }

    /**
     * @brief Read the move object from HDF5.
     *
     * @details Does nothing if there is no specialized `h5_read` function for the original move object.
     *
     * @param g h5::group to be read from.
     * @param name Name of the dataset/subgroup.
     * @param m Move object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, move &m) { m.ptr_->h5read(g, name); }

    private:
    std::unique_ptr<move_concept> ptr_;
    std::uint64_t nprop_{0};
    std::uint64_t nacc_{0};
    double acc_rate_{-1};
    bool is_move_set_{false};
  };

  // Explicit template instantiation declarations.
  extern template class move<double>;
  extern template class move<std::complex<double>>;

} // namespace triqs::mc_tools
