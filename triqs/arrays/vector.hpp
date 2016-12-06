/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
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
#include "indexmaps/cuboid/map.hpp"
#include "indexmaps/cuboid/slice.hpp"
#include "impl/indexmap_storage_pair.hpp"
#include "impl/assignment.hpp"
#include <algorithm>

namespace triqs {
namespace arrays {

 template <typename ValueType, bool Borrowed = false, bool IsConst = false> class vector_view;
 template <typename ValueType> class vector;

// ---------------------- vector_view --------------------------------

#define IMPL_TYPE                                                                                                                \
 indexmap_storage_pair<indexmaps::cuboid::map<1>, storages::shared_block<ValueType, Borrowed>, void, IsConst, true,              \
                       Tag::vector_view>

 /** */
 template <typename ValueType, bool Borrowed, bool IsConst>
 class vector_view : Tag::vector_view, TRIQS_CONCEPT_TAG_NAME(MutableVector), public IMPL_TYPE {
  public:
  using regular_type = vector<ValueType>;
  using view_type = vector_view<ValueType, false>;
  using const_view_type = vector_view<ValueType, false, true>;
  using weak_view_type = vector_view<ValueType, true>;
  using indexmap_type = typename IMPL_TYPE::indexmap_type;
  using storage_type = typename IMPL_TYPE::storage_type;

  /// Build from an IndexMap and a storage
  template <typename S> vector_view(indexmaps::cuboid::map<1> const& Ind, S const& Mem) : IMPL_TYPE(Ind, Mem) {}

  /// Build from anything that has an indexmap and a storage compatible with this class
  template <typename ISP> vector_view(const ISP& X) : IMPL_TYPE(X.indexmap(), X.storage()) {}

#ifdef TRIQS_WITH_PYTHON_SUPPORT
  /// Build from a numpy.array : throws if X is not a numpy.array
  explicit vector_view(PyObject* X) : IMPL_TYPE(X, false, "vector_view ") {}
#endif

  /// Copy construction
  vector_view(vector_view const& X) : IMPL_TYPE(X.indexmap(), X.storage()) {}

  vector_view() = delete;

  // Move
  vector_view(vector_view&& X) { this->swap_me(X); }

  /// Swap
  friend void swap(vector_view& A, vector_view& B) { A.swap_me(B); }

  /// Rebind the view
  void rebind(vector_view const& X) {
   this->indexmap_ = X.indexmap_;
   this->storage_ = X.storage_;
  }

  // rebind the other view, iif this is const, and the other is not.
  template <bool C = IsConst> ENABLE_IFC(C) rebind(vector_view<ValueType, Borrowed, !IsConst> const& X) {
   this->indexmap_ = X.indexmap_;
   this->storage_ = X.storage_;
  }

  /** Assignment.  The size of the array MUST match exactly.  */
  template <typename RHS> vector_view& operator=(const RHS& X) {
   triqs_arrays_assign_delegation(*this, X);
   return *this;
  }

  vector_view& operator=(vector_view const& X) {
   triqs_arrays_assign_delegation(*this, X);
   return *this;
  } // cf array_view class comment

  // Move assignment not defined : will use the copy = since view must copy data

  size_t size() const { return this->shape()[0]; }
  std::ptrdiff_t stride() const { return this->indexmap().strides()[0]; }

  TRIQS_DEFINE_COMPOUND_OPERATORS(vector_view);

  // to make interface similar to std::vector : forward [] to ()
  template <typename Arg> decltype(auto) operator[](Arg&& arg) const { return IMPL_TYPE::operator()(std::forward<Arg>(arg));}
  template <typename Arg> decltype(auto) operator[](Arg&& arg) { return IMPL_TYPE::operator()(std::forward<Arg>(arg));}
 };
#undef IMPL_TYPE

 template <class V, int R, bool Borrowed, bool IsConst>
 struct ISPViewType<V, R, void, Tag::vector_view, Borrowed, IsConst> {
  using type = vector_view<V, Borrowed, IsConst>;
 };

 template <typename ValueType, bool Borrowed = false> using vector_const_view = vector_view<ValueType, Borrowed, true>;

// ---------------------- vector--------------------------------
#define IMPL_TYPE                                                                                                                \
 indexmap_storage_pair<indexmaps::cuboid::map<1>, storages::shared_block<ValueType>, void, false, false, Tag::vector_view>

 template <typename ValueType> class vector : Tag::vector, TRIQS_CONCEPT_TAG_NAME(MutableVector), public IMPL_TYPE {
  public:
  using value_type = typename IMPL_TYPE::value_type;
  using storage_type = typename IMPL_TYPE::storage_type;
  using indexmap_type = typename IMPL_TYPE::indexmap_type;
  using regular_type = vector<ValueType>;
  using view_type = vector_view<ValueType>;
  using const_view_type = vector_view<ValueType, false, true>;
  using weak_view_type = vector_view<ValueType, true>;

  /// Empty vector.
  vector() {}

  // Move
  explicit vector(vector&& X) { this->swap_me(X); }

  ///
  vector(size_t dim) : IMPL_TYPE(indexmap_type(mini_vector<size_t, 1>(dim))) {}

  /// to mimic std vector
  template <typename Arg> vector(size_t dim, Arg&& arg) : IMPL_TYPE(indexmap_type(mini_vector<size_t, 1>(dim))) {
   (*this)() = std::forward<Arg>(arg);
  }

  /** Makes a true (deep) copy of the data. */
  vector(const vector& X) : IMPL_TYPE(X.indexmap(), X.storage().clone()) {}

  /**
   * Build a new vector from X.domain() and fill it with by evaluating X. X can be :
   *  - another type of array, array_view, matrix,.... (any <IndexMap, Storage> pair)
   *  - a expression : e.g. array<int, IndexOrder::C<1> > A( B+ 2*C);
   *  - ml : useless directly, since there only one ml, but used in generic code it maintains the same constructor as array,
   * matrix
   */
  template <typename T>
  vector(const T& X,
         std14::enable_if_t<ImmutableCuboidArray<T>::value && std::is_convertible<typename T::value_type, value_type>::value,
                            memory_layout<1>> ml = memory_layout<1> {})
     : IMPL_TYPE(indexmap_type(X.domain(), ml)) {
   triqs_arrays_assign_delegation(*this, X);
  }

#ifdef TRIQS_WITH_PYTHON_SUPPORT
  /// Build from a numpy.array X (or any object from which numpy can make a numpy.array). Makes a copy.
  explicit vector(PyObject* X) : IMPL_TYPE(X, true, "vector ") {}
#endif

  // build from a init_list
  template <typename T>
  vector(std::initializer_list<T> const& l)
     : IMPL_TYPE(indexmap_type(mini_vector<size_t, 1>(l.size()), memory_layout<1>{})) {
   size_t i = 0;
   for (auto const& x : l) (*this)(i++) = x;
  }

  /**
   * Resizes the vector. NB : all references to the storage is invalidated.
   * Does not initialize the vector by default: to resize and init, do resize(IND).init()
   */
  vector& resize(size_t L) {
   IMPL_TYPE::resize(typename IMPL_TYPE::domain_type(mini_vector<size_t, 1>(L)));
   return *this;
  }

  /**
   * Resizes the vector. NB : all references to the storage is invalidated.
   * Does not initialize the vector by default: to resize and init, do resize(IND).init()
   */
  vector& resize(const indexmaps::cuboid::domain_t<IMPL_TYPE::rank>& l) {
   IMPL_TYPE::resize(l);
   return *this;
  }

  /// Assignement resizes the vector.  All references to the storage are therefore invalidated.
  vector& operator=(const vector& X) {
   IMPL_TYPE::resize_and_clone_data(X);
   return *this;
  }

  /**
   * Assignement resizes the vector.  All references to the storage are therefore invalidated.
   * NB : to avoid that, do make_view(A) = X instead of A = X
   */
  template <typename RHS> vector& operator=(const RHS& X) {
   static_assert(ImmutableCuboidArray<RHS>::value, "Assignment : RHS not supported");
   IMPL_TYPE::resize(X.domain());
   triqs_arrays_assign_delegation(*this, X);
   return *this;
  }

  /// Move assignment
  vector& operator=(vector&& X) {
   this->swap_me(X);
   return *this;
  }

  friend void swap(vector& A, vector& B) { A.swap_me(B); }

  size_t size() const { return this->shape()[0]; }
  std::ptrdiff_t stride() const { return this->indexmap().strides()[0]; }

  TRIQS_DEFINE_COMPOUND_OPERATORS(vector);

  // to make interface similar to std::vector : forward [] to ()
  template <typename Arg> decltype(auto) operator[](Arg&& arg) const { return IMPL_TYPE::operator()(std::forward<Arg>(arg));}
  template <typename Arg> decltype(auto) operator[](Arg&& arg) { return IMPL_TYPE::operator()(std::forward<Arg>(arg));}

 }; // vector class
}
} // namespace triqs::arrays

#undef IMPL_TYPE

#include "./blas_lapack/scal.hpp"
#include "./blas_lapack/copy.hpp"
#include "./blas_lapack/swap.hpp"
#include "./blas_lapack/axpy.hpp"
namespace triqs {
namespace arrays {

 // norm2 squared
 template <typename V> std14::enable_if_t<ImmutableVector<V>::value, typename V::value_type> norm2_sqr(V const& a) {
  int dim = a.size();
  auto r = typename V::value_type{};
  for (int i = 0; i < dim; ++i) r += a(i) * a(i);
  return r;
 }

 // norm2
 template <typename V> std14::enable_if_t<ImmutableVector<V>::value, typename V::value_type> norm2(V const& a) {
  using std::sqrt;
  return sqrt(norm2(a));
 }

 // lexicographical comparison operators
 template <typename V1, typename V2>
 std14::enable_if_t<ImmutableVector<V1>::value&& ImmutableVector<V2>::value, bool> operator<(V1 const& a, V2 const& b) {
  return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
 }

 template <typename V1, typename V2>
 std14::enable_if_t<ImmutableVector<V1>::value&& ImmutableVector<V2>::value, bool> operator>(V1 const& a, V2 const& b) {
  return (b < a);
 }

 template <typename RHS, typename T>
 std14::enable_if_t<is_vector_or_view<RHS>::value> triqs_arrays_assign_delegation(vector<T>& lhs, RHS const& rhs) {
  blas::copy(rhs, lhs);
 }

 template <typename RHS, typename T>
 std14::enable_if_t<is_vector_or_view<RHS>::value> triqs_arrays_compound_assign_delegation(vector<T>& lhs, RHS const& rhs,
                                                                                           char_<'A'>) {
  T a = 1.0;
  blas::axpy(a, rhs, lhs);
 }

 template <typename RHS, typename T>
 std14::enable_if_t<is_vector_or_view<RHS>::value> triqs_arrays_compound_assign_delegation(vector<T>& lhs, RHS const& rhs,
                                                                                           char_<'S'>) {
  T a = -1.0;
  blas::axpy(a, rhs, lhs);
 }

 template <typename RHS, typename T>
 std14::enable_if_t<is_scalar_for<RHS, vector<T>>::value> triqs_arrays_compound_assign_delegation(vector<T>& lhs, RHS const& rhs,
                                                                                                  char_<'M'>) {
  T a = rhs;
  blas::scal(a, lhs);
 }

 template <typename RHS, typename T>
 std14::enable_if_t<is_scalar_for<RHS, vector<T>>::value> triqs_arrays_compound_assign_delegation(vector<T>& lhs, RHS const& rhs,
                                                                                                  char_<'D'>) {
  T a = 1 / rhs;
  blas::scal(a, lhs);
 }

 template <typename RHS, typename T>
 std14::enable_if_t<is_vector_or_view<RHS>::value> triqs_arrays_assign_delegation(vector_view<T>& lhs, RHS const& rhs) {
  blas::copy(rhs, lhs);
 }

 template <typename RHS, typename T>
 std14::enable_if_t<is_vector_or_view<RHS>::value> triqs_arrays_compound_assign_delegation(vector_view<T>& lhs, RHS const& rhs,
                                                                                           char_<'A'>) {
  T a = 1.0;
  blas::axpy(a, rhs, lhs);
 }

 template <typename RHS, typename T>
 std14::enable_if_t<is_vector_or_view<RHS>::value> triqs_arrays_compound_assign_delegation(vector_view<T>& lhs, RHS const& rhs,
                                                                                           char_<'S'>) {
  T a = -1.0;
  blas::axpy(a, rhs, lhs);
 }

 template <typename RHS, typename T>
 std14::enable_if_t<is_scalar_for<RHS, vector_view<T>>::value>
 triqs_arrays_compound_assign_delegation(vector_view<T>& lhs, RHS const& rhs, char_<'M'>) {
  T a = rhs;
  blas::scal(a, lhs);
 }

 template <typename RHS, typename T>
 std14::enable_if_t<is_scalar_for<RHS, vector_view<T>>::value>
 triqs_arrays_compound_assign_delegation(vector_view<T>& lhs, RHS const& rhs, char_<'D'>) {
  T a = 1 / rhs;
  blas::scal(a, lhs);
 }

 template <typename T, bool Borrowed>
 void triqs_arrays_assign_delegation(vector_view<T, Borrowed>& av, std::vector<T> const& vec) {
  std::size_t size = vec.size();
  for (std::size_t n = 0; n < size; ++n) av(n) = vec[n];
 }

 // swapping 2 vector
 template <typename V, bool B1, bool B2, bool B3, bool B4> void deep_swap(vector_view<V, B1, B2> x, vector_view<V, B3, B4> y) {
  blas::swap(x, y);
 }

 template <typename V> void deep_swap(vector<V>& x, vector<V>& y) { blas::swap(x, y); }
}
}

// The std::swap is WRONG for a view because of the copy/move semantics of view.
// Use swap instead (the correct one, found by ADL).
namespace std {
template <typename V, bool B1, bool B2, bool C1, bool C2>
void swap(triqs::arrays::vector_view<V, B1, C1>& a, triqs::arrays::vector_view<V, B2, C2>& b) = delete;
}

#include "./expression_template/vector_algebra.hpp"

