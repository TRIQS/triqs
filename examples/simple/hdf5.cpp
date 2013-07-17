/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include <triqs/arrays.hpp>

using std::cout; using std::endl;
namespace tqa = triqs::arrays;
namespace h5 = triqs::h5;
using tqa::range;

template <typename T> 
std::ostream & operator << (std::ostream & out, std::vector<T> const & v) { 
for (size_t i =0; i<v.size(); ++i) out<< v[i];
return out;
}

int main(int argc, char **argv) {
 
  

 try { 

 tqa::array<long,2> A (2,3),B,vc;
 tqa::array<double,2> D (2,3), D2;

 tqa::array<long,2> Af(FORTRAN_LAYOUT),Bf(FORTRAN_LAYOUT),vf(FORTRAN_LAYOUT);

 tqa::array<std::complex<double>,1> C(5), C2;
 std::complex<double> z(1,2);

 for (int i =0; i<5; ++i) {
  C(i) = std::complex<double>(i,i);
 }

 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
    { 
     A(i,j) = 10*i+ j;
     D (i,j) = A(i,j) / 10.0;
    }

 Af = A;
 
 std::cout<<" A= "<<A<<std::endl;
 std::cout<<" D= "<<D<<std::endl;
 std::cout<<" C= "<<C<<std::endl;
 std::cout<<" Arange(0,1),range(1,3)  = "<< A(range(),range(1,3))<<std::endl;

 H5::H5File file( "ess.h5", H5F_ACC_TRUNC );
 h5::group top(file);

 h5_write(top,"A",A);
 h5_write(top,"Af",Af);
 h5_write(top,"C",C);
 h5_write(top,"D",D);

 // testing scalar
 double x=2.3;
 h5_write(top, "x",x);

 h5_write(top, "s", std::string("a nice chain"));
 top.create_group("G");
 h5_write(top,"G/A",A);

 auto G = top.open_group("G");
 h5_write(G, "A2",A);

 h5_read (top, "A",B);   std::cout<< "B = "<< B<<std::endl;
 h5_read (top, "Af",Bf); std::cout<< "Bf = "<< Bf<<std::endl;
 h5_read (top, "D",D2);  std::cout<< "D = "<< D2<<std::endl;
 h5_read (top, "C",C2);  std::cout<< "C = "<< C2<<std::endl;

 double xx =0; h5_read(top, "x",xx); 

 std::string s2 ("----------------------------------");
 h5_read(top, "s", s2);

 //tqa::array<long,1> E; h5_read (top, "A",E);   std::cout<< "E = "<< E<<std::endl;

 } 
 catch( const char * err) { std::cout<<err<<std::endl;}


 return 0;
}


