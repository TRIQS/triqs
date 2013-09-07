#include "./common.hpp"

double x=1,y=2;

template < typename Expr > 
void test1( Expr expr) { 
 TEST(expr); 
 TEST(eval( expr,  x_=5));
 std::cout<<"-------------"<<std::endl;
}

template < typename Expr > 
void test2( Expr const & expr) { 
 // std::cout << " type is " << triqs::utility::typeid_name(expr) << std::endl; 
 std::cout<< " ------ start  test2  -----------------"<<std::endl ;
 TEST(expr); 
 TEST(eval(expr,x_ =1, y_ =2));
 TEST(eval(expr,x_ =1)); 
 TEST(eval(expr,x_ =x_ + y_));
 TEST(eval( eval ( expr,x_ =x_ + y_),  x_ = 1, y_ = 2) );
}

int main() { 

 F1 f(7);

 test1( 5*x_) ;
 test2(x_  + 2*y_);
 test2(x_  + 2*y_ + x_);
 test2(x_/2.0  + 2*y_);
 test2( f(x_) );
 test2( f(x_)   + 2*y_);
 test2( 1/f(x_)   + 2*y_);
 
#ifdef LONG
 test2( 1/f(x_)   + 2*y_ + x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_);
#endif
#ifdef LONG2
 test2( 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_+ x_ + 2*x_ + 1/f(x_)   + 2*y_ + x_ );
#endif

 { 
  auto expr1 = x_ *2  ;
  auto myf1  = make_function(expr1, x_);  
  std::cout<< myf1(2) << " = "<< 4 << std::endl;

  auto expr = x_  + 2*y_;
  auto myf  = make_function(expr, x_,y_);  
  auto myf_r  = make_function(expr, y_,x_);  
 
  std::cout<< myf(2,5) << " = "<< 12 << std::endl;
  std::cout<< myf(5,2) << " = " << 9<<std::endl;
  std::cout<< myf_r(2,5) << " = "<< 9 <<std::endl;
  std::cout<< myf_r(5,2) << " = "<< 12 << std::endl;
  std::cout<<"-------------"<<std::endl;
 }


 { 
  // testing the LHS wrting on an object caught by ref
  F1 f(7);
  std::cerr  << " operator(double) still ok "<< f(2) << std::endl; 
  std::cout<< " f.v before assign "<<f.v<<" "<<  std::endl;
  f(x_ ) << 8*x_ ;
  //f(x_ + y_) << 8*x_ ;// leads to a compile error as expected
  // test.cpp:129:14: error: no viable overloaded '='
  // f(x_ + y_) << 8*x_ ;
  // ~~~~~~~~~~ ^ ~~~~
  std::cout<< " f.v after assign "<<f.v<<" "<<  std::endl;
  std::cout<<"-------------"<<std::endl;
  std::cerr  <<F1{9}(2,x_, F1{2})<<std::endl;
  auto expr = F1{9}(x_);
  expr << 7*x_;
  std::cerr  << expr << std::endl ;
  F1{9}(x_ ) << 8*x_ ;
  std::cerr<<"-------------"<<std::endl;

 }

 { 
  // testing fnt of 2 variables
  F2 ff;
  std::cout<<"expr = "<< (ff(x_,y_)  + 2*y_)<< std::endl;
  std::cout<<"eval(expr,x_ =1, y_ =2) =  "<<   eval(ff(x_,y_)  + 2*y_ , x_=x, y_=y)  << " and it should be "<< ff(x,y)  + 2*y <<std::endl;
  auto tmp =  ff(2.0, y_) ;

  std::cout<<" tmp =" << tmp<<std::endl;
  std::cout<<"another  =  "<<   eval( tmp , x_=x)  << std::endl;
  std::cout<<"another  =  "<<   eval( ff(x_,2) , x_=x)  <<std::endl;
  std::cout<<"-------------"<<std::endl;
 }

{ 
 // testing expression if
 TEST(eval( if_else( true , 2*x_ , y_) , x_=1, y_=3));
 TEST(eval( if_else( false , 2*x_ , y_) ,x_=1, y_=3));
 TEST(eval( if_else( x_>y_ , 2*x_ , y_) ,x_=1, y_=3));
}
 std::cout << (x_ < y_) <<std::endl;
}

