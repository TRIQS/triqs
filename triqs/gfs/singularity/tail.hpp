/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2014 by M. Ferrero, O. Parcollet
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
#include <triqs/arrays.hpp>
//#include <triqs/arrays/algorithms.hpp>
#include <triqs/gfs/impl/tools.hpp>

// TO BE SEPARATED
#include <triqs/mpi/boost.hpp>
#include <boost/serialization/complex.hpp>

namespace triqs {
namespace gfs {

 using dcomplex = std::complex<double>;
 using arrays::array;
 using arrays::array_view;
 using arrays::matrix_view;
 using arrays::matrix;
 using arrays::make_shape;
 using arrays::range;
 using arrays::ellipsis;
 using arrays::mini_vector;

 class tail;            // the value class
 class tail_view;       // the view class
 class tail_const_view; // the const view class

 // ---------------------- implementation --------------------------------

 enum rvc_enum {
  Regular,
  View,
  ConstView
 };

 template <rvc_enum RVC, typename T> struct __get_rvc;
 template <rvc_enum RVC, typename T> using get_rvc = typename __get_rvc<RVC, T>::type;
 template <typename T> struct __get_rvc<Regular, T> {
  using type = T;
 };
 template <typename T> struct __get_rvc<View, T> {
  using type = typename T::view_type;
 };
 template <typename T> struct __get_rvc<ConstView, T> {
  using type = typename T::const_view_type;
 };

 /// A common implementation class.
 template <rvc_enum RVC> class tail_impl {
  public:
  TRIQS_MPI_IMPLEMENTED_VIA_BOOST;
  using view_type = tail_view;
  using const_view_type = tail_const_view;
  using regular_type = tail;

  using data_type = get_rvc<RVC, array<dcomplex, 3>>;
  using mask_type = get_rvc<RVC, array<int, 2>>;

  using mv_type = matrix_view<dcomplex>;
  using const_mv_type = matrix_view<dcomplex>;

  data_type & data() { return _data; }
  data_type const & data() const { return _data; }
  mask_type & mask() { return _mask; }
  mask_type const & mask() const { return _mask; }

  int order_min() const { return omin; }
  int order_max() const { return min_element(_mask); }
  size_t size() const { return _data.shape()[0]; }
  int smallest_nonzero() const {
   int om = omin;
   while ((om < order_max()) && (max_element(abs(_data(om - omin, ellipsis()))) < 1.e-10)) om++;
   return om;
  }

  using shape_type = mini_vector<size_t, 2>;
  shape_type shape() const { return shape_type(_data.shape()[1], _data.shape()[2]); }
  bool is_decreasing_at_infinity() const { return (smallest_nonzero() >= 1); }

  protected:
  int omin = -1;
  mask_type _mask;
  data_type _data;

  tail_impl() = default;
  tail_impl(int N1, int N2, int size_, int order_min)
     : omin(order_min), _mask(arrays::make_shape(N1, N2)), _data(make_shape(size_, N1, N2)) {
   _mask() = order_min + size_ - 1;
   _data() = 0;
  }
  tail_impl(data_type const &d, mask_type const &m, int omin_) : omin(omin_), _mask(m), _data(d) {}
  template <rvc_enum RVC2> tail_impl(tail_impl<RVC2> const &x) : omin(x.order_min()), _mask(x.mask()), _data(x.data()) {}

  friend class tail;
  friend class tail_view;
  friend class tail_const_view;
  friend tail compose(tail_const_view x, tail_const_view omega);

  // operator = for views
  void operator=(tail_const_view const &rhs); // implemented later
    
  public:

  mv_type operator()(int n) {
   if (n > order_max()) TRIQS_RUNTIME_ERROR << " n > Max Order. n= " << n << ", Max Order = " << order_max();
   if (n < order_min()) TRIQS_RUNTIME_ERROR << " n < Min Order. n= " << n << ", Min Order = " << order_min();
   return _data(n - omin, ellipsis());
  }

  const_mv_type operator()(int n) const {
   if (n > order_max()) TRIQS_RUNTIME_ERROR << " n > Max Order. n= " << n << ", Max Order = " << order_max();
   if (n < order_min()) {
    mv_type::regular_type r(shape());
    r() = 0;
    return r;
   }
   return _data(n - omin, ellipsis());
  }

  /// same as (), but if n is too large, then returns 0 instead of raising an exception
  const_mv_type get_or_zero(int n) const {
   if ((n > order_max()) || (n < order_min())) {
    mv_type::regular_type r(shape());
    r() = 0;
    return r;
   }
   return _data(n - omin, ellipsis());
  }

  friend std::string get_triqs_hdf5_data_scheme(tail_impl const &g) { return "TailGf"; }

  ///
  friend void h5_write(h5::group fg, std::string subgroup_name, tail_impl const &t) {
   auto gr = fg.create_group(subgroup_name);
   h5_write(gr, "omin", t.omin);
   h5_write(gr, "mask", t._mask);
   h5_write(gr, "data", t._data);
  }

  friend void h5_read(h5::group fg, std::string subgroup_name, tail_impl &t) {
   auto gr = fg.open_group(subgroup_name);
   h5_read(gr, "omin", t.omin);
   h5_read(gr, "mask", t._mask);
   h5_read(gr, "data", t._data);
  }

  //  BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   ar &TRIQS_MAKE_NVP("omin", omin);
   ar &TRIQS_MAKE_NVP("mask", _mask);
   ar &TRIQS_MAKE_NVP("data", _data);
  }
 };

 // -----------------------------
 class tail_const_view : public tail_impl<ConstView> {
  using B = tail_impl<ConstView>;
  // friend class tail;

  public:
  tail_const_view(B::data_type d, B::mask_type m, int order_min) : B(std::move(d), std::move(m), order_min) {}
  tail_const_view(tail_const_view const &) = default;
  tail_const_view(tail_const_view &&) = default;
  tail_const_view(tail_impl<ConstView> const &t) : B(t) {}
  tail_const_view(tail_impl<View> const &t) : B(t) {}
  tail_const_view(tail_impl<Regular> const &t) : B(t) {}

  void rebind(tail_const_view const &X);
 };

 // -----------------------------
 class tail_view : public tail_impl<View> {
  using B = tail_impl<View>;
  // friend class tail;

  public:
  tail_view(B::data_type d, B::mask_type m, int order_min) : B(std::move(d), std::move(m), order_min) {}
  tail_view(tail_view const &) = default;
  tail_view(tail_view &&) = default;
  tail_view(tail_impl<Regular> const &t) : B(t) {}
  tail_view(tail_impl<View> const &t) : B(t) {}

  void rebind(tail_view const &X);

  tail_view &operator=(tail_view const &x) { return B::operator=(tail_const_view(x)), *this; }
  template <rvc_enum RVC> tail_view &operator=(tail_impl<RVC> const &x) { return B::operator=(tail_const_view(x)), *this; }
  tail_view &operator=(dcomplex);
 };

 // -----------------------------
 // the regular class
 class tail : public tail_impl<Regular> {
  using B = tail_impl<Regular>;
  // friend class tail_view;

  public:
  using shape_type = mini_vector<int, 2>;
  
  tail() = default;
  tail(int N1, int N2, int size_ = 10, int order_min = -1) : B(N1, N2, size_, order_min) {}
  tail(shape_type const &sh, int size_ = 10, int order_min = -1) : B(sh[0], sh[1], size_, order_min) {}
  tail(mini_vector<int, 0>) : tail(1, 1) {}
  tail(B::data_type const &d, B::mask_type const &m, int order_min) : B(d, m, order_min) {}
  tail(tail const &g) : B(g) {}
  tail(tail_view const &g) : B(g) {}
  tail(tail_const_view const &g) : B(g) {}
  tail(tail &&) = default;

  tail &operator=(tail const &x) { return B::operator=(tail_const_view(x)), *this; }
  template <rvc_enum RVC> tail &operator=(tail_impl<RVC> const &x) { return B::operator=(tail_const_view(x)), *this; }
 };

 // ---- impl. of = for base class -------------
 template <rvc_enum RVC> void tail_impl<RVC>::operator=(tail_const_view const &rhs) {
   if (RVC == Regular) {
    omin = rhs.order_min();
   } else {
   if ((_data.shape() != rhs.data().shape()) || (omin != rhs.order_min()) || (size() != rhs.size()))
     TRIQS_RUNTIME_ERROR << "tails are incompatible";
   }
   _mask = rhs.mask();
   _data = rhs.data();
  }

 // ---- Factories -------------

 /// The simplest tail corresponding to omega
 tail tail_omega(int N1, int N2, int size_ = 10, int order_min = -1);

 /// The simplest tail corresponding to omega, constructed from a shape for convenience
 tail_view tail_omega(tail::shape_type const &sh, int size_, int order_min);

 /// The simplest tail corresponding to omega, built from the shape, size, ordermin of t
 tail_view tail_omega(tail_view t);

 // ---- IO -------------

 std::ostream &operator<<(std::ostream &out, tail_const_view);

 // ----  -------------

 //
 template <typename RHS> void assign_singularity_from_function(tail_view t, RHS const &rhs) {
  t = rhs(tail_omega(t.shape(), t.size(), t.order_min()));
 }

 // ---- Evaluate and compose  -------------

 /// Evaluate the tail to  sum_{n=order_min}^ordermax M_n/omega^n
 matrix<dcomplex> evaluate(tail_const_view t, dcomplex const &omega);

 tail compose(tail_const_view x, tail_const_view t);

 // ----  Ops  -------------

 tail conj(tail_const_view const &t, bool imaginary = false);
 tail transpose(tail_const_view const &t);

 /// Slice in orbital space
 tail_const_view slice_target(tail_const_view const &t, range R1, range R2);
 tail_view slice_target(tail_view t, range R1, range R2);
 inline tail_view slice_target(tail &t, range R1, range R2) { return slice_target(tail_view{t}, R1, R2);}
 inline tail_const_view slice_target(tail const &t, range R1, range R2) { return slice_target(tail_const_view{t}, R1, R2);}

 tail inverse(tail_const_view const &t);

 /// ------------------- Arithmetic operations ------------------------------

 tail operator+(tail_const_view const &l, tail_const_view const &r);
 tail operator-(tail_const_view const &l, tail_const_view const &r);

 // +/- with scalar or matrix. 
 template <typename T>
 struct is_scalar_or_matrix : std::integral_constant<bool, arrays::ImmutableMatrix<std14::decay_t<T>>::value || utility::is_in_ZRC<T>::value> {};

 template <typename T> std14::enable_if_t<is_scalar_or_matrix<T>::value, tail> operator+(T const &a, tail_const_view const &t) {
  tail res(t);
  res(0) += a;
  return res;
 }

 template <typename T> std14::enable_if_t<is_scalar_or_matrix<T>::value, tail> operator+(tail_const_view const &t, T const &a) {
  return a + t;
 }

 template <typename T> std14::enable_if_t<is_scalar_or_matrix<T>::value, tail> operator-(T const &a, tail_const_view const &t) {
  return (-a) + t;
 }

 template <typename T> std14::enable_if_t<is_scalar_or_matrix<T>::value, tail> operator-(tail_const_view const &t, T const &a) {
  return (-a) + t;
 }

 tail operator*(tail_const_view const &l, tail_const_view const &r);

 tail operator*(matrix<dcomplex> const &a, tail_const_view const &b);
 tail operator*(tail_const_view const &a, matrix<dcomplex> const &b);
 template <typename T> tail operator*(matrix_view<T> const &a, tail_const_view const &b) { return matrix<dcomplex>(a) * b; }
 template <typename T> tail operator*(tail_const_view const &a, matrix_view<T> const &b) { return a * matrix<dcomplex>(b); }

 tail operator*(dcomplex a, tail_const_view const &r);
 inline tail operator*(tail_const_view const &r, dcomplex a) { return a * r; }

 inline tail operator/(tail_const_view const &a, tail_const_view const &b) { return a * inverse(b); }
 tail operator/(tail_const_view const &r, dcomplex a);
 inline tail operator/(dcomplex a, tail_view r) { return a * inverse(r); }


// inplace operators
#define DEFINE_OPERATOR(OP1, OP2)                                                                                                \
 template <typename T> void operator OP1(tail_view g, T &&x) { g = g OP2 x; }                                                    \
 template <typename T> void operator OP1(tail &g, T &&x) { g = g OP2 x; }

 DEFINE_OPERATOR(+=, +);
 DEFINE_OPERATOR(-=, -);
 DEFINE_OPERATOR(*=, *);
 DEFINE_OPERATOR(/=, / );

#undef DEFINE_OPERATOR
}
}

