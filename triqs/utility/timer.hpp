
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

#ifndef TIMER_OP_H_238rj238rh
#define TIMER_OP_H_238rj238rh  

#include <time.h>

namespace triqs { 
namespace utility { 

class timer {
  clock_t Clock1,Clock2;
  bool running;
public:
  timer():running(false) {}
  void start() { running= true; Clock1 = clock();}
  void stop() { Clock2 = clock(); running = false;}
  operator double() const {return  (Clock2 - Clock1)/CLOCKS_PER_SEC;}  
  };
}
}


#endif


