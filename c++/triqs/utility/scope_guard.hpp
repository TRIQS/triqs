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

/**
 * @file
 * @brief RAII helper that runs a user-provided cleanup on scope exit.
 */

#pragma once

#include <utility>

namespace triqs::utility {

  /**
   * @addtogroup triqs-utility-other
   * @{
   */

  /**
   * @brief RAII wrapper that invokes a callable when it goes out of scope, unless dismissed.
   * @tparam F Callable type.
   */
  template <class F> class scope_guard {
    F f_;
    bool active_;

    public:
    /**
     * @brief Construct a guard from a callable.
     * @details The callable is run in the destructor unless dismiss() is called first.
     * @param f Callable to store and run at scope exit.
     */
    scope_guard(F f) : f_(std::move(f)), active_(true) {}

    /**
     * @brief Destructor.
     * @details Invokes the stored callable if the guard is still active.
     */
    ~scope_guard() {
      if (active_) f_();
    }

    /**
     * @brief Disable the guard so the stored callable is *not* invoked on destruction.
     */
    void dismiss() { active_ = false; }

    /**
     * @brief Query whether the guard is still active.
     * @return True if the stored callable will run on destruction, false if dismissed.
     */
    bool active() const { return active_; }

    /// Default construction is disabled: a guard must always wrap a callable.
    scope_guard() = delete;

    /// Copy construction is disabled: a guard owns a unique cleanup action.
    scope_guard(const scope_guard &) = delete;

    /**
     * @brief Move constructor.
     * @details Takes ownership of the callable and dismisses the other guard so the action runs only once.
     * @param rhs Guard to move from. It is left dismissed afterwards.
     */
    scope_guard(scope_guard &&rhs) noexcept : f_(std::move(rhs.f_)), active_(rhs.active_) { rhs.dismiss(); }

    /// Copy assignment is disabled: a guard owns a unique cleanup action.
    scope_guard &operator=(const scope_guard &) = delete;

    /**
     * @brief Move assignment.
     * 
     * @details Swaps the stored callable and activity flag with the other guard.
     * 
     * @param rhs Guard to move from.
     * @return Reference to `*this`.
     */
    scope_guard &operator=(scope_guard &&rhs) noexcept {
      using std::swap;
      swap(f_, rhs.f_);
      swap(active_, rhs.active_);
      return *this;
    }
  };

  /**
   * @brief Build a @ref scope_guard from a callable. 
   * 
   * @details The returned object must be bound to a named variable.
   * 
   * @tparam F Callable type.
   * @param f Callable to run at scope exit.
   * @return A scope guard wrapping `f`.
   */
  template <class F> scope_guard<F> exec_at_scope_exit(F f) { return {std::move(f)}; }

  /**
   * @brief Build a @ref scope_guard from a callable.
   * 
   * @details The returned object must be bound to a named variable.
   * 
   * @tparam F Callable type.
   * @param f Callable to run at scope exit.
   * @return A scope guard wrapping `f`.
   */
  template <class F> scope_guard<F> make_scope_guard(F f) { return {std::move(f)}; }

  /** @} */

  namespace detail {

    // Tag type used by the SCOPE_EXIT macro to build a scope_guard via operator+.
    enum class _scope_guard_on_exit {};

    // Build a scope_guard from a callable, used to implement the SCOPE_EXIT macro.
    template <typename F> scope_guard<F> operator+(_scope_guard_on_exit, F &&f) { return {std::forward<F>(f)}; }

  } // namespace detail

/**
 * @addtogroup triqs-utility-macros
 * @{
 */

// Helper macro for TRIQS_CONCAT.
#define TRIQS_CONCAT2(X1, X2) X1##X2

/// Concatenate two tokens, expanding them first if they are macros.
#define TRIQS_CONCAT(X1, X2) TRIQS_CONCAT2(X1, X2)

#ifdef __COUNTER__
/// Create a scope guard that runs the provided callable at scope exit.
// NOLINTNEXTLINE
#define SCOPE_EXIT auto TRIQS_CONCAT(SCOPE_EXIT_ANONYMOUS_VARIABLE, __COUNTER__) = triqs::utility::detail::_scope_guard_on_exit{} + [&]()
#else
#error "__COUNTER__ not defined for this compiler"
#endif // __COUNTER__

/** @} */

} // namespace triqs::utility
