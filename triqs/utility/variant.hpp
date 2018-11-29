/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by I. Krivenko and O. Parcollet
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

#ifdef TRIQS_USE_STD_VARIANT
#include <variant>
#else 

#include <triqs/utility/exceptions.hpp>
#include <triqs/utility/c14.hpp>
#include <utility>
#include <type_traits>
#include <tuple>
#include <array>
#include <ostream>
#include <algorithm>

namespace std {

  template <typename... Types> class variant {

    constexpr static std::size_t n_bounded_types = sizeof...(Types);
    using bounded_types                          = std::tuple<Types...>;
    template <std::size_t N> using bounded_type  = std14::tuple_element_t<N, bounded_types>;

    static_assert(n_bounded_types > 0, "triqs::utility::variant: list of bounded types must not be empty");
#if GCC_VERSION > 50100
    std::aligned_union_t<0, Types...> data; // Storage. We do not require a min len (0).
// data can store any of the Types, but it is not initialized. Will be used with
// "placement new".
#else
    // WORKAROUND FOR OLD VERSION OF GCC which do not have aligned_union
    struct aligned_union {
      template <typename T0, typename... Tail> struct max_size_t {
        using type = std14::conditional_t<(sizeof(T0) > sizeof(typename max_size_t<Tail...>::type)), T0, typename max_size_t<Tail...>::type>;
      };
      template <typename T0> struct max_size_t<T0> { using type = T0; };

      // Find a type of the maximum alignment value
      template <typename T0, typename... Tail> struct max_alignment_t {
        using type =
           std14::conditional_t<(alignof(T0) > alignof(typename max_alignment_t<Tail...>::type)), T0, typename max_alignment_t<Tail...>::type>;
      };
      template <typename T0> struct max_alignment_t<T0> { using type = T0; };

      struct type {
        alignas(typename max_alignment_t<Types...>::type) char _[sizeof(typename max_size_t<Types...>::type)];
      };
    };
    typename aligned_union::type data;
#endif
    std::size_t type_id; // Type ID of the stored value

    // Auxiliary metafunctions-predicates, i.e. currying metafunction T -> U ->
    // XX<T,U>
    template <typename T> struct equal {
      template <typename U> struct apply : std::is_same<T, U> {};
    };
    template <typename T> struct convertible_from {
      template <typename U> struct apply : std::is_convertible<T, U> {};
    };

#if GCC_VERSION > 50100
    // Find the id of the first type T for which Predicate::apply<T>::value is
    // true, n_bounded_types if not found
    template <typename Predicate> struct find_first_id {
      template <std::size_t... Is> static constexpr std::size_t invoke(std14::index_sequence<Is...>) {
        return std::min({(Predicate::template apply<bounded_type<Is>>::value ? std::size_t(Is) : n_bounded_types)...});
      }
      static constexpr std::size_t id = invoke(std14::make_index_sequence<n_bounded_types>{});
    };
#else
    template <typename Predicate, typename T0, typename... Tail> struct find_first_id_impl {
      constexpr static std::size_t id =
         Predicate::template apply<T0>::value ? n_bounded_types - sizeof...(Tail) - 1 : find_first_id_impl<Predicate, Tail...>::id;
    };
    template <typename Predicate, typename T0> struct find_first_id_impl<Predicate, T0> {
      constexpr static std::size_t id = Predicate::template apply<T0>::value ? n_bounded_types - 1 : n_bounded_types;
    };
    template <typename Predicate> struct find_first_id : find_first_id_impl<Predicate, Types...> {};
#endif

    // table of pointer to various variant methods ... see below.
    using table_e_t  = std::array<void (variant::*)(), n_bounded_types>;
    using table_cr_t = std::array<void (variant::*)(variant<Types...> const &), n_bounded_types>;
    using table_rr_t = std::array<void (variant::*)(variant<Types...> &&), n_bounded_types>;

    // Implementation: destructor
    template <typename T> void destroy_impl() {
      if (!std::is_trivially_destructible<T>::value) reinterpret_cast<T *>(&data)->~T();
    }

    void destroy() {
      constexpr static table_e_t table = {&variant::destroy_impl<Types>...};
      (this->*table[type_id])();
    }

    // Implementation: copy-constructor
    template <typename T> void copy_from_impl(variant const &other) { ::new (&data) T(*reinterpret_cast<T const *>(&other.data)); }

    void copy_from(variant const &other) {
      type_id                           = other.type_id;
      constexpr static table_cr_t table = {&variant::copy_from_impl<Types>...};
      (this->*table[type_id])(other);
    }

    // Implementation: move-constructor
    template <typename T> void move_impl(variant &&other) { ::new (&data) T(std::move(*reinterpret_cast<T *>(&other.data))); }

    void move_from(variant &&other) {
      type_id                           = other.type_id;
      constexpr static table_rr_t table = {&variant::move_impl<Types>...};
      (this->*table[type_id])(std::move(other));
    }

    // Implementation: assignment
    template <typename T> void assign_impl(variant const &other) { *reinterpret_cast<T *>(&data) = *reinterpret_cast<T const *>(&other.data); }

    void assign_from(variant const &other) {
      constexpr static table_cr_t table = {&variant::assign_impl<Types>...};
      (this->*table[type_id])(other);
    }

    // Implementation: move assignment
    template <typename T> void assign_move_impl(variant &&other) { *reinterpret_cast<T *>(&data) = std::move(*reinterpret_cast<T *>(&other.data)); }

    void assign_move_from(variant &&other) {
      constexpr static table_rr_t table = {&variant::assign_move_impl<Types>...};
      (this->*table[type_id])(std::move(other));
    }

    // Implementation: visitation
    template <typename T, typename F> std14::result_of_t<F(T &)> apply(F f) { return f(*reinterpret_cast<T *>(&data)); }
    template <typename T, typename F> std14::result_of_t<F(T const &)> apply_c(F f) const { return f(*reinterpret_cast<T const *>(&data)); }

    public:
    template <typename = std14::enable_if_t<std::is_default_constructible<bounded_type<0>>::value>> variant() : type_id(0) {
      ::new (&data) bounded_type<0>();
    }

    template <typename T, std::size_t id = find_first_id<convertible_from<T>>::id, typename = std14::enable_if_t<id != n_bounded_types>>
    variant(T const &v) : type_id(id) {
      ::new (&data) bounded_type<id>(v);
    }

    variant(variant const &other) { copy_from(other); }
    variant(variant &&other) { move_from(std::move(other)); }
    ~variant() { destroy(); }

    // Assignment
    variant &operator=(variant &&other) {
      if (type_id == other.type_id)
        assign_move_from(std::move(other));
      else {
        destroy();
        move_from(std::move(other));
      }
      return *this;
    }

    variant &operator=(variant const &other) {
      if (type_id == other.type_id)
        assign_from(other);
      else {
        destroy();
        copy_from(other);
      }
      return *this;
    }

    // Casts. Only into one of the Types, and with runtime check that the variant
    // really contains a type T.
    template <typename T, std::size_t id = find_first_id<equal<std14::decay_t<T>>>::id, typename = std14::enable_if_t<id != n_bounded_types>>
    operator T() {
      if (type_id != id)
        TRIQS_RUNTIME_ERROR << "triqs::utility::variant: cannot cast stored "
                               "value, type mismatch ("
                            << type_id << " vs " << id << ")";
      return *reinterpret_cast<T *>(&data);
    }

    template <typename T, std::size_t id = find_first_id<equal<std14::decay_t<T>>>::id, typename = std14::enable_if_t<id != n_bounded_types>>
    operator T() const {
      if (type_id != id)
        TRIQS_RUNTIME_ERROR << "triqs::utility::variant: cannot cast stored "
                               "value, type mismatch ("
                            << type_id << " vs " << id << ")";
      return *reinterpret_cast<T *>(&data);
    }

    // Access current type id
    std::size_t index() const noexcept { return type_id; }

// Visitation
// Return type of f(v) must be the same for any stored type, so we can use the
// first type.
#if not defined(__INTEL_COMPILER) and GCC_VERSION >= 50200
    template <typename F> friend auto visit(F &&f, variant &v) {
      using RType                                                               = std14::result_of_t<F(bounded_type<0> &)>;
      constexpr static std::array<RType (variant::*)(F), n_bounded_types> table = {&variant::apply<Types, F>...};
      return (v.*table[v.type_id])(std::forward<F>(f));
    }

    template <typename F> friend auto visit(F &&f, variant const &v) {
      using RType                                                                     = std14::result_of_t<F(bounded_type<0> const &)>;
      constexpr static std::array<RType (variant::*)(F) const, n_bounded_types> table = {&variant::apply_c<Types, F>...};
      return (v.*table[v.type_id])(std::forward<F>(f));
    }
#endif
#if not defined(__INTEL_COMPILER) and GCC_VERSION < 50200
    // Implementation for gcc 4.9 only.
    // A bug in gcc 4.9 (59766) forbids to use the friend auto which is clearer
    // than this. For icc, I can not leave it here, need to put it outside the class, which is ok for gcc 5.x and clang !
    template <typename F, typename RType = std14::result_of_t<F(bounded_type<0> &)>> friend RType visit(F &&f, variant &v) {
      constexpr static std::array<RType (variant::*)(F), n_bounded_types> table = {&variant::apply<Types, F>...};
      return (v.*table[v.type_id])(std::forward<F>(f));
    }

    template <typename F, typename RType = std14::result_of_t<F(bounded_type<0> const &)>>
    friend RType visit(F &&f, variant const &v) { // const version
      constexpr static std::array<RType (variant::*)(F) const, n_bounded_types> table = {&variant::apply_c<Types, F>...};
      return (v.*table[v.type_id])(std::forward<F>(f));
    };
#endif

    // Comparisons
    bool operator==(variant const &other) const {
      if (type_id != other.type_id) return false;
      return visit([&other](auto const &x) { return x == *reinterpret_cast<decltype(&x)>(&other.data); }, *this);
    }
    bool operator!=(variant const &other) const { return !(operator==(other)); }

    bool operator<(variant const &other) const {
      if (type_id != other.type_id) return type_id < other.type_id;
      return visit([&other](auto const &x) { return x < *reinterpret_cast<decltype(&x)>(&other.data); }, *this);
    }
    bool operator>(variant const &other) const {
      if (type_id != other.type_id) return type_id > other.type_id;
      return visit([&other](auto const &x) { return x > *reinterpret_cast<decltype(&x)>(&other.data); }, *this);
    }

    // Stream insertion
    friend std::ostream &operator<<(std::ostream &os, variant const &v) {
      visit([&os](auto const &x) { os << x; }, v);
      return os;
    }
#ifdef __INTEL_COMPILER
    template <typename F, typename... T> friend auto visit(F &&f, variant<T...> &v);
    template <typename F, typename... T> friend auto visit(F &&f, variant<T...> const &v);
#endif
  };

#ifdef __INTEL_COMPILER
  // Visitation
  // Return type of f(v) must be the same for any stored type, so we can use the
  // first type
  template <typename T0, typename... T> struct _first_type { using type = T0; };

  template <typename F, typename... T> auto visit(F &&f, variant<T...> &v) {
    using RType = std14::result_of_t<F(typename _first_type<T...>::type &)>;
    using v_t   = variant<T...>; // workaround a bug in icc. Can not simply replace in next line !!
    constexpr static std::array<RType (v_t::*)(F), sizeof...(T)> table = {&v_t::template apply<T, F>...};
    return (v.*table[v.type_id])(std::forward<F>(f));
  }

  template <typename F, typename... T> auto visit(F &&f, variant<T...> const &v) {
    using RType                                                              = std14::result_of_t<F(typename _first_type<T...>::type const &)>;
    using v_t                                                                = variant<T...>;
    constexpr static std::array<RType (v_t::*)(F) const, sizeof...(T)> table = {&v_t::template apply_c<T, F>...};
    return (v.*table[v.type_id])(std::forward<F>(f));
  }
#endif

  template <typename T, typename... U> decltype(auto) get(variant<U...> &var) { return T(var); }
  template <typename T, typename... U> decltype(auto) get(variant<U...> &&var) { return T(var); }
  template <typename T, typename... U> decltype(auto) get(variant<U...> const &var) { return T(var); }
} // namespace std

#endif

