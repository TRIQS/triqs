/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#include <ostream>
#include <sstream>
#include "./pyref.hpp"

namespace cpp2py {

  /**
 */
  class py_stream {

    pyref sys, sys_out;
    void _write(const char *s) { pyref res = PyObject_CallMethod(sys_out, (char*)"write", (char*)"s", s); }

    public:
    py_stream() {
      if (!Py_IsInitialized()) CPP2PY_RUNTIME_ERROR << "Construction of a py_stream before the interpreter is initialized !!";
      sys     = pyref::module("sys");
      sys_out = sys.attr("stdout");
    }

    template <class T> py_stream &operator<<(T const &x) {
      std::stringstream fs;
      fs << x;
      _write(fs.str().c_str());
      return *this;
    }

    // this is the type of std::cout
    typedef std::basic_ostream<char, std::char_traits<char>> CoutType;

    // this is the function signature of std::endl
    typedef CoutType &(*StandardEndLine)(CoutType &);

    // define an operator<< to take in std::endl
    py_stream &operator<<(StandardEndLine manip) {
      _write("\n");
      return *this;
    }
  };

} // namespace cpp2py
