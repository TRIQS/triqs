/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by I. Krivenko
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
#include <utility>
#include <type_traits>
#include <tuple>
#include <array>
#include <ostream>

#include <triqs/utility/exceptions.hpp>
#include <triqs/utility/c14.hpp>

// Forward-declarations for Boost.Serialization
// They are neccessary along with ugly friend declarations in class variant,
// if we want to keep the serialization code in a separate file
namespace triqs { namespace utility {
template <typename... Types> class variant;
}}

namespace boost { namespace serialization {
template <typename Archive, typename... Types> void save(Archive &, triqs::utility::variant<Types...> const&, const unsigned int);
template <typename Archive, typename... Types> void load(Archive &, triqs::utility::variant<Types...> &, const unsigned int);
}}

namespace triqs {
namespace utility {

// Too bad, std::aligned_union is a part of C++11, but it is supported only by g++ 5.1
// This implementation has to be a bit messy, if we want to support g++ 4.8 (cf gcc bug 56859)
template <typename... Types>
struct aligned_union {

  // Find a type of the maximum size
  template<typename T0, typename... Tail>
  struct max_size_t {
    using type = std14::conditional_t<(sizeof(T0)>sizeof(typename max_size_t<Tail...>::type)),
                                      T0,
                                      typename max_size_t<Tail...>::type>;
  };
  template<typename T0> struct max_size_t<T0> { using type = T0; };

  // Find a type of the maximum alignment value
  template<typename T0, typename... Tail>
  struct max_alignment_t {
    using type = std14::conditional_t<(alignof(T0)>alignof(typename max_alignment_t<Tail...>::type)),
                                      T0,
                                      typename max_alignment_t<Tail...>::type>;
  };
  template<typename T0> struct max_alignment_t<T0> { using type = T0; };

  struct type {
    alignas(typename max_alignment_t<Types...>::type) char _[sizeof(typename max_size_t<Types...>::type)];
  };
};

template<typename... Types>
class variant {

public:

  using bounded_types = std::tuple<Types...>;
  constexpr static std::size_t n_bounded_types = sizeof...(Types);
  template<std::size_t N> using bounded_type = std14::tuple_element_t<N,bounded_types>;

private:

  static_assert(n_bounded_types > 0,"triqs::utility::variant: list of bounded types must not be empty");
  static_assert(std::is_default_constructible<bounded_type<0>>::value,
                "triqs::utility::variant: the first bounded type must be DefaultConstructible");
  typename aligned_union<Types...>::type data; // Storage
  std::size_t type_id; // Type ID of the stored value

  // Support for Boost.Serialization
  template<typename Archive, typename... Types_>
  friend void boost::serialization::save(Archive & ar, variant<Types_...> const& v, const unsigned int version);
  template<typename Archive, typename... Types_>
  friend void boost::serialization::load(Archive & ar, variant<Types_...> & v, const unsigned int version);

  // Auxiliary metafunctions-predicates
  template<typename T> struct equal {
    template<typename U> struct apply : std::is_same<T,U> {};
  };
  template<typename T> struct convertible_from {
    template<typename U> struct apply : std::is_convertible<T,U> {};
  };

  template<typename Predicate, typename T0, typename... Tail> struct find_first_type_impl {
    using type = std14::conditional_t<Predicate::template apply<T0>::value,
                                      T0,
                                      typename find_first_type_impl<Predicate,Tail...>::type>;
    constexpr static std::size_t id = Predicate::template apply<T0>::value ?
                                 n_bounded_types - sizeof...(Tail) - 1 :
                                 find_first_type_impl<Predicate,Tail...>::id;
  };
  template<typename Predicate, typename T0> struct find_first_type_impl<Predicate,T0> {
    using type = std14::conditional_t<Predicate::template apply<T0>::value,T0,void>;
    constexpr static std::size_t id = Predicate::template apply<T0>::value ? n_bounded_types - 1 : -1;
  };

  // Find the first bounded type T, for which Predicate::apply<T>::value is true
  template<typename Predicate> struct find_first_type : find_first_type_impl<Predicate,Types...> {};

  // Implementation: destructor
  template<typename T> void destroy() {
    if(!std::is_trivially_destructible<T>::value)
      reinterpret_cast<T*>(&data)->~T();
  }

  // Implementation: copy-constructor
  template<typename T> void copy(variant const& other) {
    ::new(&data) T(*reinterpret_cast<T const*>(&other.data));
  }

  // Implementation: move-constructor
  template<typename T> void move(variant && other) {
    ::new(&data) T(std::move(*reinterpret_cast<T*>(&other.data)));
  }

  // Implementation: assignment
  template<typename T> void assign(variant const& other) {
    *reinterpret_cast<T*>(&data) = *reinterpret_cast<T const*>(&other.data);
  }

  // Implementation: equality comparison
  struct equal_visitor {
    variant const& other;
    equal_visitor(variant const& other) : other(other) {}
    template<typename T> bool operator()(T const& x){
      return x == *reinterpret_cast<T const*>(&other.data);
    }
  };

  // Implementation: less-than comparison
  struct less_visitor {
    variant const& other;
    less_visitor(variant const& other) : other(other) {}
    template<typename T> bool operator()(T const& x){
      return x < *reinterpret_cast<T const*>(&other.data);
    }
  };

  // Implementation: stream insertion
  struct print_visitor {
    std::ostream & os;
    print_visitor(std::ostream & os) : os(os) {}
    template<typename T> void operator()(T const& x){ os << x; }
  };

  // Implementation: visitation
  template<typename T, typename F>
  std14::result_of_t<F(T&)> apply(F f) {
    return f(*reinterpret_cast<T*>(&data));
  }
  template<typename T, typename F>
  std14::result_of_t<F(T)> apply(F f) const { // const version
    return f(*reinterpret_cast<T const*>(&data));
  }

  // Declare dispatch tables
#define DECLARE_DT(NAME,RTYPE,ARGS) \
  constexpr static std::array<RTYPE(variant::*)ARGS,n_bounded_types> \
  NAME##_dt = {&variant::NAME<Types>...};
  DECLARE_DT(destroy,      void,())
  DECLARE_DT(copy,         void,(variant<Types...> const&))
  DECLARE_DT(move,         void,(variant<Types...> &&))
  DECLARE_DT(assign,       void,(variant<Types...> const&))
#undef DECLARE_DT

public:

#warning I'd like to delete the default constructor; this would require a change to c++2py
  // Default constructor
  variant() : type_id(0) {
    ::new(&data) bounded_type<0>();
  }

  // Constructor
  template<typename T, typename MatchingType = find_first_type<convertible_from<T>>,
           typename = std14::enable_if_t<MatchingType::id != -1>>
  variant(T v) : type_id(MatchingType::id) {
    ::new(&data) typename MatchingType::type(v);
  }

  // Copy-constructor
  variant(variant const& other) : type_id(other.type_id) {
    (this->*copy_dt[type_id])(other);
  }

  // Move-constructor
  variant(variant && other) : type_id(other.type_id) {
    (this->*move_dt[type_id])(std::move(other));
  }

  // Destructor
  ~variant() { (this->*destroy_dt[type_id])(); }

  // Assignment
  variant & operator=(variant const& other) {
    if(type_id == other.type_id) {
      (this->*assign_dt[type_id])(other);
    } else {
      (this->*destroy_dt[type_id])();
      type_id = other.type_id;
      (this->*copy_dt[type_id])(other);
    }
    return *this;
  }

  // Casts
  template<typename T,
           typename MatchingType = find_first_type<equal<std14::remove_const_t<std14::remove_reference_t<T>>>>,
           typename = std14::enable_if_t<MatchingType::id != -1>>
  operator T() {
    static const std::size_t cast_type_id = MatchingType::id;
    if(type_id != cast_type_id)
      TRIQS_RUNTIME_ERROR << "triqs::utility::variant: cannot cast stored value, type mismatch (" 
                          << type_id << " vs " << cast_type_id << ")";
    return *reinterpret_cast<T*>(&data);
  }

  // Visitation
  // Return type of f(v) must be the same for any stored type,
  // that is why we can use the first type.
  template <typename F, typename RType = std14::result_of_t<F(bounded_type<0>&)>>
  friend RType apply_visitor(F &&f, variant & v) {
    constexpr static std::array<RType(variant::*)(F),n_bounded_types> apply_dt = {&variant::apply<Types,F>...};
    return (v.*apply_dt[v.type_id])(std::forward<F>(f));
  }
  template <typename F, typename RType = std14::result_of_t<F(bounded_type<0>)>>
  friend RType apply_visitor(F &&f, variant const& v) { // const version
    constexpr static std::array<RType(variant::*)(F) const,n_bounded_types> apply_dt = {&variant::apply<Types,F>...};
    return (v.*apply_dt[v.type_id])(std::forward<F>(f));
  }

  // Comparisons
  bool operator==(variant const& other) const {
    if(type_id != other.type_id)
      TRIQS_RUNTIME_ERROR << "triqs::utility::variant: cannot compare stored values of different types";
    return apply_visitor(equal_visitor(other),*this);
  }
  bool operator!=(variant const& other) const { return !(operator==(other)); }

  bool operator<(variant const& other) const {
    if(type_id != other.type_id)
      TRIQS_RUNTIME_ERROR << "triqs::utility::variant: cannot compare stored values of different types";
    return apply_visitor(less_visitor(other),*this);
  }

  // Stream insertion
  friend std::ostream & operator<<(std::ostream & os, variant const &v) {
    apply_visitor(print_visitor(os),v);
    return os;
  }

};

// Define dispatch tables
#define DEFINE_DT(NAME,RTYPE,ARGS) \
template<typename... Types> \
constexpr std::array<RTYPE(variant<Types...>::*)ARGS,variant<Types...>::n_bounded_types> \
variant<Types...>::NAME##_dt;

DEFINE_DT(destroy,      void,())
DEFINE_DT(copy,         void,(variant<Types...> const&))
DEFINE_DT(move,         void,(variant<Types...> &&))
DEFINE_DT(assign,       void,(variant<Types...> const&))
#undef DEFINE_DT

}
}
