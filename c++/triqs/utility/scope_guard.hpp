// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Olivier Parcollet, Nils Wentzell

#pragma once
#include <utility>

namespace triqs {
  namespace utility {

    // adapted from A. Alexandrescu talk, C++ & beyond 2013.
    template <class Fun> class scope_guard {
      Fun f_;
      bool active_;

      public:
      scope_guard(Fun f) : f_(std::move(f)), active_(true) {}
      ~scope_guard() {
        if (active_) f_();
      }
      void dismiss() { active_ = false; }
      bool active() const { return active_; }
      scope_guard()                    = delete;
      scope_guard(const scope_guard &) = delete;
      scope_guard(scope_guard &&rhs) : f_(std::move(rhs.f_)), active_(rhs.active_) { rhs.dismiss(); }
      scope_guard &operator=(const scope_guard &) = delete;
      scope_guard &operator=(scope_guard &&rhs) {
        using std::swap;
        swap(f_, rhs.f_);
        swap(active_, rhs.active_);
        return *this;
      }
    };

    /// The function f will be called at the end of the scope, whatever happens.
    template <class Fun> scope_guard<Fun> exec_at_scope_exit(Fun f) { return {std::move(f)}; }

    template <class Fun> scope_guard<Fun> make_scope_guard(Fun f) { return {std::move(f)}; }

    // SCOPE_EXIT macro
    namespace detail {

      enum class _scope_guard_on_exit {};
      template <typename F> scope_guard<F> operator+(_scope_guard_on_exit, F &&f) { return {std::forward<F>(f)}; }

#define TRIQS_CONCAT2(X1, X2) X1##X2
#define TRIQS_CONCAT(X1, X2) TRIQS_CONCAT2(X1, X2)
#ifdef __COUNTER__
#define SCOPE_EXIT auto TRIQS_CONCAT(SCOPE_EXIT_ANONYMOUS_VARIABLE, __COUNTER__) = triqs::utility::detail::_scope_guard_on_exit{} + [&]()
#else
#error "__COUNTER__ not defined for this compiler"
#endif
    } // namespace detail
  }   // namespace utility
} // namespace triqs
