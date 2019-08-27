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

#include "./stack_trace.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

namespace triqs::utility {

  std::string stack_trace() {

    // On Os X, we use lldb, on linux gdb to decipher the call stack for us
    // We launch it with a pipe, and get back the output
#ifdef __APPLE__
    std::string cmd             = "lldb -p " + std::to_string(getpid()) + " --batch -o \"bt\" 2>&1";
    const char *PYTHON_SENTINEL = "Python";
#else
    std::string cmd             = "gdb --batch -n -ex bt -p " + std::to_string(getpid()) + " 2>&1";
    const char *PYTHON_SENTINEL = "libpython";
#endif

    const int max_buffer = 256;
    char buffer2[max_buffer];
    std::string pipe_output;
    FILE *stream = popen(cmd.c_str(), "r");
    if (stream) {
      while (!feof(stream))
        if (fgets(buffer2, max_buffer, stream) != NULL) pipe_output.append(buffer2);
      pclose(stream);
    }

    std::stringstream ss(pipe_output);
    std::string to, r = "\n";

    while (std::getline(ss, to, '\n') and (to.find("triqs::exception::exception") == std::string::npos)) {}

    while (std::getline(ss, to, '\n')) {
      if (to.find(PYTHON_SENTINEL) != std::string::npos) break;
      r += to + '\n';
    }

    return r;
  }

} // namespace triqs::utility
