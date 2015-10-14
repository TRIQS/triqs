#include <triqs/utility/first_include.hpp>
#include <triqs/operators/util/extractors.hpp>

using namespace triqs::operators;
using namespace triqs::operators::util;

int main(){
 auto gf_struct = gf_struct_t({{"up", {0}}, {"down", {0}}});
 auto H = 0.1* n("up",0)*n("down",0)+ 0.5*n("up",1)*n("down",0) ;
 auto U_dict = extract_U_dict2(H);
 try{
  auto U_matrix = dict_to_matrix(U_dict, gf_struct);
 }
 catch(triqs::exception const & e) {
  std::cout << "Error: " << e.what();
 }

 auto H2 = 0.1* n("up",0)*n("down",0);
 auto U_dict2 = extract_U_dict2(H2);
 auto U_matrix2 = dict_to_matrix(U_dict2, gf_struct);
 std::cout << "U_matrix2 " << U_matrix2 << std::endl;

 auto U_dict4 = extract_U_dict4(H2);
 auto U_matrix4 = dict_to_matrix(U_dict4, gf_struct);
 std::cout << "U_matrix4 " << U_matrix4 << std::endl;

 auto H3 = -0.5*(n(0) + n(1)) + n(1)*n(0) + c_dag(1)*c(1)*c(2)*c(0);
 try{
  auto U_dict3 = extract_U_dict2(H3);
 }
 catch(triqs::exception const & e) {
  std::cout << "Error: " << e.what();
 }

 return 0;
}
