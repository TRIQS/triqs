#include <triqs/clef/clef.hpp>
#include <triqs/clef/io.hpp>
using namespace triqs::clef;

triqs::clef::placeholder <1> x_; 
triqs::clef::placeholder <2> y_; 
triqs::clef::placeholder <3> z_; 
namespace tql= triqs::clef;

struct F1{ 
 double v; 
 F1(double v_): v(v_){} 
 F1(F1 const &) = delete; // non copyable
 double operator() (double x) const { return 10*x;}

 template< typename... Args>
  typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<const F1>,Args...>::type
  operator()( Args&&... args ) const { return  make_expr_call (std::ref(*this),args...);}

 template< typename... Args>
  typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<F1>,Args...>::type
  operator()( Args&&... args ) { return  make_expr_call (std::ref(*this),args...);}

 template<typename Fnt> friend void triqs_clef_auto_assign (F1 & x, Fnt f) { x.v++; std::cerr<< " called triqs_clef_auto_assign "<< f(100)<<std::endl;}
 friend std::ostream & operator<<(std::ostream & out, F1 const & x) { return out<<"F1";}
};

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl;
double x=1,y=2;

template < typename Expr > 
void test1( Expr expr) { 
 TEST(expr); 
 TEST(eval( expr,  x_=5));
 TEST(eval( expr,  y_=5, x_=9));
 TEST(eval( expr,  x_=5, y_=9));
 std::cout<<"-------------"<<std::endl;
}

template < typename Expr > 
void test2( Expr const & expr) { 
 // std::cout << " type is " << triqs::utility::typeid_name(expr) << std::endl; 
 std::cout<< " ------ start  test2  -----------------"<<std::endl ;
 TEST(expr);
 //TEST(expr._eval( x_ =1, y_ =2));
 TEST(eval(expr,x_ =1, y_ =2));
 TEST(eval(expr,x_ =1)); 
 TEST(eval(expr,x_ =x_ + y_));
 //TEST(eval( eval ( expr,x_ =x_ + y_),  x_ = 1, y_ = 2) );
 // std::cout<< " List of placeholder of the expression : "<< placeholder_list_as_string(expr) <<std::endl<<"-------------"<<std::endl;
}

int main() { 

 F1 f(7);

 test1( x_/2.0) ;
 test1( 5*x_) ;
 test2(x_  + y_);
 test2(x_  + 2*y_);
 test2(x_  + 2*y_ + x_);
 test2(x_/2.0  + 2*y_);
 test2(x_/2.0  + 2*y_ - x_);

 auto expr = x_ + y_ + x_ + 2*x_  + x_  + 2*y_  - x_+ x_ + 4*y_;
 std::cout  << ph_set<decltype(expr)>::value <<std::endl	;
 TEST(eval(expr,x_ =1, y_ =2));
 TEST(eval(expr,x_ =1)); 

 test2(x_  + 2*y_ - x_ + x_);
 test2( f(x_) );
 test2( f(x_)   + 2*y_);
 test2( 1/f(x_)   + 2*y_);

 //#define LONG
 //#define LONG2
#ifdef LONG
 test2( x_  + 2*y_ + x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_);
 //test2( 1/f(x_)   + 2*y_ + x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_);
#endif
#ifdef LONG2
 test2( 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_ + 1/f(x_)   + 2*y_ + x_ );
#endif

 { 
  // testing the LHS wrting on an object caught by ref
  F1 f(7);

  auto ex = f(x_);
  auto F = make_function(ex ,x_);
  std::cout << F(8) << std::endl;

  std::cout<< " f.v before assign "<<f.v<<" "<<  std::endl;
  f(x_ ) << 8*x_ ;
  //f(x_ + y_) << 8*x_ ;// leads to a compile error as expected
  // test.cpp:129:14: error: no viable overloaded '='
  // f(x_ + y_) = 8*x_ ;
  // ~~~~~~~~~~ ^ ~~~~
  std::cout<< " f.v after assign "<<f.v<<" "<<  std::endl;
  std::cout<<"-------------"<<std::endl;

 }


}

