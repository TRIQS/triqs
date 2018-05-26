/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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
#include "callbacks.hpp"
#include "signal_handler.hpp"
#include <chrono>

namespace triqs {
  namespace utility {

    std::function<bool()> clock_callback(int time_in_seconds) {
      signal_handler::start();
      if (time_in_seconds <= 0) return []() { return false; };

      auto end_time = std::chrono::system_clock::now() + std::chrono::seconds(time_in_seconds);
      return [end_time]() { return (std::chrono::system_clock::now() > end_time); };
    }
  } // namespace utility
} // namespace triqs
