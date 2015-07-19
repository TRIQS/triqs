#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
#include <triqs/gfs/functions/functions.hpp>

using namespace triqs::gfs;

void print(gf<imtime> const & gt){
 for(auto tau : gt.mesh()){
  std::cout << tau << " ---> " << gt[tau](0,0) << std::endl;
 }
}

int main() {

 double beta = 1;

 auto gt1 =  gf<imtime> {{beta, Fermion, 9}, {1,1}};
 auto gt2 =  gf<imtime> {{beta, Fermion, 10}, {1,1}};

 triqs::clef::placeholder<0> tau_;
 make_gf_view_without_tail(gt1)(tau_) << -0.25 - 0.5*tau_;
 make_gf_view_without_tail(gt2)(tau_) << -0.25 - 0.5*(beta-tau_);

 std::cout << "gt1:" << std::endl;
 print(gt1);

 auto gt1_new = rebinning_tau(gt1,3);
 std::cout << "gt1_new:" << std::endl;
 print(gt1_new);

 std::cout << "gt2:" << std::endl;
 print(gt2);

 auto gt2_new = rebinning_tau(gt2,3);
 std::cout << "gt2_new:" << std::endl;
 print(gt2_new);

 // Apply rebinning_tau to all blocks in a full GF
 auto bgf = make_block_gf({"up","dn"},{gt1,gt2});
 auto rebin = [](gf_const_view<imtime> const& g) { return rebinning_tau(g,3); };
 auto res = map(rebin,bgf);

 std::cout << "bgf_new_up:" << std::endl;
 print(res[0]);
 std::cout << "bgf_new_dn:" << std::endl;
 print(res[1]);

 if (get_block_names(res) != std::vector<std::string>{"up","dn"}) TRIQS_RUNTIME_ERROR<< "Block names are wrong";
 std::cerr << " block_names : " << get_block_names(res)[0] <<  "  "<<std::endl;

 return 0;
}

