
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

#ifndef TRIQS_SIGNAL_HANDLER_H
#define TRIQS_SIGNAL_HANDLER_H
#include <iostream>
#include <assert.h>
#include <vector>

namespace triqs { 

 class signal_handler {
  static std::vector<int> signals_list;
  //static volatile sig_atomic_t keep_going;
  public:
  signal_handler();
  bool empty(){ return (signals_list.size() ==0); }
  int top(){ return signals_list.back(); }
  void pop(){ return signals_list.pop_back(); }
  static void slot(int signal);
 };
}

#endif
