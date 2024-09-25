// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once

#include <concepts>
#include <memory>

namespace triqs::mc_tools {

  /**
   * @brief Type erasure class for auxiliary MC measures.
   *
   * @details It takes any type that is callable and erases its type.
   *
   * Any auxiliary MC measure must define the following method:
   * - `void operator()()`: Performs an auxiliary measurement.
   *
   * @note The object is passed and stored as a shared pointer. That means it can be used and shared among multiple
   * measures.
   */
  class measure_aux {
    private:
    // Auxiliary MC measure concept defines the interface for auxiliary MC measures.
    struct measure_aux_concept {
      virtual ~measure_aux_concept() = default;
      virtual void call()            = 0;
    };

    // Auxiliary MC measure model implements the auxiliary MC measure concept by calling the appropriate methods of the
    // type earased object.
    template <typename T>
      requires std::invocable<T>
    struct measure_aux_model : public measure_aux_concept {
      std::shared_ptr<T> ptr_;
      measure_aux_model(std::shared_ptr<T> const &m_ptr) : ptr_{m_ptr} {}
      virtual void call() override { return (*ptr_)(); }
    };

    public:
    /**
     * @brief Constructor takes an object that is callable and erases its type.
     *
     * @tparam T Original type of the auxiliary MC measure object.
     * @param m_ptr Shared pointer to the auxiliary MC measure object.
     */
    template <typename T>
      requires std::invocable<T>
    measure_aux(std::shared_ptr<T> const &m_ptr) : ptr_{std::make_unique<measure_aux_model<T>>(m_ptr)} {}

    /// Function call operator performs the auxiliary measurement.
    void operator()() { ptr_->call(); }

    private:
    std::unique_ptr<measure_aux_concept> ptr_;
  };

} // namespace triqs::mc_tools
