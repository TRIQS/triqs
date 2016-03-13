#include <triqs/test_tools/gfs.hpp>
#include <sstream>

#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/hilbert_space.hpp>
#include <triqs/hilbert_space/imperative_operator.hpp>
#include <triqs/hilbert_space/state.hpp>

using namespace triqs::hilbert_space;

template<typename T> std::string as_string(T x) {
 std::stringstream ss; ss << x;
 return ss.str();
}

TEST(hilbert_space, fundamental_operator_set) {
 fundamental_operator_set fop1(std::vector<int>(2,4));
 for (int i=0; i<2; ++i)
  for (int j=0; j<4; ++j) {
   EXPECT_EQ(4*i + j, (fop1[{i,j}]));
  }
 EXPECT_EQ(8, fop1.size());

 fundamental_operator_set fop2;
 fop2 = fop1;
 EXPECT_EQ(5, (fop2[{1,1}]));

 fundamental_operator_set fop3;
 for (int i=0; i<4; ++i) fop3.insert(i);
 EXPECT_EQ(2, (fop3[{2}]));
 EXPECT_EQ(4, fop3.size());

 fundamental_operator_set fop4;
 for (int i=0; i<2; ++i) fop4.insert("up",i);
 for (int i=0; i<2; ++i) fop4.insert("down",i);
 EXPECT_EQ(0, (fop4[{"down",0}]));
 EXPECT_EQ(4, fop4.size());
}

TEST(hilbert_space, hilbert_space) {
 fundamental_operator_set fop(std::vector<int>(2,4));

 using triqs::hilbert_space::hilbert_space;
 hilbert_space hs1(fop);
 EXPECT_EQ(256, hs1.size());

 EXPECT_EQ(1, hs1.has_state(130));        // Fock state 130 is here
 EXPECT_EQ(0, hs1.has_state(256));        // Fock state 256 is here
 EXPECT_EQ(120, hs1.get_fock_state(120)); // Fock state for index 120
 EXPECT_EQ(120, hs1.get_state_index(hs1.get_fock_state(120))); // index of fock state 120
 // Fock state for vacuum
 EXPECT_EQ(0, hs1.get_fock_state(fop, std::set<fundamental_operator_set::indices_t>{}));
 // Fock state for c^+(0,1)c^+(1,3)|vac>
 EXPECT_EQ(130, hs1.get_fock_state(fop,std::set<fundamental_operator_set::indices_t>{{0,1},{1,3}})); //

 hilbert_space hs2;
 hs2 = hs1;
 EXPECT_EQ(256, hs2.size());

 // HDF5
 auto hs_h5 = rw_h5(hs1, "hilbert_space");
 EXPECT_EQ(hs1, hs_h5);
}

TEST(hilbert_space, fock_state) {
 fundamental_operator_set fop;
 for (int i=0; i<4; ++i) fop.insert(i);

 using triqs::hilbert_space::hilbert_space;
 hilbert_space hs(fop);

 fock_state_t fs1 = hs.get_fock_state(10);
 EXPECT_EQ(10, fs1);
 fock_state_t fs2 = fs1;
 EXPECT_EQ(10, fs2);
}

TEST(hilbert_space, state) {
 fundamental_operator_set fop;
 for (int i=0; i<5; ++i) fop.insert("up",i);

 using triqs::hilbert_space::hilbert_space;
 hilbert_space h_full(fop);

 state<hilbert_space,double, true> st(h_full);
 st(0) = 3.0;
 st(3) = 5.0;
 EXPECT_EQ(" +(5)|3> +(3)|0>", as_string(st));
}

TEST(hilbert_space, imperative_operator) {
 fundamental_operator_set fop;
 for (int i=0; i<5; ++i) fop.insert("up",i);

 using triqs::hilbert_space::hilbert_space;
 hilbert_space h_full(fop);

 using triqs::operators::c;
 using triqs::operators::c_dag;

 auto H = 3 * c_dag("up",1) * c("up",1) + 2 * c_dag("up",2) * c("up",2) + c("up",1) * c("up",2);
 EXPECT_EQ("3*C^+(up,1)C(up,1) + 2*C^+(up,2)C(up,2) + -1*C(up,2)C(up,1)", as_string(H));

 auto opH = imperative_operator<hilbert_space>(H, fop);

 state<hilbert_space, double, true> old_state(h_full);
 old_state(7) = 1.0;
 EXPECT_EQ(" +(1)|7>", as_string(old_state));

 auto new_state = opH(old_state);
 EXPECT_EQ(" +(-1)|1> +(5)|7>", as_string(new_state));
}

TEST(hilbert_space, sub_hilbert_space) {
 using triqs::operators::c_dag;
 auto Cdag = c_dag("up",2);
 EXPECT_EQ("1*C^+(up,2)", as_string(Cdag));

 fundamental_operator_set fop1;
 for (int i=0; i<5; ++i) fop1.insert("up",i);
 fundamental_operator_set fop2;
 for (int i=0; i<2; ++i) fop2.insert("up",i);
 for (int i=0; i<2; ++i) fop2.insert("down",i);

 using triqs::hilbert_space::hilbert_space;
 hilbert_space hs2(fop2);

 sub_hilbert_space phs0(0);
 phs0.add_fock_state(hs2.get_fock_state(0)); // 000
 phs0.add_fock_state(hs2.get_fock_state(1)); // 001
 phs0.add_fock_state(hs2.get_fock_state(2)); // 010
 phs0.add_fock_state(hs2.get_fock_state(3)); // 011

 EXPECT_TRUE(phs0.has_state(hs2.get_fock_state(2))); // phs0 has state 2
 EXPECT_FALSE(phs0.has_state(hs2.get_fock_state(6))); // phs0 has state 6

 sub_hilbert_space phs1(1);
 phs1.add_fock_state(hs2.get_fock_state(4)); // 100
 phs1.add_fock_state(hs2.get_fock_state(5)); // 101
 phs1.add_fock_state(hs2.get_fock_state(6)); // 110
 phs1.add_fock_state(hs2.get_fock_state(7)); // 111

 EXPECT_FALSE(phs1.has_state(hs2.get_fock_state(2))); // phs1 has state 2
 EXPECT_TRUE(phs1.has_state(hs2.get_fock_state(6))); // phs1 has state 6

 std::vector<int> Cdagmap(2,-1);
 Cdagmap[phs0.get_index()] = phs1.get_index();
 std::vector<sub_hilbert_space> sub1{phs0, phs1};
 auto opCdag = imperative_operator<sub_hilbert_space, double, true>(Cdag, fop1, Cdagmap, &sub1);

 state<sub_hilbert_space,double, false> start(phs0);
 start(0) = 1.0;
 start(1) = 2.0;
 start(2) = 3.0;
 start(3) = 4.0;

 EXPECT_EQ(" +(1)|0> +(2)|1> +(3)|2> +(4)|3>", as_string(start));
 EXPECT_EQ(" +(1)|4> +(-2)|5> +(-3)|6> +(4)|7>", as_string(opCdag(start)));

 // HDF5
 auto hs_h5 = rw_h5(phs1, "sub_hilbert_space");
 EXPECT_EQ(phs1, hs_h5);
}

TEST(hilbert_space, QuarticOperators) {
 fundamental_operator_set fops;
 fops.insert("up",0);
 fops.insert("down",0);
 fops.insert("up",1);
 fops.insert("down",1);

 using triqs::hilbert_space::hilbert_space;
 hilbert_space hs(fops);
 EXPECT_EQ(16, hs.size());

 using triqs::operators::c;
 using triqs::operators::c_dag;
 auto quartic_op = -1.0*c_dag("up",0)*c_dag("down",1)*c("up",1)*c("down",0);

 state<hilbert_space,double, false> st1(hs);
 st1(9) = 1.0; // 0110
 EXPECT_EQ(" +(1)|9>", as_string(st1)); // old state
 EXPECT_EQ("1*C^+(down,1)C^+(up,0)C(up,1)C(down,0)", as_string(quartic_op)); // quartic operator
 EXPECT_EQ(" +(1)|6>", as_string(imperative_operator<hilbert_space>(quartic_op,fops)(st1))); // new state
}

TEST(hilbert_space, StateProjection) {
 fundamental_operator_set fop;
 for (int i=0; i<3; ++i) fop.insert("s",i);

 using triqs::hilbert_space::hilbert_space;
 hilbert_space hs_full(fop);

 state<hilbert_space,double,true> st(hs_full);
 st(0) = 0.1;
 st(2) = 0.2;
 st(4) = 0.3;
 st(6) = 0.4;
 EXPECT_EQ(" +(0.4)|6> +(0.3)|4> +(0.2)|2> +(0.1)|0>", as_string(st)); // original state

 sub_hilbert_space hs(0);
 hs.add_fock_state(hs_full.get_fock_state(4));
 hs.add_fock_state(hs_full.get_fock_state(5));
 hs.add_fock_state(hs_full.get_fock_state(6));
 hs.add_fock_state(hs_full.get_fock_state(7));

 auto proj_st = project<state<sub_hilbert_space,double,false>>(st,hs);
 EXPECT_EQ(" +(0.3)|4> +(0.4)|6>", as_string(proj_st)); // projected state
}

MAKE_MAIN;
