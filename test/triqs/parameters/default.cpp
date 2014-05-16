#include <triqs/parameters.hpp>
#include <iostream>
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

using namespace triqs::params;
using namespace triqs::arrays;

int main() {
 try {  

 parameters P,P2;

 P.add_field("A", no_default<int>(), "really ?")
     .add_field("D", 2.0, " int ")
     .add_field("E", 3u, " unsigned int ")
     .add_field("F", long(4), " long ")
     .add_field("B", short(0), " short ")
     .add_field("C", 1u, " unsigned short ")
     .add_field("G", 5ll, " long ")
     .add_field("I", double(7.8), " doube ")
     .add_field("K", std::complex<double>(12), " double complex ")
     .add_field("L", std::string("13"), " string ")
     .add_field("M", std::vector<double>{1, 4}, " vector ")
     .add_field("Ml", std::vector<double>{1, 2, 3, 4, 5, 6, 7, 8, 9}, " a long vector ")
     .add_field("N", double(15), "")
     .add_field("s", no_default<std::string>(), "")
     .add_field("sc", no_default<std::string>(), "")
     .add_field("a1", no_default<array<double, 2>>(), " A nice array")
     .add_field("a2", no_default<array<long, 1>>(), "")
     .add_field("W", int(16), "");

 std::cout  << P << std::endl;
 P2 = P;
 P2.sort_by_key();
 std::cout  << P2 << std::endl;
 
 auto P_vide = P ;

 P["A"] = long(1);

 std::cout  << P["A"];

 P["D"] = 2;
 P["D"] = 2.5;
 P["s"] = std::string("-14.3");
 P["sc"] = "-14.3";

 array<double,2> A(2,2); A()=0;A(0,0) = 1.3; A(1,1) = -8.2;
 array<long,1> B(100); B()=8; B(0) = 3; B(1) = -8;
 P["a1"] = std::move(A);
 P["a2"] =B;

 // errors
 //P["D"] = "eee";
 //int i =  P["D"];
 std::cout  << P << std::endl;

// P2.update(pdef);

  // testing that copy is a copy 
  P2["A"] = 12;
 
  std::cout  << P["A"] << std::endl;
  std::cout  << P2["A"] << std::endl;

 // put a subparameter ...
 P.add_group("Group1", " doc ");
 P["Group1"].add_field("a", 1, "sub a 1"); 
 P["Group1"]["a"] = 10;
 P["Group1"].add_group("subgroup1.2", " doc ");
 P["Group1"]["subgroup1.2"].add_field("d", 1.8, "d is a double"); 
 P["Group1"]["subgroup1.2"]["d"] = 10.2;

 std::cout  << P << std::endl;
 //std::cout  << P["Group1"] << std::endl;

 // hdf5 
 {
   H5::H5File file( "ess.h5", H5F_ACC_TRUNC );
   h5_write( file, "Parameters", P);
 }

  {
   H5::H5File file( "ess.h5", H5F_ACC_RDONLY );
   h5_read( file.openGroup("/"), "Parameters", P_vide);
  }

  std::cout << P_vide << std::endl;

   {
   H5::H5File file( "ess.h5", H5F_ACC_RDONLY );
   h5_read( file.openGroup("/"), "Parameters", P);
  }

 //std::cout  << P << std::endl;


 }
 catch (triqs::runtime_error const & e) { 
  std::cout<< " Caught  "<< e.what() << std::endl ;
 }

}

