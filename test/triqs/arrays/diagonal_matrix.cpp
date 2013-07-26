/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#include <iostream>

namespace triqs { namespace arrays { 

 template<typename T> class immutable_diagonal_matrix_view : TRIQS_MODEL_CONCEPT(ImmutableMatrix) { 
  array_view<T,1> data;
  public:

  //immutable_diagonal_matrix_view(size_t i=1) : data ( array<T,1>(i)) {}
  immutable_diagonal_matrix_view(array_view<T,1> v) : data (v) {}
 
  // the ImmutableMatrix concept 
  typedef T value_type;
  T operator()(size_t i, size_t j) const { return (i==j ? data(i) : 0);}

  typedef indexmaps::cuboid::domain_t<2> domain_type;
  domain_type domain() const { auto s = data.shape()[0]; return mini_vector<size_t,2>(s,s);}

  //
  friend std::ostream & operator<<(std::ostream & out, immutable_diagonal_matrix_view const & d) { return out<<"diagonal_matrix "<<d.data;}
  
  // ----------------------
  // should be remove from concept. redundant....
  // need to clean dim0, dim1 and shape and make them free function everywhere (deduced from domain)
  size_t dim0() const { return data.shape()[0];}
  size_t dim1() const { return data.shape()[0];}
  mini_vector<size_t,2> shape() const { auto s = data.shape()[0]; return mini_vector<size_t,2>(s,s);}
 
 };
 
}}

using namespace triqs::arrays;

int main(int argc, char **argv) {

 auto a = array<int,1> { 1,2,3,4};

 auto d = immutable_diagonal_matrix_view<int>(a);

 std::cout  << d<< std::endl;
 std::cout << matrix<int>( 2* d) << std::endl;
 std::cout << matrix<int>( d * d) << std::endl;
 return 0;
}





