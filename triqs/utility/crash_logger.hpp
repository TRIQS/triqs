/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#include "./first_include.hpp"
#include <string>
#include <exception>
#include <stdexcept>
#include <functional>
#include <vector>
#include <iostream>
#include "./typeid_name.hpp"
#include "./scope_guard.hpp"
#include "../h5.hpp"

namespace triqs {
namespace utility {

 /**
  * Usage :
  *
  * Suppose we have objects a,b,c ... which are all H5_Serializable
  * Then :
  *  { // a scope, whatever
  *    auto log = crash_logger("dump.h5");
  *    log(a,"a")(b,"b")(c,"c"); // <---- pls make a macro here !
  *    // ... do some work
  *    log.dismiss();
  *  }
  *
  * Now : if the calculation proceeds correctly, nothing happens. log is destroyed.
  * if however the calculation does not reach the dismiss, e.g. an exception occurs
  * then log is destroyed without a dismiss and it will dump all objects in the hdf5 file.
  *
  * "some work" can be as complex as needed...
  *
  */
 class crash_logger {
  std::string filename_;
  std::vector<scope_guard<std::function<void()>>> guards;
  std::vector<std::string> names;

  public:
  ///
  crash_logger(std::string filename) : filename_(filename) {}
  crash_logger(const crash_logger&) = delete;
  crash_logger(crash_logger&&) = default;
  crash_logger& operator=(const crash_logger&) = delete;
  crash_logger& operator=(crash_logger&& x) {
   using std::swap;
   swap(guards, x.guards);
   swap(names, x.names);
   return *this;
  }
  ///
  template <typename Obj> crash_logger& operator()(Obj const& obj, std::string name) {
   names.push_back(name);
   guards.emplace_back([&obj, this, name]() {
    using triqs::h5::h5_write; // to have the proper overload for scalar type !!
    try {
     h5_write(h5::group(h5::file(this->filename_.c_str(), H5F_ACC_RDWR)), name, obj);
    } catch (...) {
     std::cerr << "An exception has occurred in crash_logger for an object of type " << typeid_name(obj) << " named " << name
               << std::endl;
    }
   }); // end lambda
   return *this;
  }
  ///
  ~crash_logger() noexcept() {
   if ((guards.size() > 0) && (guards.front().active())) {
    std::cerr << "crash_logger : I am destroyed without being dismissed. Dumping the objects : ";
    for (auto& x : names) std::cerr << "\"" << x << "\" ";
    std::cerr << std::endl;
    h5::file(this->filename_.c_str(), H5F_ACC_TRUNC); // create the file
   }
  }
  ///
  void dismiss() {
   for (auto& g : guards) g.dismiss();
  }
 };
}
}
