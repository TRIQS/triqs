#include "test_tools.hpp"

#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <vector>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/operators/util/extractors.hpp>

using namespace triqs::operators;
using namespace triqs::operators::util;
using namespace triqs::arrays;
using namespace triqs::hilbert_space;

using dcomplex = std::complex<double>;

double U1 = 2.0, U2 = 2.2, U3 = 2.4;
double eps1 = 1.0, eps2 = 1.1, eps3 = 1.2;
double t12 = 0.4, t23 = 0.5, t31 = 0.6;
double V12 = 1.8, V23 = 1.7, V31 = 1.6;

auto gf_struct = gf_struct_t({{"up", {1,2,3}}, {"dn", {1,2,3}}});
std::vector<indices_t> ind = {{"dn",1},{"dn",2},{"dn",3},{"up",1},{"up",2},{"up",3}};

// Test extract_h_dict
TEST(Extractors, extract_h_dict) {
 auto h0 = eps1*(n("up",1)+n("dn",1)) +
           eps2*(n("up",2)+n("dn",2)) +
           eps3*(n("up",3)+n("dn",3));
 auto hop = t12*(c_dag("up",1)*c("up",2) + c_dag("dn",1)*c("dn",2)) +
            t23*(c_dag("up",2)*c("up",3) + c_dag("dn",2)*c("dn",3)) +
            t31*(c_dag("up",3)*c("up",1) + c_dag("dn",3)*c("dn",1));
 h0 += hop + dagger(hop);

 array<double,2> h_matrix_ref
  {{eps1, t12,  t31,  .0,   .0,   .0  },
   {t12,  eps2, t23,  .0,   .0,   .0  },
   {t31,  t23,  eps3, .0,   .0,   .0  },
   {.0,   .0,   .0,   eps1, t12,  t31 },
   {.0,   .0,   .0,   t12,  eps2, t23 },
   {.0,   .0,   .0,   t31,  t23,  eps3}};

 dict2_t<real_or_complex> h_dict_ref;
 for(int i=0; i<6; ++i)
  for(int j=0; j<6; ++j){
   auto val = h_matrix_ref(i,j);
   if(val!=.0) h_dict_ref[std::make_tuple(ind[i],ind[j])] = val;
  }

 auto h_dict = extract_h_dict(h0);
 EXPECT_EQ(h_dict_ref,h_dict);
 EXPECT_CLOSE_ARRAY(h_matrix_ref,                   dict_to_matrix(h_dict,gf_struct));
 EXPECT_CLOSE_ARRAY(h_matrix_ref,                   dict_to_matrix<double>(h_dict,gf_struct));
 EXPECT_CLOSE_ARRAY((array<dcomplex,2>)h_matrix_ref,dict_to_matrix<dcomplex>(h_dict,gf_struct));
 EXPECT_CLOSE_ARRAY(h_matrix_ref,                   (array<double,2>)dict_to_variant_matrix(h_dict,gf_struct));

 auto nn = n("up",1)*n("dn",1);
 EXPECT_THROW(extract_h_dict(nn),triqs::exception);
 EXPECT_NO_THROW(extract_h_dict(nn,true));

 auto cc = c("up",1)*c("dn",2);
 EXPECT_THROW(extract_h_dict(cc),triqs::exception);
 EXPECT_NO_THROW(extract_h_dict(cc,true));

 auto wrong_index = n("up",1) + n("dn",5);
 EXPECT_THROW(dict_to_matrix(extract_h_dict(wrong_index),gf_struct),triqs::exception);
}

TEST(Extractors, extract_U_dict2) {
 auto h_int = U1*n("up",1)*n("dn",1) +
              U2*n("up",2)*n("dn",2) +
              U3*n("up",3)*n("dn",3);
 auto N1 = n("up",1)+n("dn",1);
 auto N2 = n("up",2)+n("dn",2);
 auto N3 = n("up",3)+n("dn",3);
 h_int += V12*N1*N2 + V23*N2*N3 + V31*N1*N3;

 array<double,2> U_matrix2_ref
  {{.0,   V12,  V31,  U1,   V12,  V31},
   {V12,  .0,   V23,  V12,  U2,   V23},
   {V31,  V23,  .0,   V31,  V23,  U3 },
   {U1,   V12,  V31,  .0,   V12,  V31},
   {V12,  U2,   V23,  V12,  .0,   V23},
   {V31,  V23,  U3,   V31,  V23,  .0 }};

 dict2_t<real_or_complex> U_dict2_ref;
 for(int i=0; i<6; ++i)
  for(int j=0; j<6; ++j){
   auto val = U_matrix2_ref(i,j);
   if(val!=.0) U_dict2_ref[std::make_tuple(ind[i],ind[j])] = val;
  }

 auto U_dict2 = extract_U_dict2(h_int);
 EXPECT_EQ(U_dict2_ref,U_dict2);
 EXPECT_CLOSE_ARRAY(U_matrix2_ref,                   dict_to_matrix(U_dict2,gf_struct));
 EXPECT_CLOSE_ARRAY(U_matrix2_ref,                   dict_to_matrix<double>(U_dict2,gf_struct));
 EXPECT_CLOSE_ARRAY((array<dcomplex,2>)U_matrix2_ref,dict_to_matrix<dcomplex>(U_dict2,gf_struct));
 EXPECT_CLOSE_ARRAY(U_matrix2_ref,                   (array<double,2>)dict_to_variant_matrix(U_dict2,gf_struct));

 auto quadratic = n("up",1) + n("dn",1);
 EXPECT_THROW(extract_U_dict2(quadratic),triqs::exception);
 EXPECT_NO_THROW(extract_U_dict2(quadratic,true));

 auto non_nn = c_dag("up",1)*c_dag("dn",1)*c("up",3)*c("dn",2);
 EXPECT_THROW(extract_U_dict2(non_nn),triqs::exception);
 EXPECT_NO_THROW(extract_U_dict2(non_nn,true));

 auto wrong_index = n("up",4)*n("dn",2);
 EXPECT_THROW(dict_to_matrix(extract_U_dict2(wrong_index),gf_struct),triqs::exception);
}

TEST(Extractors, extract_U_dict4) {
 auto h_int = U1*n("up",1)*n("dn",1) +
              U2*n("up",2)*n("dn",2) +
              U3*n("up",3)*n("dn",3);
 auto N1 = n("up",1)+n("dn",1);
 auto N2 = n("up",2)+n("dn",2);
 auto N3 = n("up",3)+n("dn",3);
 h_int += V12*N1*N2 + V23*N2*N3 + V31*N1*N3;

 array<double,2> U_matrix2_ref
  {{.0,   V12,  V31,  U1,   V12,  V31},
   {V12,  .0,   V23,  V12,  U2,   V23},
   {V31,  V23,  .0,   V31,  V23,  U3 },
   {U1,   V12,  V31,  .0,   V12,  V31},
   {V12,  U2,   V23,  V12,  .0,   V23},
   {V31,  V23,  U3,   V31,  V23,  .0 }};

 dict4_t<real_or_complex> U_dict4_ref;
 array<double,4> U_matrix4_ref(6,6,6,6);
 U_matrix4_ref() = 0;
 for(int i=0; i<6; ++i)
  for(int j=0; j<6; ++j){
   auto val = U_matrix2_ref(i,j);
   if(val!=.0) {
    U_dict4_ref[std::make_tuple(ind[i],ind[j],ind[i],ind[j])] = 0.5*val;
    U_dict4_ref[std::make_tuple(ind[j],ind[i],ind[j],ind[i])] = 0.5*val;
    U_dict4_ref[std::make_tuple(ind[i],ind[j],ind[j],ind[i])] = -0.5*val;
    U_dict4_ref[std::make_tuple(ind[j],ind[i],ind[i],ind[j])] = -0.5*val;

    U_matrix4_ref(i,j,i,j) = U_matrix4_ref(j,i,j,i) = 0.5*val;
    U_matrix4_ref(i,j,j,i) = U_matrix4_ref(j,i,i,j) = -0.5*val;
   }
  }

 auto U_dict4 = extract_U_dict4(h_int);
 EXPECT_EQ(U_dict4_ref,U_dict4);
 EXPECT_CLOSE_ARRAY(U_matrix4_ref,                   dict_to_matrix(U_dict4,gf_struct));
 EXPECT_CLOSE_ARRAY(U_matrix4_ref,                   dict_to_matrix<double>(U_dict4,gf_struct));
 EXPECT_CLOSE_ARRAY((array<dcomplex,4>)U_matrix4_ref,dict_to_matrix<dcomplex>(U_dict4,gf_struct));
 EXPECT_CLOSE_ARRAY(U_matrix4_ref,                   (array<double,4>)dict_to_variant_matrix(U_dict4,gf_struct));

 auto quadratic = n("up",1) + n("dn",1);
 EXPECT_THROW(extract_U_dict4(quadratic),triqs::exception);
 EXPECT_NO_THROW(extract_U_dict4(quadratic,true));

 auto non_conserving = c_dag("up",1)*c_dag("dn",1)*c_dag("up",3)*c("dn",2);
 EXPECT_THROW(extract_U_dict4(non_conserving),triqs::exception);
 EXPECT_NO_THROW(extract_U_dict4(non_conserving,true));

 auto wrong_index = n("up",4)*n("dn",2);
 EXPECT_THROW(dict_to_matrix(extract_U_dict4(wrong_index),gf_struct),triqs::exception);
}

MAKE_MAIN;
