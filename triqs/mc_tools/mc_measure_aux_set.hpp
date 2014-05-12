/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#include <functional>
#include <memory>
#include <map>

namespace triqs {
namespace mc_tools {

 // mini concept checking : does T have void operator()() ?
 template <typename T, typename Enable = void> struct is_callable : std::false_type {};
 template <typename T> struct is_callable<T, decltype(std::declval<T>()())> : std::true_type {};

 //--------------------------------------------------------------------

 class measure_aux {

  std::shared_ptr<void> impl_;
  std::function<void()> call_;

  public:
  template <typename MeasureAuxType> measure_aux(std::shared_ptr<MeasureAuxType> p) {
   static_assert(is_callable<MeasureAuxType>::value, "This measure_aux is not callable");
   impl_ = p;
   call_ = [p]() mutable { (*p)(); }; // without the mutable, the operator () of the lambda is const, hence p ...
  }

  void operator()() { call_(); }
 };

 }
} // end namespace

