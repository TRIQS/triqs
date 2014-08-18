#include <triqs/parameters.hpp>
#include <iostream>
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

using namespace triqs;
using namespace triqs::params;
using namespace triqs::arrays;
using triqs::arrays::array;

// what is the concept of things that can be put in the dict ?
struct my_obj { 
 int i;
 my_obj(int i_) { i=i_;}

 my_obj() { i=0;}
 my_obj(my_obj const & x) { i = x.i;}
 my_obj(my_obj && x) { std::swap(i,x.i);}
 my_obj & operator = (my_obj && x)= default;

 // h5 operation 
 friend std::string get_triqs_hdf5_data_scheme( my_obj const&) { return "myobject_is_nice";} 

 friend void h5_write ( h5::group F, std::string const & subgroup_name, my_obj const & p){
  h5::group gr =  F.create_group(subgroup_name);
  gr.write_triqs_hdf5_data_scheme(p);
  h5_write(gr,"data",p.i);
 }

 friend void h5_read ( h5::group F, std::string const & subgroup_name, my_obj & p){
  auto gr = F.open_group(subgroup_name);
  h5_read(gr,"data",p.i);
 }

 // print
 friend std::ostream & operator << (std::ostream & out, my_obj const & p) { return out<< "myobject("<<p.i<<")";}

 // boost serialization 
 friend class boost::serialization::access;
 template<class Archive>
  void serialize(Archive & ar, const unsigned int version) { ar & boost::serialization::make_nvp("i",i); }
};

int main() {

 try { 

  parameters P;

  P.add_field("a", 1, "?")
      .add_field("d", 2.0, "")
      .add_field("s", "", "")
      .add_field("sc", "", "")
      .add_field("U", no_default<array<double, 2>>(), "")
      .add_field("A", no_default<matrix<double>>(), "")
      .add_field("V", no_default<arrays::vector<double>>(), "")
      .add_field("B", no_default<array<long, 1>>(), "")
      //.add_field("myobject1", no_default<my_obj>(), "")
      ;

  //P["myobject1"] = my_obj(18);

  P["a"] = long(1);
  P["d"] = 2.7;
  P["s"] = std::string("-14.3");
  P["sc"] = "-14.3";

  // raises an error as it should ...
  try {
   std::cout  << extract<int>(P["d"]) << std::endl;
  }
  catch(std::exception const & e) { 
   std::cout  << "caught error "<< e.what() <<std::endl;
  }

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
  triqs::arrays::array_view<long, 1> VC = extract<triqs::arrays::array_view<long, 1>>(P["B"]);
  std::cout << "VC" << VC << std::endl;
  VC(0) *= 2;

  C = extract<decltype(C)>(P["B"]);
  std::cout << "C" << C << std::endl;

  /*array<array<int,2>, 1> aa(2);
  aa(0) = A; aa(1) = 2*A;
  P["aa"] = aa;
  std::cout << "get_triqs_hdf5_data_scheme :"<< get_triqs_hdf5_data_scheme(aa)<< std::endl;
  */

  long j = P["a"];
  double x = P["d"];
  double y = P["a"];
  double z = P["s"];
  double zc = P["sc"];
  std::string rs = P["s"];

  std::cout  << j << std::endl ;
  std::cout  << x << std::endl;
  std::cout  << y << std::endl ; 
  std::cout  << z << std::endl ; 
  std::cout  << zc << std::endl ; 
  std::cout  << P["a"] << std::endl ; 
  //std::cout  << P["aa"] << std::endl ; 
  std::cout  << P["s"] << std::endl ; 

  std::cout  << P << std::endl;

  {
   h5::file file( "ess.h5", H5F_ACC_TRUNC );
   h5_write( file, "Parameters", P);
  }

  auto P4 = P;
  {
   h5::file file( "ess.h5", H5F_ACC_RDONLY );
   h5_read( file, "Parameters", P4);
  }
  {
   h5::file file( "ess_relo.h5", H5F_ACC_TRUNC );
   h5_write( file, "Parameters", P4);
  }
  std::cout << "P4 after : \n"<< P4<< std::endl ;

 }
 TRIQS_CATCH_AND_ABORT;
}
