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

#ifndef __APPLE__
#include <array>
#include <cstdio>
#include <sstream>

#include <sys/types.h>
#include <unistd.h>
#endif // __APPLE__

namespace triqs::utility {

  std::string stack_trace() {
#ifdef __APPLE__
    // The lldb solution used on Linux (see below) no longer works on Mac OS 10.15 (Catalina).
    // The lldb command was: "lldb -p " + std::to_string(getpid()) + " --batch -o \"bt\" 2>&1"
    // TODO Replace by implementation of stacktrace standardization proposal (e.g. https://github.com/boostorg/stacktrace)
    return "Stack-trace currently not available on Mac OS";
#else
    // On Linux we use gdb to decipher the call stack for us.
    // We launch it with a pipe and read back the output.
    std::string const cmd         = "gdb --batch -n -ex bt -p " + std::to_string(getpid()) + " 2>&1";
    std::string const py_sentinel = "libpython";

    constexpr int max_buffer = 256;
    std::array<char, max_buffer> buffer{};
    std::string pipe_output;
    if (FILE *stream = popen(cmd.c_str(), "r"); stream != nullptr) {
      while (fgets(buffer.data(), max_buffer, stream) != nullptr) pipe_output.append(buffer.data());
      pclose(stream);
    }

    std::stringstream ss(pipe_output);
    std::string to;
    std::string r = "\n";

    while (std::getline(ss, to, '\n') and (to.find("triqs::exception::exception") == std::string::npos)) {}

    while (std::getline(ss, to, '\n')) {
      if (to.find(py_sentinel) != std::string::npos) break;
      r += to + '\n';
    }

    return r;
#endif
  }

} // namespace triqs::utility
