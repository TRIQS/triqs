#include "./common.hpp"

struct AA{ 
 double v; 
 AA(double v_): v(v_){} 
 AA(AA const &) = delete; // non copyable
 AA(AA &&x) : v(x.v) { std::cerr << "Moving AA "<< std::endl;}

 double operator() (double x) const { return 10*x;}

 TRIQS_CLEF_IMPLEMENT_LAZY_CALL(AA);

 template<typename Fnt> friend void triqs_clef_auto_assign (AA & x, Fnt f) { x.v++; std::cerr<< " called triqs_clef_auto_assign "<< f(100)<<std::endl;}
 
 friend std::ostream & operator<<(std::ostream & out, AA const & x) { return out<<"AA";}

 double my_method(double x) { return 2*x;}

#ifndef TRIQS_COMPILER_OBSOLETE_GCC
 TRIQS_CLEF_IMPLEMENT_LAZY_METHOD(AA,my_method);
#else
 TRIQS_CLEF_IMPLEMENT_LAZY_METHOD(AA,my_method,double);
#endif

};



int main() { 

 AA f{8};

 TEST(eval( f.my_method(x_), x_ = 10));


};

