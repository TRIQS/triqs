
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

 struct _cdress {double const & tau; int const & a; m_t const * m; _cdress( mcit_t const & it, m_t const * mm): tau(it->first), a(it->second),m(mm){} };

 // const iterator with cyclic condition
 struct const_iterator : public dressed_iterator< mcit_t,_cdress, m_t const> {
  const_iterator (mcit_t it, m_t const * m): dressed_iterator<mcit_t,_cdress, m_t const>(it,m) {}
  friend const_iterator next_cyclic(const_iterator it) { 
   ++it; 
   if (it.get() == it.get_aux()->end()) return const_iterator(it.get_aux()->begin(),it.get_aux());
   else return it;
  }
 };
 const_iterator begin() const { return const_iterator(m.begin(),&m);}
 const_iterator end()   const { return const_iterator(m.end(),&m);}

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
 it = next_cyclic(it);
 std::cout  << it->tau << std::endl ;


}
