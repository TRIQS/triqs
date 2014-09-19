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
#include <iostream>
#include <type_traits>
#include <triqs/arrays.hpp>
#include <triqs/mpi.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::mpi;

struct my_object {

 array<double, 1> a, b;

 TRIQS_MPI_IMPLEMENTED_AS_TUPLEVIEW;

 my_object() = default;

 my_object(int s) : a(s), b(s) {
  clef::placeholder<0> i_;
  a(i_) << i_;
  b(i_) << -i_;
 }

 // construction from the lazy is delegated to =
 template <typename Tag> my_object(mpi_lazy<Tag, my_object> x) : my_object() { operator=(x); }

 // assigment is almost done already...
 template <typename Tag> my_object &operator=(mpi_lazy<Tag, my_object> x) {
  return mpi_impl<my_object>::complete_operation(*this, x);
 }
};

// non intrusive 
auto view_as_tuple(my_object const &x) RETURN(std::tie(x.a, x.b));
auto view_as_tuple(my_object &x) RETURN(std::tie(x.a, x.b));

// --------------------------------------

int main(int argc, char *argv[]) {

#ifdef TRIQS_C11
 mpi::environment env(argc, argv);
 mpi::communicator world;
 
 std::ofstream out("t2_node" + std::to_string(world.rank()));

 auto ob = my_object(10);
 mpi::broadcast(ob);
 
 out << "  a = " << ob.a << std::endl;
 out << "  b = " << ob.b << std::endl;
 
 auto ob2 = ob;

 // ok scatter all components
 ob2 = mpi::scatter(ob);

 out << " scattered  a = " << ob2.a << std::endl;
 out << " scattered  b = " << ob2.b << std::endl;

 ob2.a *= world.rank()+1; // change it a bit

 // now regroup...
 ob = mpi::gather(ob2);
 out << " gather a = " << ob.a << std::endl;
 out << " gather b = " << ob.b << std::endl;

 // allgather
 ob = mpi::allgather(ob2);
 out << " allgather a = " << ob.a << std::endl;
 out << " allgather b = " << ob.b << std::endl;

 out << "----------------------------"<< std::endl;
#endif
}

