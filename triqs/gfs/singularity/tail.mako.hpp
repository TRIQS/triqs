/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2016 by M. Ferrero, O. Parcollet
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
#include "../../arrays.hpp"
#include "../gf/targets.hpp"
#include "../../utility/static_if.hpp"

namespace triqs {
namespace gfs {

 using utility::static_if;
 using utility::bool_;
 using utility::is_in_ZRC;
 using triqs::arrays::isnan;
 using triqs::arrays::any;
 using triqs::arrays::max_element;

 inline dcomplex inverse(dcomplex const &z) { return 1 / z; }

 // FIXME replace to __tail_gf
 template <typename T> class __tail;
 template <typename T> class __tail_view;
 template <typename T> class __tail_const_view;

 // alias for backward compat.
 using tail = __tail<matrix_valued>;
 using tail_view = __tail_view<matrix_valued>;
 using tail_const_view = __tail_const_view<matrix_valued>;

 // a trait to recognize tail, tail_view, tail_const_view in generic code.
 struct tag_is_tail {};
 template <typename T> constexpr bool is_tail_v = std::is_base_of<tag_is_tail, std::decay_t<T>>::value;

 /*mako
 %for ROOT in ['__tail'] :
  %for RVC in ['regular', 'view', 'const_view'] :
  <%
     EXT = ('_' + RVC) if 'view' in RVC else ''
     TAIL = ROOT + EXT
     ARRAY = 'arrays::array' + EXT
     TV = ROOT + '_' + (RVC if 'view' in RVC else 'view')
     ISVIEW = str('view' in TAIL).lower()
     ISCONST = str(RVC == 'const_view').lower()
  %>
 */

 /**
  * The high-frequency behavior (or **tail**) of a Green's function is of paramount importance in the context of Fourier
  *transformations: since fermionic Green's functions are slowly decaying functions of frequency, special care has to be taken
  *when Fourier-transforming them to time.
  *
  * The "tail" of a Green's function encodes the behavior of the Green's function $G$ at large Matsubara frequencies, namely
  *
  * $$\mathbf{G}(i\omega_n) \stackrel {=}{\infty} \mathbf{a}_{-1}\cdot i\omega_n + \mathbf{a}_{0} +\mathbf{a}_{1}\cdot \frac{1}{
  *i\omega_n} +\mathbf{a}_{2}\cdot \frac{1}{ (i\omega_n)^2} +\dots$$
  *
  *
  * Generically, the tail is parametrized by matrix-valued coefficients
  * $\mathbf{a}_{i}$ (of size $N_1 \times N_2$)
  *
  * $t = \sum_{i=o_{min}}^{o_{max}} \mathbf{a}_i (i\omega_n)^{-i}$
  */
 template <typename T> class MAKO_TAIL : tag_is_tail {
  static constexpr bool is_scalar_target = (T::rank ==0);

  public:
  static constexpr bool is_view = MAKO_ISVIEW;
  static constexpr bool is_const = MAKO_ISCONST;

  using target_t = T;
  using regular_type = __tail<T>;
  using mutable_view_type = __tail_view<T>;
  using const_view_type = __tail_const_view<T>;
  using view_type = MAKO_TV<T>;

  using data_t = MAKO_ARRAY<dcomplex, T::rank + 1>;

  using crv_t = typename const_view_type_if_exists_else_type<typename T::slice_t>::type;
  //using crv_t = std14::conditional_t<is_scalar_target, dcomplex, const_view_or_type_t<typename T::slice_t>>;
  // mako %if RVC == 'const_view' :
  using rv_t = crv_t;
  // mako %else:
  using rv_t = std14::conditional_t<is_scalar_target, dcomplex &, view_or_type_t<typename T::slice_t>>;
  // mako %endif

  /// The minimum order of the expansion : omega^2
  static constexpr int order_min() { return -2; }

  /// The maximum order of the expansion : 1/omega^8. Included !
  static constexpr int order_max() { return 8; }

  // Number of coefficients
  static constexpr size_t _size() { return order_max() - order_min() + 1; }

  private:
  data_t _data;

  public:
  /**
   3-dim array of the coefficients: data(i,n,m) stands for $(\mathbf{a}_{i+o_\mathrm{min}})_{nm}$
  */
  data_t &data() { return _data; }

  /**
   3-dim array of the coefficients: data(i,n,m) stands for $(\mathbf{a}_{i+o_\mathrm{min}})_{nm}$
  */
  data_t const &data() const { return _data; }

  /// Copy
  MAKO_TAIL(MAKO_TAIL const &) = default;

  /// Move
  MAKO_TAIL(MAKO_TAIL &&) = default;

  /// Construction from a data
  template <typename ArrayType, typename Enable = std14::enable_if_t<arrays::ImmutableCuboidArray<ArrayType>::value>>
  MAKO_TAIL(ArrayType const &x)
     : _data(x) {}

  // mako ## ------------------------------------------------  regular class ---------------------------------
  // mako %if RVC == 'regular' :

  /// Default construction
  __tail() {
   mini_vector<int, T::rank + 1> sh;
   sh[0] = _size();
   for (int u = 0; u < T::rank; ++u) sh[u + 1] = 1;
   _data.resize(sh);
  }

  // FIXME : int only ?
  /// Construct from the target shape. Initialized to 0.
  template <typename Int> explicit __tail(mini_vector<Int, T::rank> sh) : _data(sh.front_append(_size())) { zero(); }

  /**
    * @param sh target shape.
    * @param n_orders number of moments in the tail ...
    * @param o_min ... starting at o_min.
    *
    * Only n_orders + o_min matters. Kept this way for backward compatibility.
    * Precondition : o_min >= order_min()
    *
    * Tail is initialized to 0 for n <= o_min + n_orders and NaN afterwards.
    */
  template <typename Int> explicit __tail(mini_vector<Int, T::rank> sh, int n_orders, int o_min) : __tail{sh} {
   if (o_min < order_min()) TRIQS_RUNTIME_ERROR << "Tail construction : o_min is < " << order_min();
   reset(o_min + n_orders + 1);
  }

  /**
    * Parameters are (for rank R)
    *
    * (n0, ... n_{R-1}) : dimensions of the target shape
    * or
    * (n0, ... n_{R-1}, n_orders, o_min)
    *
    * @param n_orders number of moments in the tail ...
    * @param o_min ... starting at o_min.
    *
    * Only n_orders + o_min matters. Kept this way for backward compatibility.
    * Precondition : o_min >= order_min()
    *
    * Tail is initialized to 0 for n <= o_min + n_orders and NaN afterwards.
    */
  template <typename... Args> TRIQS_DEPRECATED("Deprecated constructor") explicit __tail(int n0, Args const &... args) {
   constexpr int n_args = sizeof...(Args) + 1;
   static_assert((n_args - T::rank == 2) or ((n_args - T::rank == 0)), "Too many arguments in tail construction");
   mini_vector<int, T::rank> shape;
   mini_vector<int, n_args> _args{n0, int(args)...};
   for (int i = 0; i < T::rank; ++i) shape[i] = _args[i];
   *this = (n_args == T::rank ? __tail{shape} : __tail{shape, _args[n_args - 2], _args[n_args - 1]});
  }

  ///
  __tail(view_type const &x) : _data(x.data()) {}

  ///
  __tail(const_view_type const &x) : _data(x.data()) {}

  /// FIXME : only a shape ?
  /// Returns a omega. Args is anything from which to construct a tail (shape, tail to copy ...).
  template <typename... Args> static __tail omega(Args const &... args) {
   __tail t(args...);
   t.data()() = 0; // we can pass a tail for a copy, so we can not be sure data is 0.
   t(-1) = 1;      // if matrix, it will be unit.
   return t;
  }

  // mako ## ------------------------------------------------  const view class ---------------------------------
  // mako %elif RVC == 'const_view' :

  ///
  __tail_const_view(regular_type const &x) : _data(x.data()) {}

  ///
  __tail_const_view(mutable_view_type const &x) : _data(x.data()) {}

  /// Rebind the view
  void rebind(__tail_const_view X) { _data.rebind(X._data); }

  /// Rebind the view
  void rebind(__tail_view<T> X) { _data.rebind(X._data); }

  // mako ## ------------------------------------------------  view class ---------------------------------
  // mako %elif RVC == 'view' :

  /// View from a tail
  __tail_view(regular_type &x) : _data(x.data()) {}

  /// View from a tail
  __tail_view(regular_type &&x) : _data(x.data()) {}

  /// Rebind the view
  void rebind(__tail_view x) { _data.rebind(x._data); }

  // mako %endif

  /// Swap
  friend void swap(MAKO_TAIL &a, MAKO_TAIL &b) noexcept { swap(a._data, b._data); }

  /// Makes a view to itself
  view_type operator()() { return *this; }

  /// Makes a view to itself
  const_view_type operator()() const { return *this; }

  // mako ## ------------------------------------ Operator = ---------------------------------

  // mako %if RVC in ['regular', 'view'] :

  MAKO_TAIL &operator=(std::complex<double> const &x) {
   _data() = 0.0;
   rv_t(_data(0, ellipsis())) = x; // to have the matrix unit correct if matrix valued
   return *this;
  }

  MAKO_TAIL &operator=(const_view_type const &x) {
   _data = x.data();
   return *this;
  }
  MAKO_TAIL &operator=(regular_type const &x) { return operator=(const_view_type{x}); }
  MAKO_TAIL &operator=(view_type const &x) { return operator=(const_view_type{x}); }

  // mako ## ------------------------------------ reset ---------------------------------

  /// Sets order < n to 0 and Nan from n to order_max
  void reset(int n = order_min()) {
   n = std::min(n, order_max() + 1) - order_min();
   _data(range(0, n), ellipsis()) = 0;
   _data(range(n, _size()), ellipsis()) = std::numeric_limits<double>::quiet_NaN();
  }

  /// Sets the whole tail to 0
  void zero() { _data() = 0; }

  // mako %endif

  // mako ## ------------------------------------ inspection of the tail ---------------------------------
  // Is any element at the order NaN
  bool _isnan(int order) const { return any(isnan(_data(order - order_min(), ellipsis{}))); }

  // Is the element at this order > 0. DOES NOT CHECK NaN.
  bool is_non_zero(int order) const { return (max_element(abs(_data(order - order_min(), ellipsis{}))) > 1.e-10); }

  // Returns the first om which is non zero and non nan.
  //   order_max() + 1 is tail is all 0
  //   order_min() - 1 is tail is first non zero is nan.
  int smallest_nonzero() const {
   for (int om = order_min(); om <= order_max(); ++om) {
    if (_isnan(om)) return order_min() - 1;
    if (is_non_zero(om)) return om;
   }
   return order_max() + 1;
  }

  /// Is the tail empty, i.e. dominant order is nan.
  bool empty() const { return _isnan(order_min()); }

  /// Returns the last order which is non nan and  order_min() - 1 if tail is nan.
  int largest_non_nan() const {
   for (int om = order_min(); om <= order_max(); ++om)
    if (_isnan(om)) return om - 1;
   return order_max();
  }

  // for backward compat. only. The old order_min.
  int backwd_omin() const {
   for (int om = order_min(); om <= order_max(); ++om) {
    if (_isnan(om)) return om - 1;
    if (is_non_zero(om)) return om;
   }
   return order_max();
  }

  /// Number of valid orders in the tail
  int n_valid_orders() const {
   int om = order_min();
   while ((om <= order_max()) and (!_isnan(om))) { ++om; }
   return om - order_min();
  }
  ///
  arrays::mini_vector<int, T::rank> target_shape() const { return _data.shape().front_pop(); }

  /// true iif the tail decreasing at infinity
  bool is_decreasing_at_infinity() const {
   for (int om = order_min(); om <= 0; ++om) {
    if (_isnan(om) or (is_non_zero(om))) return false;
   }
   return true;
  }

  // mako ## ------------------------------------ operator() ---------------------------------

  public:
  /**
    * @param n order
    * @return $a_n$ as in $a_n/(i\omega)^n$
    */
  rv_t operator()(int n) {
   if (n > order_max()) TRIQS_RUNTIME_ERROR << " n > Max Order. n= " << n << ", Max Order = " << order_max();
   if (n < order_min()) TRIQS_RUNTIME_ERROR << " n < Min Order. n= " << n << ", Min Order = " << order_min();
   return _data(n - order_min(), ellipsis());
  }

  /**
    * @param n order
    * @return $a_n$ as in $a_n/(i\omega)^n$
    */
  crv_t operator()(int n) const {
   if (n > order_max()) TRIQS_RUNTIME_ERROR << " n > Max Order. n= " << n << ", Max Order = " << order_max();
   if (n < order_min()) TRIQS_RUNTIME_ERROR << " n < Min Order. n= " << n << ", Min Order = " << order_min();
   return _data(n - order_min(), ellipsis());
  }

#ifdef __cpp_if_constexpr

  /// same as (), but if n in an undefined order (i.e. NaN) it returns 0.
  crv_t get_or_zero(int n) const {
   auto r = operator()(n);
   if
    constexpr(!is_scalar_target) {
     if (!any(_isnan(r))) return r;
     auto r2 = make_clone(r);
     r2() = 0;
     return r2;
    }
   else {
    return (!_isnan(r) ? r : 0);
   }
  }

#else

  private:
  template <typename U> crv_t __get_or_zero(U, int n) const {
   auto r = _data(n - order_min(), ellipsis());
   if (!any(isnan(r))) return r;
   auto r2 = make_clone(r);
   r2() = 0;
   return r2;
  }
  crv_t __get_or_zero(scalar_valued, int n) const {
   auto r = _data(n - order_min()); // operator()(n);
   return (!isnan(r) ? r : 0);
  }

  public:
  /// same as (), but if n in an undefined order (i.e. NaN) it returns 0.
  crv_t get_or_zero(int n) const { return __get_or_zero(T{}, n); }
#endif

  friend std::string get_triqs_hdf5_data_scheme(MAKO_TAIL const &g) { return "TailGf"; }

  /// write to h5
  friend void h5_write(h5::group fg, std::string subgroup_name, MAKO_TAIL const &t) {
   auto gr = fg.create_group(subgroup_name);
   h5_write(gr, "data", t._data);
   h5_write(gr, "omin", t.order_min());
  }

  /// read from h5
  friend void h5_read(h5::group fg, std::string subgroup_name, MAKO_TAIL &t) {
   auto gr = fg.open_group(subgroup_name);
   if (!gr.has_key("mask")) {     // if no mask, we have the latest version of the tail
    h5_read(gr, "data", t._data); // Add here backward compat code IF order_min/max where to change
   } else {
    // backward compatibility code
    int omin = h5::h5_read<int>(gr, "omin");
    auto d = h5::h5_read<typename data_t::regular_type>(gr, "data");
    if (omin < t.order_min()) TRIQS_RUNTIME_ERROR << "Reading h5 file for a tail : order_min is < -2 which is not permitted";
    auto sh = d.shape();
    sh[0] = t._size();
    resize_or_check_if_view(t._data, sh);
    auto mask_arr = h5::h5_read<array<int, 2>>(gr, "mask");
    auto m = min_element(mask_arr);
    int omax = std::min(m, t.order_max());
    // int omax = std::min(omin + int(d.shape()[0]) - 1, t.order_max());
    // std::cout << "omax" << omax << std::endl;
    // std::cout << "omax" << omin << t._data.shape() << std::endl;
    // std::cout << omin - t.order_min() <<" ---- "<< omax + 1 - t.order_min()<< " ---"<<  omax - omin + 2 << std::endl;
    auto _ = ellipsis();
    t.reset(omax + 1);
    t._data(range(omin - t.order_min(), omax + 1 - t.order_min()), _) = d(range(0, omax - omin + 1), _);
   }
  }

  /// BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar &_data; }

  /// mpi broadcast
  friend void mpi_broadcast(MAKO_TAIL &t, mpi::communicator c = {}, int root = 0) { mpi_broadcast(t._data); }

  // mako ## --------------------------  implementation of some function for tail_const_view ---------------------------
  // mako %if RVC == 'const_view' :
  ///
  friend std::ostream &operator<<(std::ostream &out, __tail_const_view x) {
   if (x.empty()) return out << "empty tail" << std::endl;
   for (int u = x.order_min(); u <= x.order_max() and (!x._isnan(u)); ++u) out << "\n ...  Order " << u << " = " << x(u);
   return out;
  }

  /**
   *  Conjugate of the tail
   * \tilde(a)_n = coeff of order n in conj(tail) = (-)^n * conj(a_n)
   */
  friend regular_type conj(__tail_const_view t, bool imaginary) {
   auto r = regular_type{t};
   for (int j = 0; j < t._size(); j++)
    r.data()(j, ellipsis()) = (((j - order_min()) % 2 == 1 && imaginary) ? -1 : 1) * conj(r.data()(j, ellipsis()));
   return r;
  }

  /// transpose
  friend regular_type transpose(__tail_const_view t) { return {transposed_view(t.data(), 0, 2, 1)}; }

  // mako ## -------------------------- trivial implementation for tail and tail_view ---------------------------
  // mako %else :
  ///
  friend std::ostream &operator<<(std::ostream &out, MAKO_TAIL const &x) { return out << const_view_type{x}; }

  /// Conj
  friend regular_type conj(MAKO_TAIL const &x, bool imaginary) { return conj(const_view_type{x}, imaginary); }

  ///
  friend regular_type transpose(MAKO_TAIL const &x) { return transpose(const_view_type{x}); }

  // mako  %endif
 };

 // mako %endfor
 // mako %endfor
 // mako ## End of class tail, tail_view, tail_const_view

 // ---- mpi_reduce  -------------

 template <typename T>
 auto mpi_reduce(__tail_const_view<T> t, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
  __tail<T> r{t};
  r.data() = mpi_reduce(t.data(), c, root, all, op);
  return r;
 }

 template <typename T>
 auto mpi_reduce(__tail<T> const &t, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
  return mpi_reduce(__tail_const_view<T>{t}, c, root, all, op);
 }

 template <typename T>
 auto mpi_reduce(__tail_view<T> t, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
  return mpi_reduce(__tail_const_view<T>{t}, c, root, all, op);
 }

 // ---- Evaluate  -------------

 /// Evaluate the tail to sum_{n=order_min()}^ordermax M_n/omega^n
 template <typename T> auto evaluate(__tail_const_view<T> t, dcomplex const &omega) {
  using rv_t = typename __tail_const_view<T>::rv_t;
  auto r = typename T::slice_t{}; // is 0 if it is a double
#ifdef __cpp_if_constexpr
  if
   constexpr(T::rank != 0) {
    r.resize(t.target_shape());
    r() = 0;
   }
#else
  static_if(bool_<T::rank != 0>{}) // if not a number, need to resize and put to 0
      .then([&](auto &rr) {
       rr.resize(t.target_shape());
       rr() = 0;
      })(r);
#endif
  for (int u = t.largest_non_nan(); u >= t.order_min(); --u) r = r / omega + t(u); // NB () returns a matrix if matrix_valued
  r /= std::pow(omega, t.order_min());
  return r;
 }

 /// Evaluate the tail to  $\sum_{n={ordermin}}^{ordermax} M_n/{\omega}^n$
 template <typename T> auto evaluate(__tail<T> const &t, dcomplex const &omega) {
  return evaluate(__tail_const_view<T>{t}, omega);
 }

 /// Evaluate the tail to  $\sum_{n={ordermin}}^{ordermax} M_n/{\omega}^n$
 template <typename T> auto evaluate(__tail_view<T> const &t, dcomplex const &omega) {
  return evaluate(__tail_const_view<T>{t}, omega);
 }

 // ---- Compose  -------------

 /// Composition
 template <typename T> __tail<T> __compose(__tail_const_view<T> x, __tail_const_view<T> t) {
  __tail<T> r = x, z = t, t_inv = inverse(t);
  r.zero();
  r(0) = x(0);
  for (int u = -1; u >= x.order_min(); --u) {
   if (x._isnan(u)) continue;
   r += x(u) * z;
   z = z * t;
  }
  z = t_inv;
  for (int u = 1; u <= x.order_max(); ++u) {
   if (x._isnan(u)) break;
   r += x(u) * z;
   z = z * t_inv;
  }
  return r;
 }

 /// Composition
 template <typename A, typename B>
 std14::enable_if_t<is_tail_v<A> and is_tail_v<B>, typename A::regular_type> compose(A const &a, B const &b) {
  return __compose(a(), b());
 }

 // ---- Inversion  -------------

 /// Inverse of the tail
 template <typename T> __tail<T> inverse(__tail_const_view<T> const &t) {
  int omin1 = -t.smallest_nonzero();
  int omax1 = std::min(t.order_max() + 2 * omin1, t.order_max());
  int si = omax1 - omin1 + 1;

  __tail<T> res = t;
  if (t.empty() or (omin1 == -t.order_max() - 1)) { // is not invertible. return nan
   res.reset();
   return res;
  }
  res.data()() = 0;
  res(omin1) = inverse(t(-omin1));

  for (int n = 1; n < si; n++) {
   for (int p = 0; p < n; p++) { res(omin1 + n) -= t(n - omin1 - p) * res(omin1 + p); }
   res(omin1 + n) = res(omin1) * res(omin1 + n);
  }
  return res;
 }

 /// Inverse of the tail
 template <typename T> __tail<T> inverse(__tail<T> const &x) { return inverse(__tail_const_view<T>{x}); }

 /// Inverse of the tail
 template <typename T> __tail<T> inverse(__tail_view<T> const &x) { return inverse(__tail_const_view<T>{x}); }


 // ----  -------------
 //
 // FIXME : make_fun_impl CHANGE THE NAME : it is not an implementation detail ...
 // We evaluate the expression into the tail.
 // If the placeholder of the expression is a placeholder of rank > 0, we do nothing...
 template <typename T, typename Expr, int... Is>
 void assign_singularity_from_function(__tail_view<T> t, clef::make_fun_impl<Expr, Is...> const &rhs) {
  static_assert(sizeof...(Is) == 1, "???"); // one var only
  // We assign iif the placeholder of the expression has rank 0
  constexpr bool ph_has_rank0 = clef::get_placeholder_rank(clef::_ph<Is...>()) == 0;
#ifdef __cpp_if_constexpr
  if
   constexpr(ph_has_rank0) { t = rhs(__tail<T>::omega(t.data())); }
  else {
   t.reset();
  }
#else
  static_if(bool_<ph_has_rank0>{})
      .then([&](auto &&t) { t = rhs(__tail<T>::omega(t.data())); })
      .else_([&](auto &&t) { t.reset(); })(t);
#endif
 }

 // FIXME : remove this.
 template <typename T, typename RHS> void assign_singularity_from_function(__tail<T> &t, RHS const &rhs) {
  assign_singularity_from_function(__tail_view<T>{t}, rhs);
 }

 // ----  Slice T  -------------

 ///
 template <typename T, typename... Args> __tail_const_view<T> slice_target_sing(__tail<T> const &t, Args const &... args) {
  return {t.data()(arrays::range{}, args...)};
 }

 ///
 template <typename T, typename... Args> __tail_const_view<T> slice_target_sing(__tail_const_view<T> t, Args const &... args) {
  return {t.data()(arrays::range{}, args...)};
 }

 ///
 template <typename T, typename... Args> __tail_view<T> slice_target_sing(__tail<T> &t, Args const &... args) {
  return {t.data()(arrays::range{}, args...)};
 }

 ///
 template <typename T, typename... Args> __tail_view<T> slice_target_sing(__tail_view<T> t, Args const &... args) {
  return {t.data()(arrays::range{}, args...)};
 }

 // ----  Slice target to scalar  -------------

 template <typename T, typename... Args>
 __tail_view<scalar_valued> slice_target_to_scalar_sing(__tail_view<T> t, Args const &... args) {
  return {t.data()(arrays::range{}, args...)};
 }

 ///
 template <typename T, typename... Args>
 __tail_const_view<scalar_valued> slice_target_to_scalar_sing(__tail_const_view<T> t, Args const &... args) {
  return {t.data()(arrays::range{}, args...)};
 }

 template <typename T, typename... Args>
 __tail_view<scalar_valued> slice_target_to_scalar_sing(__tail<T> &t, Args const &... args) {
  return {t.data()(arrays::range{}, args...)};
 }

 ///
 template <typename T, typename... Args>
 __tail_const_view<scalar_valued> slice_target_to_scalar_sing(__tail<T> const &t, Args const &... args) {
  return {t.data()(arrays::range{}, args...)};
 }


 /// ------------------- Arithmetic operations ------------------------------

 // +/- with scalar or matrix.
 template <typename T> constexpr bool is_scalar_or_array_v = arrays::ImmutableArray_v<T> || utility::is_in_ZRC<T>::value;

 template <typename T> constexpr bool is_matrix_v = arrays::ImmutableMatrix_v<T>;

 template <typename T>
 constexpr bool is_scalar_or_array_or_matrix_v = arrays::ImmutableCuboidArray_v<T> || utility::is_in_ZRC<T>::value;

 // ----- addition, substraction

 // mako %for OP in ['+', '-'] :
 template <typename A, typename B>
 std14::enable_if_t<is_tail_v<A> and is_tail_v<B>, typename A::regular_type> operator MAKO_OP(A const &a, B const &b) {
  return {typename A::regular_type::data_t(a.data() MAKO_OP b.data())};
 }

 template <typename A, typename B>
 std14::enable_if_t<is_tail_v<A> and is_scalar_or_array_or_matrix_v<B>, typename A::regular_type>
 operator MAKO_OP(A const &a, B const &b) {
  typename A::regular_type res = a;
  res(0) = res(0) MAKO_OP b;
  return res;
 }

 template <typename A, typename B>
 std14::enable_if_t<is_scalar_or_array_or_matrix_v<A> and is_tail_v<B>, typename B::regular_type>
 operator MAKO_OP(A const &a, B const &b) {
  typename B::regular_type res = b;
  // mako %if OP=='-' :
  res.data() = -res.data();
  // mako %endif
  res(0) = a + res(0);
  return res;
 }
 // mako %endfor

 // ----- multiplication

 template <typename T> __tail<T> operator*(__tail_const_view<T> const &l, __tail_const_view<T> const &r) {
  if ((T::rank != 0) and (l.data().shape()[2] != r.data().shape()[1]))
   TRIQS_RUNTIME_ERROR << "tail multiplication: shape mismatch";
  // FIXME : error in non square !!
  __tail<T> res = l;
  res.zero();
  for (int n = res.order_min(); n <= res.order_max(); ++n) {
   // sum_{p}^n a_p b_{n-p}. p <= a.n_max, p >= a.n_min and n-p <=b.n_max and n-p >= b.n_min
   // hence p <= min ( a.n_max, n-b.n_min ) and p >= max ( a.n_min, n- b.n_max)
   const int pmin = std::max(l.order_min(), n - r.order_max());
   const int pmax = std::min(l.order_max(), n - r.order_min());
   for (int p = pmin; p <= pmax; ++p) { res(n) += l(p) * r(n - p); }
  }
  return res;
 }

 template <typename A, typename B>
 std14::enable_if_t<is_tail_v<A> and is_tail_v<B>, typename A::regular_type> operator*(A const &a, B const &b) {
  return a() * b();
 }

 // the matrix case is special since the SHAPE can change !!
 template <typename A, typename B>
 std14::enable_if_t<is_matrix_v<A> and is_tail_v<B>, typename B::regular_type> operator*(A const &a, B const &b) {
  typename B::regular_type res(make_shape(first_dim(a), b.data().shape()[1 + 1]));
  for (int n = b.order_min(); n <= b.order_max(); ++n) res(n) = a * b(n);
  return res;
 }

 template <typename A, typename B>
 std14::enable_if_t<is_tail_v<A> and is_matrix_v<B>, typename A::regular_type> operator*(A const &a, B const &b) {
  typename A::regular_type res(make_shape(a.data().shape()[1 + 0], second_dim(b)));
  for (int n = a.order_min(); n <= a.order_max(); ++n) res(n) = a(n) * b;
  return res;
 }

 // scalar valued or tensor_valued
 template <typename A, typename B>
 std14::enable_if_t<is_scalar_or_array_v<A> and is_tail_v<B>, typename B::regular_type> operator*(A const &a,
                                                                                                          B const &b) {
  typename B::regular_type res = b;
  for (int n = b.order_min(); n <= b.order_max(); ++n) res(n) = a * b(n);
  return res;
 }

 template <typename A, typename B>
 std14::enable_if_t<is_tail_v<A> and is_scalar_or_array_v<B>, typename A::regular_type> operator*(A const &a, B const &b) {
  typename A::regular_type res = a;
  for (int n = a.order_min(); n <= a.order_max(); ++n) res(n) = a(n) * b;
  return res;
 }

 // ----- division

 template <typename A, typename B>
 std14::enable_if_t<is_tail_v<B> and is_tail_v<A>, typename B::regular_type> operator/(A const &a, B const &b) {
  return a * inverse(b);
 }

 template <typename A, typename B>
 std14::enable_if_t<is_tail_v<B> and is_in_ZRC<A>::value, typename B::regular_type> operator/(A const &a, B const &b) {
  return a * inverse(b);
 }

 template <typename A, typename B>
 std14::enable_if_t<is_tail_v<A> and is_scalar_or_array_v<B>, typename A::regular_type> operator/(A const &a, B const &b) {
  return a * (1.0 / b);
 }

 template <typename A, typename B>
 std14::enable_if_t<is_tail_v<A> and is_matrix_v<B>, typename A::regular_type> operator/(A const &a, B const &b) {
  return a * inverse(b);
 }

 // ----- compound

 template <typename T, typename U> void operator+=(__tail_view<T> g, U &&x) { g = g + x; }
 template <typename T, typename U> void operator-=(__tail_view<T> g, U &&x) { g = g - x; }
 template <typename T, typename U> void operator*=(__tail_view<T> g, U &&x) { g = g * x; }
 template <typename T, typename U> void operator/=(__tail_view<T> g, U &&x) { g = g / x; }

 template <typename T, typename U> void operator+=(__tail<T> &g, U &&x) { g = g + x; }
 template <typename T, typename U> void operator-=(__tail<T> &g, U &&x) { g = g - x; }
 template <typename T, typename U> void operator*=(__tail<T> &g, U &&x) { g = g * x; }
 template <typename T, typename U> void operator/=(__tail<T> &g, U &&x) { g = g / x; }

 // ----- reinterpret_as_matrix_valued

 inline __tail_view<matrix_valued> reinterpret_as_matrix_valued_sing(__tail_view<scalar_valued> t) {
  return {reinterpret_array_add_1x1(t.data())};
 }
 inline __tail_const_view<matrix_valued> reinterpret_as_matrix_valued_sing(__tail_const_view<scalar_valued> t) {
  return {reinterpret_array_add_1x1(t.data())};
 }

 // ----- make_tail_view_from_data. make a tail_view from the data, determining if it is a const_view or not
 //
 // if A is a const_view or A is passed by const &, then the proxy is const, otherwise it is not.
 template <typename A>
 std14::conditional_t<std14::decay_t<A>::is_const || std::is_const<std14::remove_reference_t<A>>::value,
                      __tail_const_view<target_from_array<A, 1>>, __tail_view<target_from_array<A, 1>>>
 make_tail_view_from_data(A &&a) {
  return {std::forward<A>(a)};
 }
}
}

