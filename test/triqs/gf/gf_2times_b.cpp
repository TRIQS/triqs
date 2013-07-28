#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <triqs/gf/gf.hpp>
#include <triqs/gf/two_real_times.hpp>
#include <complex>
using namespace triqs::gfs;
using namespace std;
using triqs::arrays::make_shape;

int main(){

 double dt=0.001;
 double tmax=0.005;
 int nt=tmax/dt;
 auto  R= make_gf<two_real_times> (tmax,nt,make_shape(1,1));//results

 for(auto point:R.mesh()) R[point]=0;

 const auto rg = on_mesh(R);
 R.on_mesh(1,1) = 10;

 std::cout << rg (1,1)<< std::endl ;
 std::cout << R.on_mesh(1,1)<< std::endl ;
 std::cout << R(0.001,0.001)<< std::endl ;

 auto R2 = R;

 //on_mesh(R2)(1,1) =  on_mesh(R)(1,1) * on_mesh(R)(1,1);
 on_mesh(R2)(1,1)() =  on_mesh(R)(1,1) * on_mesh(R)(1,1);

 std::cout  << on_mesh(R2)(1,1)<< std::endl; 
 return 0;
};
