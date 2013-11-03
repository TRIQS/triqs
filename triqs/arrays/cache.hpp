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
#ifndef TRIQS_ARRAYS_CACHE_H
#define TRIQS_ARRAYS_CACHE_H
#include "./array.hpp"
#include <triqs/utility/view_tools.hpp>
#include <triqs/utility/c14.hpp>
namespace triqs {
namespace arrays {

 // ----------------- the implementation class -----------------------
 template <typename DataType, bool IsConst> class cache_impl {
  protected:
  typedef memory_layout<DataType::domain_type::rank> ml_t;
  ml_t ml;
  std::c14::conditional_t<IsConst, typename const_view_type_if_exists_else_type<DataType>::type,
                          typename view_type_if_exists_else_type<DataType>::type> keeper;
  bool need_copy;
  using CacheType = array<typename DataType::value_type, DataType::domain_type::rank>;
  using exposed_view_type = std::c14::conditional_t<IsConst, typename CacheType::const_view_type, typename CacheType::view_type>;
  struct internal_data {
   CacheType copy;
   exposed_view_type view;
   internal_data(cache_impl const& P, ml_t ml) : copy(CacheType(P.keeper, ml)), view(copy) {
#ifdef TRIQS_ARRAYS_CACHE_COPY_VERBOSE
    std::cerr << " Cache : copy made " << std::endl << " -- TRACE = --" << std::endl << triqs::utility::stack_trace()
              << std::endl;
#endif
   }
  };
  friend struct internal_data;
  mutable std::shared_ptr<internal_data> _id;
  internal_data& id() const {
   if (!_id) _id = std::make_shared<internal_data>(*this, ml);
   return *_id;
  }

  // avoid compiling the transformation keeper-> exposed_view_type when it does not make sense, e.g. expressions
  exposed_view_type view1(std::true_type) const {
   if (need_copy)
    return id().view;
   else
    return keeper;
  }
  exposed_view_type view1(std::false_type) const { return id().view; }
  exposed_view_type view2() const { return view1(bool_constant<is_amv_value_or_view_class<DataType>::value>()); }

  bool need_copy_dynamic(DataType const& x, std::false_type) const { return false; }
  bool need_copy_dynamic(DataType const& x, std::true_type) const { return (x.indexmap().memory_indices_layout() != ml); }

  void back_update(std::true_type) {}
  void back_update(std::false_type) {
   if (need_copy) keeper = id().view;
  }

  public:
  cache_impl(DataType const& x, ml_t ml_ = ml_t()) : ml(ml_), keeper(x) {
   need_copy = (!is_amv_value_or_view_class<DataType>::value) || need_copy_dynamic(x, is_amv_value_or_view_class<DataType>()) ||
               (!has_contiguous_data(x));
  }

  ~cache_impl() { back_update(bool_constant< IsConst>()); }
 
  cache_impl(cache_impl &&) = default; 
  cache_impl(cache_impl const&) = delete; 
  cache_impl& operator =(cache_impl const&) = delete; 
  cache_impl& operator =(cache_impl &&) = delete; 

  void update() {
   if (need_copy && _id) id().view = keeper;
  }

  exposed_view_type view() const { return view2(); }
  operator exposed_view_type() const { return view2(); }
 };

 // ---------------- Users class and factories ------------------

 template <typename DataType> using cache = cache_impl<DataType, false>;
 template <typename DataType> using const_cache = cache_impl<DataType, true>;

 template <typename D>
 const_cache<D> make_const_cache(D const& x, memory_layout<D::domain_type::rank> ml = memory_layout<D::domain_type::rank>{'C'}) {
  return {x, ml};
 }

 template <typename D>
 cache<D> make_cache(D const& x, memory_layout<D::domain_type::rank> ml = memory_layout<D::domain_type::rank>{'C'}) {
  return {x, ml};
 }
}
} // namespace triqs::arrays
#endif

