#include <triqs/parameters.hpp>
#include <iostream>
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

using namespace triqs::utility;


int main() {

 parameters P;

 P["a"] = long(1);
 P["d"] = 2.7;
 P["s"] = std::string("-14.3");

 long j = P["a"];
 double x = P["d"];
 double y = P["a"];
 double z = P["s"];

 std::cout  << j << std::endl ;
 std::cout  << x << std::endl;
 std::cout  << y << std::endl ; 
 std::cout  << z << std::endl ; 

 return 0;
}
