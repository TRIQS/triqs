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
#include <triqs/utility/crash_logger.hpp>
#include <triqs/h5.hpp>
#include <triqs/h5/scalar.hpp>
using namespace triqs;

class A  {
 int i;
 public :
 A() : i(8) {}
 friend void h5_write (h5::group g, std::string const & name, A const & a) {
  auto gr = g.create_group(name);
  //throw " hi hi hi";
  h5_write(gr,"i", a.i);
 }
  void make_exp() { throw std::runtime_error("hi hi hi ");}
};

void f() { 

 int i =3;
 double d =-2.3;
 A a;

 auto log = triqs::utility::crash_logger ("rr.h5");
 log(a,"a")(i,"i")(d,"d"); 

 //try { 
 throw std::runtime_error(std::string("an exception occurs"));
 // }
 //catch(...) { }

 //a.make_exp();

 log.dismiss();
}

int main(int argc, char **argv) {

 try {
  f();
 }
 // ok we have to catch something, don't change this to TRIQS_CATCH_AND_ABORT
 catch(std::exception const & e) { std::cerr  << e.what() << std::endl ;}

}
