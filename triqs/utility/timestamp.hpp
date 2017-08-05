
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by Hugo U.R. Strand
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
#pragma once

#include <iomanip>
#include <chrono>
#include <sstream>

namespace triqs {
  namespace utility {

    std::string inline timestamp() {
      std::ostringstream s;
      auto now          = std::chrono::system_clock::now();
      std::time_t now_c = std::chrono::system_clock::to_time_t(now);
      s << std::put_time(std::localtime(&now_c), "%H:%M:%S");
      return s.str();
    }

    std::string inline estimate_time_left(int Ntot, int iter, timer & Timer) {
      using namespace std::chrono;
      std::ostringstream s;

      Timer.stop();
      double eta = (Ntot - 1 - iter) * double(Timer) / (iter + 1);
      Timer.start();
      time_point<system_clock> tp(milliseconds(long(eta * 1000)) - hours(1));

      std::time_t tp_c = system_clock::to_time_t(tp);
      s << std::put_time(std::localtime(&tp_c), "%H:%M:%S");

      return s.str();
    }
    
  } // namespace utility
} // namespace triqs
