#include <triqs/utility/first_include.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/hilbert_space.hpp>
#include <triqs/hilbert_space/imperative_operator.hpp>
#include <triqs/hilbert_space/state.hpp>
#include <iostream>

using namespace triqs::hilbert_space;

int main() {
 try { 
  std::cout << std::endl << "Part I: the fundamental_operator_set class" << std::endl << std::endl;

  fundamental_operator_set f1(std::vector<int>(2,4));
  for (int i=0; i<2; ++i)
    for (int j=0; j<4; ++j)
      std::cout << "(" << i << "," << j << ") --> " << f1[{i,j}] << std::endl;
  std::cout << "n operators = " << f1.size() << std::endl;

  std::cout << std::endl;
  fundamental_operator_set f2;
  f2 = f1;
  std::cout << "(1,1) --> " << f2[{1,1}] << std::endl;

  std::cout << std::endl;
  fundamental_operator_set f3;
  for (int i=0; i<4; ++i) f3.insert(i);
  std::cout << "2 --> " << f3[{2}] << std::endl;
  std::cout << "n operators = " << f3.size() << std::endl;

  std::cout << std::endl;
  fundamental_operator_set f4;
  for (int i=0; i<2; ++i) f4.insert("up",i);
  for (int i=0; i<2; ++i) f4.insert("down",i);
  std::cout << "(down,0) --> " << f4[{"down",0}] << std::endl;
  std::cout << "n operators = " << f4.size() << std::endl;

  std::cout << std::endl << "Part II: the hilbert_space class" << std::endl << std::endl;

  hilbert_space h(f1);
  std::cout << "dim = " << h.size() << std::endl;
  std::cout << "fock state 130 is here: " << h.has_state(130) << std::endl;
  std::cout << "fock state 256 is here: " << h.has_state(256) << std::endl;
  std::cout << "fock state for index 120 = " << h.get_fock_state(120) << std::endl;
  std::cout << "index of fock state 120 = " << h.get_state_index(h.get_fock_state(120)) << std::endl;
  std::cout << "fock state for vacuum = " << h.get_fock_state(f1, std::set<fundamental_operator_set::indices_t>{}) << std::endl;
  std::cout << "fock state for c^+(0,1)c^+(1,3)|vac> = " << h.get_fock_state(f1,std::set<fundamental_operator_set::indices_t>{{0,1},{1,3}}) << std::endl;
  hilbert_space h2;
  h2 = h;
  std::cout << "dim = " << h.size() << std::endl;
  std::cout << std::endl;

  std::cout << std::endl << "Part III: the fock_state class" << std::endl << std::endl;

  hilbert_space h3(f3);

  fock_state_t fs1 = h3.get_fock_state(10);
  std::cout << fs1 << std::endl;
  fock_state_t fs2 = fs1;
  std::cout << fs2 << std::endl;
  std::cout << std::endl;

  std::cout << std::endl << "Part IV: the state operator" << std::endl << std::endl;

  fundamental_operator_set fop;
  for (int i=0; i<5; ++i) fop.insert("up",i);

  hilbert_space h_full(fop);
  state<hilbert_space,double, true> st(h_full);
  st(0) = 3.0;
  st(3) = 5.0;
  std::cout << "state is: " << st << std::endl;

  std::cout << std::endl << "Part V: the declarative operator" << std::endl << std::endl;

  using triqs::operators::c;
  using triqs::operators::c_dag;

  auto H = 3 * c_dag("up",1) * c("up",1) + 2 * c_dag("up",2) * c("up",2) + c("up",1) * c("up",2);
  std::cout << "H = " << H << std::endl;

  std::cout << std::endl << "Part VI: the imperative operator" << std::endl << std::endl;

  auto opH = imperative_operator<hilbert_space>(H, fop);

  state<hilbert_space, double, true> old_state(h_full);
  old_state(7) = 1.0;
  std::cout << "old state is: " << old_state << std::endl;

  auto new_state = opH(old_state);
  std::cout << "new state is: " << new_state << std::endl;

  // machine dependent apparently ...
  std::cerr << "size of hilbert: " << sizeof(h_full) << std::endl;
  std::cerr << "size of op: " << sizeof(opH) << std::endl;


  std::cout << std::endl << "Part VII: partial hilbert space" << std::endl << std::endl;

  auto Cdag = c_dag("up",2);

  fundamental_operator_set fop2;
  for (int i=0; i<5; ++i) fop2.insert("up",i);

  hilbert_space h4(f4);

  sub_hilbert_space phs0(0);
  phs0.add_fock_state(h4.get_fock_state(0)); // 000
  phs0.add_fock_state(h4.get_fock_state(1)); // 001
  phs0.add_fock_state(h4.get_fock_state(2)); // 010
  phs0.add_fock_state(h4.get_fock_state(3)); // 011

  std::cout << "phs0 has state 2: "  << phs0.has_state(h4.get_fock_state(2)) << std::endl;
  std::cout << "phs0 has state 6: "  << phs0.has_state(h4.get_fock_state(6)) << std::endl;

  sub_hilbert_space phs1(1);
  phs1.add_fock_state(h4.get_fock_state(4)); // 100
  phs1.add_fock_state(h4.get_fock_state(5)); // 101
  phs1.add_fock_state(h4.get_fock_state(6)); // 110
  phs1.add_fock_state(h4.get_fock_state(7)); // 111

  std::cout << "phs1 has state 2: "  << phs1.has_state(h4.get_fock_state(2)) << std::endl;
  std::cout << "phs1 has state 6: "  << phs1.has_state(h4.get_fock_state(6)) << std::endl;

  std::vector<int>  Cdagmap(2,-1);
  Cdagmap[phs0.get_index()] = phs1.get_index();
  std::vector<sub_hilbert_space> sub1{phs0, phs1};
  auto opCdag = imperative_operator<sub_hilbert_space, double, true>(Cdag, fop2, Cdagmap, &sub1 );

  state<sub_hilbert_space,double, false> start(phs0);

  std::cout << "operator is: " << Cdag << std::endl;

  start(0) = 1.0;
  start(1) = 2.0;
  start(2) = 3.0;
  start(3) = 4.0;

  std::cout << "old state is: " << start << std::endl;
  std::cout << "new state is: " << opCdag(start) << std::endl;

  std::cout << std::endl << "Part VIII: quartic operators" << std::endl << std::endl;

  {
  fundamental_operator_set FOPS;
  FOPS.insert("up",0);
  FOPS.insert("down",0);
  FOPS.insert("up",1);
  FOPS.insert("down",1);
  hilbert_space HS(FOPS);
  std::cerr  << " HS dimension "<< HS.size() << std::endl;

  triqs::operators::many_body_operator quartic_op;
  quartic_op = -1.0*c_dag("up",0)*c_dag("down",1)*c("up",1)*c("down",0);

  state<hilbert_space,double, false> st1(HS);
  st1(9) = 1.0; // 0110
  std::cout << "old state is: " << st1 << std::endl;
  std::cout << "operator is: " << quartic_op << std::endl;
  std::cout << "new state is: " << imperative_operator<hilbert_space>(quartic_op,FOPS)(st1) << std::endl;
  }

  std::cout << std::endl << "Part IX: state projection" << std::endl << std::endl;

  {
  fundamental_operator_set fop;
  for (int i=0; i<3; ++i) fop.insert("s",i);

  hilbert_space hs_full(fop);
  state<hilbert_space,double,true> st(hs_full);
  st(0) = 0.1;
  st(2) = 0.2;
  st(4) = 0.3;
  st(6) = 0.4;
  std::cout << "original state: " << st << std::endl;

  sub_hilbert_space hs(0);
  hs.add_fock_state(hs_full.get_fock_state(4));
  hs.add_fock_state(hs_full.get_fock_state(5));
  hs.add_fock_state(hs_full.get_fock_state(6));
  hs.add_fock_state(hs_full.get_fock_state(7));

  auto proj_st = project<state<sub_hilbert_space,double,false>>(st,hs);
  std::cout << "projected state: " << proj_st << std::endl;
  }
 }
 catch(std::exception const & e) { 
  std::cout  << e.what() << std::endl;
 }
 return 0;

}
