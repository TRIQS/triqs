/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by O. Parcollet
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
namespace triqs {
namespace gfs {

 // FOR LEGACY PYTHON CODE ONLY
 // THIS MUST be kept for python operations
 // specific operations (for legacy python code).
 // +=, -= with a matrix
 inline void operator+=(gf_view<imfreq> g, arrays::matrix<std::complex<double>> const &m) {
  for (int u = 0; u < int(first_dim(g.data())); ++u) g.data()(u, arrays::ellipsis()) += m;
  g.singularity()(0) += m;
 }

 inline void operator-=(gf_view<imfreq> g, arrays::matrix<std::complex<double>> const &m) {
  for (int u = 0; u < int(first_dim(g.data())); ++u) g.data()(u, arrays::ellipsis()) -= m;
  g.singularity()(0) -= m;
 }

 inline void operator+=(gf_view<imfreq> g, std::complex<double> a) {
  operator+=(g, make_matrix(a*arrays::make_unit_matrix<double>(g.target_shape()[0])));
 }
 inline void operator-=(gf_view<imfreq> g, std::complex<double> a) {
  operator-=(g, make_matrix(a*arrays::make_unit_matrix<double>(g.target_shape()[0])));
 }


 inline gf<imfreq> operator+(gf<imfreq> g, arrays::matrix<std::complex<double>> const &m) {
  g() += m;
  return g;
 }

 inline gf<imfreq> operator+(gf<imfreq> g, std::complex<double> const &m) {
  g() += m; // () is critical of infinite loop -> segfault
  return g;
 }

 inline gf<imfreq> operator+(std::complex<double> const &m, gf<imfreq> g) { return g + m; }
 inline gf<imfreq> operator+(arrays::matrix<std::complex<double>> const &m, gf<imfreq> g) { return g + m; }

 inline gf<imfreq> operator-(gf<imfreq> g, arrays::matrix<std::complex<double>> const &m) {
  g() -= m;
  return g;
 }

 inline gf<imfreq> operator-(gf<imfreq> g, std::complex<double> const &m) {
  g() -= m;
  return g;
 }

 inline gf<imfreq> operator-(std::complex<double> const &m, gf<imfreq> g) {
  g *= -1;
  g += m;
  return g;
 }

 inline gf<imfreq> operator-(arrays::matrix<std::complex<double>> const &m, gf<imfreq> g) {
  g *= -1;
  g += m;
  return g;
 }


 // FOR LEGACY PYTHON CODE ONLY
 // THIS MUST be kept for python operations
 // specific operations (for legacy python code).
 // +=, -= with a matrix
 inline void operator+=(gf_view<refreq> g, arrays::matrix<std::complex<double>> const &m) {
  for (int u = 0; u < int(first_dim(g.data())); ++u) g.data()(u, arrays::ellipsis()) += m;
  g.singularity()(0) += m;
 }

 inline void operator-=(gf_view<refreq> g, arrays::matrix<std::complex<double>> const &m) {
  for (int u = 0; u < int(first_dim(g.data())); ++u) g.data()(u, arrays::ellipsis()) -= m;
  g.singularity()(0) -= m;
 }

 inline void operator+=(gf_view<refreq> g, std::complex<double> a) {
  operator+=(g, make_matrix(a*arrays::make_unit_matrix<double>(g.target_shape()[0])));
 }
 inline void operator-=(gf_view<refreq> g, std::complex<double> a) {
  operator-=(g, make_matrix(a*arrays::make_unit_matrix<double>(g.target_shape()[0])));
 }


 inline gf<refreq> operator+(gf<refreq> g, arrays::matrix<std::complex<double>> const &m) {
  g() += m;
  return g;
 }

 inline gf<refreq> operator+(gf<refreq> g, std::complex<double> const &m) {
  g() += m; // () is critical of infinite loop -> segfault
  return g;
 }

 inline gf<refreq> operator+(std::complex<double> const &m, gf<refreq> g) { return g + m; }
 inline gf<refreq> operator+(arrays::matrix<std::complex<double>> const &m, gf<refreq> g) { return g + m; }

 inline gf<refreq> operator-(gf<refreq> g, arrays::matrix<std::complex<double>> const &m) {
  g() -= m;
  return g;
 }

 inline gf<refreq> operator-(gf<refreq> g, std::complex<double> const &m) {
  g() -= m;
  return g;
 }

 inline gf<refreq> operator-(std::complex<double> const &m, gf<refreq> g) { 
  g *= -1;
  g+=m;
  return g;
  }

 inline gf<refreq> operator-(arrays::matrix<std::complex<double>> const &m, gf<refreq> g) { 
  g *= -1;
  g+=m;
  return g;
 }

}
}
