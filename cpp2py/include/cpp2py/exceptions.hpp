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
#pragma once
#include <exception>
#include <string>
#include <sstream>

#define CPP2PY_ERROR(CLASS, NAME) throw CLASS() << ".. Triqs " << NAME << " at " << __FILE__ << " : " << __LINE__ << "\n\n"
#define CPP2PY_RUNTIME_ERROR CPP2PY_ERROR(cpp2py::runtime_error, "runtime error")
#define CPP2PY_KEYBOARD_INTERRUPT CPP2PY_ERROR(cpp2py::keyboard_interrupt, "Ctrl-C")
#define CPP2PY_ASSERT(X)                                                                                                                             \
  if (!(X)) CPP2PY_RUNTIME_ERROR << BOOST_PP_STRINGIZE(X)

namespace cpp2py {

  /**
   *  Exception with a << stream operator for simple error message.
   */
  class exception : public std::exception {
    std::stringstream acc;
    std::string _trace;
    mutable std::string _what;

    public:
    exception();
    exception(exception const& e) throw() : acc(e.acc.str()), _trace(e._trace), _what(e._what) {}
    exception(exception &&e)      = default;
    virtual ~exception() {}

    static bool show_cpp_trace;

    template <typename T> exception &operator<<(T const &x) { return acc << x, *this; }

    // ???
    //exception &operator<<(const char *mess) {
    //  (*this) << std::string(mess);
    //  return *this;
    //} // to limit code size

    virtual const char *what() const noexcept;
    //virtual const char *trace() const { return _trace.c_str(); }
  };

  class runtime_error : public exception {
    public:
    runtime_error() : exception() {}

    virtual ~runtime_error() {}

    template <typename T> runtime_error &operator<<(T &&x) {
      exception::operator<<(x);
      return *this;
    }
  };

  class keyboard_interrupt : public exception {
    public:
    keyboard_interrupt() : exception() {}

    virtual ~keyboard_interrupt() {}

    template <typename T> keyboard_interrupt &operator<<(T &&x) {
      exception::operator<<(x);
      return *this;
    }
  };

} // namespace cpp2py
