/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by I. Krivenko
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
#include <triqs/utility/numeric_ops.hpp>
#include <complex>
#include <iostream>

using namespace triqs::utility;

int main() {

  int i             = 1;
  int &i_ref        = i;
  int const &i_cref = i;

  std::cout << "i = " << i << std::endl;
  std::cout << "is_zero(i) = " << is_zero(i) << std::endl;
  std::cout << "is_zero(i_ref) = " << is_zero(i_ref) << std::endl;
  std::cout << "is_zero(i_cref) = " << is_zero(i_cref) << std::endl;
  std::cout << "conj(i) = " << conj(i) << std::endl;
  std::cout << "conj(i_ref) = " << conj(i_ref) << std::endl;
  std::cout << "conj(i_cref) = " << conj(i_cref) << std::endl;

  double d             = 0;
  double &d_ref        = d;
  double const &d_cref = d;

  std::cout << "d = " << d << std::endl;
  std::cout << "is_zero(d) = " << is_zero(d) << std::endl;
  std::cout << "is_zero(d_ref) = " << is_zero(d_ref) << std::endl;
  std::cout << "is_zero(d_cref) = " << is_zero(d_cref) << std::endl;
  std::cout << "conj(d) = " << conj(d) << std::endl;
  std::cout << "conj(d_ref) = " << conj(d_ref) << std::endl;
  std::cout << "conj(d_cref) = " << conj(d_cref) << std::endl;

  std::complex<double> c(1, 1);
  std::complex<double> &c_ref        = c;
  std::complex<double> const &c_cref = c;

  std::cout << "c = " << c << std::endl;
  std::cout << "is_zero(c) = " << is_zero(c) << std::endl;
  std::cout << "is_zero(c_ref) = " << is_zero(c_ref) << std::endl;
  std::cout << "is_zero(c_cref) = " << is_zero(c_cref) << std::endl;
  std::cout << "conj(c) = " << conj(c) << std::endl;
  std::cout << "conj(c_ref) = " << conj(c_ref) << std::endl;
  std::cout << "conj(c_cref) = " << conj(c_cref) << std::endl;

  return 0;
}
