#include <triqs/parameters.hpp>
#include <iostream>
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

using namespace triqs;
using namespace triqs::utility;

int main() {

 parameters P;

 P["a"] = long(1);
 P["d"] = 2.7;
 P["s"] = std::string("-14.3");
 P["sc"] = "-14.3";

 triqs::arrays::array<double,2> A(2,2); A()=0;A(0,0) = 1.3; A(1,1) = -8.2;
 triqs::arrays::array<long,1> B(3); B()=0;B(0) = 3; B(1) = -8;
 triqs::arrays::matrix<double> U(2,2);U()=1.;
 P["U"] = U;
 triqs::arrays::vector<double> V(3);V()=1.;
 P["V"] = V;
 P["A"] = std::move(A);
 P["B"] = B;
 std::cout  << "A"<< P["A"] << std::endl;
 std::cout  << "B"<< P["B"] << std::endl;

 triqs::arrays::array<long,1> C;
 C = extract<decltype(C)>(P["B"]);
 std::cout  << "C" << C << std::endl;

 // testing that I can extract a view and modify it.
 triqs::arrays::array_view <long,1> VC = extract<triqs::arrays::array_view <long,1>>(P["B"]);
 std::cout  << "VC" << VC << std::endl;
 VC(0) *=2;

 C = extract<decltype(C)>(P["B"]);
 std::cout  << "C" << C << std::endl;

 {
  H5::H5File file( "ess.h5", H5F_ACC_TRUNC );
  h5_write( file, "Parameters", P);
 }

 return 0;
}
