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
#include <triqs/arrays.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>

using namespace triqs::arrays;
using namespace triqs::clef;
using namespace triqs;
const int N1 = 500, N2 = 300, Nu = 20;

struct simple_loop {
 void operator()() {
  array<double, 3> A(N1, N1, N1);
  for (int u = 0; u < Nu; ++u)
   for (int i = 0; i < N1; ++i)
    for (int j = 0; j < N1; ++j)
     for (int k = 0; k < N1; ++k) A(i, j, k) = i - j + u * k;
 }
};

struct with_clef {
 void operator()() {
  array<double, 3> A(N1, N1, N1);

  placeholder<0> i_;
  placeholder<1> j_;
  placeholder<2> k_;
  for (int u = 0; u < Nu; ++u)
   A(i_,j_,k_) << i_ - j_ + u * k_;
 }
};


struct with_clef_custom_order {
 void operator()() {
  array<double, 3> A(N1, N1, N1, make_memory_layout(1,0,2));
  //array<double, 3> A(N1, N1, N1, make_memory_layout(1,2,0));

  placeholder<0> i_;
  placeholder<1> j_;
  placeholder<2> k_;
  for (int u = 0; u < Nu; ++u)
    A(i_,j_,k_) << i_ - j_ + u * k_;
 }
};


struct loop_2 {
 void operator()() {
  long ind[3];
  long Ns[3] = {N1, N1, N1};
  array<double, 3> A(N1, N1, N1);

  auto l = [&A](int i, int j, int k) { A(i, j, k) = i - j + 2 * k; };

  for (int u = 0; u < Nu; ++u)
   for (ind[0] = 0; ind[0] < Ns[0]; ++ind[0])
    for (ind[1] = 0; ind[1] < Ns[0]; ++ind[1])
     for (ind[2] = 0; ind[2] < Ns[0]; ++ind[2]) l(ind[0], ind[1], ind[2]);
 }
};

struct loop_reorder {
 void operator()() {
  int p[3];
  for (int u = 0; u<3; ++u) p[u] = (u+2)%3; // = {2, 0, 1};
  //int p[] = {0,1,2};
  long ind[3];
  long Ns[3] = {N1, N1, N1};
  array<double, 3> A(N1, N1, N1);

  auto l = [&A](int i, int j, int k) { A(i, j, k) = i - j + 2 * k; };

  for (int u = 0; u < Nu; ++u)
   for (ind[0] = 0; ind[0] < Ns[0]; ++ind[0])
   for (ind[1] = 0; ind[1] < Ns[1]; ++ind[1])
    for (ind[2] = 0; ind[2] < Ns[2]; ++ind[2]) 
     
    l(ind[p[1]], ind[p[2]], ind[p[0]]);
    // l(ind[p[0]], ind[p[1]], ind[p[2]]);
 }
};

struct loop_reorder2 {
 void operator()() {
  //int p[] = {2, 0, 1};
   int p[] = {0,1,2};
  int ind[3];
  int Ns[3] = {N1, N1, N1};
  array<double, 3> A(N1, N1, N1);

  auto l = [&A](int i, int j, int k) { A(i, j, k) = i - j + 2 * k; };

  for (int u = 0; u < Nu; ++u)
   for (ind[p[0]] = 0; ind[p[0]] < Ns[p[0]]; ++ind[p[0]])
   for (ind[p[1]] = 0; ind[p[1]] < Ns[p[1]]; ++ind[p[1]])
    for (ind[p[2]] = 0; ind[p[2]] < Ns[p[2]]; ++ind[p[2]]) l(ind[0], ind[1], ind[2]);
 }
};

// a very bad idea ... 15 x slower...
struct iter2 {
 void operator()() {
  array<double, 3> A(N1, N1, N1);
  auto l = [&A](int i, int j, int k) { A(i, j, k) = i - j + 2 * k; };

  long m = N1 * N1 * N1;
  long N11 = N1 * N1;
  for (int u = 0; u < Nu; ++u)
   for (long ind = 0; ind < m; ++ind) {
    l(ind % N1, (ind / N1) % N1, (ind / N11) % N1);
   }
 }
};

struct iter {
 void operator()() {
  array<double, 3> A(N1, N1, N1);
  auto l = [&A](int i, int j, int k) { A(i, j, k) = i - j + 2 * k; };

  for (int u = 0; u < Nu; ++u)
   for (auto it = A.begin(); it; ++it) {
    l(it.indices()[0], it.indices()[1], it.indices()[2]);
   }
 }
};


#include "./speed_tester.hpp"
int main() {
 const int l = 5;
 speed_tester<simple_loop>(l);
 speed_tester<with_clef>(l);
 speed_tester<with_clef_custom_order>(l);
 speed_tester<loop_2>(l);
 speed_tester<loop_reorder>(l);
 speed_tester<loop_reorder2>(l);
 speed_tester<iter>(l);
 //speed_tester<iter2>(l);
 return 0;
}

