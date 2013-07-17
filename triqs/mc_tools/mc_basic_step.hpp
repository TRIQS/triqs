
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

#ifndef MC_STEPS_H
#define MC_STEPS_H

#include <boost/mpi.hpp>
#include "mc_move_set.hpp"

namespace triqs { namespace mc_tools { namespace Step { 

 // Performs one Metropolis step
 template<typename MCSignType>
  struct Metropolis {  
   static void do_it (move_set<MCSignType> & MoveGroup, random_generator & RNG, MCSignType & signe){
    double  r = MoveGroup.attempt();
    if (RNG() < std::min(1.0,r)) { 
     signe *= MoveGroup.accept();
#ifdef DEBUG
     std::cerr<<"   Metropolis sign = "<< signe <<std::endl;
#endif
    }
    else MoveGroup.reject();
   }
  };

 // Put heat Bath here ....

}}}
#endif


