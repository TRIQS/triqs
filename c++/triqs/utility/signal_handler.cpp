// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet, Nils Wentzell

#include "signal_handler.hpp"
#include <signal.h>
#include <string.h>
#include <vector>
#include <iostream>
namespace triqs {
  namespace signal_handler {

    namespace {

      std::vector<int> signals_list;
      bool initialized = false;

      void slot(int signal) {
        std::cerr << "TRIQS : Received signal " << signal << std::endl;
        signals_list.push_back(signal);
      }
    } // namespace

    void start() {
      if (initialized) return;
      static struct sigaction action;
      memset(&action, 0, sizeof(action));
      action.sa_handler = slot;
      sigaction(SIGINT, &action, NULL);
      sigaction(SIGTERM, &action, NULL);
      sigaction(SIGXCPU, &action, NULL);
      sigaction(SIGQUIT, &action, NULL);
      sigaction(SIGUSR1, &action, NULL);
      sigaction(SIGUSR2, &action, NULL);
      sigaction(SIGSTOP, &action, NULL);
      initialized = true;
    }

    void stop() {
      signals_list.clear();
      initialized = false;
    }

    bool received(bool pop_) {
      //if (!initialized) start();
      bool r = signals_list.size() != 0;
      if (r && pop_) pop();
      return r;
    }

    int last() { return signals_list.back(); }
    void pop() { return signals_list.pop_back(); }
  } // namespace signal_handler
} // namespace triqs
