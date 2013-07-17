
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#ifndef TRIQS_SPEEDTESTER_H
#define TRIQS_SPEEDTESTER_H
#include <triqs/utility/first_include.hpp>
#include "boost/date_time/posix_time/posix_time.hpp" 
#include <triqs/utility/typeid_name.hpp>

/**
 * To measure the speed of a function.
 */
template<typename Obj>
void speed_tester(size_t n_iter) { 

 Obj x;
 using namespace boost::posix_time;
 using namespace boost::gregorian;

 boost::posix_time::ptime start_time = boost::posix_time::second_clock::local_time();

 for (size_t u=0; u<n_iter; ++u) x();

 boost::posix_time::ptime stop_time = boost::posix_time::second_clock::local_time();

 time_duration td = stop_time - start_time;

 std::cerr << triqs::utility::typeid_name(x)<<" : "<<to_simple_string(td) << std::endl;

};

#endif


