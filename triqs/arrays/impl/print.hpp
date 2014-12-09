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
#pragma once
#include <triqs/utility/first_include.hpp>
namespace triqs {
namespace arrays {

 /// ------------  Pretty Printing : specializing the default behaviour for d=1,2  -------------------------
 namespace PrettyPrint_details {
  template <int R, typename A> struct print_impl {
   std::ostream &out;
   A const &a;
   print_impl(std::ostream &out_, A const &a_) : out(out_), a(a_) {}
   template <typename A0, typename... Args> void operator()(A0 const &a0, Args const &... args) const {
    out << a(a0, args...) << " ";
   }
   void print() const {
    out << "[";
    // indexmaps::cuboid::foreach<permutations::identity(A::domain_type::rank)>(a.domain(),
    // std::cref(*this)); // foreach(a, std::cref(*this));
    indexmaps::cuboid::foreach_impl(_traversal_fortran{}, a.domain(), FORTRAN_LAYOUT, *this);
    //     foreach(a, *this);
    out << "]";
   }
  };

  template <typename A> struct print_impl<1, A> {
   std::ostream &out;
   A const &a;
   print_impl(std::ostream &out_, A const &a_) : out(out_), a(a_) {}
   void print() const {
    auto d = a.indexmap().domain();
    out << "[";
    for (size_t i = 0; i < d.lengths()[0]; ++i) out << (i > 0 ? "," : "") << a(i);
    out << "]";
   }
  };

  template <typename A> struct print_impl<2, A> {
   std::ostream &out;
   A const &a;
   print_impl(std::ostream &out_, A const &a_) : out(out_), a(a_) {}
   void print() const {
    auto d = a.indexmap().domain();
    out << "\n[";
    for (size_t i = 0; i < d.lengths()[0]; ++i) {
     out << (i == 0 ? "[" : " [");
     for (size_t j = 0; j < d.lengths()[1]; ++j) out << (j > 0 ? "," : "") << a(i, j);
     out << "]" << (i == d.lengths()[0] - 1 ? "" : "\n");
    }
    out << "]";
   }
  };
 }
 template <typename A> void pretty_print(std::ostream &out, A const &a) {
  PrettyPrint_details::print_impl<A::domain_type::rank, A>(out, a).print();
 }
}
}
