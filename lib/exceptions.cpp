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
#include "cpp2py/exceptions.hpp"
//#include <mpi.h>
#include <sstream>

#ifndef CPP2PY_TRACE_MAX_FRAMES
#define CPP2PY_TRACE_MAX_FRAMES 50
#endif

#ifdef __GNUC__

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace cpp2py {

  std::string demangle(const char *name) {
    std::stringstream fs;
    int status;
    char *demangled = abi::__cxa_demangle(name, NULL, NULL, &status);
    if (!status) {
      std::string res(demangled);
      fs << res;
      free(demangled);
    } else
      fs << name;
    return fs.str();
  }

  std::string demangle(std::string const &name) { return demangle(name.c_str()); }

  std::string stack_trace() {
    std::ostringstream buffer;
    void *stack[CPP2PY_TRACE_MAX_FRAMES + 1];
    std::size_t depth = backtrace(stack, CPP2PY_TRACE_MAX_FRAMES + 1);
    if (!depth)
      buffer << "  empty  " << std::endl;
    else {
      char **symbols = backtrace_symbols(stack, depth);
      for (std::size_t i = 1; i < depth; ++i) {
        std::string symbol = symbols[i];
        std::istringstream iss(symbol);
        std::vector<std::string> strs{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
        for (auto const &x : strs) buffer << " " << demangle(x);
        buffer << std::endl;
      }
      free(symbols);
    }
    return buffer.str();
  }

} // namespace cpp2py
#else

namespace cpp2py {
  std::string stack_trace() { return std::string("stacktrace only available in gcc"); }
} // namespace cpp2py

#endif

namespace cpp2py {

  bool exception::show_cpp_trace = false;
 
  exception::exception() : std::exception() { _trace = stack_trace(); }
  
  const char *exception::what() const noexcept {
    std::stringstream out;
    out << acc.str() << "\n.. Error occurred ";
    int is_init;
    //MPI_Initialized(&is_init);
    //if (is_init) {
    //  int rank;
    //  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //  out << " on node " << rank;
    // }
    out << "\n";
    if (show_cpp_trace) out << ".. C++ trace is : " << _trace << "\n";
    _what = out.str();
    return _what.c_str();
  }



} // namespace cpp2py



