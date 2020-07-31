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

#include <triqs/utility/dressed_iterator.hpp>
#include <iostream>
#include <map>
using namespace triqs::utility;

struct data {
  int a;
  mutable int b;
};

struct S {

  typedef std::map<double, data> m_t;
  typedef m_t::const_iterator mcit_t;
  typedef m_t::iterator mit_t;
  m_t m;

  struct _cdress {
    double const &tau;
    int const &a;
    int &b;
    _cdress(mcit_t const &it) : tau(it->first), a(it->second.a), b(it->second.b) {}
  };
  struct _dress {
    double const &tau;
    int &a;
    int &b;
    _dress(mit_t const &it) : tau(it->first), a(it->second.a), b(it->second.b) {}
  };

  // const iterator
  struct const_iterator : public dressed_iterator<mcit_t, _cdress> {
    const_iterator(mcit_t it = mcit_t()) : dressed_iterator<mcit_t, _cdress>(it) {}
  };
  const_iterator begin() const { return m.begin(); }
  const_iterator end() const { return m.end(); }

  //iterator
  struct iterator : public dressed_iterator<mit_t, _dress> {
    iterator(mit_t it = mit_t()) : dressed_iterator<mit_t, _dress>(it) {}
  };
  iterator begin() { return m.begin(); }
  iterator end() { return m.end(); }
};

int main() {

  S s;
  s.m.insert(std::make_pair(8.4, data{1, 2}));
  s.m.insert(std::make_pair(1.3, data{3, 4}));

  for (auto const &p : s) std::cout << p.tau << " -- " << p.a << std::endl;
  //for (auto const & p : s.m) std::cout << p.first << " -- "<< p.second << std::endl;

  auto it = s.begin();
  std::cout << it->tau << std::endl;
  ++it;
  std::cout << it->tau << std::endl;
  --it;
  std::cout << it->tau << std::endl;

  it->a *= 10;

  S const &s2(s);

  for (auto const &p : s2) std::cout << p.tau << "  " << p.a << "  " << p.b << std::endl;

  auto it2 = s2.begin();
  std::cout << it2->tau << std::endl;
  it2->b *= -1;
  //it2->a *= -1;

  for (auto const &p : s2) std::cout << p.tau << "  " << p.a << "  " << p.b << std::endl;
}
