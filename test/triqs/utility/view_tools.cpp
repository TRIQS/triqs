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
#include <triqs/utility/view_tools.hpp>
using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::utility;

template <typename T> struct check {
 static_assert(std::is_same<typename T::view_type, typename view_type_if_exists_else_type<T>::type >::value, "err");
 static_assert(std::is_same<typename T::regular_type, typename regular_type_if_exists_else_type<T>::type >::value, "err");
};

int main(int argc, char **argv) {

 static_assert(has_view< array<int,2> > ::value, " err");
 static_assert(has_view< array_view<int,1> > ::value, " err");
 static_assert(has_view< matrix<double> > ::value, " err");
 static_assert(!has_view< double > ::value, " err");
 
 check<array<int,1>> ();
 check<array_view<int,1>>();
 check<matrix<int>>();
 check<matrix_view<int>>();

 return 0;
}

