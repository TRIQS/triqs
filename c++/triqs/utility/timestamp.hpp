// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Hugo U. R. Strand, Nils Wentzell

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

    std::string inline hours_minutes_seconds_from_seconds(double sec) {
      auto s = std::chrono::seconds{int(sec)};
      auto h = std::chrono::duration_cast<std::chrono::hours>(s);
      auto m = std::chrono::duration_cast<std::chrono::minutes>(s -= h);
      s -= m;
      std::ostringstream os;
      os << std::setfill('0') << std::setw(2) << h.count() << ":" << std::setfill('0') << std::setw(2) << m.count() << ":" << std::setfill('0')
         << std::setw(2) << s.count();
      return os.str();
    }

    std::string inline estimate_time_left(int Ntot, int iter, timer &Timer) {
      double eta = (Ntot - 1 - iter) * double(Timer) / (iter + 1);
      return hours_minutes_seconds_from_seconds(eta);
    }

  } // namespace utility
} // namespace triqs
