
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

#include "signal_handler.hpp"
//#include <time.h>
#include <signal.h>
#include <string.h>
//#include <stdlib.h>
//#include <unistd.h>
// volatile sig_atomic_t signal_handler::keep_going;


namespace triqs {

 /*
    namespace signal_details { 
 // The signal handler just clears the flag and re-enables itself. 
 // to replace with settimer if needed 
 void catch_alarm (int sig)
 {
 MySignalHandler::keep_going = 0;
 signal (sig, Catch_alarm);
 }
 }
 */
 std::vector<int> signal_handler::signals_list;

 signal_handler::signal_handler() {
  static bool initialized;
  if (initialized)  return;
  //keep_going = 1; /* Establish a handler for SIGALRM signals. */  
  static struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_handler = &signal_handler::slot;
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGXCPU, &action, NULL);
  sigaction(SIGQUIT, &action, NULL);
  sigaction(SIGUSR1, &action, NULL);
  sigaction(SIGUSR2, &action, NULL);
  sigaction(SIGSTOP, &action, NULL);
  //signal (SIGALRM, _MYSIGNAL::Catch_alarm);
  //alarm (0);  
  initialized = true;
 }

 void signal_handler::slot(int signal) {
  std::cerr << "Received signal " << signal << std::endl;
  signals_list.push_back(signal);
 }

}

