#include "triqs/clef.hpp"

namespace tql = triqs::clef;
// This macro just prints its argument literaly, and then its value
#define PRINT(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl;

int main() { 

 tql::placeholder <1> x_; 
 tql::placeholder <2> y_; 

 PRINT ( x_  + 2*y_ );
 PRINT (eval(x_  + 2*y_ , x_ = 1, y_ = 2));
 PRINT (eval(x_  + 2*y_ , x_ = 1));
 PRINT (eval(x_  + 2*y_ , x_ = x_ + y_));

}

