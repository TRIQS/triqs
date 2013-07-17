#include "./speed_tester.hpp"
#include <triqs/clef.hpp>
#include "./clef_common.hpp"

double S =0;
struct eval_exp1 { 

 void operator()() {
  F1 f(2); 
  double y=2;
  //double x=1,y=2;

  auto expr = f(x_)   + 2*y_;

  double s=0;
  for (int u =0; u<500000; ++u)  { s+=eval( expr,x_=u/10000.0, y_=y);}
  S += s/10000.0;
  //std::cout<< "s =" <<s <<std::endl;
 }
};

struct eval_exp_c { 

 void operator()() {
  F1 f(2); 
  //double x=1,y=2;

  triqs::clef::function <double (double)>  f7;
  f7(x_) = f(x_) + 2*x_;

  triqs::clef::function <double (double)> f2;
  f2(x_) = f(x_)  + 2*x_;

  double s=0;
  for (int u =0; u<500000; ++u)  { s+= f2(u/10000.0);}
  S += s/10000.0;
  //std::cout<< "s =" <<s <<std::endl;
 }
};

struct eval_manu1 { 

 void operator()() {
  F1 f(2); 
  double x=1,y=2;

  double s=0;
  for (int u =0; u<500000; ++u)  { x = u/10000.0; s+= f(x) + 2*y; }
  //std::cout<< "s =" <<s <<std::endl;
  S += s/10000.0;
 }
};


int main() { 

 const int N = 10000;
 speed_tester<eval_exp1>(N);
 speed_tester<eval_exp_c>(N);
 speed_tester<eval_manu1>(N);

}

