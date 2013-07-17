/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#include <triqs/utility/dressed_iterator.hpp>
#include <iostream>
#include <map>
using namespace triqs::utility;

struct S { 

 typedef std::map<double, int> m_t;
 typedef m_t::const_iterator mcit_t;
 typedef m_t::iterator mit_t;
 m_t m;

 struct _cdress {double const & tau; int const & a; _cdress( mcit_t const & it): tau(it->first), a(it->second){} };
 struct _dress  {double const & tau; int & a;       _dress ( mit_t const & it) : tau(it->first), a(it->second){} };

 // const iterator
 struct const_iterator : public dressed_iterator< mcit_t,_cdress> {
  const_iterator (mcit_t it):  dressed_iterator<mcit_t,_cdress>(it) {}
 };
 const_iterator begin() const { return m.begin();}
 const_iterator end()   const { return m.end();}

 //iterator
 struct iterator : public dressed_iterator< mit_t,_dress> {
  iterator (mit_t it = mit_t()):  dressed_iterator<mit_t,_dress>(it) {}
 };
 iterator begin() { return m.begin();}
 iterator end()   { return m.end();}

};


int main() {

 S s;
 s.m.insert(std::make_pair(8.4,2));
 s.m.insert(std::make_pair(1.3,1));

 for (auto const & p : s) std::cout << p.tau << " -- "<< p.a << std::endl;
 //for (auto const & p : s.m) std::cout << p.first << " -- "<< p.second << std::endl;

 auto it = s.begin();
 std::cout  << it->tau << std::endl ;
 ++it;
 std::cout  << it->tau << std::endl ;
 --it;
 std::cout  << it->tau << std::endl ;

 it-> a *=10;

 for (auto const & p : s) 
  std::cout << p.tau << " -- "<< p.a << std::endl;

}
