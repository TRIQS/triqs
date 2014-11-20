/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#include "./tail.hpp"

namespace triqs {
namespace gfs {

 // tail_const_view class
 void tail_const_view::rebind(tail_const_view const &X) {
  omin = X.omin;
  _mask.rebind(X._mask);
  _data.rebind(X._data);
 }

 // tail_view class
 void tail_view::rebind(tail_view const &X) {
  omin = X.omin;
  _mask.rebind(X._mask);
  _data.rebind(X._data);
 }

 tail_view &tail_view::operator=(std::complex<double> x) {
  _data() = 0.0;
  mv_type(_data(-omin, ellipsis())) = x;
  mask() = omin + size() - 1;
  return *this;
 }

  /// --- functions

 std::ostream &operator<<(std::ostream &out, tail_const_view x) {
  if (x.data().is_empty()) return out << "empty tail" << std::endl;
  out << "tail/tail_view: min/smallest/max = " << x.order_min() << " " << x.smallest_nonzero() << " " << x.order_max();
  for (int u = x.order_min(); u <= x.order_max(); ++u) out << "\n ...  Order " << u << " = " << x(u);
  return out;
 }

 /// Evaluate the tail to  sum_{n=order_min}^ordermax M_n/omega^n
 matrix<dcomplex> evaluate(tail_const_view t, dcomplex const &omega) {
  auto r = arrays::matrix<dcomplex>{t.shape()};
  r() = 0;
  if (t.data().is_empty()) return r;
  auto omin = t.order_min();
  auto omax = t.order_max(); // precompute since long to do...
  for (int u = omax; u >= omin; --u)
   r = r / omega + matrix_view<dcomplex>{t.data()(u - omin, ellipsis())}; // need to make a matrix view because otherwise + is not defined
  r /= pow(omega, omin);
  return r;
 }

 /// Evaluate the tail to  sum_{n=order_min}^ordermax M_n/omega^n
 tail compose(tail_const_view x, tail_const_view t) {
  if (x.data().is_empty()) return x;
  auto r = tail(x.shape(), x.size(), x.order_min()); // a new tail of same size, init to 0
  auto t_inv = inverse(t);
  auto omin = x.order_min();
  auto omax = x.order_max(); // precompute since long to do...
  if ((omin<=0) && (omax>=0)) r(0) = x(0);
  tail z = t;
  for (int u = -1; u >= omin; --u) {
   r += x(u) * z;
   z = z * t;
  }
  z = t_inv;
  for (int u = 1; u <= omax; ++u) {
   r += x(u) * z;
   z = z * t_inv;
  }
  return r;
 }

 /// Factories
 tail tail_omega(int N1, int N2, int size_, int order_min) {
  tail t(N1, N2, size_, order_min);
  t(-1) = 1;
  return t;
 }
 tail_view tail_omega(tail::shape_type const &sh, int size_, int order_min) { return tail_omega(sh[0], sh[1], size_, order_min); }
 tail_view tail_omega(tail_view t) { return tail_omega(t.shape(), t.size(), t.order_min()); }

 // Ops
 tail conj(tail_const_view const &t) {
  return {conj(t.data()), t.mask(), t.order_min()};
 }

 tail transpose(tail_const_view const &t) {
  return {transposed_view(t.data(), 0, 2, 1), transposed_view(t.mask(), 1, 0), t.order_min()};
 }

 /// Slice in orbital space
 tail_view slice_target(tail_view t, range R1, range R2) {
  return {t.data()(range(), R1, R2), t.mask()(R1, R2), t.order_min()};
 }

 tail_const_view slice_target(tail_const_view const &t, range R1, range R2) {
  return {t.data()(range(), R1, R2), t.mask()(R1, R2), t.order_min()};
 }

 // inverse
 tail inverse(tail_const_view const &t) {
  int omin1 = -t.smallest_nonzero();
  int omax1 = std::min(t.order_max() + 2 * omin1, t.order_min() + int(t.size()) - 1);
  int si = omax1 - omin1 + 1;

  tail res(t.shape(), t.size(), t.order_min());
  res.mask()() = omax1;

  res(omin1) = inverse(t(-omin1));

  // optimize for the case 1x1
  if (1 && (t.shape()==make_shape(1,1))) {
   for (int n = 1; n < si; n++) {
    for (int p = 0; p < n; p++) {
     res(omin1 + n)(0,0) -= t(n - omin1 - p)(0,0) * res(omin1 + p)(0,0);
    }
    res(omin1 + n)(0,0) = res(omin1)(0,0) * res(omin1 + n)(0,0);
   }
  }
  else {
   for (int n = 1; n < si; n++) {
    for (int p = 0; p < n; p++) {
     res(omin1 + n) -= t(n - omin1 - p) * res(omin1 + p);
    }
    res(omin1 + n) = res(omin1) * res(omin1 + n);
    //res(omin1 + n) = res(omin1) * make_clone(res(omin1 + n));
   }
  }
  return res;
 }

 // Arithmetic ops

 tail operator*(matrix<dcomplex> const &a, tail_const_view const &b) {
  auto res = tail(first_dim(a), b.shape()[1], b.size(), b.order_min()); // no {} constructor to avoid narrowing:...
  for (int n = b.order_min(); n <= b.order_max(); ++n) res(n) = a * b(n);
  res.mask()() = b.order_max();
  return res;
 }

 tail operator*(tail_const_view const &a, matrix<dcomplex> const &b) {
  auto res = tail(a.shape()[0], second_dim(b), a.size(), a.order_min());
  for (int n = a.order_min(); n <= a.order_max(); ++n) res(n) = a(n) * b;
  res.mask()() = a.order_max();
  return res;
 }

 tail operator*(tail_const_view const &l, tail_const_view const &r) {
  if (l.shape()[1] != r.shape()[0] || l.order_min() != r.order_min() || l.size() != r.size())
   TRIQS_RUNTIME_ERROR << "tail multiplication: shape mismatch";
  // int omin1 = l.smallest_nonzero() + r.smallest_nonzero();
  int omax1 = std::min(std::min(r.order_max() + l.smallest_nonzero(), l.order_max() + r.smallest_nonzero()),
                       r.order_min() + int(r.size()) - 1);
  // int si = omax1-omin1+1;
  tail res(l.shape(), l.size(), l.order_min());
  res.mask()() = omax1;
  for (int n = res.order_min(); n <= res.order_max(); ++n) {
   // sum_{p}^n a_p b_{n-p}. p <= a.n_max, p >= a.n_min and n-p <=b.n_max and n-p >= b.n_min
   // hence p <= min ( a.n_max, n-b.n_min ) and p >= max ( a.n_min, n- b.n_max)
   const int pmin = std::max(l.smallest_nonzero(), n - r.order_max());
   const int pmax = std::min(l.order_max(), n - r.smallest_nonzero());
   for (int p = pmin; p <= pmax; ++p) {
    res(n) += l(p) * r(n - p);
   }
  }
  return res;
 }

 tail operator*(dcomplex a, tail_const_view const &r) {
  tail res(r);
  if (r.data().is_empty()) return r;
  res.data() *= a;
  return res;
 }

 tail operator/(tail_const_view const &r, dcomplex a) {
  tail res(r);
  if (r.data().is_empty()) return r;
  res.data() /= a;
  return res;
 }

 tail operator+(tail_const_view const &l, tail_const_view const &r) {
  if (l.shape() != r.shape() || l.order_min() != r.order_min() || (l.size() != r.size()))
   TRIQS_RUNTIME_ERROR << "tail addition: shape mismatch" << l << r;
  tail res(l.shape(), l.size(), l.order_min());
  res.mask()() = std::min(l.order_max(), r.order_max());
  for (int i = res.order_min(); i <= res.order_max(); ++i) res(i) = l(i) + r(i);
  return res;
 }

 tail operator-(tail_const_view const &l, tail_const_view const &r) {
  if (l.shape() != r.shape() || l.order_min() != r.order_min() || (l.size() != r.size()))
   TRIQS_RUNTIME_ERROR << "tail addition: shape mismatch" << l << r;
  tail res(l.shape(), l.size(), l.order_min());
  res.mask()() = std::min(l.order_max(), r.order_max());
  for (int i = res.order_min(); i <= res.order_max(); ++i) res(i) = l(i) - r(i);
  return res;
 }

}
}
