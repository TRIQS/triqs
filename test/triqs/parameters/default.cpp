#include <triqs/parameters.hpp>
#include <iostream>
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

using namespace triqs::utility;

int main() {

 parameters P,P2;
 parameter_defaults pdef;

 pdef.required ( "A", int(), "really ?") ;

 pdef.optional( "B", short(0), " short ")
  .optional( "C", 1u, " unsigned short ")
  .optional( "D", int(2), " int ")
  .optional( "E", 3u, " unsigned int ")
  .optional( "F", long(4), " long ")
  .optional( "G", 5ll, " long ")
  .optional( "H", float(6), " float ")
  .optional( "I", double(7.8), " doube ")
  .optional( "K", std::complex<double>(12), " double complex ")
  .optional( "L", std::string("13"), " string ")
  .optional( "M", std::vector<double> { 1,4 }, " vector ")
  .optional( "N", double(15), "")
  .optional( "W", int(16), "")
  ;

 P["a"] = long(1);
 P["d"] = 2.7;
 P["s"] = std::string("-14.3");
 P["sc"] = "-14.3";

 triqs::arrays::array<double,2> A(2,2); A()=0;A(0,0) = 1.3; A(1,1) = -8.2;
 triqs::arrays::array<long,1> B(3); B()=0;B(0) = 3; B(1) = -8;
 P["A"] = std::move(A);
 P["B"] = B;

 std::cout  << P << std::endl;
 std::cout  << pdef<< std::endl;

 try { 
  P2 = P;
  P2.update(pdef);
 }
 catch (triqs::runtime_error const & e) { 
  std::cout<< " Caught as expected : update_with_defaults error "<< e.what() << std::endl ;
 }
 std::cout  << P2 << std::endl;

}

