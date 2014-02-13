#include <triqs/parameters.hpp>
#include <iostream>
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

using namespace triqs;
using namespace triqs::utility;
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

// just to make the object printable
namespace std { 
 template<typename K, typename V> 
  std::ostream & operator << (std::ostream & out, std::map<K,V> const &) { return out << "map";}
}

namespace std { 
 template<typename K, typename V> 
  std::string get_triqs_hdf5_data_scheme (std::map<K,V> const &) { 
   using triqs::get_triqs_hdf5_data_scheme;// for the basic types, not found by ADL
   std::stringstream fs; 
   fs<<"map<"<<get_triqs_hdf5_data_scheme(K())<<","<<get_triqs_hdf5_data_scheme(V())<<">";
   return fs.str();
  } 
}

int main() {

 try { 

  parameters P;

  P["myobject1"] = my_obj(18);

  P["a"] = long(1);
  P["d"] = 2.7;
  P["s"] = std::string("-14.3");
  P["sc"] = "-14.3";

  std::map<std::string, int> mm;
  mm["qwe"] = 123;
  P["mm"] = mm;
  std::cout  <<  " map = "<< P["mm"]<< std::endl ;
  std::cout  << "123 = "<<utility::extract<std::map<std::string,int>>(P["mm"])["qwe"]<< std::endl ;

  // raises an error as it should ...
  //std::cout  << utility::extract<int>(P["d"]) << std::endl;

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

  array<array<int,2>, 1> aa(2);
  aa(0) = A; aa(1) = 2*A;
  P["aa"] = aa;

  std::cout << "get_triqs_hdf5_data_scheme :"<< get_triqs_hdf5_data_scheme(aa)<< std::endl;

  long j = P["a"];
  double x = P["d"];
  double y = P["a"];
  double z = P["s"];
  double zc = P["sc"];

  std::cout  << j << std::endl ;
  std::cout  << x << std::endl;
  std::cout  << y << std::endl ; 
  std::cout  << z << std::endl ; 
  std::cout  << zc << std::endl ; 
  std::cout  << P["a"] << std::endl ; 
  std::cout  << P["aa"] << std::endl ; 
  std::cout  << P["s"] << std::endl ; 
 
  std::string rs = P["s"];

  // testing that copy is a copy 
  parameters P2 = P; 
  P2["a"] = 12.3;

  // Put P2 in P ... 
  P["P2"] = P2;

  std::cout  << P << std::endl;

  {
   H5::H5File file( "ess.h5", H5F_ACC_TRUNC );
   h5_write( file, "Parameters", P);
  }

  {
   H5::H5File file( "ess2.h5", H5F_ACC_TRUNC );
   h5_write( file, "Parameters", P2);
  }

  std::string s = triqs::serialize(P);
  parameters P3 = triqs::deserialize<parameters>(s);
  {
   H5::H5File file( "ess3.h5", H5F_ACC_TRUNC );
   h5_write( file, "Parameters", P3);
  }

  parameters P4;
  parameters::register_type<triqs::arrays::array<double,1>>();
  //parameters::register_type<triqs::arrays::array<int,2>>();
  std::cout << "P4 before : "<< P4<< std::endl ;
  {
   H5::H5File file( "ess.h5", H5F_ACC_RDONLY );
   h5_read( file.openGroup("/"), "Parameters", P4);
  }
  {
   H5::H5File file( "ess_relo.h5", H5F_ACC_TRUNC );
   h5_write( file.openGroup("/"), "Parameters", P4);
  }
  std::cout << "P4 after : "<< P4<< std::endl ;

  std::cout  << "map retrieval "<<utility::extract<std::map<std::string,int>>(P4["mm"])["qwe"]<< std::endl ;
   
  //std::cout  << triqs::deserialize<array<array<int,2>,1>>(utility::extract<std::string>(P4["aa"])) << std::endl ;
 }
 TRIQS_CATCH_AND_ABORT;
}
