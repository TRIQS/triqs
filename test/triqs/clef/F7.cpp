#include "./common.hpp"

struct F7 { 
 double v; 
 F7(double v_): v(v_){} 
 double operator() (int i1, int i2, int i3, int i4, int i5, int i6, int i7 ) const { return 10*i1;}
 
 template< typename... Args>
  typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<const F7>,Args...>::type
  operator()( Args&&... args ) const { return  make_expr_call (std::ref(*this),args...);}

 template< typename... Args>
  typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<F7>,Args...>::type
  operator()( Args&&... args ) { return  make_expr_call (std::ref(*this),args...);}

 template<typename Fnt> friend void triqs_clef_auto_assign (F7 & x, Fnt f) { x.v++; std::cerr<< " called triqs_clef_auto_assign "<< f(1,2,3,4,5,6,7)<<std::endl;}
 friend std::ostream & operator<<(std::ostream & out, F7 const & x) { return out<<"F7";}

};

triqs::clef::placeholder <1> x1_; 
triqs::clef::placeholder <2> x2_; 
triqs::clef::placeholder <3> x3_; 
triqs::clef::placeholder <4> x4_; 
triqs::clef::placeholder <5> x5_; 
triqs::clef::placeholder <6> x6_; 
triqs::clef::placeholder <7> x7_; 
triqs::clef::placeholder <8> x8_; 

int main() { 

 F7 f(7), g(8), h(7);
 TEST(tql::eval(f(x1_,x2_,x3_,x4_,x5_,x6_,x7_),x_ =1, y_ =2));
 
f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) << x1_ + x2_ + x3_ + x4_ + x5_ + x6_ + x7_  ;
f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) << x1_ + 2*x2_ + x3_ + x4_ + x5_ + x6_ + x7_  ;
f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) << x1_ + 2*x2_ + 4*x3_ + x4_ + x5_ + x6_ + x7_ + g(x1_,x2_,x3_,x4_,x5_,x6_,x7_)* h(x1_,x2_,x3_,x4_,x5_,x6_,x7_)  ;
f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) << x1_ + 2*x2_ + 4*x3_ + 8*x4_ + x5_ + x6_ + x7_ + g(x1_,x2_,x3_,x4_,x5_,x6_,x7_)* h(x1_,x2_,x3_,x4_,x5_,x6_,x7_)  ;

f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) << x1_ + x2_ -x3_ + x4_ + x5_ + x6_ + x7_  ;
f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) << x1_ + 2*x2_ + x3_ + x4_ - x5_ + x6_ + x7_  ;
f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) << x1_ + 2*x2_ + 4*x3_ + x4_ - x5_ + x6_ + x7_ + g(x1_,x2_,x3_,x4_,x5_,x6_,x7_)* h(x1_,x2_,x3_,x4_,x5_,x6_,x7_)  ;
f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) <<  g(x1_,x2_,x3_,x4_,x5_,x6_,x7_)/ h(x1_,x2_,x3_,x4_,x5_,x6_,x7_)   + g(x1_,x2_,x3_,x4_,x5_,x6_,x7_)* h(x1_,x2_,x3_,x4_,x5_,x6_,x7_)  ;

f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) << x1_ + x2_ -x3_ + x4_ + x5_ + x6_ - x7_  ;
f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) << x1_ + 2*x2_ + x3_ + x4_ - x5_ + x6_ - x7_  ;
f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) << x1_ + 2*x2_ + 4*x3_ + x4_ - x5_ + x6_ + x7_ - g(x1_,x2_,x3_,x4_,x5_,x6_,x7_)* h(x1_,x2_,x3_,x4_,x5_,x6_,x7_)  ;
f(x1_,x2_,x3_,x4_,x5_,x6_,x7_) <<  g(x1_,x2_,x3_,x4_,x5_,x6_,x7_)/ h(x1_,x2_,x3_,x4_,x5_,x6_,x7_)   - g(x1_,x2_,x3_,x4_,x5_,x6_,x7_)* h(x1_,x2_,x3_,x4_,x5_,x6_,x7_)  ;

}
