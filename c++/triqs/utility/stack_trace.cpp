// Copyright (c) 2013-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2017 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019-2020 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

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
    // The LLDB Solution implemented below is no longer working on Mac OS 10.15 (Catalina)
    // TODO Replace by implementation of stacktrace standardization proposal (e.g. https://github.com/boostorg/stacktrace)
    return {"Stack-trace currently not available on Mac OS"};
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
