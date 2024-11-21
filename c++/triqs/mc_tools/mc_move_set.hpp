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
#include "./mc_move.hpp"
#include "./random_generator.hpp"

#include <fmt/format.h>
#include <h5/h5.hpp>

#include <cassert>
#include <cstddef>
#include <complex>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace triqs::mc_tools {

  /**
   * @brief MC move set class.
   *
   * @details It combines multiple MC moves.
   *
   * Since it models the triqs::mc_tools::MCMove concept, it can be used just like any other MC move:
   *
   * - move_set::attempt: Selects a move from the set according to the given weights (>= 0) and calls the move::attempt
   * method of the selected move.
   * - move_set::accept and move_set::reject: Calls the move::accept and move::reject method of the previously attempted
   * move, respectively.
   * - move_set::calibrate, move_set::collect_statistics and the HDF5 routines loop over all moves and call the
   * corresponding method for each registered move.
   *
   * @tparam MCSignType triqs::mc_tools::DoubleOrComplex type of the sign/weight of a MC configuration.
   */
  template <DoubleOrComplex MCSignType> class move_set {
    public:
    /**
     * @brief Construct a move set with a given random number generator (stored in a `std::reference_wrapper`).
     * @param rng RNG object.
     */
    move_set(random_generator &rng) : rng_(rng) {}

    /// Deleted copy constructor.
    move_set(move_set const &) = delete;

    /// Deleted copy assignment operator.
    move_set &operator=(move_set const &) = delete;

    /// Default move constructor.
    move_set(move_set &&) = default;

    /// Default move assignment operator.
    move_set &operator=(move_set &&) = default;

    /**
     * @brief Add a new move to the set with a given name and weight.
     *
     * @details It adds a new MC move to the set, normalizes the weights of all moves and calculates the accumulated
     * probabilities.
     *
     * Throws a `std::runtime_error` if the weight is negative.
     *
     * @tparam T triqs::mc_tools::MCMove type.
     * @param m MC move to add to the set.
     * @param name Name of the move.
     * @param weight Weight of the move (>= 0).
     */
    template <typename T>
      requires MCMove<T, MCSignType>
    void add(T &&m, std::string name, double weight) {
      if (weight < 0.0) throw std::runtime_error(fmt::format("Error in move_set: Weight of move {} is negative: {} < 0", name, weight));
      moves_.emplace_back(std::forward<T>(m));
      weights_.emplace_back(weight);
      names_.emplace_back(std::move(name));
      initialize();
    }

    /**
     * @brief Propose a new MC configuration.
     * @details It selects a move from the set based on the given weights and calls the move::attempt method of the
     * selected move.
     * @return The acceptance ratio of the proposed move.
     */
    double attempt();

    /**
     * @brief Accept the new MC configuration.
     * @details It calls the move::accept method of the previously attempted move.
     * @return Sign correction returned by move::accept times the sign of the acceptance ratio from move::attempt.
     */
    MCSignType accept() { return moves_[current_].accept() * attempt_sign_; }

    /**
     * @brief Reject the new MC configuration.
     * @details It calls the move::reject method of the previously attempted move.
     */
    void reject() { moves_[current_].reject(); }

    /**
     * @brief Clear the statistics of all the moves in the set.
     * @details It calls the move::clear_statistics method for each move.
     */
    void clear_statistics();

    /**
     * @brief Collect statistics for all the moves in the set from multiple MPI processes.
     * @details It calls the move::collect_statistics method for each move.
     * @param c MPI communicator.
     */
    void collect_statistics(mpi::communicator const &c);

    /**
     * @brief Calibrate all the moves in the set.
     * @details It calls the move::calibrate method for each move.
     * @param c MPI communicator.
     */
    void calibrate(mpi::communicator const &c);

    /**
     * @brief Get the acceptance rates of all moves in the set.
     * @return `std::map` containing the names of the moves and their acceptance rates.
     */
    [[nodiscard]] std::map<std::string, double> get_acceptance_rates() const;

    /**
     * @brief Get a formatted string showing the acceptance rates of all moves.
     * @param prefix Prefix string to be added to the beginning of each line (not intended for the user).
     * @return String containing the number of proposed and accepted configurations and the acceptance rate of all moves
     * in the set.
     */
    [[nodiscard]] std::string get_statistics(std::string const &prefix = "") const;

    /// Get the index of the current move.
    [[nodiscard]] auto current() const { return current_; }

    /// Get the probabilities of all the moves.
    [[nodiscard]] auto const &probabilities() const { return probs_; }

    /// Get the sign of the last attempt.
    [[nodiscard]] auto attempt_sign() const { return attempt_sign_; }

    /**
     * @brief Write the move set object to HDF5.
     *
     * @details It loops over all registered moves and calls the `h5_write` function for each move.
     *
     * @param g h5::group to be written to.
     * @param name Name of the subgroup.
     * @param ms Move set object to be written.
     */

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "move_set"; }

    friend void h5_write(h5::group g, std::string const &name, move_set const &ms) {
      auto gr = g.create_group(name);
      h5::write_hdf5_format(gr, ms);
      for (std::size_t i = 0; i < ms.moves_.size(); ++i) h5::write(gr, ms.names_[i], ms.moves_[i]);
    }

    /**
     * @brief Read the move set object from HDF5.
     *
     * @details It loops over all registered moves and calls the `h5_read` function for each move.
     *
     * @param g h5::group to be read from.
     * @param name Name of the subgroup.
     * @param ms Move set object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, move_set &ms) {
      auto gr = g.open_group(name);
      h5::assert_hdf5_format(gr, ms);
      for (std::size_t i = 0; i < ms.moves_.size(); ++i) h5::read(gr, ms.names_[i], ms.moves_[i]);
    }

    private:
    // Normalize the weights and calculate the accumulated probabilities.
    void initialize();

    // Check the acceptance ratio, handle possible infinites, set the attemped sign and return the absolute value.
    double check_ratio(MCSignType ratio);

    private:
    std::vector<move<MCSignType>> moves_;
    std::vector<std::string> names_;
    std::size_t current_{0};
    std::reference_wrapper<random_generator> rng_;
    std::vector<double> weights_;
    std::vector<double> probs_;
    std::vector<double> acc_probs_;
    MCSignType attempt_sign_{1.0};
  };

  // Explicit template instantiation declarations.
  extern template class move_set<double>;
  extern template class move_set<std::complex<double>>;

} // namespace triqs::mc_tools
