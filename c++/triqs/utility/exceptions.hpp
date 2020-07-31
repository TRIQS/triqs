// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2015 Igor Krivenko
// Copyright (c) 2018-2019 Simons Foundation
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

#pragma once

#include <mpi/mpi.hpp>
#include "./stack_trace.hpp"
#include <exception>
#include <string>
#include <sstream>
#include <stdlib.h>

#define TRIQS_ERROR(CLASS, NAME) throw CLASS() << ".. Triqs " << NAME << " at " << __FILE__ << " : " << __LINE__ << "\n\n"
#define TRIQS_RUNTIME_ERROR TRIQS_ERROR(triqs::runtime_error, "runtime error")
#define TRIQS_KEYBOARD_INTERRUPT TRIQS_ERROR(triqs::keyboard_interrupt, "Ctrl-C")
#define TRIQS_ASSERT(X)                                                                                                                              \
  if (!(X)) TRIQS_RUNTIME_ERROR << BOOST_PP_STRINGIZE(X);
#define TRIQS_ASSERT2(X, ...)                                                                                                                        \
  if (!(X)) TRIQS_RUNTIME_ERROR << BOOST_PP_STRINGIZE(X) << "\n " << __VA_ARGS__;

namespace triqs {

  class exception : public std::exception {
    std::stringstream acc;
    std::string _trace;
    mutable std::string _what;

    public:
    exception() throw() : std::exception() { _trace = utility::stack_trace(); }
    exception(exception const &e) throw() : acc(e.acc.str()), _trace(e._trace), _what(e._what) {}
    virtual ~exception() throw() {}
    template <typename T> exception &operator<<(T const &x) {
      acc << x;
      return *this;
    }
    exception &operator<<(const char *mess) {
      (*this) << std::string(mess);
      return *this;
    } // to limit code size
    virtual const char *what() const throw() {
      std::stringstream out;
      out << acc.str() << "\n.. Error occurred on node ";
      if (mpi::is_initialized()) out << mpi::communicator().rank() << "\n";
      if (getenv("TRIQS_SHOW_EXCEPTION_TRACE")) out << ".. C++ trace is : " << trace() << "\n";
      _what = out.str();
      return _what.c_str();
    }
    virtual const char *trace() const throw() { return _trace.c_str(); }
  };

  class runtime_error : public exception {
    public:
    runtime_error() throw() : exception() {}
    virtual ~runtime_error() throw() {}
    template <typename T> runtime_error &operator<<(T &&x) {
      exception::operator<<(x);
      return *this;
    }
  };

  class keyboard_interrupt : public exception {
    public:
    keyboard_interrupt() throw() : exception() {}
    virtual ~keyboard_interrupt() throw() {}
    template <typename T> keyboard_interrupt &operator<<(T &&x) {
      exception::operator<<(x);
      return *this;
    }
  };

} // namespace triqs
