#include <triqs/utility/first_include.hpp>

#include <set>
#include <vector>
#include <algorithm>
#include <sstream>

#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/space_partition.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <triqs/hilbert_space/hilbert_space.hpp>
#include <triqs/hilbert_space/imperative_operator.hpp>
#include <triqs/hilbert_space/state.hpp>

using namespace triqs::hilbert_space;
using triqs::utility::many_body_operator;
using triqs::utility::c;
using triqs::utility::c_dag;
using triqs::utility::n;

int main() {

 // 3 bands Kanamori

 // Parameters
 double mu = 0.7;
 double U = 3.0;
 double J = 0.3;

 // basis of operators to use
 fundamental_operator_set fops;
 for (int o = 0; o < 3; ++o) {
  fops.insert("up", o);
  fops.insert("dn", o);
 }

 // Hamiltonian
 many_body_operator<double> H;
 for (int o = 0; o < 3; ++o) {
  H += -mu * (n("up", o) + n("dn", o));
 }
 for (int o = 0; o < 3; ++o) {
  H += U * n("up", o) * n("dn", o);
 }
 for (int o1 = 0; o1 < 3; ++o1)
  for (int o2 = 0; o2 < 3; ++o2) {
   if (o1 == o2) continue;
   H += (U - 2 * J) * n("up", o1) * n("dn", o2);
  }
 for (int o1 = 0; o1 < 3; ++o1)
  for (int o2 = 0; o2 < 3; ++o2) {
   if (o2 >= o1) continue;
   H += (U - 3 * J) * n("up", o1) * n("up", o2);
   H += (U - 3 * J) * n("dn", o1) * n("dn", o2);
  }

 for (int o1 = 0; o1 < 3; ++o1)
  for (int o2 = 0; o2 < 3; ++o2) {
   if (o1 == o2) continue;
   H += -J * c_dag("up", o1) * c_dag("dn", o1) * c("up", o2) * c("dn", o2);
   H += -J * c_dag("up", o1) * c_dag("dn", o2) * c("up", o2) * c("dn", o1);
  }

 // Hilbert space
 hilbert_space hs(fops);

 // Imperative operator for H
 imperative_operator<hilbert_space, double, false> Hop(H, fops);

 // Sample state
 state<hilbert_space, double, true> st(hs);

 // Space partition
 space_partition<state<hilbert_space, double, true>, imperative_operator<hilbert_space, double, false>> SP(st, Hop);

 /////////////////////////////
 // Part I: Check subspaces //
 /////////////////////////////

 // Calculated classification of states
 // sets are used to neglect order of subspaces and of states within a subspace
 std::vector<std::set<int>> v_cl(SP.n_subspaces());
 foreach(SP, [&v_cl](int st, int spn) { v_cl[spn].insert(st); });
 std::set<std::set<int>> cl{v_cl.cbegin(), v_cl.cend()};

 int u0 = 1 << fops[{"up", 0}];
 int u1 = 1 << fops[{"up", 1}];
 int u2 = 1 << fops[{"up", 2}];
 int d0 = 1 << fops[{"dn", 0}];
 int d1 = 1 << fops[{"dn", 1}];
 int d2 = 1 << fops[{"dn", 2}];

 // Expected classification of states
 std::set<std::set<int>> ref_cl{
     // N=0
     {0},
     // N=1
     {d0},
     {d1},
     {d2},
     {u0},
     {u1},
     {u2},
     // N=2, same spin
     {d0 + d1},
     {d0 + d2},
     {d1 + d2},
     {u0 + u1},
     {u0 + u2},
     {u1 + u2},
     // N=2, pair hopping
     {d0 + u0, d1 + u1, d2 + u2},
     // N=2, spin flip
     {d0 + u1, d1 + u0},
     {d0 + u2, d2 + u0},
     {d1 + u2, d2 + u1},
     // N=3
     {d0 + d1 + d2},
     {u0 + u1 + u2},
     {d0 + d1 + u0, d1 + d2 + u2},
     {d0 + d2 + u0, d1 + d2 + u1},
     {d0 + d1 + u1, d0 + d2 + u2},
     {d0 + u0 + u1, d2 + u1 + u2},
     {d1 + u0 + u1, d2 + u0 + u2},
     {d0 + u0 + u2, d1 + u1 + u2},
     {d1 + d2 + u0, d0 + d2 + u1, d0 + d1 + u2},
     {d2 + u0 + u1, d0 + u1 + u2, d1 + u0 + u2},
     // N=4, 2 holes with the same spin
     {d2 + u0 + u1 + u2},
     {d1 + u0 + u1 + u2},
     {d0 + u0 + u1 + u2},
     {d0 + d1 + d2 + u2},
     {d0 + d1 + d2 + u1},
     {d0 + d1 + d2 + u0},
     // N=4, pair hopping
     {d1 + d2 + u1 + u2, d0 + d2 + u0 + u2, d0 + d1 + u0 + u1},
     // N=4, spin flip
     {d1 + d2 + u0 + u2, d0 + d2 + u1 + u2},
     {d1 + d2 + u0 + u1, d0 + d1 + u1 + u2},
     {d0 + d2 + u0 + u1, d0 + d1 + u0 + u2},
     // N=5
     {d1 + d2 + u0 + u1 + u2},
     {d0 + d2 + u0 + u1 + u2},
     {d0 + d1 + u0 + u1 + u2},
     {d0 + d1 + d2 + u1 + u2},
     {d0 + d1 + d2 + u0 + u2},
     {d0 + d1 + d2 + u0 + u1},
     // N=6
     {d0 + d1 + d2 + u0 + u1 + u2}};

 if (cl != ref_cl) return -1;

 ////////////////////////////////////
 // Part II: Check matrix elements //
 ////////////////////////////////////

 // Calculated matrix elements
 std::vector<std::set<std::tuple<int, int, double>>> v_melem(SP.n_subspaces());
 auto melem_map = SP.get_matrix_elements();
 for (auto x : melem_map) {
  v_melem[SP.lookup_basis_state(x.first.first)].insert(std::make_tuple(x.first.first, x.first.second, x.second));
 }
 std::set<std::set<std::tuple<int, int, double>>> melem(v_melem.cbegin(), v_melem.cend());

 auto t = [](int from, int to, double val) { return std::make_tuple(from, to, val); };

 // Expected matrix elements
 std::set<std::set<std::tuple<int, int, double>>> ref_melem{
     // N=0
     std::set<std::tuple<int, int, double>>{},
     // N=1
     {t(d0, d0, -mu)}, {t(d1, d1, -mu)}, {t(d2, d2, -mu)}, {t(u0, u0, -mu)}, {t(u1, u1, -mu)}, {t(u2, u2, -mu)},
     // N=2, same spin
     {t(d0 + d1, d0 + d1, -2 * mu + U - 3 * J)}, {t(d0 + d2, d0 + d2, -2 * mu + U - 3 * J)},
     {t(d1 + d2, d1 + d2, -2 * mu + U - 3 * J)}, {t(u0 + u1, u0 + u1, -2 * mu + U - 3 * J)},
     {t(u0 + u2, u0 + u2, -2 * mu + U - 3 * J)}, {t(u1 + u2, u1 + u2, -2 * mu + U - 3 * J)},
     // N=2, pair hopping
     {t(d0 + u0, d0 + u0, -2 * mu + U), t(d1 + u1, d1 + u1, -2 * mu + U), t(d2 + u2, d2 + u2, -2 * mu + U),
      t(d0 + u0, d1 + u1, J),           t(d0 + u0, d2 + u2, J),           t(d1 + u1, d2 + u2, J),
      t(d1 + u1, d0 + u0, J),           t(d2 + u2, d0 + u0, J),           t(d2 + u2, d1 + u1, J)},
     // N=2, spin flip
     {t(d0 + u1, d0 + u1, -2 * mu + U - 2 * J), t(d1 + u0, d1 + u0, -2 * mu + U - 2 * J),
      t(d0 + u1, d1 + u0, J),                   t(d1 + u0, d0 + u1, J)},
     {t(d0 + u2, d0 + u2, -2 * mu + U - 2 * J), t(d2 + u0, d2 + u0, -2 * mu + U - 2 * J),
      t(d0 + u2, d2 + u0, J),                   t(d2 + u0, d0 + u2, J)},
     {t(d1 + u2, d1 + u2, -2 * mu + U - 2 * J), t(d2 + u1, d2 + u1, -2 * mu + U - 2 * J),
      t(d1 + u2, d2 + u1, J),                   t(d2 + u1, d1 + u2, J)},
     // N=3
     {t(d0 + d1 + d2, d0 + d1 + d2, -3 * mu + 3 * U - 9 * J)}, {t(u0 + u1 + u2, u0 + u1 + u2, -3 * mu + 3 * U - 9 * J)},
     {t(d0 + d1 + u0, d0 + d1 + u0, -3 * mu + 3 * U - 5 * J), t(d1 + d2 + u2, d1 + d2 + u2, -3 * mu + 3 * U - 5 * J),
      t(d0 + d1 + u0, d1 + d2 + u2, -J),                      t(d1 + d2 + u2, d0 + d1 + u0, -J)},
     {t(d0 + d2 + u0, d0 + d2 + u0, -3 * mu + 3 * U - 5 * J), t(d1 + d2 + u1, d1 + d2 + u1, -3 * mu + 3 * U - 5 * J),
      t(d0 + d2 + u0, d1 + d2 + u1, J),                       t(d1 + d2 + u1, d0 + d2 + u0, J)},
     {t(d0 + d1 + u1, d0 + d1 + u1, -3 * mu + 3 * U - 5 * J), t(d0 + d2 + u2, d0 + d2 + u2, -3 * mu + 3 * U - 5 * J),
      t(d0 + d1 + u1, d0 + d2 + u2, J),                       t(d0 + d2 + u2, d0 + d1 + u1, J)},
     {t(d0 + u0 + u1, d0 + u0 + u1, -3 * mu + 3 * U - 5 * J), t(d2 + u1 + u2, d2 + u1 + u2, -3 * mu + 3 * U - 5 * J),
      t(d0 + u0 + u1, d2 + u1 + u2, -J),                      t(d2 + u1 + u2, d0 + u0 + u1, -J)},
     {t(d1 + u0 + u1, d1 + u0 + u1, -3 * mu + 3 * U - 5 * J), t(d2 + u0 + u2, d2 + u0 + u2, -3 * mu + 3 * U - 5 * J),
      t(d1 + u0 + u1, d2 + u0 + u2, J),                       t(d2 + u0 + u2, d1 + u0 + u1, J)},
     {t(d0 + u0 + u2, d0 + u0 + u2, -3 * mu + 3 * U - 5 * J), t(d1 + u1 + u2, d1 + u1 + u2, -3 * mu + 3 * U - 5 * J),
      t(d0 + u0 + u2, d1 + u1 + u2, J),                       t(d1 + u1 + u2, d0 + u0 + u2, J)},
     {t(d1 + d2 + u0, d1 + d2 + u0, -3 * mu + 3 * U - 7 * J), t(d0 + d2 + u1, d0 + d2 + u1, -3 * mu + 3 * U - 7 * J),
      t(d0 + d1 + u2, d0 + d1 + u2, -3 * mu + 3 * U - 7 * J), t(d1 + d2 + u0, d0 + d2 + u1, J),
      t(d0 + d2 + u1, d1 + d2 + u0, J),                       t(d1 + d2 + u0, d0 + d1 + u2, -J),
      t(d0 + d1 + u2, d1 + d2 + u0, -J),                      t(d0 + d2 + u1, d0 + d1 + u2, J),
      t(d0 + d1 + u2, d0 + d2 + u1, J)},
     {t(d2 + u0 + u1, d2 + u0 + u1, -3 * mu + 3 * U - 7 * J), t(d0 + u1 + u2, d0 + u1 + u2, -3 * mu + 3 * U - 7 * J),
      t(d1 + u0 + u2, d1 + u0 + u2, -3 * mu + 3 * U - 7 * J), t(d2 + u0 + u1, d0 + u1 + u2, -J),
      t(d0 + u1 + u2, d2 + u0 + u1, -J),                      t(d2 + u0 + u1, d1 + u0 + u2, J),
      t(d1 + u0 + u2, d2 + u0 + u1, J),                       t(d0 + u1 + u2, d1 + u0 + u2, J),
      t(d1 + u0 + u2, d0 + u1 + u2, J)},
     // N=4, 2 holes with the same spin
     {t(d2 + u0 + u1 + u2, d2 + u0 + u1 + u2, -4 * mu + 6 * U - 13 * J)},
     {t(d1 + u0 + u1 + u2, d1 + u0 + u1 + u2, -4 * mu + 6 * U - 13 * J)},
     {t(d0 + u0 + u1 + u2, d0 + u0 + u1 + u2, -4 * mu + 6 * U - 13 * J)},
     {t(d0 + d1 + d2 + u0, d0 + d1 + d2 + u0, -4 * mu + 6 * U - 13 * J)},
     {t(d0 + d1 + d2 + u1, d0 + d1 + d2 + u1, -4 * mu + 6 * U - 13 * J)},
     {t(d0 + d1 + d2 + u2, d0 + d1 + d2 + u2, -4 * mu + 6 * U - 13 * J)},
     // N=4, pair hopping
     {t(d1 + d2 + u1 + u2, d1 + d2 + u1 + u2, -4 * mu + 6 * U - 10 * J),
      t(d0 + d2 + u0 + u2, d0 + d2 + u0 + u2, -4 * mu + 6 * U - 10 * J),
      t(d0 + d1 + u0 + u1, d0 + d1 + u0 + u1, -4 * mu + 6 * U - 10 * J),
      t(d1 + d2 + u1 + u2, d0 + d2 + u0 + u2, J),
      t(d0 + d2 + u0 + u2, d1 + d2 + u1 + u2, J),
      t(d1 + d2 + u1 + u2, d0 + d1 + u0 + u1, J),
      t(d0 + d1 + u0 + u1, d1 + d2 + u1 + u2, J),
      t(d0 + d2 + u0 + u2, d0 + d1 + u0 + u1, J),
      t(d0 + d1 + u0 + u1, d0 + d2 + u0 + u2, J)},
     // N=4, spin flip
     {t(d1 + d2 + u0 + u2, d1 + d2 + u0 + u2, -4 * mu + 6 * U - 12 * J),
      t(d0 + d2 + u1 + u2, d0 + d2 + u1 + u2, -4 * mu + 6 * U - 12 * J),
      t(d1 + d2 + u0 + u2, d0 + d2 + u1 + u2, J),
      t(d0 + d2 + u1 + u2, d1 + d2 + u0 + u2, J)},
     {t(d1 + d2 + u0 + u1, d1 + d2 + u0 + u1, -4 * mu + 6 * U - 12 * J),
      t(d0 + d1 + u1 + u2, d0 + d1 + u1 + u2, -4 * mu + 6 * U - 12 * J),
      t(d1 + d2 + u0 + u1, d0 + d1 + u1 + u2, J),
      t(d0 + d1 + u1 + u2, d1 + d2 + u0 + u1, J)},
     {t(d0 + d2 + u0 + u1, d0 + d2 + u0 + u1, -4 * mu + 6 * U - 12 * J),
      t(d0 + d1 + u0 + u2, d0 + d1 + u0 + u2, -4 * mu + 6 * U - 12 * J),
      t(d0 + d2 + u0 + u1, d0 + d1 + u0 + u2, J),
      t(d0 + d1 + u0 + u2, d0 + d2 + u0 + u1, J)},
     // N=5
     {t(d1 + d2 + u0 + u1 + u2, d1 + d2 + u0 + u1 + u2, -5 * mu + 10 * U - 20 * J)},
     {t(d0 + d2 + u0 + u1 + u2, d0 + d2 + u0 + u1 + u2, -5 * mu + 10 * U - 20 * J)},
     {t(d0 + d1 + u0 + u1 + u2, d0 + d1 + u0 + u1 + u2, -5 * mu + 10 * U - 20 * J)},
     {t(d0 + d1 + d2 + u1 + u2, d0 + d1 + d2 + u1 + u2, -5 * mu + 10 * U - 20 * J)},
     {t(d0 + d1 + d2 + u0 + u2, d0 + d1 + d2 + u0 + u2, -5 * mu + 10 * U - 20 * J)},
     {t(d0 + d1 + d2 + u0 + u1, d0 + d1 + d2 + u0 + u1, -5 * mu + 10 * U - 20 * J)},
     // N=6
     {t(d0 + d1 + d2 + u0 + u1 + u2, d0 + d1 + d2 + u0 + u1 + u2, -6 * mu + 15 * U - 30 * J)}};

 // many_body_operator<int> cannot be instantiated for some reason...
 // So I have to compare text representations, because comparison of double doesn't work
 std::stringstream melem_text, ref_melem_text;
 for (auto sp : ref_melem) {
  for (auto s : sp) ref_melem_text << std::get<0>(s) << " -> " << std::get<1>(s) << ": " << std::get<2>(s) << '\t';
  ref_melem_text << std::endl;
 }
 for (auto sp : melem) {
  for (auto s : sp) melem_text << std::get<0>(s) << " -> " << std::get<1>(s) << ": " << std::get<2>(s) << '\t';
  melem_text << std::endl;
 }
 if (melem_text.str() != ref_melem_text.str()) return -1;

 //////////////////////////////////////
 // Part III: Check merged subspaces //
 //////////////////////////////////////
 std::vector<decltype(Hop)> Cd, C;
 Cd.reserve(6);
 C.reserve(6);

 for (int o = 0; o < 3; ++o)
  for (auto spin : {"up", "dn"}) {
   Cd.emplace_back(c_dag(spin, o), fops);
   C.emplace_back(c(spin, o), fops);

   SP.merge_subspaces(Cd.back(), C.back());
  }

 // Calculated classification of states
 // std::set<std::set<int>> cl;
 v_cl.clear();
 v_cl.resize(SP.n_subspaces());
 foreach(SP, [&v_cl](int st, int spn) { v_cl[spn].insert(st); });
 cl.clear();
 cl.insert(v_cl.cbegin(), v_cl.cend());

 // Are all operators subspace-to-subspace mappings?
 decltype(Cd) all_ops;
 all_ops.reserve(12);
 for (auto const& op : Cd) all_ops.push_back(op);
 for (auto const& op : C) all_ops.push_back(op);

 for (auto op : all_ops) {
  for (auto i_sp : cl) {
   std::set<int> f_sp;
   for (auto i : i_sp) {
    st(i) = 1.0;
    auto final_state = op(st);
    foreach(final_state, [&f_sp](int f, double amplitude) {
     if (std::fabs(amplitude) < 1e-10) return;
     f_sp.insert(f);
    });
    st(i) = 0;
   }

   // op maps i_sp onto zero
   if (f_sp.size() == 0) continue;

   // Check if op maps i_sp to only one subspace
   int n = 0;
   for (auto f_sp_ref : cl)
    if (std::includes(f_sp_ref.cbegin(), f_sp_ref.cend(), f_sp.cbegin(), f_sp.cend())) n++;
   if (n != 1) return -1;
  }
 }

 return 0;
}
