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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a signal handler for the **TRIQS** library.
 */

#pragma once

/**
 * @addtogroup triqs-utility-runtime
 * @{
 */

/// Signal handling utilities for the **TRIQS** library.
namespace triqs::signal_handler {

  /// Empty exception type that callers may use to signal a graceful shutdown.
  class exception {};

  /**
   * @brief Install the TRIQS signal handler. 
   * @details After this call, raised signals are queued instead of terminating the process.
   */
  void start();

  /**
   * @brief Restore the previous signal disposition. 
   * @details Subsequent signals revert to their default behavior.
   */
  void stop();

  /**
   * @brief Whether at least one signal has been queued since the last reset.
   * 
   * @param pop If `true`, also pop the most recent queued signal before returning.
   * @return True if at least one signal is queued.
   */
  bool received(bool pop = false);

  /// Integer identifier of the most recently received signal.
  int last();

  /// Pop the most recently received signal from the queue (no-op if empty).
  void pop();

} // namespace triqs::signal_handler

/** @} */
