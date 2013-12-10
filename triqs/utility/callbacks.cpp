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

// switch to std. Remove old boost code when ok on several compilers
#ifndef TRIQS_USE_BOOST_CHRONO
#include <chrono>

namespace triqs {
namespace utility {

 std::function<bool()> clock_callback(int time_in_seconds) {
  if (time_in_seconds <= 0)
   return []() { return (!triqs::signal_handler().empty()); };

  // auto end_time = boost::posix_time::second_clock::local_time() + boost::posix_time::seconds(time_in_seconds);
  // return [end_time]() { return (!triqs::signal_handler().empty()) || boost::posix_time::second_clock::local_time() > end_time

  auto end_time = std::chrono::system_clock::now() + std::chrono::seconds(time_in_seconds);
  return [end_time]() { return (!triqs::signal_handler().empty()) || (std::chrono::system_clock::now() > end_time); };
 }
}
}

#else

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>

namespace triqs {
namespace utility {

 bool clock_callback_impl(boost::posix_time::ptime const& end_time) {
  return (!triqs::signal_handler().empty()) || boost::posix_time::second_clock::local_time() > end_time;
 }

 bool false_callback_impl() { return (!triqs::signal_handler().empty()); }

 std::function<bool()> clock_callback(int time_in_seconds) {
  return (time_in_seconds > 0 ? boost::bind(&clock_callback_impl, boost::posix_time::second_clock::local_time() +
                                                                      boost::posix_time::seconds(time_in_seconds))
                              : std::function<bool()>(&false_callback_impl));
 }
}
}
#endif

