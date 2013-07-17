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
#include "./python_stuff.hpp"

#include "./src/h5/array_proxy.hpp"
#include "./src/h5/simple_read_write.hpp"
#include <iostream>
#include "./src/asserts.hpp"


using namespace triqs::arrays;

template < class T>
void test(std::string filename, T init) { 

 h5::H5File file( filename.c_str(), H5F_ACC_TRUNC );

 const size_t N = 12, d= 2;

 array<T,2> A(d,d+1), A2(d,d+1);
 array<T,3> A_stack_keep(N,d,d+1), A_stack_compare(N,d,d+1);

 array<T,1> B(d), B2(d);
 array<T,2> B_stack_keep(N,d), B_stack_compare(N,d);


 for (size_t u = 0; u<N; ++u)  {
  A() = double(u+1)* init; 
  B() = double(u+1)* init; 
  A(0,0) *=2;
  B(0) *=2;
  A_stack_keep(u,range(),range()) = A;
  B_stack_keep(u,range()) = B;
 }
 
 array<T,2> M(6,6);
 for (int u = 0; u<6; ++u) 
  for (int v = 0; v<6; ++v)
   M(u,v) = 10*u + v;

 h5_write( file, "A", A_stack_keep);
 h5_write( file, "A2", A_stack_keep);
 h5_write( file, "B", B_stack_keep);
 h5_write( file, "M", M);

 h5::array_proxy<T,3,3> P( file, "A2");
 P (0,range(), range())  = T(2) *A;

 h5::array_proxy<T,3,3> Pnew( file, "Z", make_shape(2,2,3) );

 file.close();

 // READING 
 //
 h5::H5File file2 ( filename.c_str(), H5F_ACC_RDONLY );

 h5::array_proxy<T,3,3> AA( file2, "A");
 A_stack_compare = AA; 
 assert_all_close (A_stack_keep, A_stack_compare, 1.e-13);
 std::cerr << "A_stack_compare = "<< A_stack_compare << std::endl; 

 array<T,2> A3(1,1);
 for (size_t u = 0; u<N; ++u)  {
  A() = double(u+1)* init; 
  B() = double(u+1)* init; 
  A(0,0) *=2;
  B(0) *=2;

  A2 =  AA (u,range(), range()) ;
  std::cerr << "A from file = "<< A2 << std::endl; 
  std::cerr << " ... should be = "<< A << std::endl; 
  assert_all_close (A, A2, 1.e-15);

  A3 =  AA (u,range(), range()) ;
  std::cerr << "A from file with resize  = "<< A3 << std::endl; 
  std::cerr << " ... should be = "<< A << std::endl; 
  assert_all_close (A, A3, 1.e-15);

 }

 std::cerr << " ... done ... now matrix "<< std::endl;

 h5::array_proxy<T,2> Pm( file2, "M");
 A3 = Pm;
 std::cerr << "M from file with resize  = "<< A3 << std::endl; 
 std::cerr << " ... should be = "<< M << std::endl; 
 assert_all_close (M, A3, 1.e-15);

 A3 = Pm( range(1,6,2), range (1,6,2));
 array_view<T,2> V = M( range(1,6,2), range (1,6,2));
 std::cerr << "M from file with resize  = "<< A3 << std::endl; 
 std::cerr << " ... should be = "<< V << std::endl; 
 assert_all_close (V, A3, 1.e-15);

 std::cerr << " ... done "<< std::endl;

 file2.close();

}

int main(int argc, char **argv) {

 init_python_stuff(argc,argv);

 test("proxy_d.h5", 1.0 );
 test("proxy_c.h5", std::complex<double>(1,2) );

}

