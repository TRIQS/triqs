#include "./common.hpp"

int main() { 

 { 
  auto r =   x_ >> 2 ;
  auto r3 =   r(3) ;
  std::cout<<"expr = "<< r3 << std::endl;
  std::cout<<"-------------"<<std::endl;
  }
{ 
  auto  expr  =  2*x_  + 1;
  auto r=  make_function(expr ,  x_) ;
  auto r4 =   x_ >> expr ;
  std::cerr<<"expr = "<< r4 << std::endl;
  auto r3 =   r(3) ;
  std::cout<<"expr = "<< r3 << std::endl;
  std::cout<<"-------------"<<std::endl;
  }
 { 
  TEST( (y_ >>  x_  + 2*y_) );
  TEST( x_ >> (y_ >>  x_  + 2*y_ ));
  auto  r = y_ >>  x_  + 2*y_ ;
   std::cout<<" r >> = "<< r << std::endl;
  auto  r2 =  x_ >> r ;
  std::cout<<" r2 >> = "<< r2 << std::endl;
 } 
 { 
  auto  expr  =  x_  + 2*y_;
  std::cout<<"expr = "<< expr<< std::endl;
  auto r=  make_function(expr ,  x_) ;
  auto r2=  tql::eval( r  ,  y_ = 2) ;
  auto r3 =   r2(3) ;
  std::cout<<"expr = "<< r3 << std::endl;
  TEST( tql::eval ( make_function(x_ + 2*y_ , x_) , y_ = 2) (3));
  TEST( make_function(x_ + 2 , x_)  (3));
  std::cout<<"-------------"<<std::endl;
 }

{ 
  auto  expr  =  x_  + 2*y_ + z_;
  std::cout<<"expr = "<< expr<< std::endl;
 }

}

